
#include "common.h"
#include "globals.h"
#include "utils.h"
#include "Time.h"

string delspace( const string& s)
{

        string buf2="";

        for ( int nn=0; nn<s.length(); nn=nn+1)
        {
            if ( s[nn] != 32 ) buf2=buf2+s[nn];
        };
        return buf2;
}

unsigned short word2short( const unsigned char& w1, const unsigned char& w2)
{

 return hex2<unsigned short>( ch2hex(w1,2)+ch2hex(w2,2) );

}


string hex2str( const string& s)
{

        char ch[255]; char bb=0; string buf2="";
        for ( char nn=0; nn<s.length(); nn=nn+2)
        {
            ch[bb]=hex2<int>(s.substr(nn,2));
            buf2=buf2+ch[bb];
            bb++;
        };
        return buf2;
}
string int2str( const int& t, const int& n, const char& c)
{
    stringstream buf;
    buf << setw(n) << setfill (c)<< right << t;
    return string( buf.str());
}

string int2str( const int& t)
{
    stringstream buf;
    buf << t;
    return string( buf.str());
}
string ush2str( const unsigned short& t, const int& n, const char& c)
{
    stringstream buf;
    buf << setw(n) << setfill (c)<< right << t;
    return string( buf.str());
}

string ush2str( const unsigned short& t)
{
    stringstream buf;
    buf << t;
    return string( buf.str());
}
string uch2str( const unsigned char& t)
{
    stringstream buf;
    buf << t;
    return string( buf.str());
}

string sec2timesec( const double& t)
{
// 1.234 -> 1 234000000
    string ttt;
    ttt=double2str(t);
    stringstream buf;
    buf << setw(9) << setfill ('0')<< left << ttt.substr(ttt.find('.')+1,9);
    return string( ttt.substr(0,ttt.find('.'))+" "+buf.str());
}
string double2str( const double& t)
{
    stringstream buf;
    buf << t;
    return string( buf.str());
}
string float2str( const float& t)
{
    stringstream buf;
    buf << t;
    return string( buf.str());
}

string int2hex( const int& t, const unsigned& n)
{
    stringstream buf;
    buf << setw(n) << setfill ('0') << uppercase << hex << t;
    return string( buf.str());
}
string uns2hex( const unsigned short& t, const char& n)
{
    stringstream buf;
    buf << setw(n) << setfill ('0') << uppercase << hex << t;

    return string( buf.str());
}
string short2hex( const short& t, const unsigned& n)
{
    stringstream buf;
    buf << setw(n) << setfill ('0') << uppercase << hex << t;
    return string( buf.str());
}
string long2hex( const long& t, const unsigned& n)
{
    stringstream buf;
    buf << setw(n) << setfill ('0') << uppercase << hex << t;
    return string( buf.str());
}

string ch2hex( const unsigned char& t, const unsigned& n)
{
	string an="";
        an=an+int2hex(t,n);
        return an;
}
string str2hex( const string& s)
{
            string an=""; unsigned char b;
            for (int i=0;i<s.size();i++){
            b=s[i];
            an=an+int2hex(b,2);
            };
          return an;
}
string str2hex( const string& s, const string& v)
{
            string an=""; unsigned char b;
            for (int i=0;i<s.size();i++){
            b=s[i];
            an=an+int2hex(b,2)+v;
            };
          return an;
}
string str2ascii( const string& s)
{
            string an="";
            char ch[]=".";
            char b;
            for (int i=0;i<s.size();i++){
                b = ( s[i] > 31) ? s[i] : ch[0];
                an=an+b;
            };
          return an;
}

string timespec2isotime( const timespec& t)
{
    stringstream buf;
    buf << t;
    return string( buf.str());
}

timespec isotime2timespec( const string& s)
{
    stringstream buf( s);
    timespec t;
    buf >> t;
    return t;
}

ostream& operator<<( ostream& f, const timespec& t)
{
    tm localTime;

    if ( &localTime != localtime_r( &t.tv_sec, &localTime))
        throw runtime_error(error());

    f << setfill('0') << 1900 + localTime.tm_year;
    f << '-' << setw(2) << 1 + localTime.tm_mon;
    f << '-' << setw(2) << localTime.tm_mday;
    f << ' ' << setw(2) << localTime.tm_hour;
    f << ':' << setw(2) << localTime.tm_min;
    f << ':' << setw(2) << localTime.tm_sec;
    f << '.' << setw(6) << t.tv_nsec / 1000;
    return f;
}

