
#ifndef NetworkA_h
#define NetworkA_h

#include <functional>
#include <algorithm>
#include <stdexcept>
	

#include "Backend.h"
#include "Registry.h"

class Parameter;

class Network
 : public Backend {

public:
//    Network::Network();
    Network() {};
//    virtual Network::~Network();
    virtual ~Network() {};    

    void thread();
    void attach( Parameter*, Backend*);
    void detach( Parameter*);
    void detach( Parameter*, Backend*);

private:
    //Backend can be pugged/unplugged dynamically
    Registry< Parameter*, Backend*> listeners;
};

class ReadError
 : public runtime_error {
public:
    ReadError( std::string& s) : runtime_error(s) {}
private:
    ReadError();
};

class WriteError
 : public runtime_error {
public:
    WriteError( std::string& s) : runtime_error(s) {}
private:
    WriteError();
};

#endif
