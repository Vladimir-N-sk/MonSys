//  $Id: Parameter.cc,v 1.48 2003/01/10 07:00:10 denis Exp $

#include "common.h"
#include "globals.h"

#include "Parameter.h"
#include "Message.h"
#include "Message_Body.h"
#include "Transport.h"
#include "Network.h"
#ifdef EXPRESSION
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
//    MOND_DEBUG << name << " added to parameters" << endl;
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

//MOND_DEBUG << " Parameter msg->getname:" <<msg->getParameterName() << " lm:" <<lm->getDoubleValue() << " msg:" <<msg->getDoubleValue()  << endl;

    if ( NULL != properties->dontTrustExpr
           && properties->dontTrustExpr->evaluate()) {
/*
        if ( NULL != old_message)  {
MOND_DEBUG << " Parameter msg->getname:" <<msg->getParameterName()<< " SET old msg:" <<old_message->getDoubleValue() << " new msg:" <<msg->getDoubleValue()  << endl;
        } else {
MOND_DEBUG << " Parameter msg->getname:" <<msg->getParameterName()<< " SET old msg NULL new msg:" <<msg->getDoubleValue()  << endl;
        }
*/
        setLM( old_message);

/*        if ( NULL != old_message)  {
MOND_DEBUG << " Parameter msg->getname:" <<msg->getParameterName()<< " RETURN SET old msg:" <<old_message->getDoubleValue() << " LM msg:" <<lm->getDoubleValue()  << endl;
        }
*/
        return;
    }

    lm = old_message;
/*        if ( NULL != old_message)  {
MOND_DEBUG << " Parameter msg->getname:" <<msg->getParameterName()<< " AFTER RETURN old msg:" <<old_message->getDoubleValue() << " LM msg:" <<lm->getDoubleValue()  << endl;
        } else {
MOND_DEBUG << " Parameter msg->getname:" <<msg->getParameterName()<< " AFTER RETURN old_msg NULL"   << endl;
        }
*/

  }
#endif //EXPRESSIONS

    setLM( msg);

//    MOND_DEBUG << " Parameter msg->getname:" <<msg->getParameterName()<< " AFTER setLM msg:" <<msg->getDoubleValue() << " LM msg:" <<lm->getDoubleValue()  << endl;


//XXX the rest must be separate function
//private void postLM()
#ifdef EXPRESSIONS

    if ( NULL != properties->transformExpr)
        setLM( new Message( this, properties->transformExpr->evaluate()));

// put alarms and Parameter status
    unsigned a;
    if ( MAXALARM != (a = properties->alarms.examine()))
        getLM()->setAlarm( a);

    getLM()->setStatus( getLM()->getStatus() | status);
#endif //EXPRESSIONS
//MOND_DEBUG << " Parameter :" <<getLM()->getParameterName()<< " getValue():" <<getLM()->getTextValue()<< " getAlarm:" <<getLM()->getAlarm() << " getstatus:" <<getLM()->getStatus() << endl;
    net->send( getLM());
}

void Parameter::sendWedgedMessage()
{

/*-----------------07.05.2008 12:26-----------------
Message *tmpm = NULL != lm
//XXX this leads to several message records with the same timestamp
                               ? new Message( lm, lm->getTime())
                               : new Message( this);
 * --------------------------------------------------*/

    Message *tmpm = new Message( this);
//MOND_DEBUG << " Parameter sendwedged 1 tmpm:" <<tmpm->getParameterName()<< " tmpm->gettextvalue:" <<tmpm->getTextValue()  << endl;
    tmpm->setStatus( tmpm->getStatus() | status);
//MOND_DEBUG << " Parameter sendwedged 2 tmpm:" <<tmpm->getParameterName()<< " tmpm->gettextvalue:" <<tmpm->getTextValue()  << endl;
    setLM( tmpm);
//MOND_DEBUG << " Parameter sendwedged 2 tmpm:" <<tmpm->getParameterName()<< " tmpm->gettextvalue:" <<tmpm->getTextValue()  << endl;
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
//   MOND_DEBUG  << "AlarmList::push i:"<< i << endl;
}

unsigned AlarmList::examine()
{
    for ( alarms_type::iterator a = alarms.begin(); a != alarms.end(); a++) {
        if ( a->second->evaluate()) {
//MOND_DEBUG  << "3 AlarmList::examine a->second->evaluate():"<< a->second->evaluate()<< " return :"<< a->first<< endl;
            return a->first;
        }
    }
//MOND_DEBUG  << "4 AlarmList::examine: return MAXALARM"<< endl;
    return MAXALARM;
}
#endif //EXPRESSIONS
