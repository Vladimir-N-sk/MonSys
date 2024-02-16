
#ifndef Message_Body_h
#define Message_Body_h

#include "Message.h"
#include "Resource.h"

#include <string>

template <typename V>
Message::BodyBase* Message::Body<V>::copy()
{
    return new Body(value);
}

template <typename V>
Message::Body<V>::Body(V v)
  : value( v),
    stringNum( value)
{
}

template <typename V>
string Message::Body<V>::toString()
{
    MONSYS_DEBUG << "unknown body type" << endl;
    return "";
}

template <typename V>
double Message::Body<V>::toDouble()
{
    MONSYS_DEBUG << "unknown body type" << endl;
    return 0;
}

template <typename V>
int Message::Body<V>::toInt()
{
    MONSYS_DEBUG << "unknown body type" << endl;
    return 0;
}

template <typename V>
long Message::Body<V>::toLong()
{
    MONSYS_DEBUG << " unknown body type"  << endl;
    return 0;
}

// -----------
template <>
class Message::Body<char*>
 : public Message::BodyBase {

public:
    Body( char* v);
    virtual ~Body() {};
    virtual string toString();
    virtual double toDouble();
    virtual int toInt();
    virtual long toLong();
    virtual BodyBase* copy();

private:
    Body();
    string value;
};
//---------------------------------------------------------

template <>
string Message::Body<int>::toString();

template <>
double Message::Body<int>::toDouble();

template <>
int Message::Body<int>::toInt();

template <>
long Message::Body<int>::toLong();

template <>
string Message::Body<long>::toString();

template <>
double Message::Body<long>::toDouble();

template <>
int Message::Body<long>::toInt();

template <>
long Message::Body<long>::toLong();

template <>
string Message::Body<string>::toString();

template <>
double Message::Body<double>::toDouble();

template <>
string Message::Body<double>::toString();

template <>
string Message::Body<const char*>::toString();

#endif
