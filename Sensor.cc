
#include "common.h"
#include "globals.h"
#include "utils.h"

#include "Sensor.h"
#include "Message.h"
#include "Network.h"

#include <cmath>

Sensor::Sensor( const string& n, const Address& a, Network* q)
 : Parameter( n, a, q)
{
}

RTSensor::RTSensor( const string& n, const Address& a, Network* q,
                    const timespec& t, const timespec& d, unsigned wt)
 : Sensor(n, a, q),
   timeout(t),
   delay(d),
   firstTimeRun(true),
   timeouts(0),
   wedgedThreshold(wt)
{
}

void RTSensor::thread()
{

    if ( firstTimeRun) {           // slow start

        firstTimeRun = false;
        nanosleep( (delay + timeout) * 2 * drand48());
    }

    timespec remains = now();


    body();//XXX bad name



    remains += delay - now();

    if ( getPStatus() & WEDGED) {


     if (timeouts < 5) {
            remains = remains * (timeouts-1);
        } else remains *= 5;
    }

    if ( remains > ZEROTIME) nanosleep( remains);
    else if ( sched_yield()) throw runtime_error( error());

}

void RTSensor::body()
{

    try {

	timespec time_begin=now();
        Message * const msg = getSensorData();
	timespec time_end=now();


	if (timeouts > 0) timeouts -= 1;

        if ( getPStatus() & WEDGED) {
            setPStatus( getPStatus() & ~WEDGED);

            MONSYS_DEBUG << this->getName() << ": sensor answer, attempt: " << timeouts <<  " time_work: " << milisec(time_end-time_begin) << endl;
            timeouts = 0;
        }  else {
            MONSYS_DEBUG << this->getName() << " attempt: " << timeouts <<  " time_work: " << milisec(time_end-time_begin) << endl;
            }
        SOK();
        msg->post();
    }
    catch( runtime_error& e) {

        timeouts = ( 0 != timeouts + 1) ? timeouts + 1 : timeouts;

	if (timeouts > 5)  timeouts =5;


        if ( getPStatus() & WEDGED)  {
            sendWedgedMessage();
            MONSYS_DEBUG << "Sensor "<<this->getName()  <<" "
                         << e.what() << endl;
            return;
        }

        if ( timeouts < wedgedThreshold) {
            MONSYS_DEBUG << this->getName()  << " attempt: "<< timeouts <<" "
                         << e.what() << endl;
            return;
        }

        setPStatus( getPStatus() | WEDGED);
        sendWedgedMessage();
        setLastReadError( e);

        MONSYS_DEBUG << this->getName() << ": sensor no answer, attempt:" << timeouts <<" "
                     << e.what() << endl;
    }
}

MBSensor::MBSensor( const string& n, const Address& a, Network* q)
 : Sensor(n, a, q)
{
}

void MBSensor::thread()
{
       Message * const msg = getMBData();
       msg->post();

}
/***************************************************************************************************************************************************************************************
 *
 *
 *
 * ***************************************************************************************************************************************************************************************/

WTSensor::WTSensor( const string& n, const Address& a, Network* q,
                    const timespec& t, const timespec& d, const unsigned& wt)
 : Sensor(n, a, q),
   timeout(t),
   delay(d),
   firstTimeRun(true),
   timeouts(0),
   wedgedThreshold(wt)
{
}

void WTSensor::thread()
{

    if ( firstTimeRun) {           // slow start

        firstTimeRun = false;
        nanosleep( (delay + timeout) * 2 * drand48());
    }

    timespec remains = now();


    body();//XXX bad name

    remains += delay - now();

    if ( getPStatus() & WEDGED) {

        remains += timeout;
    }

    if ( remains > ZEROTIME) nanosleep( remains);
    else if ( sched_yield()) throw runtime_error( error());

}

void WTSensor::body()
{

    try {

    timespec time_begin=now();
    Message * const msg = getSensorData();
    timespec time_end=now();

    if (timeouts > 0) timeouts -= 1;

        if ( getPStatus() & WEDGED) {
            setPStatus( getPStatus() & ~WEDGED);

            MONSYS_DEBUG << this->getName() << ": sensor answer, attempt: " << timeouts <<  " time_work: " << milisec(time_end-time_begin) << endl;
            timeouts = 0;
        }  else {
            MONSYS_DEBUG << this->getName() << " attempt: " << timeouts <<  " time_work: " << milisec(time_end-time_begin) << endl;
            }
        SOK();
        msg->post();
    }
    catch( runtime_error& e) {

        timeouts = ( 0 != timeouts + 1) ? timeouts + 1 : timeouts;

        if ( getPStatus() & WEDGED)  {
            sendWedgedMessage();
            MONSYS_DEBUG << "Sensor "<<this->getName()  << " sensor no answer, attempt: "<< timeouts <<" "
                         << e.what() << endl;
            return;
        }

        if ( timeouts < wedgedThreshold) {
            MONSYS_DEBUG << this->getName()  << " attempt: "<< timeouts <<" "
                         << e.what() << endl;
            return;
        }

        setPStatus( getPStatus() | WEDGED);
        sendWedgedMessage();
        setLastReadError( e);

        MONSYS_DEBUG << this->getName() << ": sensor no answer, attempt:" << timeouts <<" "
                     << e.what() << endl;
    }
}
