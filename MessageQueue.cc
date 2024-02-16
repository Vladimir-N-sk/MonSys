
#include "common.h"
#include "globals.h"

#include "Message.h"
#include "MessageQueue.h"

void MessageQueue::putMessage(Message *message) throw(runtime_error)
{
    message->incUses();
    QueueTransport::putMessage( message);
}

void BHMessageQueue::putMessage( Message* m) throw(runtime_error)
{
    if ( !black_hole) MessageQueue::putMessage( m);
}
