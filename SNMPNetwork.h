#ifdef NETSNMP

#ifndef SNMPNetwork_h
#define SNMPNetwork_h

#include "common.h"
#include "globals.h"
#include "Network.h"
#include "MessageQueue.h"
#include "MTSync.h"
#include "utils.h"
#include "Time.h"

#include <sys/socket.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

class SNMPNetwork
 : public Network, MessageQueue {

public:
    SNMPNetwork();
    virtual ~SNMPNetwork();

private:
    static bool init;
};

/*
class SNMPOpenNetwork
  : public Network, public Mutex, MessageQueue {

public:
    SNMPOpenNetwork();

    virtual ~SNMPOpenNetwork();

    void CloseSession() throw(WriteError);
    void ConnectSNMP( const string&, const string&, const unsigned&, const timespec&) 
    throw(WriteError);

protected:
    oid anOID[MAX_OID_LEN];
    size_t anOID_len;
    string host;

private:
    static bool init_m;

    void *sessp;             // an opaque pointer, not a struct pointer
    snmp_session *sptr;
    snmp_pdu *pdu;
};
*/

#endif
#endif