
#include "common.h"
#include "globals.h"
#include "utils.h"
#include "Time.h"
#include "RndSensor.h"
#include "Message.h"
#include "Message_Body.h"
#include "Parameter.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

Message* RndSensor::getSensorData() throw(exception)
{

    double val;
    Message *m = new Message( this, val=drand48());
MONSYS_DEBUG << "RndSensor:"<< val  <<  endl;    
    return m;
}

RndSensor::RndSensor( const string& n, const Address& a,
                                    Network *m, const timespec& d)
 : RTSensor(n,a,m,ZEROTIME,d)
{
}

