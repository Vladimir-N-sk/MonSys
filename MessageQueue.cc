
#include "common.h"
#include "globals.h"

#include "Message.h"
#include "MessageQueue.h"

void MessageQueue::putMessage(Message *message) throw(runtime_error)
{
//MONSYS_DEBUG << " MessageQueue  pered incUses:"<< message<<" msg->getname:" <<message->getParameterName()<< " msg->gettextvalue:" <<message->getTextValue()  << endl;
    message->incUses();
    QueueTransport::putMessage( message);
//MONSYS_DEBUG << " MessageQueue  posle putMessage:"<< message<<" msg->getname:" <<message->getParameterName()<< " msg->gettextvalue:" <<message->getTextValue()  << endl;
}

void BHMessageQueue::putMessage( Message* m) throw(runtime_error)
{
//MONSYS_DEBUG << " B!H!MessageQueue  message"<< m<<" msg->getname:" <<m->getParameterName()<< " msg->gettextvalue:" <<m->getTextValue()  << endl;
    if ( !black_hole) MessageQueue::putMessage( m);
}
