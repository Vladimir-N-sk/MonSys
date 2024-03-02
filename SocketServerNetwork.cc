
#include "common.h"
#include "globals.h"
#include "utils.h"
#include "Time.h"

#include "MTSync.h"
#include "SocketServerNetwork.h"
//#include "SerialNetwork.h"

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
#include <netinet/tcp.h>
#include <arpa/inet.h>



#include <fstream>
#include <memory>

//SocketServerNetwork::SocketServerNetwork( const string& ah, unsigned por)

SocketServerNetwork::SocketServerNetwork(unsigned por)
 : port(por)
 //address(ah)
{

    struct hostent *hp;
    char *endptr;
    char **p;


}


int SocketServerNetwork::GetSocketUDP() throw ( runtime_error)
{

    const int on = 1;
    const int size_buf = 900;

    bzero( &addr, sizeof(addr));
    addr_udp.sin_family = AF_INET;
    addr_udp.sin_addr.s_addr = htonl( INADDR_ANY );
    addr_udp.sin_port = htons( port);

struct sockaddr_in * names = new struct sockaddr_in;
unsigned nameslen = sizeof (struct sockaddr_in);

    sock = socket( AF_INET, SOCK_DGRAM, 0);
    if ( -1 == sock )
        throw runtime_error(" UDP Error open socket: " + error());

//      MOND_DEBUG << " TCP open socket:"<< sock << endl;

    if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, ( char * )&on, sizeof( on ) ) )
        throw runtime_error(" TCP setsockopt SO_REUSEADDR failed - " + error());

    if ( setsockopt( sock, SOL_SOCKET, SO_RCVBUF, ( char * )&size_buf, sizeof( size_buf ) ) )
        throw runtime_error(" UDP setsockopt SO_RCVBUF failed - " + error());


    if ( bind( sock, ( struct sockaddr * ) &addr_udp, sizeof( addr_udp ) ) )
        throw runtime_error(" UDP bind failed - " + error());


    if ( -1 == getsockname(sock, ( struct sockaddr * )names, &nameslen) )
        throw runtime_error(" UDP Error getsockname: " + error());

      MOND_DEBUG << " UDP Listen IP:"<<inet_ntoa(names->sin_addr) << " port:"<< int2str(names->sin_port)  <<" socket:" << sock <<endl;

    return sock;
}


int SocketServerNetwork::GetSocket() throw ( runtime_error)
{

    const int on = 1;
    const int size_buf = 900;

    bzero( &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl( INADDR_ANY );
    addr.sin_port = htons( port);

//   MOND_DEBUG << " TCP Get socket" << endl;

struct sockaddr_in * names = new struct sockaddr_in;
unsigned nameslen = sizeof (struct sockaddr_in);


    sock = socket( AF_INET, SOCK_STREAM, 0);
    if ( -1 == sock )
        throw runtime_error(" TCP Error open socket: " + error());

//      MOND_DEBUG << " TCP open socket:"<< sock << endl;

    if ( setsockopt( sock, SOL_SOCKET, SO_KEEPALIVE, ( char * )&on, sizeof( on ) ) )
        throw runtime_error(" TCP setsockopt SO_KEEPALIVE failed - " + error());


    if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, ( char * )&on, sizeof( on ) ) )
        throw runtime_error(" TCP setsockopt SO_REUSEADDR failed - " + error());


    if ( setsockopt( sock, SOL_SOCKET, SO_RCVBUF, ( char * )&size_buf, sizeof( size_buf ) ) )
        throw runtime_error(" TCP setsockopt SO_RCVBUF failed - " + error());


    if ( setsockopt( sock, SOL_SOCKET, SO_SNDBUF, ( char * )&size_buf, sizeof( size_buf ) ) )
        throw runtime_error(" TCP setsockopt SO_SNDBUF failed - " + error());


    if ( setsockopt( sock, IPPROTO_TCP, TCP_NODELAY, ( char * )&on, sizeof( on ) ) )
        throw runtime_error(" setsockopt TCP_NODELAY failed - " + error());


    if ( bind( sock, ( struct sockaddr * ) &addr, sizeof( addr ) ) )
        throw runtime_error(" TCP bind failed - " + error());


    if (-1 == listen( sock, 5 ))
        throw runtime_error( "TCP Error listen socket: " + error());

    if ( -1 == getsockname(sock, ( struct sockaddr * )names, &nameslen) )
        throw runtime_error(" TCP Error getsockname: " + error());

