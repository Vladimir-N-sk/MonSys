//  $Id: Message_Body.cc,v 1.17 2002/12/16 06:15:59 denis Exp $

#include "common.h"
#include "globals.h"

#include "Message_Body.h"

// specialization for regarding char* as string -----------
Message::Body<char*>::Body( char* v)
 : value(v)
{
}

Message::BodyBase* Message::Body<char*>::copy()
{
    return new Body<string>(value);
}

string Message::Body<char*>::toString()
{
    return value;
}

double Message::Body<char*>::toDouble()
{
    return 0;
}

long Message::Body<char*>::toLong()
{
    return 0;
}

int Message::Body<char*>::toInt()
{
    return 0;
}
//----------------------------------------------------------
template<>
string Message::Body<int>::toString()
{
    return stringNum.getValue();
}

template<>
double Message::Body<int>::toDouble()
{
    return value;
}

template<>
int Message::Body<int>::toInt()
{
    return value;
}

template<>
long Message::Body<int>::toLong()
{
    return value;
}

template<>
string Message::Body<long>::toString()
{
    return stringNum.getValue();
}

template<>
double Message::Body<long>::toDouble()
{
    return value;
}

template<>
int Message::Body<long>::toInt()
{
    return value;
}

template<>
long Message::Body<long>::toLong()
{
    return value;
}

template<>
string Message::Body<string>::toString()
{
    return value;
}

template<>
double Message::Body<double>::toDouble()
{
    return value;
}

template<>
string Message::Body<double>::toString()
{
    return stringNum.getValue();
}

template<>
string Message::Body<const char *>::toString()
{
    return value;
}

/*  AVN to *.h
Message::BodyBase::~BodyBase()
{
}

Message::Body<char*>::~Body()
{
}
*/
