
#include "SocketSensorModbus.h"

/********************************************************************************************************************************
 *
 *
 */
Message* SocketOpenSensorModbusTCP::getSensorData() throw( runtime_error)
{

   net->lock();

    string bbb;
	int send;
    char ch[30];
    int nnn=0;

	try {
        buf = buf2 = "";
		buf=command;
 	int bb=0;
	for ( char nn=0; nn<buf.length(); nn=nn+2)
	{
    		ch[bb]=hex2<int>(buf.substr(nn,2));
			buf2=buf2+ch[bb];
    		bb++;
 	};

timespec time_b=now();

  nnn=net->ConnectSocket();
  net->writeSocket(buf2);

buf=buf2;
timespec time_s=now();

        buf2="";
        buf2 = net->RecvSocket( answerSize, timeout);

/****************************************
 *задержка в timeout сек. для ПЛК ОВЕН
 */
        sleep( timeout.tv_sec);

		timespec time_e=now();
MONSYS_DEBUG << this->getName()<< " Send:"<<str2hex(buf, " ")<< " Recive:"<<str2hex(buf2, " ")<<" time_send: " << milisec(time_s-time_b) <<" time_recive: " << milisec(time_e-time_s)<< endl;

	}
    catch ( WriteError& e) {
	  net->unlock();
        throw;
    }
    catch ( ReadError& e) {
	  net->unlock();
        throw;
    }

    net->unlock();
    try {

        if (    (buf2[0]+buf2[1]+buf2[2]+buf2[3]) != 0     ){
           bbb = "Error recive answer:"+str2hex(buf2, " ") ;
         throw ReadError(bbb);
         }

            buf="";
            buf=str2hex(buf2);
            buf=buf.substr(18,valSize*2);
          return makeMessage(buf);
    }
    catch (exception& e) {
	bbb="Error makeMessage:"+ str2hex(buf2," ")  ;
        throw ReadError( bbb);
    }
}

SocketOpenSensorModbusTCP::SocketOpenSensorModbusTCP( const string& name, const Address& a, SocketOpenNetwork *n,
                         const SModEntry& s)
 : RTSensor(name,a,n,s.timeout,s.delay),
   net(n),
   command(s.command),
   valSize(s.valSize),
   answerSize(s.valSize + 9)
{
}

SocketOpenSensorModbusTCP::~SocketOpenSensorModbusTCP()
{
}

/******************************************************************************************************************
 *
 *
 *
 *
 * ********************************************************************************************************************/

Message* SocketOpenSensorModbus::getSensorData() throw( runtime_error)
{

   net->lock();

    string bbb;
	int send, nnn=0;
    char ch[30];

	try {

        buf = buf2 = "";
		buf=command+modbus_rtu_crc(command);        
 	int bb=0;
	for ( char nn=0; nn<buf.length(); nn=nn+2)
	{
    		ch[bb]=hex2<int>(buf.substr(nn,2));
		buf2=buf2+ch[bb];
    		bb++;
 	};

timespec time_b=now();

  nnn=net->ConnectSocket();

  net->writeSocket(buf2);

buf=buf2;
timespec time_s=now();

        buf2="";
        buf2 = net->RecvSocket( answerSize, timeout);

		timespec time_e=now();
MONSYS_DEBUG << this->getName()<< " Send:"<<str2hex(buf, " ")<< " Recive("<< buf2.size() << "):"<<str2hex(buf2, " ")<<" time_send: " << milisec(time_s-time_b) <<" time_recive: " << milisec(time_e-time_s)<< endl;



	}
    catch ( WriteError& e) {
	  net->unlock();
        throw;
    }
    catch ( ReadError& e) {
	  net->unlock();
        throw;
    }

    net->unlock();

    try {

        if ( !checkCRC(buf2)){            
        bbb = "Error checkCRC:"+buf2 ;
    	 throw ReadError(bbb);
    	 }
            buf="";
            buf=str2hex(buf2);
            buf=buf.substr(6,valSize*2);
			buf2="";
          return makeMessage(buf);
    }
    catch (exception& e) {
	bbb="Error makeMessage:"+ buf2 ;
        throw ReadError( bbb);
    }
}

SocketOpenSensorModbus::SocketOpenSensorModbus( const string& name, const Address& a, SocketOpenNetwork *n,
                         const SModEntry& s)
 : RTSensor(name,a,n,s.timeout,s.delay),
   net(n),
   command(s.command),
   valSize(s.valSize),
   answerSize(s.valSize + 5)
{
}

SocketOpenSensorModbus::~SocketOpenSensorModbus()
{
}

/******************************************************************************************************************
 *
 *
 * ********************************************************************************************************************/

Message* SocketSensorModbus::getSensorData() throw( runtime_error)
{
   net->lock();

    string bbb;
	int send, nnn=0;
    char ch[30];
    try {

        buf = buf2 = "";

		timespec time_b=now();

		buf=command+modbus_rtu_crc(command);        
 	int bb=0;
	for ( char nn=0; nn<buf.length(); nn=nn+2)
	{
    		ch[bb]=hex2<int>(buf.substr(nn,2));
		buf2=buf2+ch[bb];
    		bb++;
 	};

  net->writeSocket(buf2);


buf=buf2;
timespec time_s=now();


        buf2="";
        buf2 = net->RecvSocket( answerSize, timeout);

timespec time_e=now();
MONSYS_DEBUG << this->getName()<< " Send:"<<str2hex(buf, " ")<< " Recive:"<<str2hex(buf2, " ")<<" time_send: " << milisec(time_s-time_b) <<" time_recive: " << milisec(time_e-time_s)<< endl;


	}
    catch ( WriteError& e) {
	  net->unlock();
        throw;
    }
    catch ( ReadError& e) {
	  net->unlock();
        throw;
    }

    net->unlock();

    try {

        if ( !checkCRC(buf2)){            
        bbb = "Error checkCRC:"+buf2 ;
    	 throw ReadError(bbb);
    	 }
            buf="";
            buf=str2hex(buf2);
            buf=buf.substr(6,(answerSize-5)*2);
          return makeMessage(buf);
    }
    catch (exception& e) {
	bbb="Error makeMessage:"+ buf2 ;
        throw ReadError( bbb);
    }
}

SocketSensorModbus::SocketSensorModbus( const string& name, const Address& a, SocketNetwork *n,
                         const SModEntry& s)
 : RTSensor(name,a,n,s.timeout,s.delay),
   net(n),
   command(s.command),
   valSize(s.valSize),
   answerSize(s.valSize + 5)
{
}

SocketSensorModbus::~SocketSensorModbus()
{
}

SModEntry::SModEntry( const Config::Properties& p)
{
    command = trim( p["command"]);
    valSize = dec2<unsigned>( p["valsize"]);
    timeout = read_timespec( p["timeout"]);
    delay = read_timespec( p["delay"]);
}
