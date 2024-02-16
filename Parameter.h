#ifndef Parameter_h
#define Parameter_h

#include <string>
#include <list>
#include "MTmap.h"
#include <stdexcept>

typedef string Address;

const unsigned char WEDGED = 1;

class Message;
class Network;


template <typename A>
class Expression;

class AlarmList {
public:
    ~AlarmList();
    void push( unsigned, Expression<bool>*);
    unsigned examine();

private:
    typedef list< pair<unsigned, Expression<bool>*> > alarms_type;
    alarms_type alarms;

};


class Parameter {

friend class RTSensor;
friend class Message;

public:
    class Properties;

    class FindError
     : public runtime_error {
    public:
        FindError( const string& a) : runtime_error(a) {};
    private:
        FindError();
    };

    static list<Parameter*> all;

    static Parameter* get( const string&) throw (Parameter::FindError);
    static Parameter* getByName( const string&);
    static Parameter* getByAddr( const Address&);
    static Parameter* getNextByAddr( const Address&);

    Parameter( const string&, const Address&, Network*);

    virtual ~Parameter() noexcept;

    Properties* getProperties() const;
    void setProperties( Properties*);

    const string& getName() const;
    const Address& getAddr() const;

    Message* getLM() const
    {
        return lm;
    }

    Network* getNetwork() const;

    virtual runtime_error getLastReadError()
    {
        return lastReadError;
    }
    virtual void setLastReadError( runtime_error& e)
    {
        lastReadError = e;
    }
    virtual void SOK()
    {
              runtime_error("SENSOR OK");
    }
    virtual void S_BAD()
    {
        runtime_error("SENSOR BAD");
    }

    virtual unsigned short getPStatus()
    {
        return status;
    }
    virtual void setPStatus( unsigned short s)
    {
        status = s;
    }
    virtual void sendWedgedMessage();

protected:
    unsigned short status;
    runtime_error lastReadError;

private:
    Parameter();
    void setLM( Message* const); // remove last message and put new

    void postMessage( Message* const);

    typedef MTmap< string, Parameter*> pbn_type;
    typedef MTmap< Address, Parameter*> pba_type;

    static pbn_type parametersByName;
    static pba_type parametersByAddr;

    const pbn_type::Data::const_iterator name;
    const pba_type::Data::const_iterator address;
    Network* net;
    Message* lm;
    Properties* properties;
};

class Parameter::Properties {
public:
    Properties();
    string serialize() const;

    string desc;	// text
    unsigned rindex;	// resource set number TODO: dynamicaly change
    unsigned type;	// 0 - text, 1 - numeric
    string format;	// printf style format string
    string measure;	// text
    double threshold;	// see ThresholdFilter
#ifdef EXPRESSIONS
    string colors;      // indicator colour scheme:
                        // <alarm code> <delimiter> ... <alarm code>
    AlarmList alarms;
    Expression<double>* transformExpr;
    Expression<bool>* dontTrustExpr;
#endif //EXPRESSIONS
};

#endif
