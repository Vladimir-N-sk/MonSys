#ifdef PSQL

#include "PGBackend.h"

const string PG_TIME_FORMAT = "\'YYYY-MM-DD HH24:MI:SS.US\'";

PGLBackend::PGLBackend( const string& c)
 : conninfo(c),
   checked(false),
   down(ZEROTIME),
   reconnectTimeout(3) //TODO config var
{
    conn = PQconnectdb(c.data());

    if (PQstatus(conn) != CONNECTION_OK) {
        switch(PQstatus(conn))
        {
            case CONNECTION_STARTED:
                MONSYS_DEBUG << "DB status: Подключение..." << conninfo << endl;
                break;
            case CONNECTION_MADE:
                MONSYS_DEBUG << "DB status: Подключение к серверу..." << conninfo << endl;
                break;
            case CONNECTION_AWAITING_RESPONSE:
                MONSYS_DEBUG << "DB status: Ожидание ответа от сервера" << conninfo << endl;
                break;
            case CONNECTION_AUTH_OK:
                MONSYS_DEBUG << "DB status: Аутентификация получена; ожидание завершения запуска серверной части." << conninfo << endl;
                break;
            case CONNECTION_SSL_STARTUP:
                MONSYS_DEBUG << "DB status: Согласование SSL-шифрования." << endl;
                break;
            case CONNECTION_SETENV:
                MONSYS_DEBUG << "DB status: Согласование значений параметров, зависящих от программной среды."<< endl;
                break;
            default:
                MONSYS_DEBUG << "DB status: Не определено..." << conninfo << endl;
        }
    }
    else
        MONSYS_DEBUG << "DB status: Подключено - " << conninfo << endl;
}

PGLBackend::~PGLBackend()
{
    PQclear(res);
    PQfinish(conn);
    MONSYS_DEBUG << "DB connection closed" << endl;
}

void PGLBackend::thread()
{

    Message* msg = NULL;

    if (checked) msg = wait();
    for(;;){
        try {
            if ( !checked) {
                MONSYS_DEBUG << "DB checking..." << endl;
                if ( ZEROTIME == ( down = lastRecordTime())) {
                    MONSYS_DEBUG << "DB ok" << endl;
                } else {
                    MONSYS_DEBUG << "DB fixing..." << endl;
                    down = ZEROTIME;
                }
                checked = true;
                setBHMode( false);
                initFilters();
                break;
            }
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

            MONSYS_ERROR << "DB error: " << e.what() << endl;

            while ( PQstatus(conn) != CONNECTION_OK) {
            MONSYS_DEBUG << "Error connection to DB" << endl;
                sleep( reconnectTimeout);
                PQclear(res);
                PQfinish(conn);
            MONSYS_DEBUG << "Try connection to DB" << endl;
                conn = PQconnectdb(conninfo.data());
            }
             sleep( reconnectTimeout); //XXX paranoia

        switch(PQstatus(conn))
        {
            case CONNECTION_OK:
                MONSYS_DEBUG << "DB status: Подключено." << conninfo << endl;
                break;
            case CONNECTION_STARTED:
                MONSYS_DEBUG << "DB status: Подключение..." << conninfo << endl;
                break;
            case CONNECTION_MADE:
                MONSYS_DEBUG << "DB status: Подключение к серверу..." << conninfo << endl;
                break;
            case CONNECTION_AWAITING_RESPONSE:
                MONSYS_DEBUG << "DB status: Ожидание ответа от сервера" << conninfo << endl;
                break;
            case CONNECTION_AUTH_OK:
                MONSYS_DEBUG << "DB status: Аутентификация получена; ожидание завершения запуска серверной части." << conninfo << endl;
                break;
            case CONNECTION_SSL_STARTUP:
                MONSYS_DEBUG << "DB status: Согласование SSL-шифрования." << endl;
                break;
            case CONNECTION_SETENV:
                MONSYS_DEBUG << "DB status: Согласование значений параметров, зависящих от программной среды."<< endl;
                break;
            default:
                MONSYS_DEBUG << "DB status: Не определено..." << conninfo << endl;
        }


        }
        catch ( const Message::empty& e) {
            break;
        }
    }
}

void PGLBackend::write( Message* msg) throw( PGLBackend::DBError, Message::empty)
{

    string id = getParameterID( msg->getSource());

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

MONSYS_DEBUG << "PGLBakend write text: "<<msg->getTextValue()<< " at time:" << msg->getTextTime()<< endl;

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
    return;
    }

if ( PQgetvalue(res, 0, 0)  != p->getProperties()->desc ) {

    stringstream nt;
    nt << p->getProperties()->type;

   q =
  "UPDATE sdesc SET description= \'" + p->getProperties()->desc
   + "\', measure= \'"     + p->getProperties()->measure
   + "\', type= \'" + nt.str()
   + "\' WHERE id = "  + id  + ";";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) throw DBError(p->getName()+": "+ q);

    return;
}
}

string PGLBackend::addParameter( Parameter* p)
                                throw( PGLBackend::DBError)
{
    string q =
      "INSERT INTO sname(name) VALUES (\'" + p->getName() + "\');";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) throw DBError(p->getName()+": "+ q);

    const string id = getParameterID( p);

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

    q = "INSERT INTO sost(id,alarm) VALUES ("
  + id
  + ",512);";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) throw DBError(p->getName()+": "+ q);
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

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) throw DBError( q);

    if ( 1 != PQntuples(res)) return "";

    return PQgetvalue(res, 0, 0);
}

timespec PGLBackend::lastRecordTime()
                  throw( PGLBackend::DBError, runtime_error)
{

    if ( isDBClosed()) return ZEROTIME;

    const string q =
       "SELECT to_char( time, " + PG_TIME_FORMAT + ")"
         " FROM svalue ORDER BY time DESC LIMIT 1;";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) throw DBError( q);


    if ( 1 != PQntuples(res))
        throw runtime_error("fatal error, DB was modified during check");

     return isotime2timespec( PQgetvalue(res, 0, 0) );
}

bool PGLBackend::isDBClosed() throw( PGLBackend::DBError)
{

    const string q =
        "SELECT id,alarm FROM svalue WHERE time = (SELECT max(time) FROM svalue "
        "where id = (select min(id) from svalue)) and alarm & 1::int2 = 0;";

    PQclear(res);
    res = PQexec(conn, q.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) throw DBError( q);

        if ( PQntuples(res) == 0) {
            return true;
        }
        return false;

}

void PGLBackend::close( const timespec& dt) throw( PGLBackend::DBError)
{

}

#endif