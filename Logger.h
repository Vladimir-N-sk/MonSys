
#ifndef Logger_h
#define Logger_h

#include <ostream>
#include <sstream>


class Loggerbuf
 : public std::stringbuf {

public:
    Loggerbuf( int);
    ~Loggerbuf();

protected:
    virtual int sync();

private:
    static const std::string ident;
    static const int logopt, facility;
    static int logs;
    Loggerbuf();
    const int level;
    bool opend;
};

class Logger
 : public std::ostream {

  public:
    Logger( int);
    virtual ~Logger();

  private:
    Logger();
};
#endif
