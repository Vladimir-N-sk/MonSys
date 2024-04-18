
#ifndef MSGBackend_h
#define MSGBackend_h

#include "Backend.h"
#include "MessageQueue.h"

#include <string>
#include <map>
#include <vector>

#include "MTSync.h"

class Parameter;

class MSGBackend
 : public Backend, MessageQueue {

public:
    static string getMessage( Parameter*);    

    MSGBackend();
    ~MSGBackend();
    void thread();

private:
    typedef map< Parameter*, Message*> store_type;
    static store_type store;
    static RWLock store_lock;
};

#endif
