
#ifndef MessageBuffer_h
#define MessageBuffer_h

#include "Transport.h"
#include <map>

class Message;

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
