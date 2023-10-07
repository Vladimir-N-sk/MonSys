
#ifndef MTSync_h
#define MTSync_h

#include <pthread.h>
#include <stdexcept>

class Mutex {

public:
    Mutex() throw (runtime_error) : mutex( getMutex()){}
    ~Mutex() throw (runtime_error);
    void lock() throw (runtime_error);
    void unlock() throw (runtime_error);

protected:
    pthread_mutex_t mutex;

private:
    static pthread_mutex_t getMutex() throw (runtime_error);
};

class Condition
 : public Mutex {

public:
    Condition() throw (runtime_error);
    ~Condition() throw (runtime_error);
    void signal() throw (runtime_error);
    void wait() throw (runtime_error);
    class Timeout {};
    void wait( const timespec& abstime) throw (runtime_error, Timeout);

private:
    pthread_cond_t cond;
};

class RWLock {

public:
    RWLock() throw (runtime_error);
    ~RWLock() throw (runtime_error);
    void rdlock() throw (runtime_error);
    void wrlock() throw (runtime_error);
    void unlock() throw (runtime_error);

private:
    pthread_rwlock_t lock;
};
#endif
