//  $Id: Runable.cc,v 1.20 2003/02/21 06:34:49 denis Exp $

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

//AVN 18.02.2010       MOND_ERROR << "sleep for minute" << endl;
       const timespec minute = { 60, 60};
       nanosleep( minute); //XXX

       MOND_ERROR << "trynig to restart" << endl;
//MOND_DEBUG << this->getName() << "trynig to restart" << n <<endl;
//       exit(-1);
    }
    return NULL;
}
