// $Id: Robot.cc,v 1.9 2002/01/21 05:57:55 denis Exp $

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

