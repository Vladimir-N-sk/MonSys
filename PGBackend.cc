#include "PGBackend.h"

const string PG_TIME_FORMAT = "\'YYYY-MM-DD HH24:MI:SS.US\'";

PGLBackend::PGLBackend( const string& c)
 : //conn = PQconnectdb(c.data()),
// PgDatabase( c.data()),
   conninfo(c),
   checked(false),
   down(ZEROTIME),
   reconnectTimeout(3) //TODO config var
{
//    if ( ConnectionBad())
    conn = PQconnectdb(c.data());

    if (PQstatus(conn) != CONNECTION_OK) {
        switch(PQstatus(conn))
        {
            case CONNECTION_STARTED:
                MOND_DEBUG << "DB status: Подключение..." << conninfo << endl;
                break;
            case CONNECTION_MADE:
                MOND_DEBUG << "DB status: Подключение к серверу..." << conninfo << endl;
                break;
            case CONNECTION_AWAITING_RESPONSE:
                MOND_DEBUG << "DB status: Ожидание ответа от сервера" << conninfo << endl;
                break;
            case CONNECTION_AUTH_OK:
                MOND_DEBUG << "DB status: Аутентификация получена; ожидание завершения запуска серверной части." << conninfo << endl;
                break;
            case CONNECTION_SSL_STARTUP:
                MOND_DEBUG << "DB status: Согласование SSL-шифрования." << endl;
                break;
            case CONNECTION_SETENV:
                MOND_DEBUG << "DB status: Согласование значений параметров, зависящих от программной среды."<< endl;
                break;
            default:
                MOND_DEBUG << "DB status: Не определено..." << conninfo << endl;
        }
    }
    else
        MOND_DEBUG << "DB status: Подключено - " << conninfo << endl;
}

PGLBackend::~PGLBackend()
{
    PQclear(res);
    PQfinish(conn);
    MOND_DEBUG << "DB connection closed" << endl;
}

void PGLBackend::thread()
{

    Message* msg = NULL;

    if (checked) msg = wait();
    for(;;){
        try {
            if ( !checked) {
                MOND_DEBUG << "DB checking..." << endl;
                if ( ZEROTIME == ( down = lastRecordTime())) {
                    MOND_DEBUG << "DB ok" << endl;
                } else {
                    MOND_DEBUG << "DB fixing..." << endl;
                    down = ZEROTIME;
                }
                checked = true;
                setBHMode( false);
                initFilters();
                break;
            }
      /**
       *  write msg into db or push the rest of buffer
       *  and close db in case of resuming after db failure
       */
            if ( NULL != msg) {
                write( msg);
                msg->decUses();
                msg = NULL;
            }

            if ( ZEROTIME == down) break;

            if ( !isEmpty())
                msg = wait();
            else {
                down = ZEROTIME;
                setBHMode( false);
                initFilters();
                break;
            }
        }
        catch ( const DBError& e) {
            setBHMode( true);
            if ( ZEROTIME == down) down = now();

            MOND_ERROR << "DB error: " << e.what() << endl;

            while ( PQstatus(conn) != CONNECTION_OK) {
            MOND_DEBUG << "Error connection to DB" << endl;
                sleep( reconnectTimeout);
                PQclear(res);
                PQfinish(conn);
//                 Connect( conninfo.data());
            MOND_DEBUG << "Try connection to DB" << endl;
                conn = PQconnectdb(conninfo.data());
            }
             sleep( reconnectTimeout); //XXX paranoia

        switch(PQstatus(conn))
        {
            case CONNECTION_OK:
                MOND_DEBUG << "DB status: Подключено." << conninfo << endl;
                break;
            case CONNECTION_STARTED:
                MOND_DEBUG << "DB status: Подключение..." << conninfo << endl;
                break;
            case CONNECTION_MADE:
                MOND_DEBUG << "DB status: Подключение к серверу..." << conninfo << endl;
                break;
            case CONNECTION_AWAITING_RESPONSE:
                MOND_DEBUG << "DB status: Ожидание ответа от сервера" << conninfo << endl;
                break;
            case CONNECTION_AUTH_OK:
                MOND_DEBUG << "DB status: Аутентификация получена; ожидание завершения запуска серверной части." << conninfo << endl;
                break;
            case CONNECTION_SSL_STARTUP:
                MOND_DEBUG << "DB status: Согласование SSL-шифрования." << endl;
                break;
            case CONNECTION_SETENV:
                MOND_DEBUG << "DB status: Согласование значений параметров, зависящих от программной среды."<< endl;
                break;
            default:
                MOND_DEBUG << "DB status: Не определено..." << conninfo << endl;
        }




//             MOND_DEBUG << "Successful connection to DB" << endl;
        }
        catch ( const Message::empty& e) {
            break;
        }
    }
}