istream& operator>>( istream& f, timespec& t)
{
    tm localTime;
    unsigned i; char c;

    noskipws( f);
    f >> setw(4) >> i; localTime.tm_year = i - 1900;
    f >> c >> setw(2) >> i; localTime.tm_mon = i - 1;
    f >> c >> setw(2) >> localTime.tm_mday;
    f >> c >> setw(2) >> localTime.tm_hour;
    f >> c >> setw(2) >> localTime.tm_min;
    f >> c >> setw(2) >> localTime.tm_sec;

    localTime.tm_isdst = -1; // determine dst automatically
    if ( -1 == (t.tv_sec = mktime( &localTime)))
        throw runtime_error(error());

    f >> c >> setw(6) >> i; t.tv_nsec = i * 1000;

    return f;
}

istream& operator>>( istream& f, QuotedString& s)
{
    noskipws( f);
    f >> ws;

    if ( !( f >> static_cast<string&>(s)) || '"' != s[0]) return f; //XXX

    s = s.substr(1);

    char n;
    while ( f >> n && n != '"') s += n;
    return f;
}

string error()
{
    return string( strerror( errno));
}

string error( int err)
{
    return string( strerror( err));
}

ostream& error( ostream& os)
{
    os << strerror( errno);
    return os;
}

string trim( const string& s)
{
    const string blank = " \t\r\n";
    size_t p1, p2, start;

    if ( string::npos == (p1 = s.find_first_not_of( blank)))
        return "";

    start = p1;

    do {
        if ( string::npos == (p2 = s.find_first_of( blank, p1))) {
            return s.substr( start);
        }
    } while ( string::npos != ( p1 = s.find_first_not_of( blank, p2)));

    return s.substr( start, p2-start);
}

    /**
     * split string to pieces by delimeter
     * and return piece number n, beginning from 0
     */

string cut( unsigned i, const string& delim, const string& s)
{
    size_t p1=0, p2;
    while ( i--) {
        if ( string::npos == ( p1 = s.find_first_of( delim, p1)))
            return ""; //XXX exception?
        p1++;
    }
    if ( string::npos == ( p2 = s.find_first_of( delim, p1)))
        return s.substr( p1);
    return s.substr( p1, p2-p1);
}


void sig_chld(int signo)
{
//    pid_t   pid_chld;
//    int     stat;

//    while ( (pid_chld = waitpid(-1, &stat, WNOHANG)) > 0)
//        MONSYS_DEBUG << "sig_chld, Child " << pid_chld <<  " terminated"<<endl;
    return;
}

void daemonize()
{
    if ( SIG_ERR == signal(SIGTTOU, SIG_IGN) ||
         SIG_ERR == signal(SIGTTIN, SIG_IGN) ||
         SIG_ERR == signal(SIGTSTP, SIG_IGN) )
            throw runtime_error( "signal: " + error());

signal(SIGCHLD, sig_chld);

    switch( fork()) {
    case 0:
        break;
    case -1:
        throw runtime_error( "fork: " + error());
    default:
        _exit(0);
    }

    if ( -1 == setsid())
        throw runtime_error( "setsid: " + error());

    for ( unsigned fd=0; fd < 3; fd++)
        close(fd);

    if ( 0 != open( "/dev/null", O_RDONLY)
                      || 1 != dup( 0) || 2 != dup( 0))
        throw runtime_error( "Can't open stdin, stdout, stderr");

    if ( -1 == chdir("/"))
        throw runtime_error( "chdir: " + error());

    umask(0);

    MONSYS_DEBUG << "daemon started" << endl;
}

static const long BILLION = 1000*1000*1000;

unsigned long long milisec( const timespec& t)
{
    const unsigned long long tmp = t.tv_sec;
    return tmp*1000 + t.tv_nsec/1000/1000;
}

unsigned long long microsec( const timespec& t)
{
    const unsigned long long tmp = t.tv_sec;
    return tmp*1000*1000 + t.tv_nsec/1000;
}

unsigned long long nanosec( const timespec& t)
{
    const unsigned long long tmp = t.tv_sec;
    return tmp*BILLION + t.tv_nsec;
}

timespec nanosec2timespec( unsigned long long m)
{
    timespec t;
    t.tv_sec = m / BILLION;
    t.tv_nsec = m % BILLION;
    return t;
}

timespec read_timespec( const string& s)
{
    timespec t;
    if ( ! (stringstream( s) >> t.tv_sec >> ws >> t.tv_nsec))
        throw runtime_error( "can't read time");
    return t;
}

