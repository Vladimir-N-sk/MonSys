#ifdef NETSNMP

#include "common.h"
#include "globals.h"

#include "SNMPNetwork.h"

#include <sys/socket.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

bool SNMPNetwork::init = false;

SNMPNetwork::SNMPNetwork()
{
    if ( !init) {               //  Initialize the SNMP library
        init_snmp("");
        init = true;
    }
}

SNMPNetwork::~SNMPNetwork()
{
}

#endif