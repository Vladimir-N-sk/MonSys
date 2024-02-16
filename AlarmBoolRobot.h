#ifndef AlarmBoolRobot_h
#define AlarmBoolRobot_h

#include "Robot.h"
#include "common.h"
#include "globals.h"

#include "Message.h"
#include "Expression.h"
#include "Parser.h"

#include "Message_Body.h"
#include "MessageQueue.h"
#include "MessagePool.h"

template <typename R>
class Expression;
class Message;

class PoolRobot
 : public Robot {
public:
    PoolRobot( const string& a, const Address& b, Network* n)
     : Robot( a, b, n) {}
    virtual ~PoolRobot() noexcept {};
protected:
    virtual void automat( Message*);
    virtual void sendMessage() = 0;
    MessagePool pool;
};

class AlarmBoolRobot
 : public PoolRobot, MessageQueue {
public:
    virtual ~AlarmBoolRobot() noexcept {};
    AlarmBoolRobot( const string&, const Address&, Network*, const string&);
    void pushAlarm( unsigned, const string&, const string&)
                        throw( Parameter::FindError, runtime_error);

protected:
    virtual void sendMessage();

private:
    AlarmList alarms;
    map< unsigned, string> alarmMessages;
};


class ArithmeticRobot
 : public PoolRobot, MessageQueue {
public:
    virtual ~ArithmeticRobot() noexcept {};
    ArithmeticRobot( const string&, const Address&, Network*, const string&)
                        throw( Parameter::FindError, runtime_error);

protected:
    virtual void sendMessage();

private:
    const Expression<double>*  const expr;
};

#endif

