
#ifdef PSQL

#ifndef PGBackend_h
#define PGBackend_h

#include <string>
#include <unistd.h>
#include <sstream>
#include "libpq-fe.h"

#include "Backend.h"
#include "Filtered.h"
#include "MessageQueue.h"
#include "Message.h"
#include "Sensor.h"

#include "common.h"
#include "globals.h"
#include "Parameter.h"
#include "Message.h"
#include "Time.h"


class Parameter;

class PGLBackend
 : public Backend, public Filtered<BHMessageQueue> {

public:
    class DBError
     : public runtime_error {
    public:
        DBError( const string& a)
         : runtime_error(a) {}
    };

    PGLBackend( const string&);
    ~PGLBackend();
    void thread();

private:
    void close( const timespec&) throw( DBError);
    void write( Message*) throw( DBError, Message::empty);
	void updDesc( Parameter*, const string&) throw( DBError);
    string addParameter( Parameter*) throw( DBError);
    string getParameterID( Parameter*) throw( DBError);
    string getIDsost( const string&) throw( DBError);
    timespec lastRecordTime() throw( DBError, runtime_error);
    bool isDBClosed() throw( DBError);

    const string conninfo;
    bool checked;
    const int reconnectTimeout;
    timespec down;
    PGconn     *conn;
    PGresult   *res;
};

#endif
#endif