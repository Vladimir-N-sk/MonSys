//  $Id: Sensor.cc,v 1.44 2003/01/08 11:25:27 denis Exp $

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

//        remains += timeout;
    }

/*    if ( remains > ZEROTIME) {
      nanosleep( remains);
    }
    else {
    	if ( sched_yield()) throw runtime_error( error());
    }
*/
// origin code
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

            MOND_DEBUG << this->getName() << ": sensor answer, attempt: " << timeouts <<  " time_work: " << milisec(time_end-time_begin) << endl;
            timeouts = 0;
        }  else {
            MOND_DEBUG << this->getName() << " attempt: " << timeouts <<  " time_work: " << milisec(time_end-time_begin) << endl;
            }
        SOK();
        msg->post();
    }
    catch( runtime_error& e) {

        timeouts = ( 0 != timeouts + 1) ? timeouts + 1 : timeouts;

	if (timeouts > 5)  timeouts =5;


        if ( getPStatus() & WEDGED)  {
            sendWedgedMessage();
            MOND_DEBUG << "Sensor "<<this->getName()  <<" "
                         << e.what() << endl;
            return;
        }

        if ( timeouts < wedgedThreshold) {
            MOND_DEBUG << this->getName()  << " attempt: "<< timeouts <<" "
                         << e.what() << endl;
            return;
        }

        setPStatus( getPStatus() | WEDGED);
        sendWedgedMessage();
        setLastReadError( e);

        MOND_DEBUG << this->getName() << ": sensor no answer, attempt:" << timeouts <<" "
                     << e.what() << endl;
    }
}

/* перенес в Sensor.h
Sensor::~Sensor()
{
}

RTSensor::~RTSensor()
{
}

MBSensor::~MBSensor()
{
}

WTSensor::~WTSensor()
{
}
*/

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

/* 20/09/17
    if (timeouts < 5) {
            remains = remains * (timeouts-1);
        } else remains *= 5;
*/
// 20/09/17 ������� � ����.����
        remains += timeout;
    }

/*    if ( remains > ZEROTIME) {
      nanosleep( remains);
    }
    else {
        if ( sched_yield()) throw runtime_error( error());
    }
*/
// origin code
    if ( remains > ZEROTIME) nanosleep( remains);
    else if ( sched_yield()) throw runtime_error( error());

}



void WTSensor::body()
{

    try {

    timespec time_begin=now();
    Message * const msg = getSensorData();
    timespec time_end=now();

//        timeouts = 0;

    if (timeouts > 0) timeouts -= 1;

        if ( getPStatus() & WEDGED) {
            setPStatus( getPStatus() & ~WEDGED);

            MOND_DEBUG << this->getName() << ": sensor answer, attempt: " << timeouts <<  " time_work: " << milisec(time_end-time_begin) << endl;
            timeouts = 0;
        }  else {
            MOND_DEBUG << this->getName() << " attempt: " << timeouts <<  " time_work: " << milisec(time_end-time_begin) << endl;
            }
        SOK();
        msg->post();
    }
    catch( runtime_error& e) {

        timeouts = ( 0 != timeouts + 1) ? timeouts + 1 : timeouts;

//    if (timeouts > 5)  timeouts =5;

        if ( getPStatus() & WEDGED)  {
            sendWedgedMessage();
            MOND_DEBUG << "Sensor "<<this->getName()  << " sensor no answer, attempt: "<< timeouts <<" "
                         << e.what() << endl;
            return;
        }

        if ( timeouts < wedgedThreshold) {
            MOND_DEBUG << this->getName()  << " attempt: "<< timeouts <<" "
                         << e.what() << endl;
            return;
        }


        setPStatus( getPStatus() | WEDGED);
        sendWedgedMessage();
        setLastReadError( e);

        MOND_DEBUG << this->getName() << ": sensor no answer, attempt:" << timeouts <<" "
                     << e.what() << endl;
    }
}
