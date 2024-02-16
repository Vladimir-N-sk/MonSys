
#include "common.h"
#include "MTSync.h"
#include "utils.h"

#include <cerrno>


pthread_mutex_t Mutex::getMutex() throw (runtime_error)
{
    int err;
    pthread_mutex_t m;
    if ( err = pthread_mutex_init( &m, NULL))
        throw runtime_error( error( err));
    return m;
}

Mutex::~Mutex() throw (runtime_error)
{
    int err;
    if ( err = pthread_mutex_destroy( &mutex))
        throw runtime_error( error( err));
}

void Mutex::lock() throw (runtime_error)
{
    int err;
    if ( err = pthread_mutex_lock( &mutex))
        throw runtime_error( error( err));
}

void Mutex::unlock() throw (runtime_error)
{
    int err;
    if ( err = pthread_mutex_unlock( &mutex))
        throw runtime_error( error( err));
}

Condition::Condition() throw (runtime_error)
{
    int err;
    if(err = pthread_cond_init( &cond, NULL)) throw runtime_error( error( err));
}

Condition::~Condition() throw (runtime_error)
{
    int err;
    if(err = pthread_cond_destroy( &cond)) throw runtime_error( error( err));
}

void Condition::wait( const timespec& abstime)
                                throw (runtime_error, Condition::Timeout)
{
    int err;
    if ( ETIMEDOUT ==
               (err = pthread_cond_timedwait( &cond, &mutex, &abstime))) {
        unlock();
        throw Condition::Timeout();
    }
    if ( err) throw runtime_error( error( err));
}

void Condition::wait() throw (runtime_error)
{
    int err;
    if(err = pthread_cond_wait( &cond, &mutex)) throw runtime_error( error( err));
}

void Condition::signal() throw (runtime_error)
{
    int err;
    if(err = pthread_cond_signal( &cond)) throw runtime_error( error( err));
}

RWLock::RWLock()  throw (runtime_error)
{
    int err;
    if(err = pthread_rwlock_init( &lock, NULL)) throw runtime_error( error( err));
}

RWLock::~RWLock()  throw (runtime_error)
{
    int err;
    if(err = pthread_rwlock_destroy( &lock)) throw runtime_error( error( err));
}

void RWLock::rdlock() throw (runtime_error)
{
    int err;
    if (err = pthread_rwlock_rdlock( &lock)) throw runtime_error( error( err));
}

void RWLock::wrlock() throw (runtime_error)
{
    int err;
    if (err = pthread_rwlock_wrlock( &lock)) throw runtime_error( error( err));
}

void RWLock::unlock() throw (runtime_error)
{
    int err;
    if (err = pthread_rwlock_unlock( &lock)) throw runtime_error( error( err));
}

