
#ifndef Dispatcher_h
#define Dispatcher_h

#include "MTmap.h"

using namespace std;

class Config;
//class Config::Properties;
class Sensor;
class Parameter;
class Network;
class Backend;
class Robot;
class Runable;

class Dispatcher {

public:
    Dispatcher( const Config&);
    ~Dispatcher();

    Robot* makeRobot( const string&) throw ( string, runtime_error);
    Runable* makeSensor( const string&) throw ( string, exception);
    void dropSensor( const string&);
    Runable* makeNetwork( const string&) throw ( string, runtime_error);

// AVN for test    
//    Runable* makeNetwork() throw ( string, runtime_error);    

    void dropNetwork( const string&);
    Runable* makeBackend( const string&) throw ( string, exception);
    void dropBackend( const string&);
    void attach( const string&, const string&);
    void detach( const string&, const string&);
    void detach( const string&);
    void doConfig() throw ( string, exception);

private:
    void make_and_run( const list<string>&,
                       Runable* (Dispatcher::*)( const string&),
                       const string&)
                       throw ( exception, string);

    void attachBackend( const Config::Properties&);

    MTmap< string, Network* > networks;
    MTmap< string, Backend* > backends;

    const Config& config;
};
#endif
