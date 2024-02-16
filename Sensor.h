#ifndef Sensor_h
#define Sensor_h

#include "Runable.h"
#include "Parameter.h"

#include <string>

class Network;
class Message;

class Sensor
 : virtual public Runable, public Parameter {


public:
    Sensor( const string&, const Address&, Network*);
    virtual ~Sensor() noexcept {};
};

class RTSensor
 : public Sensor {

public:
    RTSensor( const string&, const Address&, Network*,
              const timespec&, const timespec&, unsigned wt = 3);
    virtual ~RTSensor() noexcept {};

protected:
    void thread();
    void body();
    virtual Message* getSensorData() throw (exception) = 0;
    const timespec timeout;

private:
    const timespec delay;
    bool firstTimeRun;
    unsigned timeouts;
    const unsigned wedgedThreshold;
};

/*************************************************************************************************************************************************************************************/
class MBSensor
 : public Sensor {

public:
    MBSensor( const string&, const Address&, Network*);

    virtual ~MBSensor()noexcept {};

protected:
    void thread();
    virtual Message* getMBData() throw (exception) = 0;

private:
};

/**********************************************************************************************************************************************************************************************/
class WTSensor
 : public Sensor {

public:
    WTSensor( const string&, const Address&, Network*,
              const timespec&, const timespec&, const unsigned&);
    virtual ~WTSensor() noexcept {};

protected:
    void thread();
    void body();
    virtual Message* getSensorData() throw (exception) = 0;
    const timespec timeout;

private:
    const timespec delay;
    bool firstTimeRun;
    unsigned timeouts;
    const unsigned wedgedThreshold;
};
#endif
