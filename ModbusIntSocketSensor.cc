

#include "common.h"
#include "globals.h"

#include "SocketNetwork.h"
#include "ModbusIntSocketSensor.h"
#include "utils.h"
#include "Time.h"
#include "Message.h"
#include "Message_Body.h"

#include <sched.h>
#include <unistd.h>


/****************************************************************************************************************************
 *
 *
 */
ModbusTCPIntSocketSensor::ModbusTCPIntSocketSensor( const string& name, const Address& a,
                   SocketOpenNetwork *n, const SModEntry& q)
 : SocketOpenSensorModbusTCP(name,a,n,q)
{
}


Message* ModbusTCPIntSocketSensor::makeMessage(const string& str)
{
    int readed = hex2<int>(str);
    if ( readed > 32767) readed -= 65536;
MONSYS_DEBUG << this->getName() <<  " ModbusTCPIntSocketSensor:"<<readed << endl;
    return new Message( this, readed);
}

/***************************************************************************************************************************************************************************************************
 *
 *
 */

ModbusTCPFloatOvenSensor::ModbusTCPFloatOvenSensor( const string& name, const Address& a,
                   SocketOpenNetwork *n, const SModEntry& q)
 : SocketOpenSensorModbusTCP(name,a,n,q)
{
}
Message* ModbusTCPFloatOvenSensor::makeMessage(const string& str)
{

// ОВЕН - младшим байтом вперед

    string str2=str.substr(4,4)+str.substr(0,4) ;
    int readed = hex2<int>(str2);
    double exp, mant, s;

s=readed >> 31;
s = ( 0 == s) ? 1 : -1;
readed=readed & 0x7FFFFFFF;

    exp=readed >> 23;
    exp=exp-127;
    mant=readed & 0x7FFFFF;
    mant=mant/0x800000;
    exp=pow(2,exp);
    mant=(1+mant)*exp*s;

MONSYS_DEBUG << this->getName() <<  " ModbusTCPFloatOvenSensor: "<<mant << endl;

    return new Message( this, mant);
}

/***************************************************************************************************************************************************************************************************
 *
 *
 */

ModbusTCPFloatSensor::ModbusTCPFloatSensor( const string& name, const Address& a,
                   SocketOpenNetwork *n, const SModEntry& q)
 : SocketOpenSensorModbusTCP(name,a,n,q)
{
}
Message* ModbusTCPFloatSensor::makeMessage(const string& str)
{

    int readed = hex2<int>(str);
    double exp, mant, s;

    s=readed >> 31;
    s = ( 0 == s) ? 1 : -1;
    readed=readed & 0x7FFFFFFF;

    exp=readed >> 23;
    exp=exp-127;
    mant=readed & 0x7FFFFF;
    mant=mant/0x800000;
    exp=pow(2,exp);
    mant=(1+mant)*exp*s;

MONSYS_DEBUG << this->getName() <<  " ModbusTCPFloatSensor: "<<mant << endl;

    return new Message( this, mant);
}


/*
 *
 *
 */

ModbusIntSocketSensor::ModbusIntSocketSensor( const string& name, const Address& a,
                   SocketNetwork *n, const SModEntry& q)
 : SocketSensorModbus(name,a,n,q)
{
}


Message* ModbusIntSocketSensor::makeMessage(const string& str)
{
    int readed = hex2<int>(str);
    if ( readed > 32767) readed -= 65536;
MONSYS_DEBUG << this->getName() <<  " ModbusIntSocketSensor:"<<readed << endl;
    return new Message( this, readed);
}


ModbusLongSocketSensor::ModbusLongSocketSensor( const string& name, const Address& a,
                   SocketNetwork *n, const SModEntry& q)
 : SocketSensorModbus(name,a,n,q)
{
}


Message* ModbusLongSocketSensor::makeMessage(const string& str)
{
    long readed = hex2<long>(str);
MONSYS_DEBUG << this->getName() <<  " ModbusLongSocketSensor:"<<readed << endl;
    return new Message( this, readed);
}


/*****************************************************************************************************/
ModbusIntOpenSocketSensor::ModbusIntOpenSocketSensor( const string& name, const Address& a,
                   SocketOpenNetwork *n, const SModEntry& q)
 : SocketOpenSensorModbus(name,a,n,q)
{
}

