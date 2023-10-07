
#include "common.h"
#include "globals.h"
#include "utils.h"
#include "Time.h"

#include "Sensor.h"
#include "Network.h"
#include "Backend.h"
#include "Robot.h"
#include "Config.h"

#ifdef NETSNMP
    #include "SNMPSensor.h"
    #include "SNMPNetwork.h"
    #include "SNMPTrapBackend.h"
    #undef MAX_NAME_LEN      // buggy net-snmp

    #include "SerialNetwork.h"
    #include "SocketNetwork.h"
    #include "SocketServerNetwork.h"
#endif

#ifdef EXPRESSION
    #include "Expression.h"
    #include "Parser.h"    
#endif


#include "RndSensor.h"
#include "PseudoNetwork.h"
#include "S2SMapRobot.h"
#include "LogBackend.h"

#include "Dispatcher.h"

#include <sstream>

#include "Logger.h"

const string TIMEOUT = "timeout";
const string DELAY = "delay";
const string TYPE = "type";
const string NET = "net";

void Dispatcher::doConfig() throw ( string, exception)
{
    const list<string> obj = config.objects();

    string type;
    list< string> nsrb = obj; //Network, Sensor, Robot, Backend

    while ( nsrb.size() != 0) {
        list< string> srb;
        list< string> rb;
        list< string> bb;

        for ( list<string>::iterator p = nsrb.begin(); p!=nsrb.end(); ++p) {

            try {
                type = config[*p][TYPE];
            }
            catch (Config::NoSuchProp& e) {
                continue;
            }

            try {              
                 if ( type.npos != type.rfind( "Network")) {
                    MOND_DEBUG << "Network: " << *p << endl;                    
                    makeNetwork(*p)->run();                    
                    continue;                    
                 } else {
                    srb.push_back( *p);
                 }
            }
            catch ( Parameter::FindError& e) {}
        }

        for ( list<string>::iterator p = srb.begin(); p!=srb.end(); ++p) {

            try {
                type = config[*p][TYPE];
            }
            catch (Config::NoSuchProp& e) {
                continue;
            }

            try {              
                 if ( type.npos != type.rfind( "Sensor")) {
                    MOND_DEBUG << "Sensor: " << *p << endl;                    
                    makeSensor(*p)->run();
                    continue;                    
                 } else {
                    rb.push_back( *p); 
                }        
            }               
            catch ( Parameter::FindError& e) {}
        }
        srb.clear();

        for ( list<string>::iterator p = rb.begin(); p!=rb.end(); ++p) {

            try {
                type = config[*p][TYPE];
            }
            catch (Config::NoSuchProp& e) {
                continue;
            }

            try {              
                 if ( type.npos != type.rfind( "Robot")) {
                    MOND_DEBUG << "robot: " << *p << endl;                    
                    makeRobot(*p)->run();                  
                    continue;
                 } else {
                    bb.push_back( *p);
                 }
            }
            catch ( Parameter::FindError& e) {}
        }
        rb.clear();

        for ( list<string>::iterator p = bb.begin(); p!=bb.end(); ++p) {

            try {
                type = config[*p][TYPE];
            }
            catch (Config::NoSuchProp& e) {
                continue;
            }

            try {              
                 if ( type.npos != type.rfind( "Backend")) {
                    MOND_DEBUG << "Backend: " << *p << endl;                    
                    makeBackend(*p)->run();                  
                    continue;
                 } else {
//                    bb.push_back( *p);
                 }
            }
            catch ( Parameter::FindError& e) {
//                new_failures.push_back( *p);
            }

        }
        bb.clear();
        nsrb.clear();
    }

    for ( list<string>::const_iterator p = obj.begin(); p!=obj.end(); ++p) {

        try {
            type = config[*p][TYPE];
        }
        catch (Config::NoSuchProp& e) {
            continue;
        }

        if ( type.npos != type.rfind( "Robot")
          || type.npos != type.rfind( "Backend")) {
            MOND_DEBUG << "attaching parameters to " << *p << endl;
            attachBackend( config[*p]);
        }
    }
}

void fillParameters( Parameter* s, const Config::Properties& p)
{
    Parameter::Properties* const prop = new Parameter::Properties();
    s->setProperties( prop);

    try {
        prop->desc = trim( p["desc"]);
    }
    catch (Config::NoSuchProp& e) {
    }

    try {
        prop->rindex = dec2<unsigned>( p["rindex"]);
    }
    catch (Config::NoSuchProp& e) {
    }

    try {
        prop->type = dec2<unsigned>( p["valtype"]);
    }
    catch (Config::NoSuchProp& e) {
    }

    try {
        prop->format = trim( p["format"]);
    }
    catch (Config::NoSuchProp& e) {
    }

    try {
        prop->measure = trim( p["measure"]);
    }
    catch (Config::NoSuchProp& e) {
    }

    try {
                          //TODO: must be an expression
        prop->threshold = dec2<double>( p["threshold"]);
    }
    catch (Config::NoSuchProp& e) {
    }
}

Runable* Dispatcher::makeSensor( const string& name)
                        throw ( string, exception)
{
    const Config& c = config;
    string type = c[name][TYPE];

string addr;

//    string addr = c[name]["addr"];
//    if ( addr.size() > 0 ) addr = trim(addr);


    try {
        addr = trim(c[name]["addr"]);
    }
    catch (Config::NoSuchProp& e) {
        addr = "";
    }


    Sensor* s = NULL;

    if ( !networks.isThere(c[name][NET]))      //check network exists
        throw "no network: " + c[name][NET];

    Network& net = *networks[ c[name][NET]];


    if ( type == "RndSensor") {
        s = new RndSensor( name, addr, &net, read_timespec(c[name][DELAY]));
    }


    if ( NULL == s) throw "Wrong sensor type: " + type;

    fillParameters( s, c[name]);

    return s;
}