void PGLBackend::write( Message* msg) throw( PGLBackend::DBError, Message::empty)
{

    string id = getParameterID( msg->getSource());
//     MOND_DEBUG << "DB ID:"<< id << " PGLBackend::write:" << endl;

    if ( "" == id)
        id = addParameter( msg->getSource());

        updDesc( msg->getSource(), id );

    stringstream as;  //combines status and alarm in one field
    as << msg->getAlarm() * 0x100 + msg->getStatus();

    string q =
            "INSERT INTO svalue(id, value, alarm, time) VALUES ( \'" +
            id + "\',\'" +
            msg->getTextValue() + "\',\'" +
            as.str() + "\',\'" +
            msg->getTextTime() + "\');";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) throw DBError( msg->getParameterName() +" : "+q);

    if ( getIDsost(id) == "" ) {
	    q = "INSERT INTO sost(id,alarm) VALUES ("
		  + id
		  + ",512);";
    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) throw DBError( q);
	}

    q = "UPDATE sost SET value = \'"
		+ msg->getTextValue()
		+ "\',alarm = \'"
		+ as.str()
		+ "\',time = \'"
		+ msg->getTextTime()
        + "\' WHERE id = "  + id  + ";";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) throw DBError( q);

//     MOND_DEBUG << "DB ID:"<< id << " PGLBackend::write:" << q << endl;
}

void PGLBackend::updDesc( Parameter* p, const string& id)
                                throw( PGLBackend::DBError)
{


    string q = "SELECT description FROM sdesc WHERE id = "  + id  + ";";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) throw DBError(p->getName()+": "+ q);

    if ( 1 != PQntuples(res)) {

    stringstream nt;
    nt << p->getProperties()->type;

    q = "INSERT INTO sdesc(id,description,measure,type) VALUES ("
                + id
                + ",\'" + p->getProperties()->desc + "\'"
                + ",\'" + p->getProperties()->measure + "\'"
                + "," + nt.str()

                + ");";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) throw DBError(p->getName()+": "+ q);
// MOND_DEBUG << "DB ID:"<< id << " Insert SDESC:" << q << endl;
    return;
    }

if ( PQgetvalue(res, 0, 0)  != p->getProperties()->desc ) {

   q =
  "UPDATE sdesc SET description= \'" + p->getProperties()->desc
   + "\', measure= \'"     + p->getProperties()->measure
   + "\' WHERE id = "  + id  + ";";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) throw DBError(p->getName()+": "+ q);

//  MOND_DEBUG << "DB ID:"<< id << " UpdSDESC:" << q << endl;
    return;
}
}

string PGLBackend::addParameter( Parameter* p)
                                throw( PGLBackend::DBError)
{

    string q =
      "INSERT INTO sname(name) VALUES (\'" + p->getName() + "\');";

// MOND_DEBUG << "PGLBackend::addParameter q:"<< q << endl;

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) throw DBError(p->getName()+": "+ q);
//     if ( !ExecCommandOk( q.data())) throw DBError( q);

// MOND_DEBUG << "PGLBackend::addParameter after DBerror"  << endl;

    const string id = getParameterID( p);

// MOND_DEBUG << "PGLBackend::addParameter id:"<< id << endl;

    stringstream nt;
    nt << p->getProperties()->type;

    q = "INSERT INTO sdesc(id,description,measure,type) VALUES ("
                + id
                + ",\'" + p->getProperties()->desc + "\'"
                + ",\'" + p->getProperties()->measure + "\'"
                + "," + nt.str()
                + ");";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) throw DBError(p->getName()+": "+ q);


