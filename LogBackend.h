
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

#endif
