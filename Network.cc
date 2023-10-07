//  $Id: Network.cc,v 1.16 2003/01/09 04:10:15 denis Exp $

#include "common.h"
#include "globals.h"

#include "Message.h"
#include "Network.h"

/* AVN define in network.h
Network::Network()
{
}

Network::~Network()
{
}
*/
void Network::attach( Parameter *s, Backend *b)
{
    listeners.add( s, b);
}

void Network::detach( Parameter *s, Backend *b)
{
    listeners.remove( s, b);
}


void Network::detach( Parameter *s)
{
    listeners.remove( s);
}

void Network::thread()
{
    Message *const msg = wait();

    list< Backend*> *const pt = listeners.search( msg->getSource());

    for_each( pt->begin(), pt->end(),
              bind2nd( mem_fun( &Transport<Message*>::send), msg));

    delete pt;
    msg->decUses();
}
