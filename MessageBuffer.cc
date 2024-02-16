
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

    msg->incUses();

    Parameter* const p = msg->getSource();

    buf_type::iterator i;

    if ( buf.end() == (i = buf.find( p))) {

        buf.insert( buf_type::value_type( p, msg));

    } else {
        i->second->decUses();
        i->second = msg;
    }
}

Message* MessageBuffer::getMessage() throw(runtime_error)
{
    Message* const msg = buf.begin()->second;
    buf.erase( buf.begin());
    return msg;
}

