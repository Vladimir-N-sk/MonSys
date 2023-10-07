//  $Id: RndSensor.h,v 1.12 2003/02/16 10:30:44 denis Exp $

#ifndef RndSensor_h
#define RndSensor_h

#include "Sensor.h"

#include <fstream>

#include "utils.h"

#include "Message.h"
#include "Message_Body.h"
//#include "S2SMapRobot.h"
#include "Parameter.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>



class Network;
class Message;

class RndSensor
 : public RTSensor {

public:
    virtual ~RndSensor() noexcept {};
    RndSensor( const string&, const Address&, Network*, const timespec&);
    Message* getSensorData() throw(exception);
//    Message* getSensorData();
};

/*
class RndIntSensor
 : public RTSensor {

public:
    RndIntSensor( const string&, const Address&, Network*, const timespec&);
    Message* getSensorData() throw(exception);
};

class ConstSensor
 : public RTSensor {

public:
    ConstSensor( const string&, const Address&, Network*, const timespec&,
                                  double);
    Message* getSensorData() throw(exception);
private:
    double n;
};


class ConstStringSensor
 : public RTSensor {

public:
    ConstStringSensor( const string&, const Address&, Network*, const timespec&,
                                  string);
    Message* getSensorData() throw(exception);
private:
    string s;
};

class ScriptSensor
 : public RTSensor {

public:
    ScriptSensor( const string&, const Address&, Network*, const timespec&, const timespec&,
                                  string);
//    Message* getSensorData() throw(exception);
    Message* getSensorData() throw(runtime_error);
private:
    string s;
    string sss;
};

class TmpSensor
 : public RTSensor {

public:
    TmpSensor( const string&, const Address&, Network*, const timespec&,
                                  string);
    Message* getSensorData() throw(exception);
private:
    string t;
    bool first;
};

class ConstDiscrSensor
 : public RTSensor {

public:
    ConstDiscrSensor( const string&, const Address&, Network*, const timespec&,
                                  string, const string&, const string&, unsigned, unsigned);
    Parameter d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,d14,d15;

    Message* getSensorData() throw(exception);
private:
    	string str;
    	string zeroString, oneString;
	unsigned zeroAlarm, oneAlarm;

};


class FileReadSensor
 : public RTSensor {

public:
    FileReadSensor( const string&, const Address&, Network*, const timespec&, const timespec&,
                                  string);

    Message* getSensorData() throw(runtime_error);
private:
    string s;
    string sss;
char ch;
ifstream infile;

};
*/

#endif
