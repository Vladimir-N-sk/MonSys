
#ifndef SocketSensorModbus_h
#define SocketSensorModbus_h

#include <sched.h>
#include <unistd.h>

#include <termios.h>
#include <string>

#include "common.h"
#include "globals.h"
#include "Time.h"
#include "Message.h"
#include "Message_Body.h"
#include "SocketNetwork.h"
#include "Sensor.h"
#include "Config.h"
#include "utils.h"

class SocketNetwork;
class SocketOpenNetwork;
class Message;
//class utils;

struct  SModEntry {
    SModEntry( const Config::Properties&);
    string command;
    int valSize;
    timespec delay;
    timespec timeout;
};

class SocketSensorModbus
 : public RTSensor {

public:
    SocketSensorModbus( const string&, const Address&, SocketNetwork*, const SModEntry&);
    virtual ~SocketSensorModbus();

    Message* getSensorData() throw(runtime_error);

protected:
    virtual Message* makeMessage( const string&)=0;

private:

    SocketNetwork *net;

    const string command;
    const string setport;
    const int valSize;
	int seconds;
//    const speed_t speed;
    const int answerSize;
    string buf, buf2;
};

class SocketOpenSensorModbus
 : public RTSensor {

public:
    SocketOpenSensorModbus( const string&, const Address&, SocketOpenNetwork*, const SModEntry&);
    virtual ~SocketOpenSensorModbus();

    Message* getSensorData() throw(runtime_error);

protected:
    virtual Message* makeMessage( const string&)=0;

private:

    SocketOpenNetwork *net;

    const string command;
    const int valSize;
	int seconds;
//    const speed_t speed;
    const int answerSize;
    string buf, buf2;
};



class SocketOpenSensorModbusTCP
 : public RTSensor {

public:
    SocketOpenSensorModbusTCP( const string&, const Address&, SocketOpenNetwork*, const SModEntry&);
    virtual ~SocketOpenSensorModbusTCP();

    Message* getSensorData() throw(runtime_error);

protected:
    virtual Message* makeMessage( const string&)=0;

private:

    SocketOpenNetwork *net;

    const string command;
    const int valSize;
	int seconds;
//    const speed_t speed;
    const int answerSize;
    string buf, buf2;
};


#endif
