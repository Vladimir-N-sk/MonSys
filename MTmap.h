
#ifndef MTmap_h
#define MTmap_h

#include "MTSync.h"

#include <map>

//using namespace std;

template <typename K, typename D>
class MTmap {

public:
    typedef map<K, D> Data;

    bool isThere( const K&) const;
    typename Data::iterator insert( const K& key, const D& d)
    {
        lock.wrlock();
        typename Data::iterator ret = data.insert( data.begin(), typename Data::value_type(key, d));
        lock.unlock();
        return ret;
    }

    size_t erase( const K& key)
    {
        lock.wrlock();
        size_t ret = data.erase( key);
        lock.unlock();
        return ret;
    }

    void set( const K&, const D&);
    D getNext( const K&) const;
    D operator[]( const K&) const;

private:
    Data data;
    mutable RWLock lock;
};

template <typename K, typename D>
void MTmap<K,D>::set( const K& key, const D& d)
{
    lock.wrlock();
    data[key] = d;
    lock.unlock();
}

template <typename K, typename D>
D MTmap<K,D>::operator[]( const K& key) const
{
    D ret = NULL;

    lock.rdlock();

    typename Data::const_iterator const p = data.find( key);
    if ( data.end() != p) ret = p->second;

    lock.unlock();

    return ret;
}

template <typename K, typename D>
D MTmap<K,D>::getNext( const K& key) const
{
    D ret = NULL;

    lock.rdlock();

    typename Data::const_iterator p = data.find( key);
    if ( data.end() != p && data.end() != ++p) ret = p->second;

    lock.unlock();

    return ret;
}

template <typename K, typename D>
bool MTmap<K,D>::isThere( const K& key) const
{
    bool ret;

    lock.rdlock();

    if ( data.end() == data.find( key)) {
        ret = false;
    } else {
        ret = true;
    }

    lock.unlock();

    return ret;
}

#endif