timespec now()
{
    timespec t;
    if ( clock_gettime( CLOCK_REALTIME, &t))
        throw runtime_error( "clock_gettime: " + error());
    if ( t.tv_sec < 31536000)
        throw runtime_error( "clock are broken: " + timespec2isotime( t));
    return t;
}

inline void norm_add_timespec( timespec& t)
{
    if (t.tv_nsec >= BILLION) {
        t.tv_sec++;
        t.tv_nsec -= BILLION;
        return;
    }
    if (t.tv_nsec < 0) {
        t.tv_sec--;
        t.tv_nsec += BILLION;
    }
}

inline void timeaddval( timespec& t1, const timespec& t2)
{
    t1.tv_sec += t2.tv_sec;
    t1.tv_nsec += t2.tv_nsec;
    norm_add_timespec(t1);
}

inline void timesubval( timespec& t1, const timespec& t2)
{
    t1.tv_sec -= t2.tv_sec;
    t1.tv_nsec -= t2.tv_nsec;
    norm_add_timespec(t1);
}

timespec operator+( const timespec& t1, const timespec& t2)
{
    timespec ret = t1;
    timeaddval( ret, t2);
    return ret;
}

timespec operator+=( timespec& t1, const timespec& t2)
{
    timeaddval( t1, t2);
    return t1;
}

timespec operator-( const timespec& t1, const timespec& t2)
{
    timespec ret = t1;
    timesubval( ret, t2);
    return ret;
}

timespec operator-=( timespec& t1, const timespec& t2)
{
    timesubval( t1, t2);
    return t1;
}

timespec operator*=( timespec& t, const double k)
{
    double sec = t.tv_sec + static_cast<double>(t.tv_nsec)/BILLION;
    sec *= k;
//XXX bounds checking need
    t.tv_sec = floor(sec);
    t.tv_nsec = fabs(sec - t.tv_sec) * BILLION;
    return t;
}

timespec operator*( const timespec& t1, const double t2)
{
    timespec ret = t1;
    ret *= t2;
    return ret;
}

timespec operator/=( timespec& t1, const double t2)
{
    t1 *= 1/t2;
    return t1;
}

timespec operator/( const timespec& t1, const double t2)
{
    return t1 * (1/t2);
}

bool operator>( const timespec& a, const timespec& b)
{
    return ( a.tv_sec > b.tv_sec)
           || (( a.tv_sec == b.tv_sec) && ( a.tv_nsec > b.tv_nsec));
}

bool operator<( const timespec& a, const timespec& b)
{
    return ( a.tv_sec < b.tv_sec)
           || (( a.tv_sec == b.tv_sec) && ( a.tv_nsec < b.tv_nsec));
}

bool operator>=( const timespec& a, const timespec& b)
{
    return ( a.tv_sec > b.tv_sec)
           || (( a.tv_sec == b.tv_sec) && ( a.tv_nsec >= b.tv_nsec));
}

bool operator<=( const timespec& a, const timespec& b)
{
    return ( a.tv_sec < b.tv_sec)
           || (( a.tv_sec == b.tv_sec) && ( a.tv_nsec <= b.tv_nsec));
}

bool operator==( const timespec& a, const timespec& b)
{
    return ( a.tv_sec == b.tv_sec) && ( a.tv_nsec == b.tv_nsec);
}

bool operator!=( const timespec& a, const timespec& b)
{
    return ( a.tv_sec != b.tv_sec) || ( a.tv_nsec != b.tv_nsec);
}

void nanosleep( const timespec& t)
{
    if ( -1 == nanosleep( &t, NULL))
        throw runtime_error( "nanosleep failed: " + error());
}

bool BEcheckCRC( const string& bcrc )
{

      int ttt=0;
      string crc1="", crc2="";
      string sss=bcrc.substr(1, (bcrc.size()-5) );

      for (int i=0;i<sss.size();i++){
      ttt=ttt+sss[i];
      }

      crc1=BEcrc(ttt);
      crc2=bcrc.substr( (bcrc.size()-4), 3  );

    if ( crc1 == crc2 ) return true;

    return false;
}

string BEcrc( const int& t)
{
string sum="";
        sum=int2str(t%256);
        switch ( sum.length()) {
        case 2:
          sum="0"+sum;
            break;
        case 1:
          sum="00"+sum;
          break;
        }
        return sum;
}