Robot* Dispatcher::makeRobot( const string& name)
                                         throw( string, runtime_error)
{
    const Config& c = config;
    string type = c[name][TYPE];
    Robot* s = NULL;

    if ( !networks.isThere(c[name][NET]))      //check network exists
        throw runtime_error( "no network: " + c[name][NET]);
    Network* net = networks[ c[name][NET]];


//    string addr="";
    string addr;
    
    try {
        addr = trim(c[name]["addr"]);
    }
    catch (Config::NoSuchProp& e) {
        addr = "";
    }

    if ( NULL == s && type == "Timeticks2TimeStringRobot") {
        s = new Timeticks2TimeStringRobot( name, addr, net);
    }

    if ( NULL == s && type == "S2SMapRobot") {
        S2SMapRobot::Map m;
        QuotedString arg,val;
        stringstream strval( c[name]["tval"]);
        stringstream strarg( c[name]["targ"]);
        while ( strarg >> arg && strval >> val)
            m[arg]=val;
        s = new S2SMapRobot( name, addr, net, m);
    }


    if ( NULL == s)
        throw runtime_error( "Wrong robot type: " + type);

    fillParameters( s, c[name]);

    backends.set( name, s);
    return s;
}

void Dispatcher::dropSensor( const string& name)
{
//need
}

Runable* Dispatcher::makeNetwork( const string& name)
//Runable* Dispatcher::makeNetwork()
                          throw ( string, runtime_error)
{

    string type = config[name][TYPE];
    Network* n = NULL;
    if ( type == "PseudoNetwork")
               n = new PseudoNetwork();

    if ( NULL == n)
               throw "Wrong network type: " + type;
    networks.set( name, n);
    return n;
}

void Dispatcher::dropNetwork( const string& name)
{
}

Runable* Dispatcher::makeBackend( const string& name)
                                          throw ( string, exception)
{

    const Config& c = config;
    string type = c[name][TYPE];
    Backend* b = NULL;

        if ( type == "LogBackend") {
        b = new LogBackend( c[name]["file"]);
        stringstream strf( config[name]["filters"]);
        string fn;
        while ( strf >> ws >> fn, strf) {
            Filter* f = NULL;
            if ( fn == "TransparentFilter") f = new TransparentFilter();

            if ( fn == "TimeThresholdFilter") f =
                        new TimeThresholdFilter( static_cast<LogBackend*>(b), read_timespec(config[name]["period"])  );

            if ( fn == "ThresholdFilter") f =
                        new ThresholdFilter( static_cast<LogBackend*>(b)  );

            if ( fn == "TimeFilter")
                f = new TimeFilter( read_timespec(config[name]["period"]));

            if ( fn == "AlarmFilter")
                f = new AlarmFilter( Parameter::get( config[name]["semaphore"]));
            if ( NULL != f)
                 static_cast<LogBackend*>(b)->addFilter(f);
        }
    }


    if ( NULL == b)
              throw "Wrong backend type: " + type;

    backends.set( name, b);
    return b;
}

void Dispatcher::attachBackend( const Config::Properties& prop)
{
    const string b_name = prop.getName();

    if ( "ALL" == prop["parameters"]) {
        list<string> dontattach_list;

        try {

        stringstream xx( prop["dontattach"]);
        string dont;
        while ( xx >> ws >> dont, xx) {
            MOND_DEBUG << "dontattaching " << dont
                       << " to " << b_name << endl;
	dontattach_list.push_back(dont);
	}
        }
        catch (Config::NoSuchProp& e){}


        for( list<Parameter*>::iterator i = Parameter::all.begin();
             i != Parameter::all.end(); i++) {

            if ( dontattach_list.end() != find( dontattach_list.begin(),
                                                dontattach_list.end(),
                                                (*i)->getName())) continue;

            MOND_DEBUG << "attaching " << (*i)->getName()
                       << " to " << b_name << endl;
            attach( (*i)->getName(), b_name);
        }
    } else {

        stringstream s( prop["parameters"]);
        string sens;
        while ( s >> ws >> sens, s) {
            MOND_DEBUG << "attaching " << sens
                       << " to " << b_name << endl;
            attach( sens, b_name);
        }
    }
}

void Dispatcher::dropBackend( const string& name)
{//TODO
}

void Dispatcher::attach( const string& sensor, const string& backend)
{
    Parameter* const s = Parameter::getByName( sensor);
    if ( NULL == s) {
        MOND_ERROR << "missed parameter: " << sensor << endl;
        return;
    }

    if ( !backends.isThere( backend)) throw "attach: no backend: " + backend;

    s->getNetwork()->attach( s, backends[backend]);
}

void Dispatcher::detach( const string& sensor, const string& backend)
{
    Parameter* s = Parameter::get( sensor);

    if ( !backends.isThere( backend)) throw "detach: no backend: " + backend;

    s->getNetwork()->detach( s, backends[backend]);
}

void Dispatcher::detach( const string& sensor)
{
    Parameter* s = Parameter::get( sensor);
    s->getNetwork()->detach( s);
}

Dispatcher::Dispatcher( const Config& c)
 : config(c)
{
    MOND_DEBUG << "Dispatcher constructor" << endl;
}

Dispatcher::~Dispatcher()
{
}