//      MOND_DEBUG << " TCP Listen socket:" << sock <<endl;
//MOND_DEBUG << " TCP  Listen IP:"<<inet_ntoa(addr->sin_addr) << " port:"<< int2str(addr->sin_port)  <<" socket:" << sock <<endl;
      MOND_DEBUG << " TCP Listen IP:"<<inet_ntoa(names->sin_addr) << " port:"<< int2str(port)  <<" socket:" << sock <<endl;

    return sock;
}

SocketServerNetwork::~SocketServerNetwork()
{
    if (-1 == close(sock))
        throw runtime_error( "Error close socket: " + error());
}

void SocketServerNetwork::CloseSocket(const int& s1) throw ( runtime_error)
{
//    shutdown (s1,1);

    if (-1 == close(s1))
        throw runtime_error( "Error close socket: " + error());
      MOND_DEBUG << " TCP Closed socket:"<< s1 << endl;
}

int SocketServerNetwork::AcceptSocket() throw ( runtime_error)
{

//    struct sockaddr_in peer;
//    int peerlen;
    bzero( &peer, sizeof(peer));

//      MOND_DEBUG << " TCP GOTO AcceptSocket" << endl;

    if (-1 == listen( sock, 5 ))
        throw runtime_error( "Error listen socket: " + error());

//      MOND_DEBUG << " TCP GOTO AcceptSocket listen socket"<< sock << endl;

again:

    int conn  = accept( sock, (struct sockaddr*)&peer, &peerlen );
//      int conn  = accept( sock, (struct sockaddr*)&peer, sizeof(peer) );

      MOND_DEBUG << " Accept, local socket:"<< sock << " client socket:"<< conn << " errno:"<<errno << endl;

    if ( 0 > conn) {
        if (errno == ECONNABORTED)
            goto again;
        else
            throw runtime_error("Error accept socket: " + error() );
    }

    return conn;
}

int SocketServerNetwork::ASock(const int& s1) throw ( runtime_error)
{

again2:
    int conn  = accept( s1, NULL, NULL );
//      MOND_DEBUG << " Accept, local socket:"<< s1 << " client socket:"<< conn << " errno:"<<errno << endl;
    if ( 0 > conn) {
        if (errno == ECONNABORTED)
            goto again2;
        else
            throw runtime_error("Error accept socket: " + error() );
    }

    return conn;
}


void SocketServerNetwork::writeSocket(const string& s, const int& s1) throw(WriteError)
{
   string an="";
    if ( s.size() != static_cast<unsigned>(  write( s1, s.c_str(), s.size() ))){
	an = "Error write to socket: " + error() ;
        throw WriteError(an);
        }

    MOND_DEBUG << "Write to socket: " << str2hex(s," ") << endl;
}


string SocketServerNetwork::RecvSocketUDP(const int& s1)
                         throw ( runtime_error)
{
//    int len;
    char rrr[64];
    string buf;

struct sockaddr_in * name = new struct sockaddr_in;
//int namelen = sizeof (struct sockaddr_in);
unsigned namelen = sizeof (struct sockaddr_in);

/* struct sockaddr_in * names = new struct sockaddr_in;
int nameslen = sizeof (struct sockaddr_in); */


            int re = recvfrom(s1, rrr, sizeof(rrr) ,0, ( struct sockaddr * )name, &namelen );
            if ( re == -1) throw runtime_error("Error read from socket UDP: " + error());
            if ( re == 0) throw runtime_error("Socket closed remote client");


            buf="";
            for (char i=0;i<(re);i++){
            buf=buf+rrr[i];
            }

/*
    if ( -1 == getsockname(s1, ( struct sockaddr * )names, &nameslen) )
        throw runtime_error(" UDP Error getsockname: " + error());
*/

                MOND_DEBUG << "From:"<< inet_ntoa(name->sin_addr)<<":"<< int2str(name->sin_port)<< " = " << str2hex(buf," ") << endl;

            return buf;

}


