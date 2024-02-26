#ifdef NETSNMP

#include "SNMPSensor.h"

void SNMPScanerSensor::IPplus()
{

    if ( (ua1==ua2) && (ub1==ub2) && (uc1==uc2) && (ud1==ud2) ) {
        MONSYS_DEBUG <<  "End of setting range IP-address" << endl;        
         exit(0);
    }

    if ( ud1 < 255 ) { 
        ++ud1;
        } else if ( ud1 == 255 ) { 
            ud1=0; 
            if ( uc1 < 255){
                ++uc1;
                } else if ( uc1 == 255 ) { 
                    uc1=0;
                    if (ub1<255){
                        ++ub1;   
                        } else if (ub1==255){
                            ub1=0;
                            if (ua1<255){
                                ++ua1;
                                } 
                        }
                    
                }
                
            } 
    

    snmp_sess_close(sessp);
    host = uns2str(ua1) +'.' + uns2str(ub1)+'.' + uns2str(uc1)+'.' + uns2str(ud1);
}

Message* SNMPScanerSensor::getSensorData() throw (exception)
{

    sptr = new snmp_session();
    snmp_sess_init(sptr);
    sptr->peername = const_cast<char*>(host.c_str());    
    sptr->version = version;
    sptr->timeout = microsec( zad);
    sessp = snmp_sess_open(sptr);
    delete sptr;
    if (sessp == NULL) {
        char* errstr;
        int liberr, syserr;
        snmp_error(sptr, &liberr, &syserr, &errstr);
        string ex = "Host:"+host+string(" SNMP create error: ") + errstr;
        free(errstr);
        IPplus();       
        throw ReadError(ex);
    }

// get the snmp_session pointer
    sptr = snmp_sess_session(sessp);

// change the community name
    free(sptr->community);
    sptr->community = reinterpret_cast<u_char*>(strdup(community.c_str()));
    sptr->community_len = strlen(reinterpret_cast<char*>(sptr->community));


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
                const string tmps( reinterpret_cast<char*>(vars->val.string), vars->val_len);
                const string ttt = "Host:" + host + ", answer:"+ tmps;
                Message *msg;
                msg = new Message( this, ttt);                
                MONSYS_DEBUG <<this->getName()<< " Host: "<< host <<" ASN_OCTET_STR:" << tmps <<endl;
                snmp_free_pdu( response);
                IPplus();
                return msg;
            }
            case ASN_GAUGE:
			case ASN_TIMETICKS:
            case ASN_INTEGER:
            case ASN_COUNTER: {

                Message *msg = new Message( this, *vars->val.integer);
            MONSYS_DEBUG <<this->getName()<< " Host: "<< host << " ASN_COUNTER:" << *vars->val.integer << endl;
                snmp_free_pdu( response);
                IPplus();
                return msg;
            }
            case ASN_BOOLEAN:
            case ASN_BIT_STR:
            case ASN_OBJECT_ID:
            default:
                snmp_free_pdu( response);
                IPplus();                
                bbb= "don't know type SNMP-paket" ;
                throw ReadError(bbb);
            }
        }
    }

    if ( errcode != STAT_TIMEOUT) {
        string tmps = snmp_errstring( errcode);        
        MONSYS_ERROR <<this->getName()<< " Host: "<< host<<" SNMPScanerSensor ERROR:" << tmps << endl;
        snmp_free_pdu(response);
        IPplus();
        throw ReadError( tmps);
    }


        if ( NULL != response) snmp_free_pdu(response);
    bbb= "ERROR: don't connect SNMP host: "+host ;
    IPplus();
    throw ReadError(bbb);
}


SNMPScanerSensor::SNMPScanerSensor(const string& name, const Address& a, SNMPNetwork *n,
       const string& h1, const string& h2, const string& vv, const string& cc, const unsigned& vers,
       const timespec& t, const timespec& d, const unsigned& w) throw (string)
 : WTSensor(name,a,n,t,d,w)
 {
    var = vv;
    community = cc;
    version=vers;
    zad=t;
    anOID_len = MAX_OID_LEN;
    if ( 1 != read_objid( var.c_str(), anOID, &anOID_len)) {
        snmp_sess_close(sessp);
        sessp = NULL;
        throw "unknown snmp oid: " + var;
    }
host=h1;

ua1 = dec2<unsigned>( host.substr(0, host.find('.')) );
host = host.substr(host.find('.')+1 );

ub1 = dec2<unsigned>(  host.substr(0, host.find('.'))  );
host = host.substr(host.find('.')+1 );

uc1 = dec2<unsigned>( host.substr(0, host.find('.')) );
host = host.substr(host.find('.')+1 );

ud1 = dec2<unsigned>( host.substr(0, host.find('.')) );

host=h2;

ua2 = dec2<unsigned>( host.substr(0, host.find('.')) );
host = host.substr(host.find('.')+1 );

ub2 = dec2<unsigned>(  host.substr(0, host.find('.'))  );
host = host.substr(host.find('.')+1 );

uc2 = dec2<unsigned>( host.substr(0, host.find('.')) );
host = host.substr(host.find('.')+1 );

ud2 = dec2<unsigned>( host.substr(0, host.find('.')) );

host=h1;

MONSYS_DEBUG <<this->getName()<< " Start IP-address: " << ua1<< "."<< ub1<< "." << uc1<< "."<< ud1 << endl;
MONSYS_DEBUG <<this->getName()<< " End IP-address: " << ua2<< "."<< ub2<< "." << uc2<< "."<< ud2 << endl;

if ( (ua1+ub1+uc1+ud1) > (ua2+ub2+uc2+ud2) )
{
    MONSYS_ERROR <<  "End IP-address < Start IP-addres!" << endl;
    exit (1);
}
//exit(0);
}

SNMPScanerSensor::~SNMPScanerSensor()
{
    snmp_sess_close(sessp);
}


/***************************************************************************************************************
*
******************************************************************************************************************/
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