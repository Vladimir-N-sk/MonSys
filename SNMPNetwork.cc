#ifdef NETSNMP


#include "SNMPNetwork.h"


bool SNMPNetwork::init = false;

SNMPNetwork::SNMPNetwork()
{
    if ( !init) {               //  Initialize the SNMP library
        init_snmp("");
        init = true;
    }
}

SNMPNetwork::~SNMPNetwork()
{}


/********************************************************
 * 
                        SNMPOpenNetwork
*
***********************************************************

bool SNMPOpenNetwork::init_m = false;

SNMPOpenNetwork::SNMPOpenNetwork() 
{
    if ( !init_m) {               //  Initialize the SNMP library
        init_snmp("");
        init_m = true;
    }
}

SNMPOpenNetwork::~SNMPOpenNetwork()
{
    snmp_sess_close(sessp);
}

void SNMPOpenNetwork::CloseSession() throw(WriteError)
{
  MONSYS_DEBUG << "Close SNMP session" <<endl;
  snmp_sess_close(sessp);    
}

void SNMPOpenNetwork::ConnectSNMP(
    const string& host, const string& community, const unsigned& version,
    const timespec& t) throw(WriteError)
{
    sptr = new snmp_session();
    snmp_sess_init(sptr);
    sptr->peername = const_cast<char*>(host.c_str());
    sptr->version = version;
    sptr->timeout = microsec( t);
    sessp = snmp_sess_open(sptr);
    if (sessp == NULL) {
        char* errstr;
        int liberr, syserr;
        snmp_error(sptr, &liberr, &syserr, &errstr);
        const string ex = string("SNMP create error: ") + errstr;
        free(errstr);
        throw ex; // !!!
    }
    delete sptr;

// get the snmp_session pointer
    sptr = snmp_sess_session(sessp);

// change the community name
    free(sptr->community);
    sptr->community = reinterpret_cast<u_char*>(strdup(community.c_str()));
    sptr->community_len = strlen(reinterpret_cast<char*>(sptr->community));
}
*/

#endif