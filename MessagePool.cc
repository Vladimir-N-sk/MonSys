// $Id: MessagePool.cc,v 1.8 2002/10/23 05:04:52 denis Exp $

#include "common.h"
#include "globals.h"
#include "MessagePool.h"
#include "Message.h"
#include "Time.h"

#include <algorithm>
#include <numeric>

timespec MessagePool::center()
{
    return avgTime;
}

timespec MessagePool::diameter()
{
    return maxTime - minTime;
}

unsigned short MessagePool::or_status::operator()( unsigned short a,
                                                      const value_type& e)
{
    return a | e.second->getStatus();
}

unsigned short MessagePool::status() //TODO optimization
{
    return accumulate( begin(), end(), 0, or_status());
}

void MessagePool::put( Message* m)
{
    m->incUses();

    const iterator p = find( m->getSource());
    const timespec n = m->getTime();
    if ( end() != p) {
        const timespec t = p->second->getTime();

        avgTime += ( n - t) / size();

        p->second->decUses();
        p->second = m;

        maxTime = maxTime < n ?
               n : max_element( begin(), end(), less())->second->getTime();
        minTime = minTime > n ?
               n : min_element( begin(), end(), less())->second->getTime();
    } else {
        const double k = 1.0 / ( size() + 1);
        avgTime = avgTime * ( size() * k) + n * k;

        insert( value_type( m->getSource(), m));

        maxTime = max( n, maxTime);
        minTime = min( n, minTime);
    }
}

MessagePool::MessagePool()
 : avgTime(ZEROTIME),
   maxTime(ZEROTIME),
   minTime(ZEROTIME)
{
}

bool MessagePool::less::operator()( const value_type& a, const value_type& b)
{
    return a.second->getTime() < b.second->getTime();
}