//     if ( !ExecCommandOk( q.data())) throw DBError( q);

    q = "INSERT INTO sost(id,alarm) VALUES ("
  + id
  + ",512);";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) throw DBError(p->getName()+": "+ q);


//     if ( !ExecCommandOk( q.data())) throw DBError( q);
// MOND_DEBUG << "PGLBackend::addParameter return id:"<< id << endl;
    return id;

}
string PGLBackend::getIDsost( const string& id) throw( PGLBackend::DBError)
{

    const string q = "SELECT id FROM sost WHERE id =  \'"
                      + id + "\';";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) throw DBError( q);
    if ( 1 != PQntuples(res)) return "";
    return PQgetvalue(res, 0, 0);
}

string PGLBackend::getParameterID( Parameter* p) throw( PGLBackend::DBError)
{

    const string q = "SELECT id FROM sname WHERE name =  \'"
                      + p->getName() + "\';";

// MOND_DEBUG << "PGLBackend::getParameterID q:"<< q << endl;
    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) throw DBError( q);

    if ( 1 != PQntuples(res)) return "";

//     return GetValue(0,0);

// MOND_DEBUG << "PGLBackend::getParameterID PQgetvalue(res, 0, 0):"<< PQgetvalue(res, 0, 0) << endl;

    return PQgetvalue(res, 0, 0);
}

timespec PGLBackend::lastRecordTime()
                  throw( PGLBackend::DBError, runtime_error)
{

    if ( isDBClosed()) return ZEROTIME;

    const string q =
       "SELECT to_char( time, " + PG_TIME_FORMAT + ")"
         " FROM svalue ORDER BY time DESC LIMIT 1;";

//     if ( !ExecTuplesOk( q.data())) throw DBError( q);
    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) throw DBError( q);


    if ( 1 != PQntuples(res))
        throw runtime_error("fatal error, DB was modified during check");

//     return isotime2timespec( GetValue(0,0));
     return isotime2timespec( PQgetvalue(res, 0, 0) );
}

bool PGLBackend::isDBClosed() throw( PGLBackend::DBError)
{

    const string q =
        "SELECT id,alarm FROM svalue WHERE time = (SELECT max(time) FROM svalue "
        "where id = (select min(id) from svalue)) and alarm & 1::int2 = 0;";

//         MOND_DEBUG << "Запрос:" << q << endl;

// qq=query.c_str();
    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) throw DBError( q);

        if ( PQntuples(res) == 0) {
//             PQclear(res);
            return true;
        }
//         PQclear(res);
        return false;

//     if ( !ExecTuplesOk( q.data())) throw DBError( q);
//     return 0 == Tuples();

}

void PGLBackend::close( const timespec& dt) throw( PGLBackend::DBError)
{
/*
    const string time = timespec2isotime( dt);

    const string closeRecordsWithTheSameTime =
          "UPDATE svalue SET alarm = alarm | 1::int2 WHERE time = "
          "\'" + time + "\' AND alarm & 1::int2 != 1;";

    if ( !ExecCommandOk( closeRecordsWithTheSameTime.data()))
        throw DBError( closeRecordsWithTheSameTime);

  const string closeAllUnclosedRecords =
    "INSERT INTO svalue(id,value,alarm,time)"
    "SELECT id,value,alarm | 1::int2 AS alarm, "
    "\'" + time + "\'"
    "FROM (select t1.id, t1.value, t1.alarm from "
    "(SELECT id,max(time) as ttt FROM svalue GROUP BY id) tmp, "
     "svalue as t1 where tmp.id=t1.id and tmp.ttt=t1.time AND t1.time < (select to_timestamp("
     "\'" + time + "\'"
      " ,'YYYY-MM-DD HH24:MI:SS.US')::timestamp) and t1.alarm & 1::int2 = 0) qwert;";

    if ( !ExecCommandOk( closeAllUnclosedRecords.data()))
        throw DBError( closeAllUnclosedRecords);

    MOND_DEBUG << "DB fixed" << endl;
*/
}

