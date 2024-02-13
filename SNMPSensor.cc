#ifdef NETSNMP

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "SNMPSensor.h"

/********************************************************************************************************************************
 *
 *   SNMPSensor - verion 1
 *
 * **********************************************************************************************************************************/

Message* SNMPSensor::getSensorData() throw (exception)
{
    pdu = snmp_pdu_create( SNMP_MSG_GET);
    snmp_add_null_var( pdu, anOID, anOID_len);

    int errcode;
    string bbb;
    snmp_pdu *response = NULL;
    errcode = snmp_sess_synch_response( sessp, pdu, &response);

    variable_list *vars;

    if ( errcode == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
#if 0
        for(vars = response->variables; vars; vars = vars->next_variable) {
            print_variable(vars->name, vars->name_length, vars);
        }
#endif
        for (vars = response->variables; vars; vars = vars->next_variable) {
            int counter=1;
            MONSYS_DEBUG << "SNMP-answer-type (hex) == " << uns2hex(vars->type, 2) << endl;
            switch (vars->type) {
            case ASN_OCTET_STR: {
                const string tmps( reinterpret_cast<char*>(vars->val.string),
                                                              vars->val_len);
                Message *msg;
                msg = new Message( this, tmps);
                MONSYS_DEBUG <<this->getName()<< " SNMPSensor ASN_OCTET_STR:" << tmps <<endl;
                snmp_free_pdu( response);
                return msg;
            }
            case ASN_GAUGE:
			case ASN_TIMETICKS:
            case ASN_INTEGER:
            case ASN_COUNTER: {

                Message *msg = new Message( this, *vars->val.integer);
            MONSYS_DEBUG <<this->getName()<< " SNMPSensor ASN_COUNTER:" << *vars->val.integer << endl;
                snmp_free_pdu( response);
                return msg;
            }
            case ASN_BOOLEAN:
            case ASN_BIT_STR:
            case ASN_OBJECT_ID:
            default:
                snmp_free_pdu( response);
                bbb= "don't know type SNMP-paket" ;
                throw ReadError(bbb);
            }
        }
    }

    if ( errcode != STAT_TIMEOUT) {
        string tmps = snmp_errstring( response->errstat);
        MONSYS_DEBUG <<this->getName()<< " SNMPSensor STAT_TIMEOUT:" << tmps << endl;
        snmp_free_pdu(response);
        throw ReadError( tmps);
    }

    if ( NULL != response) snmp_free_pdu(response);
    bbb= "don't connect SNMP sourse" ;
    MONSYS_ERROR <<this->getName()<< " ERROR: don't connect SNMP sourse!" << endl;
    throw ReadError(bbb);
}

SNMPSensor::SNMPSensor(const string& name, const Address& a, SNMPNetwork *n,
       const string& host, const string& var, const string& community, const unsigned& version,
       const timespec& t, const timespec& d) throw (string)
 : RTSensor(name,a,n,t,d)
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
        throw ex;
    }
    delete sptr;

// get the snmp_session pointer
    sptr = snmp_sess_session(sessp);

// change the community name
    free(sptr->community);
    sptr->community = reinterpret_cast<u_char*>(strdup(community.c_str()));
    sptr->community_len = strlen(reinterpret_cast<char*>(sptr->community));

    anOID_len = MAX_OID_LEN;
    if ( 1 != read_objid( var.c_str(), anOID, &anOID_len)) {
        snmp_sess_close(sessp);
        sessp = NULL;
        throw "unknown snmp oid: " + var;
    }
}

SNMPSensor::~SNMPSensor()
{
    snmp_sess_close(sessp);
}

#endif