
#ifndef MessageQueue_h
#define MessageQueue_h

#include "Transport.h"

class Message;

class MessageQueue
 : virtual public QueueTransport<Message*> {

public:
    MessageQueue(){};
    virtual ~MessageQueue(){};

    virtual void putMessage( Message*) throw(runtime_error);
};

class BHMessageQueue
 : public MessageQueue {
public:
    BHMessageQueue( bool mode = true)
     : black_hole( mode){}
    virtual void putMessage( Message*) throw(runtime_error);

protected:
    void setBHMode( bool m)
    {
        black_hole = m;
    }

private:
    bool black_hole;
};
#endif
