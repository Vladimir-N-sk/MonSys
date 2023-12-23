
#include <sstream>
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

#include "SNMPSensor.h"
#include "SNMPNetwork.h"
#undef MAX_NAME_LEN      // buggy net-snmp


#ifdef EXPRESSIONS
    #include "Expression.h"
    #include "Parser.h"    
#endif

#include "Logger.h"
#include "RndSensor.h"
#include "PseudoNetwork.h"
#include "S2SMapRobot.h"
#include "LogBackend.h"
#include "AlarmBoolRobot.h"

#ifdef PSQL
    #include "PGBackend.h"
#endif

#include "Dispatcher.h"


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
                    MONSYS_DEBUG << "Network: " << *p << endl;                    
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
                    MONSYS_DEBUG << "Sensor: " << *p << endl;                    
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
                    MONSYS_DEBUG << "robot: " << *p << endl;                    
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
                    MONSYS_DEBUG << "Backend: " << *p << endl;                    
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
            MONSYS_DEBUG << "attaching parameters to " << *p << endl;
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

#ifdef EXPRESSIONS
    try {
        const string dts = p["donttrust"];
        prop->dontTrustExpr = Expression<bool>::make( s, dts);
    }
    catch (Config::NoSuchProp& e) {        
    }

    try {
        const string trans_string = p["convert"];
        prop->transformExpr = Expression<double>::make( s, trans_string);
    }
    catch (Config::NoSuchProp& e) {
    }
#endif //EXPRESSIONS

}

Runable* Dispatcher::makeSensor( const string& name)
                        throw ( string, exception)
{
    const Config& c = config;
    string type = c[name][TYPE];

string addr= "";

    try {
        addr = trim(c[name]["addr"]);
    }
    catch (Config::NoSuchProp& e) {
    }

    Sensor* s = NULL;

    if ( !networks.isThere(c[name][NET]))      //check network exists
        throw "no network: " + c[name][NET];

    Network& net = *networks[ c[name][NET]];

    if ( NULL == s && type == "SNMPSensor") {

        string tmp;
        unsigned SNMP_VERSION = SNMP_VERSION_1;
        try {
            SNMP_VERSION = ( dec2<unsigned>( config[name]["version"]) == 1) ? SNMP_VERSION_1:SNMP_VERSION_2c;
            MONSYS_DEBUG << "Parametr <" << name << "> set SNMP_VERSION = " << (tmp = ( dec2<unsigned>( config[name]["version"]) == 1) ? "SNMP_VERSION_1":"SNMP_VERSION_2c") << endl;
        }
        catch (Config::NoSuchProp& e) {
            MONSYS_DEBUG << "Parametr <" << name <<"> default set SNMP_VERSION = SNMP_VERSION_1" << endl;
        }


        s = new SNMPSensor( name, addr,
                            &dynamic_cast<SNMPNetwork&>( net),
                            c[name]["host"],
                            c[name]["var"], c[name]["community"], SNMP_VERSION,
          read_timespec(c[name][TIMEOUT]), read_timespec(c[name][DELAY]));
    }

    if ( NULL == s && type == "RndSensor") {
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
    string addr="";
    
    try {
        addr = trim(c[name]["addr"]);
    }
    catch (Config::NoSuchProp& e) {
//        addr = "";
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
#ifdef EXPRESSIONS
    if ( NULL == s && type == "ArithmeticRobot") {
        s = new ArithmeticRobot( name, addr, net, c[name]["expr"]);
    }
#endif

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

    if ( NULL == n && type == "SNMPNetwork")
        n = new SNMPNetwork();

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

#ifdef PSQL
        if ( NULL == b && type == "PGLBackend") {
        b = new PGLBackend( config[name]["init"]);

        stringstream strf( config[name]["filters"]);
        string fn;
        while ( strf >> ws >> fn, strf) {
            Filter* f = NULL;
            if ( fn == "TransparentFilter") f = new TransparentFilter();

            if ( fn == "TimeThresholdFilter") f =
                        new TimeThresholdFilter( static_cast<LogBackend*>(b), read_timespec(config[name]["period"])  );

            if ( fn == "ThresholdFilter") f =
                        new ThresholdFilter( static_cast<LogBackend*>(b));

            if ( fn == "TimeFilter")
                f = new TimeFilter( read_timespec(config[name]["period"]));

            if ( fn == "AlarmFilter")
                f = new AlarmFilter( Parameter::get( config[name]["semaphore"]));

            if ( NULL != f)
                static_cast<PGLBackend*>(b)->addFilter(f);
        }
    }
#endif

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
            MONSYS_DEBUG << "dontattaching " << dont
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

            MONSYS_DEBUG << "attaching " << (*i)->getName()
                       << " to " << b_name << endl;
            attach( (*i)->getName(), b_name);
        }
    } else {

        stringstream s( prop["parameters"]);
        string sens;
        while ( s >> ws >> sens, s) {
            MONSYS_DEBUG << "attaching " << sens
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
        MONSYS_ERROR << "missed parameter: " << sensor << endl;
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
    MONSYS_DEBUG << "Dispatcher constructor" << endl;
}

Dispatcher::~Dispatcher()
{
}

