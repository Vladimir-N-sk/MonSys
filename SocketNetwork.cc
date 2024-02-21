
/*
#include "common.h"
#include "globals.h"
#include "utils.h"
#include "Time.h"
#include "MTSync.h"
*/
#include "SocketNetwork.h"
//#include "SerialNetwork.h"

/*
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <ctype.h>
#include <strings.h>
#include <netdb.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <memory>
*/

SocketNetwork::SocketNetwork( const string& ah, unsigned por)
 : address(ah),
   port(por)
{
    sock = socket( AF_INET, SOCK_STREAM, 0);
    if ( -1 == sock )
        throw runtime_error(" Error open socket: " + error());

    const char* a1 = address.data();
    struct hostent *hp;
    char **p;
    const int on = 1;

	hp = gethostbyname(a1);
    if ( hp == NULL )
        throw runtime_error("Error name of host: "+address +" "+ error());
    p = hp->h_addr_list;

    bzero( &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons( port);
    memcpy(&addr.sin_addr,*p, sizeof (addr.sin_addr));

    int conn = connect( sock, (struct sockaddr* )&addr, sizeof (addr));
    if ( 0 == conn) {
           MONSYS_DEBUG << " Connect socket to host " << hp->h_name << endl;
    } else  throw runtime_error("Error connect socket: " + error());

}

SocketNetwork::~SocketNetwork()
{
    if (-1 == close(sock))
        throw runtime_error( "Error close socket: " + error());
}


int SocketNetwork::ConnectSocket()
{
    int conn = connect( sock, (struct sockaddr* )&addr, sizeof (addr));

    if ( 0 != conn) {
	  string an="Error connect socket: " + error();
	  throw WriteError(an);
	}
    return conn;
}

void SocketNetwork::writeSocket(const string& s) throw(WriteError)
{
   string an="";
    if ( s.size() != static_cast<unsigned>(  write( sock, s.c_str(), s.size() ))){
	an ="Error write to socket: "+error() ;
        throw WriteError( an);
        }
    MONSYS_DEBUG <<" Write Socket (HEX):" << str2hex(s," ") << " ASCII:"<< str2ascii(s) << " RAW:" << s <<endl;
}

int SocketNetwork::SendToSocket(const string& s)
{

    if ( s.size() != static_cast<unsigned>(  write( sock, s.c_str(), s.size() ))){
		return 0;
        }
return 1;
}

string SocketNetwork::RecvSocket( size_t size, const timespec& timeout)
                         throw ( runtime_error)
{
    size_t readed = 0;
    char c;

    char lin[1500];
    string buf,bbb;
    int to = milisec( timeout);

    pollfd fdset;
    fdset.fd = sock;
    fdset.events = POLLIN;
    fdset.revents = 0;


        int ttt=0;

        ttt=poll( &fdset, 1, to);

          if ( ttt == 1) {

            if ( !(fdset.revents & POLLIN)) //XXX paranoia
                throw runtime_error( "poll error");

            int re = recv(sock,lin,sizeof(lin)-1,0 );
            if ( re == -1) throw runtime_error("Error read from remote serial port: " + error());
            if ( re == 0) throw runtime_error("Empty input bufer remote serial port: " + error());

          buf="";
            do {
              buf=buf+lin[readed];
              readed++;
            } while(size != readed && re != readed );

            if ( re < size){

				bbb = " Answer isn't complete from remote serial port (timeout), reded byts:" + int2str(re) +  "HEX:"+str2hex(buf," ") + " ASCII:"+buf ;
        	 throw ReadError( bbb );
        	 }

          return buf;
          }
          if ( ttt == 0) {
            an=""; unsigned char b;
            for (char i=0;i<buf.size();i++){
            b=buf[i];
            an=an+int2hex(b,2)+" ";
            }

            MONSYS_DEBUG << " From RecvSocket(timeout): "<<an << " readed: "<< readed << endl;
	    bbb = " No answer from remote serial port (timeout)" +buf ;
            throw ReadError( bbb);
            return buf;
          }
          MONSYS_ERROR << "Error poll: " << error()<< endl;
    return buf;
}
string SocketNetwork::ReadSocket(  const timespec& timeout)
                         throw ( runtime_error)
{
    size_t readed = 0;

	const int lin_size=1500;
    char lin[lin_size];
    string buf,bbb;
    int to = milisec( timeout);

    pollfd fdset;
    fdset.fd = sock;
    fdset.events = POLLIN;
    fdset.revents = 0;

        int ttt=0;

        ttt=poll( &fdset, 1, to);

          if ( ttt == 1) {

            if ( !(fdset.revents & POLLIN)) //XXX paranoia
                throw runtime_error( "poll error");

            int re = recv(sock,lin,sizeof(lin)-1,0 );

            if ( re == -1) throw runtime_error("Error read from remote serial port: " + error());
            if ( re == 0) throw runtime_error("Empty input bufer remote serial port: " + error());

            buf="";
            do {
              buf=buf+lin[readed];
              readed++;
            } while(readed != re && readed < lin_size);

            return buf;

          }
          if ( ttt == 0) {
			  bbb = " No answer from remote serial port (timeout)";
            throw ReadError( bbb);
            return buf;
          }

          MONSYS_ERROR << "Error poll: " << error()<< endl;
    return buf;
}

/*********************************************************************************************************************************************
 *
 *             SocketOpenNetwork
 *
 *
 * ***********************************************************************************************************************************************/

SocketOpenNetwork::SocketOpenNetwork( const string& ah, unsigned por)
 : address(ah),
   port(por)
{
}

SocketOpenNetwork::~SocketOpenNetwork()
{
    if (-1 == close(sock))
        throw runtime_error( "Error close socket!" );
}

void SocketOpenNetwork::CloseSocket() throw(WriteError)
{
  MONSYS_DEBUG << "Close Socket: " << sock <<endl;
    if (-1 == close(sock)) {
            bbb = " Error close socket " ;
        throw WriteError(bbb);
    }
}


int SocketOpenNetwork::ConnectSocket() throw(WriteError)
{

      sock = socket( AF_INET, SOCK_STREAM, 0);
    if ( -1 == sock ){
	    bbb = " Error open socket: " + error();
        throw WriteError(bbb);
	}

MONSYS_DEBUG << "Open Socket: " << sock <<endl;

    const char* a1 = address.data();

    char **p;
    const int on = 1;

    hp = gethostbyname(a1);
    if ( hp == NULL ){
       CloseSocket();
	bbb = "Error name of host: "+address +" "+ error();
        throw WriteError(bbb);
	}
    p = hp->h_addr_list;

    bzero( &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons( port);
    memcpy(&addr.sin_addr,*p, sizeof (addr.sin_addr));

    int conn = connect( sock, (struct sockaddr* )&addr, sizeof (addr));

    if ( 0 == conn) {
           MONSYS_DEBUG << " Connect socket to host: " << hp->h_name << " Port: "<< int2str(port)<<endl;
    } else  {
      CloseSocket();
      bbb="Error connect socket to host: ";
      bbb=bbb+a1;
      bbb=bbb+" Port:";
      bbb=bbb+int2str(port);
      bbb=bbb+" Error: "+ error();
	  throw WriteError(bbb);
	}

    return conn;
}

void SocketOpenNetwork::writeSocket(const string& s) throw(WriteError)
{
   string an="";
    if ( s.size() != static_cast<unsigned>(  write( sock, s.c_str(), s.size() ))){
       CloseSocket();
	an ="Error write to socket: "+error() ;
        throw WriteError( an);
        }
    MONSYS_DEBUG <<" Write Socket (HEX):" << str2hex(s," ") << " ASCII:"<< str2ascii(s) << " RAW:" << s <<endl;
}

int SocketOpenNetwork::SendToSocket(const string& s)
{

    if ( s.size() != static_cast<unsigned>(  write( sock, s.c_str(), s.size() ))){
		return 0;
        }
return 1;
}

/* ������ �� ������ size_t ���� � �������  timeout � ��� �������� */
string SocketOpenNetwork::RecvSocket( size_t size, const timespec& timeout)
                         throw ( runtime_error)
{
    size_t readed = 0;
    char c;
    char lin[1500];
    string buf="", bbb;
    int to = milisec( timeout);
    pollfd fdset;
    fdset.fd = sock;
    fdset.events = POLLIN;
    fdset.revents = 0;
    int ttt=0;

        ttt=poll( &fdset, 1, to);

          if ( ttt == 1) {

            if ( !(fdset.revents & POLLIN)) //XXX paranoia
                throw runtime_error( "poll error");

            int re = recv(sock,lin,sizeof(lin)-1,0 );
			 CloseSocket();

            do {
              buf=buf+lin[readed];
              readed++;
            } while(size != readed && re != readed );

            if ( re < size){
              bbb = " Answer isn't complete from remote serial port (timeout), reded byts:" + int2str(re) +  " HEX:"+str2hex(buf," ") + " ASCII:"+buf ;
				throw ReadError( bbb );
        	 }
MONSYS_DEBUG <<" Read from remote serial port (" << buf.size()<< "):" << str2hex(buf," ") << " ASCII:"<< str2ascii(buf) << " RAW:" << buf <<endl;
            return buf;
          }

          if ( ttt == 0) {
			CloseSocket();
			  bbb = " No answer from remote serial port (timeout)";
            throw ReadError( bbb);
            return buf;
          }
          CloseSocket();
          MONSYS_ERROR << "Error poll: " << error()<< endl;
    return buf;
}

string SocketOpenNetwork::RSocket( size_t size, const timespec& timeout, const char& st)
                         throw ( runtime_error)
{
    size_t readed = 0;
    char c;
    char lin[1500];
    string buf,bbb;
    int to = milisec( timeout);
//    int to = 100;
    pollfd fdset;
    fdset.fd = sock;
    fdset.events = POLLIN;
    fdset.revents = 0;


        int ttt=0;
        ttt=poll( &fdset, 1, to);
          if ( ttt == 1) {
            if ( !(fdset.revents & POLLIN)) //XXX paranoia
                throw runtime_error( "poll error");

            int re = recv(sock,lin,sizeof(lin)-1,0 );
			 CloseSocket();

            string lin_str(lin,0,sizeof(lin));
            if ( re < size){
        	  bbb = " Answer isn't complete from remote serial port (timeout): " +lin_str ;
        	 throw ReadError( bbb );
        	 }
            buf="";
            do {
			   c=lin[readed];
              buf=buf+c;
              readed++;
            } while(size != readed && c != st );
            return buf;
          }
          if ( ttt == 0) {
			CloseSocket();
            an=""; unsigned char b;
            for (char i=0;i<buf.size();i++){
            b=buf[i];
            an=an+int2hex(b,2)+" ";
            }

            MONSYS_DEBUG << " RecvSocket(timeout): "<<an << " readed: "<< readed << endl;

			  bbb = " No answer from remote serial port (timeout)" +buf ;
            throw ReadError( bbb);
            return buf;
          }
          CloseSocket();
          MONSYS_ERROR << "Error poll: " << error()<< endl;
    return buf;
}

string SocketOpenNetwork::ReadSocket(  const timespec& timeout)
                         throw ( runtime_error)
{
    size_t readed = 0;

    char lin[1500];
    string buf,bbb;
    int to = milisec( timeout);

    pollfd fdset;
    fdset.fd = sock;
    fdset.events = POLLIN;
    fdset.revents = 0;

        int ttt=0;

        ttt=poll( &fdset, 1, to);

          if ( ttt == 1) {

            if ( !(fdset.revents & POLLIN)) //XXX paranoia
                throw runtime_error( "poll error");

            int re = recv(sock,lin,sizeof(lin)-1,0 );
			 CloseSocket();

            buf="";
            do {
              buf=buf+lin[readed];
              readed++;
            } while(readed != re );

            return buf;

          }
          if ( ttt == 0) {
			CloseSocket();
            an=""; unsigned char b;
            for (char i=0;i<buf.size();i++){
            b=buf[i];
            an=an+int2hex(b,2)+" ";
            }

            MONSYS_DEBUG << " ReadSocket(timeout): "<<an << " readed: "<< readed << endl;

			  bbb = " No answer from remote serial port (timeout)" +buf ;
            throw ReadError( bbb);
            return buf;
          }
          CloseSocket();
          MONSYS_ERROR << "Error poll: " << error()<< endl;
    return buf;
}

string SocketOpenNetwork::ReadNoCloseSocket(  const timespec& timeout)
                         throw ( runtime_error)
{
    size_t readed = 0;

    char lin[1500];
    string buf,bbb;
    int to = milisec( timeout);

    pollfd fdset;
    fdset.fd = sock;
    fdset.events = POLLIN;
    fdset.revents = 0;

        int ttt=0;

        ttt=poll( &fdset, 1, to);

          if ( ttt == 1) {

            if ( !(fdset.revents & POLLIN)) //XXX paranoia
                throw runtime_error( "poll error");

            int re = recv(sock,lin,sizeof(lin)-1,0 );
//             CloseSocket();

            buf="";
            do {
              buf=buf+lin[readed];
              readed++;
            } while(readed != re );

            return buf;
          }
          if ( ttt == 0) {
            CloseSocket();
            an=""; unsigned char b;
            for (char i=0;i<buf.size();i++){
            b=buf[i];
            an=an+int2hex(b,2)+" ";
            }

            MONSYS_DEBUG << "ReadNoCloseSocket(timeout): "<<an << " readed: "<< readed << endl;

              bbb = " No answer from remote serial port (timeout)" +buf ;
            throw ReadError( bbb);
            return buf;
          }
          CloseSocket();
          MONSYS_ERROR << "Error poll: " << error()<< endl;
    return buf;
}


string SocketOpenNetwork::ReadOpenSocket( size_t size, const timespec& timeout)
                         throw ( runtime_error)
{
    size_t readed = 0;
    char c;
    char lin[1500];
    string buf="", bbb;
    int to = milisec( timeout);
    pollfd fdset;
    fdset.fd = sock;
    fdset.events = POLLIN;
    fdset.revents = 0;
    int ttt=0;

        ttt=poll( &fdset, 1, to);

          if ( ttt == 1) {

            if ( !(fdset.revents & POLLIN)) //XXX paranoia
                throw runtime_error( "poll error");

            int re = recv(sock,lin,sizeof(lin)-1,0 );

            do {
              buf=buf+lin[readed];
              readed++;
            } while(size != readed && re != readed );

            if ( re < size){
             CloseSocket();
              bbb = " Answer isn't complete from remote serial port (timeout), reded byts:" + int2str(re) +  " HEX:"+str2hex(buf," ") + " ASCII:"+buf ;
                throw ReadError( bbb );
             }
MONSYS_DEBUG <<" Read from remote serial port (" << buf.size()<< "):" << str2hex(buf," ") << " ASCII:"<< str2ascii(buf) << " RAW:" << buf <<endl;
            return buf;
          }

          if ( ttt == 0) {
            CloseSocket();
              bbb = " No answer from remote serial port (timeout)";
            throw ReadError( bbb);
            return buf;
          }
          MONSYS_ERROR << "Error poll: " << error()<< endl;
    return buf;
}
