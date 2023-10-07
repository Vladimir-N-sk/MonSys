//  $Id: MessageBuffer.cc,v 1.15 2002/10/21 10:04:59 denis Exp $

//buggy piece of code
#include "common.h"
#include "globals.h"

#include "Message.h"
#include "MessageBuffer.h"

MessageBuffer::MessageBuffer()
{
    overwrites = 0;
}

MessageBuffer::~MessageBuffer()
{
// needed
}

bool MessageBuffer::isEmpty() const throw(runtime_error)
{
    return buf.empty();
}

void MessageBuffer::putMessage( Message* msg) throw(runtime_error)
{
MOND_DEBUG << " MessageBuffer  msg->getname:" <<msg->getParameterName()<< " msg->gettextvalue:" <<msg->getTextValue()  << endl;
    msg->incUses();

    Parameter* const p = msg->getSource();

MOND_DEBUG << " MessageBuffer  p->getname:" <<p->getName()<< endl;
    buf_type::iterator i;

    if ( buf.end() == (i = buf.find( p))) {

        buf.insert( buf_type::value_type( p, msg));

    } else {

        i->second->decUses();
        i->second = msg;
        MOND_DEBUG << "MessageBuffer owerwrites == " << ++overwrites << endl;

    }
}

Message* MessageBuffer::getMessage() throw(runtime_error)
{
    Message* const msg = buf.begin()->second;
    buf.erase( buf.begin());
    return msg;
}

