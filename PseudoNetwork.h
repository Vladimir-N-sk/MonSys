#ifndef PseudoNetwork_h
#define PseudoNetwork_h

#include "Network.h"
#include "MessageQueue.h"

class PseudoNetwork : public Network, MessageQueue {

public:
    PseudoNetwork();
    virtual ~PseudoNetwork();

};

#endif
