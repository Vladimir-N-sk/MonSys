
#include "common.h"

#include "Logger.h"

#include <syslog.h>
#include <cstdio>
#include <cerrno>

Logger::Logger( int level)
 : ostream( new Loggerbuf(level))
{
}

Logger::~Logger()
{
    delete rdbuf();
}

int Loggerbuf::sync()
{

    if ( !opend) { //XXX MT Safe?
        openlog( ident.data(), logopt, facility);
        opend = true;
    }

if (level != LOG_WARNING) {
    syslog( level, "%s", str().data());
} else {
      syslog( LOG_WARNING|LOG_LOCAL3, "%s", str().data());
};


    str("");
    return 0;
}

const int Loggerbuf::logopt = LOG_PID | LOG_CONS | LOG_NDELAY;
const int Loggerbuf::facility = LOG_USER;
int Loggerbuf::logs = 0;

Loggerbuf::Loggerbuf( int l)
 : stringbuf(ios_base::out),
   level( l),
   opend( false)
{
    logs++;
}

Loggerbuf::~Loggerbuf()
{
    if ( --logs == 0) closelog();
}
