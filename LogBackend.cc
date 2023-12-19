
#include "common.h"
#include "globals.h"
#include "Time.h"
#include "utils.h"

#include "Message.h"
#include "LogBackend.h"

#include "SocketNetwork.h"


const string errmsg = "LogBackend: not open file ";
const string badmsg = "LogBackend: bad get Alarm";

LogBackend::LogBackend( const string& fName)
{
    file.open( fName.c_str(), ios::out|ios::app);
    if( !file) throw string(errmsg+fName.c_str());
    MOND_DEBUG << "log to: " << fName << endl;
}

LogBackend::~LogBackend()
{
//    stop();
}

void LogBackend::thread()
{
    Message* const msg = wait();

    if( !file) throw errmsg;

    try {
        file << msg->getParameterName() << "\tat ";
        file << msg->getTextTime() << " : ";
        file << msg->getAlarm() << " : ";
        file << msg->getStatus() << " :";
        file << msg->getTextValue();
    }
    catch ( Message::empty& e) {
    }
    file << endl;

    msg->decUses();
}
