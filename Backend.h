
#ifndef Backend_h
#define Backend_h

#include "Runable.h"
#include "Transport.h"

class Message;
class Backend
 : virtual public Runable, virtual public Transport<Message*> {

public:
    Backend() {};
    virtual ~Backend(){};
};
#endif