Message* ModbusIntOpenSocketSensor::makeMessage(const string& str)
{
    int readed = hex2<int>(str);
    if ( readed > 32767) readed -= 65536;
MONSYS_DEBUG << this->getName() <<  " ModbusIntOpenSocketSensor:"<<readed << endl;
    return new Message( this, readed);
}

/*****************************************************************************************************************************************************************/
ModbusLongOpenSocketSensor::ModbusLongOpenSocketSensor( const string& name, const Address& a,
                   SocketOpenNetwork *n, const SModEntry& q)
 : SocketOpenSensorModbus(name,a,n,q)
{
}


Message* ModbusLongOpenSocketSensor::makeMessage(const string& str)
{
    long readed = hex2<long>(str);
MONSYS_DEBUG << this->getName() <<  " ModbusLongOpenSocketSensor:"<<readed << endl;
    return new Message( this, readed);
}


/****************************************************************************************************************************************************************************************************/

ModbusFloat64OpenSocketSensor::ModbusFloat64OpenSocketSensor( const string& name, const Address& a,
                   SocketOpenNetwork *n, const SModEntry& q)
 : SocketOpenSensorModbus(name,a,n,q)
{
}
Message* ModbusFloat64OpenSocketSensor::makeMessage(const string& str)
{

/* вариант с заменой байт на обратный  */

    string str1=str.substr(12,4)+str.substr(8,4)+str.substr(4,4)+str.substr(0,4) ;

MONSYS_DEBUG << this->getName() <<  " str:"<<str  <<  " convert to str1:"<<str1  <<endl;

    long long readed = hex2<long long>(str1);
    double exp, mant;

    exp=readed >> 52;

    if ( exp>1023)
    {
        exp=exp-1023;
        mant=readed & 0xFFFFFFFFFFFFF;
        mant=mant/0x10000000000000;
        exp=pow(2,exp);
        mant=(1+mant)*exp;
    } else mant=0.0;

MONSYS_DEBUG << this->getName() <<  " ModbusFloat64OpenSocketSensor:"<<mant << endl;

    return new Message( this, mant);
}


/****************************************************************************************************************************************************************************************************/

ModbusFloatOpenSocketSensor::ModbusFloatOpenSocketSensor( const string& name, const Address& a,
                   SocketOpenNetwork *n, const SModEntry& q)
 : SocketOpenSensorModbus(name,a,n,q)
{
}
Message* ModbusFloatOpenSocketSensor::makeMessage(const string& str)
{

    int readed = hex2<int>(str);

    double exp, mant, s;

   s=readed >> 31;
    s = ( 0 == s) ? 1 : -1;
    readed=readed & 0x7FFFFFFF;

    exp=readed >> 23;

        exp=exp-127;
        mant=readed & 0x7FFFFF;
        mant=mant/0x800000;
        exp=pow(2,exp);
        mant=(1+mant)*exp*s;

MONSYS_DEBUG << this->getName() <<  " ModbusFloatOpenSocketSensor: "<<mant << endl;

    return new Message( this, mant);
}

/***************************************************************************************************************************************************************************************************
 *
 *
 */

ModbusFloatOvenOpenSocketSensor::ModbusFloatOvenOpenSocketSensor( const string& name, const Address& a,
                   SocketOpenNetwork *n, const SModEntry& q)
 : SocketOpenSensorModbus(name,a,n,q)
{
}
Message* ModbusFloatOvenOpenSocketSensor::makeMessage(const string& str)
{

    string str2=str.substr(4,4)+str.substr(0,4) ;
    int readed = hex2<int>(str2);
    double exp, mant;

    exp=readed >> 23;

//    if ( exp>125)   // считает значение только > 0.5, иначе убрать это условие
//    {
        exp=exp-127;
        mant=readed & 0x7FFFFF;
        mant=mant/0x800000;
        exp=pow(2,exp);
        mant=(1+mant)*exp;
//    } else mant=0.0;

MONSYS_DEBUG << this->getName() <<  " ModbusFloatOvenOpenSocketSensor: "<<mant << endl;

    return new Message( this, mant);
}


