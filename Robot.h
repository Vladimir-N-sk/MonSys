#ifndef Robot_h
#define Robot_h

#include "Backend.h"
#include "Sensor.h"

class Robot
 : public Backend, public Sensor {        

public:
    Robot( const string&, const Address&, Network*);
    virtual ~Robot() noexcept {};
    

protected:
    virtual void thread();
    virtual void automat(Message*)=0;
};

#endif
