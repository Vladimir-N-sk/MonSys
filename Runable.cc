
#include "common.h"
#include "globals.h"

#include "Runable.h"
#include "Time.h"

#ifdef TYPEINFO
    #include <typeinfo>
#endif

Runable::Runable()
{
}

void Runable::run() throw(runtime_error)
{
    int err;
    if ( err = pthread_create( &tid, NULL, wrapper, this))
        throw runtime_error( error( err));
}

void* Runable::wrapper1( void* obj) throw()
{
    try {
        for(;;) static_cast<Runable*>(obj)->thread();
    }
    catch ( exception& e) {
        MOND_ERROR << e.what() << endl;
    }
    catch (...) {
        MOND_ERROR << "unknown exception" << endl;
    }

    MOND_ERROR
#ifdef TYPEINFO
              << typeid(*obj).name() << " "
#endif
              << "thread died" << endl;

    return NULL;
}

void* Runable::wrapper( void* obj) throw()
{
    for (;;) {
       Runable::wrapper1( obj);

       const timespec minute = { 60, 60};
       nanosleep( minute); //XXX
       MOND_ERROR << "trynig to restart" << endl;
    }
    return NULL;
}
