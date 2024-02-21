
#ifndef SocketNetwork_h
#define SocketNetwork_h

#include <string>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h>
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

#include "common.h"
#include "globals.h"
#include "utils.h"
#include "Time.h"
#include "MTSync.h"
#include "Network.h"
#include "MessageQueue.h"

class SocketNetwork
  : public Network, public Mutex, MessageQueue {

public:
    SocketNetwork( const string&, unsigned);

    virtual ~SocketNetwork();
    void writeSocket( const string&) throw (WriteError);
    int SendToSocket( const string&);
    string RecvSocket( size_t size, const timespec& timeout) throw (runtime_error);
	string ReadSocket( const timespec& timeout)   throw (runtime_error);

  int ConnectSocket();

private:
    unsigned port;
    string address;
    string an;
    int sock;
    struct sockaddr_in addr;
};


class SocketOpenNetwork
  : public Network, public Mutex, MessageQueue {

public:
    SocketOpenNetwork( const string&, unsigned);

    virtual ~SocketOpenNetwork();
    void writeSocket( const string&) throw (WriteError);
    int SendToSocket( const string&);
    string RecvSocket( size_t size, const timespec& timeout)  throw (runtime_error);
    string ReadOpenSocket( size_t size, const timespec& timeout)  throw (runtime_error);
    string RSocket( size_t size, const timespec& timeout, const char& st) throw (runtime_error);
    string ReadSocket( const timespec& timeout)   throw (runtime_error);
    string ReadNoCloseSocket( const timespec& timeout)   throw (runtime_error);

  int ConnectSocket() throw(WriteError);
  void CloseSocket() throw(WriteError);

private:
    unsigned port;
    string address;
    string an, bbb;
    int sock;
    struct sockaddr_in addr;
	struct hostent *hp;
};

#endif
