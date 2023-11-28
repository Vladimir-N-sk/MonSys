
#include "common.h"
#include "globals.h"
#include "utils.h"

#include "Logger.h"
#include "Time.h"
#include "Config.h"
#include "Dispatcher.h"
#include "Runable.h"

#include<iostream>
#include <stdexcept>

#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <cstring>
#include <fstream>

const string CONFIG_BY_DEFAULT = "monsys.cfg";
const string Loggerbuf::ident = "monsys";

std::istream *cfile;

std::ostream *cerror_log = &cerr, *cdebug_log = &cerr, *cwarning_log = &cerr;

bool test = false;

int main(int argc, char **argv)
{

    try {

        blockSignals();

        initRND();

        startup( argc, argv);

        Config* c = new Config( *cfile);

    	Dispatcher* disp = new Dispatcher(*c);
    	disp->doConfig();

	delete disp;
	delete c;

        procSignals();
        MOND_DEBUG << "shutdown" << endl;

        return 0;
    }
    catch ( string& e) {
        MOND_ERROR << e << endl;
        return 0;
    }
    catch ( exception& e) {
        MOND_ERROR << e.what() << endl;
        return 0;
    }
}


void initRND()
{
    unsigned short seed[3];
    seed[0] = seed[1] = seed[2] = now().tv_sec;
    seed48( seed);
}


void blockSignals()
{
    sigset_t signalSet;
    if ( sigfillset( &signalSet ))
        throw runtime_error( "sigfillset failed:" + error());

    if ( pthread_sigmask( SIG_BLOCK, &signalSet, NULL ))
        throw runtime_error( "pthread_sigmask failed");
}

void procSignals()
{
    sigset_t signalSet;
    pid_t   pid_sig;
    int     stat;


    if ( sigfillset( &signalSet ))
        throw runtime_error( "sigfillset failed:" + error());

    bool timetoexit = false;
    do {
        int sig;
        if ( 0 != sigwait ( &signalSet, &sig ))
            throw runtime_error( "sigwait: " + error());

        switch ( sig) {
        case SIGTERM:
        case SIGINT:
            MOND_DEBUG << "recieved signal: " << sig << endl;
            timetoexit = true;
            break;

        case SIGCHLD:

    while ( (pid_sig = waitpid(-1, &stat, WNOHANG)) > 0)
        MOND_DEBUG << " Signal SIGCHLD, Child " << pid_sig <<  " terminated"<<endl;
            break;

        default:
            MOND_DEBUG << "unknown signal: " << sig << endl;
            break;
        }
    } while ( !timetoexit);
}

void setuid( const string& uname)
{
    passwd* pw;
    if ( NULL == (pw = getpwnam( uname.data())))
        throw runtime_error( "getpwnam: " + error());

  // zap root's groups and set user groups
    if ( setgid(pw->pw_gid))
        throw runtime_error( "setgid: " + error());
    if ( initgroups( uname.data(), pw->pw_gid))
        throw runtime_error( "initgroups: " + error());

    if ( setuid( pw->pw_uid))
        throw runtime_error( "setuid: " + error());
}

void printHelp()
{
    cout << "Mon[itoring] Sys[tem], ver 1.0, github.com/Vladimir-N-sk/MonSys" << endl;
    cout << "usage: monsys [-c <file>] [-d] [-l] [-u <user>]" << endl;
    cout << "-c <file>: use config <file>" << endl;
   cout << "-t: test config <file>" << endl;
    cout << "-d: daemon mode" << endl;
    cout << "-l: set use syslog user.debug" << endl;
	cout << "-w: set use syslog user.warning" << endl;
    cout << "-u <user>: run by <user>" << endl;
    cout << "-v: output version" << endl;

}

string printVersion()
{
    return "Mon[itoring] Sys[tem], ver 1.0, github.com/Vladimir-N-sk/MonSys";
}

void startup( int argc, char** argv)
{
    bool syslog = false;
	bool warning = false;
    bool daemon = false;


    string config;
    string uname;
    int c;
    while ( (c = getopt(argc, argv, "du:c:lwvt")) != EOF) {
        switch(c) {
        case 't':
            test = true;
            break;
        case 'd':
            daemon = true;
            break;
        case 'u':
            uname = optarg;
            break;
        case 'c':
            config = optarg;
            break;
        case 'l':
            syslog = true;
            break;
        case 'w':
            warning = true;
            break;
        case 'v':
            cout << printVersion() << endl;
            exit(0);
            break;
        default:
            printHelp();
            exit (0);
        }
    }

    if ( syslog) {
        cerror_log = new Logger( LOG_ERR);
//        cwarning_log = new Logger( LOG_WARNING);
        cdebug_log = new Logger( LOG_DEBUG);
    }

    if ( warning) {
//        cerror_log = new Logger( LOG_ERR);
        cwarning_log = new Logger( LOG_WARNING);
//        cdebug_log = new Logger( LOG_DEBUG);
    }


    MOND_DEBUG << printVersion() << endl;
    MOND_DEBUG << "monsys started" << endl;

    config = (config == "") ? CONFIG_BY_DEFAULT : config;
    MOND_DEBUG << " Read config: " << config << endl;

    cfile = new ifstream( config.data());
    if (!*cfile) throw runtime_error( "can't open " + config);

    if ( uname != "") {
        MOND_DEBUG << "seting uid: " << uname << endl;
        setuid( uname);
    }

    if ( daemon) {
        MOND_DEBUG << "daemonizing" << endl;
        daemonize();
    }
}
