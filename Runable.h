#ifndef Runable_h
#define Runable_h

#include <pthread.h>
#include <signal.h>

#include "utils.h"

class Runable {

public:
//    Runable::Runable();
    Runable();    

    void run() throw (runtime_error);
    void cancel() const throw (runtime_error)
    {
        int err;
        if ( err = pthread_cancel( tid))
            throw runtime_error( error( err));
    }
    void join() const throw (runtime_error)
    {
        int err;
        if ( err = pthread_join( tid, NULL))
            throw runtime_error( error( err));
    }
    void kill( int sig) const throw (runtime_error)
    {
        int err;
        if ( err = pthread_kill( tid, sig))
            throw runtime_error( error( err));
    }

protected:
    static void* wrapper( void*) throw();
    static void* wrapper1( void*) throw();
    virtual void thread()=0;

private:
    pthread_t tid;
};
#endif
