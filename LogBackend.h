
#ifndef LogBackend_h
#define LogBackend_h

#include "common.h"
#include "globals.h"
#include "Time.h"
#include "utils.h"

#include "Message.h"
#include "Backend.h"
#include "MessageQueue.h"
#include "Filtered.h"
#include "MSGBackend.h"

#include <fstream>
#include <string>
#include <ctime>
#include <sys/stat.h>
#include <vector>

class MSGBackend;

class LogBackend
 : public Backend, public Filtered<MessageQueue> {

public:
    LogBackend(const string&);
    ~LogBackend() {};
    void thread();

private:
    ofstream file;
};

class JsonBackend
 : public Backend, public Filtered<MessageQueue> {

public:
    JsonBackend(const string&, const string&);
    ~JsonBackend();
    void thread();

private:
    ofstream file;
    std::string fn, par, ParameterName, PName, s_msg, v_msg;
    std::vector<string> pn;
    int nn;
    timespec ttt;
    unsigned short alarm, status;
    unsigned long long ts;    
};



#endif
