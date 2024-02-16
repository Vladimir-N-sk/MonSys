
#include "common.h"
#include "globals.h"

#include "Message.h"
#include "Robot.h"

Robot::Robot( const string& name, const Address& a, Network *net)
 : Sensor(name,a,net)
{
}

void Robot::thread()
{
    Message * const m = wait();
    automat( m);
    m->decUses();
}

