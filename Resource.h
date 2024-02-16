

#ifndef Resource_h
#define Resource_h

#include "MTSync.h"
#include <iomanip>
#include <memory>


template<typename A, typename B, typename C>
class Resource {

public:
    Resource( C arg) throw (runtime_error)
     : resource(NULL), argument(arg) {}

    A getValue() throw(runtime_error)
    {
        if ( resource.get() == NULL) { // pthread_once can be used XXX
            mutex.lock();
            if ( resource.get() == NULL)
                resource = shared_ptr<const A>( generate( argument));                
            mutex.unlock();
        }
        return *resource;
    }

private:
    Resource();
    Mutex mutex;
    shared_ptr<const A> resource;
    
    B generate;
    C argument;
};

class StringTimeGenerator {

public:
    const string* operator()( timespec);
};

template <typename V>
class StringNumGenerator {

public:
    const string* operator()( const V&)  throw(runtime_error);
};

template <>
const string* StringNumGenerator<int>::operator()( const int&)
                                               throw(runtime_error);

template <>
const string* StringNumGenerator<long>::operator()( const long&)
                                               throw(runtime_error);

template <>
const string* StringNumGenerator<double>::operator()( const double&)
                                               throw(runtime_error);

template <>
const string* StringNumGenerator<string>::operator()( const string&)
                                               throw(runtime_error);

#endif
