#include "common.h"
#include "globals.h"

#include "Parameter.h"
#include "Message.h"
#include "Message_Body.h"
#include "Transport.h"
#include "Network.h"
#ifdef EXPRESSIONS
    #include "Expression.h"
#endif

#include <sstream>

list<Parameter*> Parameter::all;
Parameter::pbn_type Parameter::parametersByName;
Parameter::pba_type Parameter::parametersByAddr;

Parameter::Parameter( const string& n, const Address& a, Network* q)
 : name( parametersByName.insert( n, this)),//XXX
   address( parametersByAddr.insert( a, this)),
   net( q),
   lm( NULL),
   status( 0),
   lastReadError( runtime_error( "SENSOR_BAD"))
{
   all.push_front(this);
}

Parameter::~Parameter()
{
    all.erase( find( all.begin(), all.end(), this));
    parametersByName.erase( name->first);
    parametersByAddr.erase( address->first);
}

Parameter::Properties::Properties()
 : rindex(0),
   type(0),
   format("%f"),
   measure(""),
   threshold(0)
#ifdef EXPRESSIONS
   ,
   transformExpr(NULL),
   dontTrustExpr(NULL)
#endif //EXPRESSIONS
{
}

Parameter* Parameter::get( const string& name) throw ( Parameter::FindError)
{
    Parameter *p = parametersByName[name];
    if ( NULL == p)
        throw FindError( "there is no parameter: " + name);
    return p;
}

Parameter* Parameter::getByName( const string& a)
{
    return parametersByName[a];
}

Parameter* Parameter::getByAddr( const Address& a)
{
    return parametersByAddr[a];
}

Parameter* Parameter::getNextByAddr( const Address& a)
{
    return parametersByAddr.getNext(a);
}

string Parameter::Properties::serialize() const
{
    stringstream buf;
    buf << "type=" << type << endl;
    buf << "format=" << format << endl;
    buf << "measure=" << measure << endl;
    buf << scientific;
    buf << "threshold=" << threshold << endl;
#ifdef EXPRESSIONS
    buf << "colors_0=" << colors << endl;
#endif //EXPRESSIONS
    return buf.str();
}

void Parameter::postMessage( Message* const msg)
{
#ifdef EXPRESSIONS
  {
    Message* const old_message = lm;
    lm = msg; //XXX dirty trick

    if ( NULL != properties->dontTrustExpr ) {        
        setLM( old_message);
        return;
    } 


    lm = old_message;
  }
#endif //EXPRESSIONS

    setLM( msg);

#ifdef EXPRESSIONS

    if ( NULL != properties->transformExpr)
        setLM( new Message( this, properties->transformExpr->evaluate()));

    unsigned a;
    if ( MAXALARM != (a = properties->alarms.examine()))
    getLM()->setAlarm( a);

    getLM()->setStatus( getLM()->getStatus() | status);
#endif //EXPRESSIONS

    net->send( getLM());
}

void Parameter::sendWedgedMessage()
{

    Message *tmpm = new Message( this);
    tmpm->setStatus( tmpm->getStatus() | status);
    setLM( tmpm);
    net->send( tmpm);
}

Network* Parameter::getNetwork() const
{
    return net;
}

const Address& Parameter::getAddr() const
{
    return address->first;
}

const string& Parameter::getName() const
{
    return name->first;
}

Parameter::Properties* Parameter::getProperties() const
{
    return properties;
}

void Parameter::setProperties( Properties* f)
{
    properties = f;
}

void Parameter::setLM( Message* const msg) // remove last message and put new
{
    if ( NULL != lm) lm->decUses();
    lm = msg;
}

#ifdef EXPRESSIONS
AlarmList::~AlarmList()
{
    for ( alarms_type::iterator p = alarms.begin(); alarms.end() != p; p++) {
            delete p->second;
    }
}

void AlarmList::push( unsigned i, Expression<bool>* e)
{
    alarms.push_back( pair<unsigned, Expression<bool>*>( i, e));
}

unsigned AlarmList::examine()
{
    for ( alarms_type::iterator a = alarms.begin(); a != alarms.end(); a++) {
        if ( a->second->evaluate()) {
            return a->first;
        }
    }
    return MAXALARM;
}
#endif //EXPRESSIONS
