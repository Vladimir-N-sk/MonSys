
#ifndef Filtered_h
#define Filtered_h

#include "Transport.h"

#include <ctime>
#include <map>
#include <vector>

class Message;
class Parameter;

class Filter {
public:
    virtual ~Filter() noexcept {};
    virtual bool examine( Message*) throw(runtime_error) = 0;
    virtual void init() = 0;
};

class ProtoFiltered {
public:
    void addFilter( Filter* const);
    virtual void transparent_putMessage( Message*) throw(runtime_error) = 0;
    void filtered_putMessage( Message*) throw(runtime_error);
    void initFilters();
//    virtual ~ProtoFiltered() noexcept {};

private:
    typedef vector<Filter*> filters_type;
    filters_type filters;
    RWLock filters_lock;
};

template <typename T>
class Filtered
 : virtual public T, virtual public ProtoFiltered {
public:
    virtual void putMessage( Message* m)  throw(runtime_error)//XXX using ?
    {
        filtered_putMessage( m);
    }

    virtual void transparent_putMessage( Message* m) throw(runtime_error)
    {
        T::putMessage( m);
    }
};

class TransparentFilter
 : public Filter {
public:
    virtual bool examine( Message*) throw();
    virtual ~TransparentFilter() noexcept {};
private:
    virtual void init(){};
};
/**************************************************************************************************************/
class TAFilter
 : public Filter {
public:
    TAFilter( const timespec&);
    virtual bool examine( Message* msg) throw(runtime_error);
    virtual ~TAFilter() noexcept {};

private:
    virtual void init();

    const timespec writePeriod;

    unsigned alarm;
    bool rez;

    typedef map< Parameter*, unsigned> Jour;
    typedef map< Parameter*, timespec> Journal;

    Journal nwTime;
    Jour nwAlarm;
    RWLock nwTime_lock;
    RWLock nwAlarm_lock;
};


/*************************************************************************************************************/
class TimeFilter
 : public Filter {
public:
    TimeFilter( const timespec&);
    virtual bool examine( Message* msg) throw(runtime_error);
    virtual ~TimeFilter() noexcept {};

private:
    virtual void init();

    const timespec writePeriod;

    typedef map< Parameter*, timespec> Journal;

    Journal nwTime;
    RWLock nwTime_lock;
};

/*
class AlarmFilter
 : public Filter {
public:
    AlarmFilter( const Parameter*);
    virtual bool examine( Message* msg) throw(runtime_error);
    virtual ~AlarmFilter() noexcept {};
private:
    virtual void init();

    const Parameter *const alarmer;
    unsigned alarm;
};
*/

class ThresholdFilter
 : public Filter {
public:
    ThresholdFilter( ProtoFiltered*);
    virtual bool examine( Message* msg) throw(runtime_error);
    virtual ~ThresholdFilter() noexcept {};

private:
    virtual void init();

    class PoolNode {
    public:
        PoolNode( Message* m);
        Message* base_msg;
        timespec last_msg_time;
    private:
        PoolNode();
    };

    typedef map< Parameter*, PoolNode> Journal;

    Journal mpool;
    RWLock mpool_lock;

    ProtoFiltered * const backend;
};



class TimeThresholdFilter
 : public Filter {
public:
    TimeThresholdFilter( ProtoFiltered*, const timespec&);
    virtual bool examine( Message* msg) throw(runtime_error);
    virtual ~TimeThresholdFilter() noexcept {};

private:
    virtual void init();
    const timespec writePeriod;
//    timespec writePeriod;

    class PoolNode {
    public:
        PoolNode( Message* m, const timespec&);
        Message* base_msg;
        timespec last_msg_time;
        timespec next_msg_time;
    private:
        PoolNode();
    };

    typedef map< Parameter*, PoolNode> Journal;

    Journal mpool;
    RWLock mpool_lock;

    ProtoFiltered * const backend;
};



#endif
