
#ifndef MessagePool_h
#define MessagePool_h

#include <map>

class Parameter;
class Message;

class MessagePool
 : public map< Parameter*, Message*> {

public:
    struct less {
        bool operator()( const value_type&, const value_type&);
    };
    struct or_status {
        unsigned short operator()( unsigned short, const value_type&);
    };

    MessagePool();
    timespec diameter();
    timespec center();
    unsigned short status();
    void put( Message*);

private:
    timespec avgTime, maxTime, minTime;
};
#endif
