/* 28/04/2015 for Tomsk monsys_509
    msg->setAlarm( alarm);
    �� ������������� �������� alarm �� RPCSensor
*/


#include "common.h"
#include "globals.h"
#include "utils.h"

#include "MTSync.h"
#include "Parameter.h"
#include "Message.h"
#include "Message_Body.h"

#include <iomanip>

void Message::post()
{
//  MOND_DEBUG << " Post msg->getname:" <<getParameterName()<< " getDoubleValue:" << getDoubleValue()  << endl;
    source->postMessage( this);
}

string Message::serialize()
{
    stringstream buf;
    buf << setfill('0') << right << hex;
    buf << setw(2) << alarm << setw(2) << status;
    buf << setw(1) << source->getProperties()->rindex;
    buf << setw(16) << nanosec( getTime());

    try {
        buf << getTextValue();
    }
    catch ( empty& e) {
    }
//    MOND_DEBUG << "getTime(): " << getTime()<< " Str: "<< buf.str() << endl;
    return buf.str();
}

Message* Message::unserialize( Parameter* p, const string& s)
                                        throw (runtime_error, out_of_range)
{
    const unsigned short alarm = hex2<unsigned short>( s.substr( 0, 2));
    const unsigned short status = hex2<unsigned short>( s.substr( 2, 2));
    const unsigned short conf = hex2<unsigned short>( s.substr( 4, 1)); //XXX
    const long long ts = hex2<long long>( s.substr( 5, 16));
    const string s0 = trim(s.substr( 21));//XXX paranoia

    Message *msg;

    switch ( p->getProperties()->type) {
    case 0:
    case 4:
        msg = new Message( p, s0, nanosec2timespec( ts));
        break;
    case 1:
    case 5:
        msg = new Message( p, dec2<double>( s0), nanosec2timespec( ts));
        break;
    default:
        MOND_DEBUG << "valtype == " << p->getProperties()->type << endl;
        throw runtime_error("unknown valtype");
    }

// 28/04/2015 for Tomsk monsys_509
//    msg->setAlarm( alarm);
    msg->setAlarm( alarm);

    msg->setStatus( status); //XXX Parameter::setStatus() ???

    return msg;
}

void RCone::decUses() throw(runtime_error)
{
    bool del=false;

    useCountMutex.lock();

    if(0 == --useCount) del=true;

    useCountMutex.unlock();

    if(del) delete this;
}

void RCone::incUses() throw(runtime_error)
{
    useCountMutex.lock();

    if ( 0 == ++useCount)
        throw runtime_error("message use counter overflow");

    useCountMutex.unlock();
}

RCone::~RCone(){}

Parameter* Message::getSource() const
{
    return source;
}

const string& Message::getParameterName() const
{
    return source->getName();
}

const timespec& TimeStamp::getTime() const
{
    return time;
}

const string TimeStamp::getTextTime()
{
    return stringTime.getValue();
}

const string Message::getTextValue() throw( Message::empty)
{
    if ( body.get() == NULL) throw Message::empty();
    return body->toString();
}

int Message::getIntValue() throw( Message::empty)
{
    if ( body.get() == NULL) throw Message::empty();
    return body->toInt();
}

double Message::getDoubleValue() throw( Message::empty)
{
    if ( body.get() == NULL) throw Message::empty();
    return body->toDouble();
}

/*
long Message::getLongValue() throw( Message::empty)
{
    if ( body.get() == NULL) throw Message::empty();
    return body->toLong();
}
*/

bool Message::operator==( Message& rhs) throw (exception)
{
    Message& lhs = *this;
    Parameter * const lhs_s = lhs.getSource();
    Parameter * const rhs_s = rhs.getSource();

    if ( lhs_s != rhs_s)
        throw runtime_error( "attempt to compare messages from "
                             + lhs_s->getName()
                             + " and "
                             + rhs_s->getName());

    try {
//MOND_DEBUG<< "MESSAGE 000"<<endl;

        if ( lhs.getAlarm() != rhs.getAlarm()
             || lhs.getStatus() != rhs.getStatus()) {
//MOND_DEBUG<< "MESSAGE DIFFERENT ALARM!"<<endl;
          return false;
        }

        switch ( lhs_s->getProperties()->type) {

        case 0://text
        case 4://AVN 10.10.2017 text + no audio alarm
//MOND_DEBUG<< "MESSAGE TYPE 0 lhs.getTextValue():"<< lhs.getTextValue()<< "rhs.getTextValue() " <<rhs.getTextValue() <<endl;
            if ( lhs.getTextValue() == rhs.getTextValue()) return true;
            break;

        case 1://number
        case 5://AVN 10.10.2017 number + no audio alarm
//MOND_DEBUG<< "A("<< lhs.getDoubleValue()<< ") - B ("<< rhs.getDoubleValue()<< ") = ("<< fabs(lhs.getDoubleValue() - rhs.getDoubleValue())<< ")"<<endl;

            if ( fabs(lhs.getDoubleValue() - rhs.getDoubleValue())
                       < lhs_s->getProperties()->threshold) return true;
            break;

        case 3://bool
        case 7://AVN 10.10.2017 bool + no audio alarm
//MOND_DEBUG<< "MESSAGE TYPE 3"<<endl;
            return true;
        }
    }
    catch ( Message::empty& e) {
    }
//MOND_DEBUG<< "MESSAGE TYPE END"<<endl;
    return false;
}
