//  $Id: Backend.h,v 1.9 2002/10/21 10:04:58 denis Exp $

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