string SocketServerNetwork::RecvSocket( size_t size, const int& s1)
                         throw ( runtime_error)
{
    size_t readed = 0;
    int cnt = size, re;
    char c;
    char lin[1500];
    char* bp = lin;
    string buf;


    while ( cnt > 0 )
    {

            int re = recv(s1,bp,cnt,0 );
            if ( re == -1) throw runtime_error("Error read from socket: " + error());
//            if ( re == 0) break;
            if ( re == 0) throw runtime_error("Socket closed remote client");
            cnt -= re;
            bp += re;
    }
if ( cnt < 0) throw runtime_error("Error read from socket, readed too more bytes: " + int2str(size+(-1*cnt)) );


            buf="";
            for (char i=0;i<(size-cnt);i++){
            buf=buf+lin[i];
            }

                MOND_DEBUG << "Read from socket: " << str2hex(buf," ") << endl;
            return buf;
}




string SocketServerNetwork::RecvSocketOld( size_t size, const timespec& timeout)
//string SocketServerNetwork::RecvSocketOld( size_t size)
                         throw ( runtime_error)
{
    size_t readed = 0;
    char c;
//    char* lin = new char[1500];
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

            nanosleep(timeout);
            if ( !(fdset.revents & POLLIN)) //XXX paranoia
                throw runtime_error( "poll error");

            int re = recv(sock,lin,sizeof(lin)-1,0 );
            if ( re == -1) throw runtime_error("Error read from socket: " + error());
            if ( re == 0) throw runtime_error("Empty input bufer socket: " + error());

            string lin_str(lin,0,sizeof(lin));
            if ( re < size){
            bbb =" Answer isn't complete from socket (timeout): " +lin_str ;
             throw ReadError( bbb);
             }
            buf="";
            do {
              buf=buf+lin[readed];
              readed++;
            } while(size != readed);
            return buf;
          }
          if ( ttt == 0) {
            an=""; unsigned char b;
            for (char i=0;i<buf.size();i++){
            b=buf[i];
            an=an+int2hex(b,2)+" ";
            }

            MOND_DEBUG << " From RecvSocket(timeout): "<<an << " readed: "<< readed << endl;
	    bbb = " No answer from Socket (timeout)" +buf ;
            throw ReadError( bbb);
            return buf;
          }
          MOND_ERROR << "Error poll: " << error()<< endl;
    return buf;
}


string SocketServerNetwork::ReadSocket( const int& s1)
                         throw ( runtime_error)
{
    size_t readed = 0;
    int re;
    char c;
    char lin[1500];
    char* bp = lin;
    string buf;


//      MOND_DEBUG << " Begin read"<< endl;


            re = read(s1,bp,1500 );
//      MOND_DEBUG << " recv:"<< re<< " cnt:"<< cnt<<" lin[0]:"<< lin[0]<< " lin[1]:"<< lin[1]<< " lin[2]:"<< lin[2] <<endl;
            if ( re == -1) throw runtime_error("Error read from socket: " + error());

            if ( re == 0) MOND_DEBUG << " Client closed connection"<<endl;;

//MOND_DEBUG << " Read from socket:"<< bp <<endl;




            buf=""; an="";
//            unsigned char b;
            for (char i=0;i<re+1;i++){
//            b=lin[i];
            an=an+int2hex(lin[i],2)+" ";
            buf=buf+lin[i];
            }

                MOND_DEBUG << "Read from socket: " << an << endl;


            return buf;

}

