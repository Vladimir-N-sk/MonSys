#ifndef Transport_h
#define Transport_h

#include <list>
#include "MTSync.h"


template <typename MESSAGE>
class Transport {

public:

    virtual ~Transport(){};

    void send(MESSAGE message) throw(runtime_error)
    {
        notempty.lock();

        if( isEmpty()) notempty.signal();
        putMessage( message);
        notempty.unlock();
    }

    MESSAGE wait() throw (runtime_error)
    {
        notempty.lock();
        while( isEmpty()) notempty.wait();
        MESSAGE const m = getMessage();
        notempty.unlock();

        return m;
    }

    MESSAGE wait( const timespec& abstime)
                          throw(runtime_error, Condition::Timeout)
    {
        notempty.lock();

        while( isEmpty()) notempty.wait( abstime);

        MESSAGE m = getMessage();

        notempty.unlock();

        return m;
    }

protected:
    virtual void putMessage( MESSAGE) throw(runtime_error) =0;
    virtual bool isEmpty() const throw(runtime_error) =0;
    virtual MESSAGE getMessage() throw(runtime_error) =0;

private:
    Condition notempty;
};

template <typename MESSAGE>
class QueueTransport
 : virtual public Transport<MESSAGE> {

public:
    QueueTransport(){};
    virtual ~QueueTransport(){};

    virtual void putMessage(MESSAGE message) throw(runtime_error)
    {
        queue.push_back( message);
    }

protected:
    virtual bool isEmpty() const throw(runtime_error)
    {
        return queue.empty();
    }

    virtual MESSAGE getMessage() throw(runtime_error)
    {
        MESSAGE const m = queue.front();
        queue.pop_front();
        return m;
    }

private:
    list<MESSAGE> queue;
};
#endif
