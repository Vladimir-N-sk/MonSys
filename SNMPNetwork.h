
#ifndef SNMPNetwork_h
#define SNMPNetwork_h

#include "Network.h"
#include "MessageQueue.h"

class SNMPNetwork
 : public Network, MessageQueue {

public:
    SNMPNetwork();
    virtual ~SNMPNetwork();

private:
    static bool init;
};

#endif
