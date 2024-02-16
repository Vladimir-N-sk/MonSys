#ifndef UTILS_H
#define UTILS_H

#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cerrno>
#include <ctime>
#include <string>

#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>
#include <math.h>

using namespace std;

void initRND();
void procSignals();
void blockSignals();
void daemonize();
void startup( int, char**);
void sig_chld(int);


template <typename T>
T dec2( const std::string& s)
{
    T val;
    std::stringstream tmp( s);
    if ( !( tmp >> val) || !tmp.eof())
        throw std::runtime_error("bad number: " + s);
    return val;
}

template <typename T>
T hex2( const std::string& s)
{
    T val;
    stringstream tmp( s);
    if ( !( tmp >> hex >> val) || !tmp.eof())
        throw runtime_error("bad number: " +s);
    return val;
}

void nanosleep( const timespec& );

bool BEcheckCRC( const std::string&);
std::string BEcrc( const int&);
unsigned short word2short( const unsigned char&, const unsigned char& );
std::string delspace( const std::string&);
std::string hex2str( const std::string&);
std::string int2str( const int& , const int& , const char&);
std::string int2str( const int&);
std::string ush2str( const unsigned short&, const int&, const char&);
std::string ush2str( const unsigned short&);
std::string uch2str( const unsigned char&);
std::string double2str( const double&);
std::string float2str( const float&);
std::string sec2timesec( const double&);
std::string int2hex( const int&, const unsigned&);
std::string short2hex( const short&, const unsigned&);
std::string long2hex( const long&, const unsigned&);
std::string uns2hex( const unsigned short&, const char& );
std::string ch2hex( const unsigned char&, const unsigned&);
std::string str2hex( const std::string&);
std::string str2hex( const std::string&, const std::string&);
std::string str2ascii( const std::string&);
std::string trim( const std::string&);
std::string cut( unsigned, const std::string&, const std::string&);
std::string error();
std::string error(int);
ostream& error( ostream&);

class QuotedString
 : public string {
public:
    using string::operator=;
};

istream& operator>>( istream&, QuotedString&);

inline timeval to_timeval( const timespec& t)
{
    timeval timv;
    timv.tv_sec = t.tv_sec;
    timv.tv_usec = t.tv_nsec/1000;
    return timv;
}
#endif
