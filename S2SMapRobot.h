
#ifndef S2SMapRobot_h
#define S2SMapRobot_h

#include "Robot.h"
#include "MessageQueue.h"
#include "Parameter.h"
#include "utils.h"

#include <string>
#include <map>

class Message;

class S2SMapRobot
 : public Robot, MessageQueue {

public:
    typedef map< string, string> Map;
    S2SMapRobot( const string&, const Address&, Network*, const Map&);
    virtual ~S2SMapRobot() noexcept {};

protected:
    virtual void automat(Message*);

private:
    const Map i2smap;
};

class D2DMapRobot
 : public Robot, MessageQueue {

public:
    typedef map< string, string> Map;
    D2DMapRobot( const string&, const Address&, Network*, const Map&);
    virtual ~D2DMapRobot() noexcept {};

protected:
    virtual void automat(Message*);

private:
    const Map d2dmap;
};


class Timeticks2TimeStringRobot
 : public Robot, MessageQueue {

public:
    Timeticks2TimeStringRobot( const string&, const Address&, Network*);
    virtual ~Timeticks2TimeStringRobot() noexcept {};

protected:
    virtual void automat(Message*);

};


#endif
