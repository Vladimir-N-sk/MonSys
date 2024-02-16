
#ifndef RndSensor_h
#define RndSensor_h

#include "Sensor.h"
#include <fstream>
#include "utils.h"
#include "Message.h"
#include "Message_Body.h"
#include "Parameter.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

class Network;
class Message;

class RndSensor
 : public RTSensor {

public:
    virtual ~RndSensor() noexcept {};
    RndSensor( const string&, const Address&, Network*, const timespec&);
    Message* getSensorData() throw(exception);
};

#endif
