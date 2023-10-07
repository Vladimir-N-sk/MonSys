//  $Id: MessageBuffer.h,v 1.8 2002/10/21 10:04:59 denis Exp $

#ifndef MessageBuffer_h
#define MessageBuffer_h

#include "Transport.h"
#include <map>

class MEssage;

class MessageBuffer : virtual Transport<Message*> {

public:
    MessageBuffer();
    virtual ~MessageBuffer();

protected:
    virtual bool isEmpty() const throw(runtime_error);
    virtual void putMessage( Message*) throw(runtime_error);
    virtual Message* getMessage() throw(runtime_error);

private:
    typedef map<Parameter*, Message*> buf_type;
    buf_type buf;
    int overwrites;
};
#endif
