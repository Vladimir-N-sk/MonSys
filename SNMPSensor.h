#ifdef NETSNMP

#ifndef SNMPSensor_h
#define SNMPSensor_h

#include "common.h"
#include "globals.h"
#include "Sensor.h"

#include <string>
#include <sys/socket.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "Message.h"
#include "Message_Body.h"
#include "SNMPNetwork.h"


class SNMPNetwork;
class Message;

class SNMPSensor
 : public RTSensor {

public:
    SNMPSensor( const string&, const Address&, SNMPNetwork*, const string&,
      const string&, const string&, const unsigned int&,
      const timespec&, const timespec&)  throw(string);
    virtual ~SNMPSensor();
    Message* getSensorData() throw(exception);

private:
    void *sessp;             // an opaque pointer, not a struct pointer
    snmp_session *sptr;
    snmp_pdu *pdu;
    oid anOID[MAX_OID_LEN];
    size_t anOID_len;
};


//class SNMPOpenNetwork;

class SNMPScanerSensor
 : public WTSensor {
// : public RTSensor {  

public:
    SNMPScanerSensor( const string&, const Address&, SNMPNetwork*, const string&,
      const string&, const string&, const unsigned int&,
      const timespec&, const timespec&, const unsigned&)  throw(string);
    virtual ~SNMPScanerSensor();
    Message* getSensorData() throw(exception);

private:
//SNMPOpenNetwork *net;

    void *sessp;             // an opaque pointer, not a struct pointer
    snmp_session *sptr;
    snmp_pdu *pdu;
    oid anOID[MAX_OID_LEN];
    size_t anOID_len;
    string host, var, community;
    unsigned version;
    timespec zad;
};


#endif
#endif