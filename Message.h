//  $Id: Message.h,v 1.47 2003/01/09 11:32:17 denis Exp $

#ifndef Message_h
#define Message_h

#include "Time.h"
#include "Resource.h"
#include "Parameter.h"
#include "common.h"
#include "globals.h"

#include <memory>

class RCone {
public:
    RCone() : useCount( 1){}
    virtual ~RCone();

    void incUses() throw(runtime_error);
    void decUses() throw(runtime_error);

private:
    unsigned useCount;
    Mutex useCountMutex;
};

class TimeStamp {
public:
    TimeStamp()
    : time( now()),
      stringTime( time) {}

    TimeStamp( const timespec& t)
    : time( t),
      stringTime( time) {}

    const timespec& getTime() const;
    const string getTextTime();

private:
    const timespec time;
    Resource<string, StringTimeGenerator, const timespec&> stringTime;
};

class Message
 : public RCone, public TimeStamp {
public:
    class empty {}; //exception

    class BodyBase {
    public:
//AVN
        virtual ~BodyBase() noexcept {};
        virtual string toString() = 0;
        virtual double toDouble() = 0;
        virtual int toInt() = 0;
        virtual long toLong() = 0;
        virtual BodyBase* copy() = 0;
    };

    template <typename V>
    class Body
     : public BodyBase {

    public:
        Body( V v);
        virtual ~Body() noexcept {};
        virtual string toString();
        virtual double toDouble();
        virtual int toInt();
        virtual long toLong();
        virtual BodyBase* copy();

    private:
        Body();
        V value;
        Resource<string, StringNumGenerator<V>, V&> stringNum;
    };

    Message( Parameter* s)
    : source( s),
      alarm( UNKNOWN),
      status(0),
      body( NULL)
    {
    }

    template <typename B>
    Message( Parameter* s, B v)
    : source( s),
      alarm( NOALARM),
      status(0),
      body( new Body<B>(v))
    {
    }

    template <typename B>
    Message( Parameter* s, B v, const timespec& t)
    : TimeStamp( t),
      source( s),
      alarm( NOALARM),
      status(0),
      body( new Body<B>(v))
    {
    }

    Message( Message* m, const timespec& t)
    : TimeStamp( t),
      source( m->source),
      alarm( m->alarm),
      status( m->status),
      body( m->body->copy())
    {
    }

    void post();

    bool operator==( Message&) throw ( exception);

    string serialize();
    static Message* unserialize( Parameter*, const string&)
                                  throw (runtime_error, out_of_range);

    const string& getParameterName() const;
    Parameter* getSource() const;

    const string getTextValue() throw( empty);
    double getDoubleValue() throw( empty);
//    long getLongValue() throw( empty);
    int getIntValue() throw( empty);

    unsigned short getAlarm() const throw( empty)
    {
        if ( body.get() == NULL) throw Message::empty();
        return alarm;
    }

    void setAlarm( unsigned short i)
    {
        if ( i >= MAXALARM)
            throw runtime_error( source->getName() + ": alarm overflow");
//            MOND_ERROR << source->getName() << ": alarm overflow" << endl;
        alarm = i;
    }

    unsigned short getStatus() const
    {
        return status;
    }

    void setStatus( unsigned short i)
    {
        if ( i > 0xff)
            throw runtime_error( source->getName() + ": status overflow");
//            MOND_ERROR << source->getName() << ": status overflow" << endl;
        status = i;
    }

private:
    Message();

    Parameter * const source;

    unsigned short alarm;
    unsigned short status;

//AVN    const auto_ptr<BodyBase> body;
//    const std::unique_ptr<BodyBase> body;    
    const std::shared_ptr<BodyBase> body;        
//AVN    template <typename T> class Body;
};
#endif
