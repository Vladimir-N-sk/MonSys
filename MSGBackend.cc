
#include "common.h"
#include "globals.h"

#include "Message.h"
#include "Parameter.h"
#include "MSGBackend.h"
#include <ctime>

MSGBackend::store_type MSGBackend::store;
RWLock MSGBackend::store_lock;

string MSGBackend::getMessage( Parameter* sens)
{
    if ( NULL == sens) return "";

    store_lock.rdlock();

    store_type::iterator const p = store.find( sens);
    if ( store.end() == p) {
        store_lock.unlock();
        return "";
    }
    string resp;
    try {
        resp += p->second->serialize(); //XXX        
    }
    catch ( Message::empty& e) {
        store_lock.unlock();
        return "";
    }

    store_lock.unlock();
    return resp;
}

void MSGBackend::thread()
{
    Message* msg = wait();
    store_type::iterator p;

    store_lock.wrlock();

    if ( store.end() != ( p = store.find( msg->getSource()))) {
        p->second->decUses();
        p->second = msg;
    } else {
        store[msg->getSource()] = msg;
    }
    store_lock.unlock();
}

MSGBackend::MSGBackend()
{
}

MSGBackend::~MSGBackend()
{
}

