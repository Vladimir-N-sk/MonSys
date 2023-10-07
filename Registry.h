#ifndef Registry_h
#define Registry_h

//using namespace std;

#include <list>
#include <map>

#include "MTSync.h"

template <typename K, typename D>
class Registry {

public:
    list<D>* search();
    list<D>* search( const K&);
    void add( const K&, const D&);
    void remove( const K&);
    void remove( const K&, const D&);

protected:

private:
    typedef multimap<K, D> RegistryData;
    RegistryData data;
    RWLock lock;
};

template <typename K, typename D>
list<D>* Registry<K,D>::search( const K& key)
{
    list<D> *const ret = new list<D>;

    lock.rdlock();

//    const pair< RegistryData::iterator, RegistryData::iterator> r = data.equal_range(key);
     auto r = data.equal_range(key);    

    for ( typename RegistryData::iterator p = r.first; p != r.second; ++p) {
        ret->push_back(p->second);
    }

    lock.unlock();

    return ret;
}

template <typename K, typename D>
list<D>* Registry<K,D>::search()
{
    list<D> *const ret = new list<D>;

    lock.rdlock();

    for ( typename RegistryData::iterator p = data.begin(); p != data.end(); ++p) {
        ret->push_back( p->second);
    }

    lock.unlock();

    return ret;
}

template <typename K, typename D>
void Registry<K,D>::add( const K& key, const D& d)
{
    lock.wrlock();

    data.insert( typename RegistryData::value_type( key, d));

    lock.unlock();
}

template <typename K, typename D>
void Registry<K,D>::remove( const K& key, const D& d)
{
    lock.wrlock();

//    pair< RegistryData::iterator, RegistryData::iterator> r;
//    r = data.equal_range(key);
    auto r = data.equal_range(key);    
    for ( typename RegistryData::iterator p = r.first; p != r.second; ++p) {
        if ( p->second == d) {
            data.erase( p);
            break;
        }
    }

    lock.unlock();
}

template <typename K, typename D>
void Registry<K,D>::remove( const K& key)
{
    lock.wrlock();

    data.erase( key);

    lock.unlock();
}

#endif
