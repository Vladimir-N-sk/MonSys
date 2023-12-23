

#ifndef LogBackend_h
#define LogBackend_h

#include "Backend.h"
#include "MessageQueue.h"
#include "Filtered.h"

#include <fstream>
#include <string>
#include <ctime>

class LogBackend
 : public Backend, public Filtered<MessageQueue> {

public:
    LogBackend(const string&);
    ~LogBackend();
    void thread();

private:
    ofstream file;
};


/* AVN временно 05/10/2023
class LogSyslogBackend
 : public Backend, public Filtered<MessageQueue> {

public:
    LogSyslogBackend();
    ~LogSyslogBackend();
    void thread();

};


class SocketNetwork;

class LogSocketBackend
 : public Backend, public Filtered<MessageQueue> {

public:
    LogSocketBackend( SocketNetwork*);
    ~LogSocketBackend();
    void thread();

private:
    SocketNetwork* net;
};
*/

#endif
