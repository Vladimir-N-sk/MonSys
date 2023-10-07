//  $Id: Resource.cc,v 1.13 2002/05/18 07:49:51 denis Exp $

#include "common.h"
#include "globals.h"
#include "utils.h"

#include "Resource.h"
#include "Time.h"

#include <sstream>
#include <string>


const string* StringTimeGenerator::operator()( timespec t)
{
    return new string( timespec2isotime( t));
}

template<>
const string* StringNumGenerator<int>::operator()( const int& value)
                                               throw(runtime_error)
{
    stringstream msgbuf;
    msgbuf << value;
    return new string(msgbuf.str());
}

template<>
const string* StringNumGenerator<long>::operator()( const long& value)
                                               throw(runtime_error)
{
    stringstream msgbuf;
    msgbuf << value;
    return new string(msgbuf.str());
}

template<>
const string* StringNumGenerator<double>::operator()( const double& value)
                                               throw(runtime_error)
{
    stringstream msgbuf;
//    msgbuf << value;
    msgbuf  << setprecision(7) << value;
    return new string(msgbuf.str());
}

template<>
const string* StringNumGenerator<string>::operator()( const string& value)
                                               throw(runtime_error)
{
    throw runtime_error( "there is something wrong in Message bodies");
}
