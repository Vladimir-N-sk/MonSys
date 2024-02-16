
#ifndef Config_h
#define Config_h

#include <string>
#include <list>
#include <map>
#include <set>

class Config {
public:

    class NoSuchProp
     : public runtime_error {

    public:
        NoSuchProp( const string& pn, const string& n) 
         : runtime_error( "unknown property: " + pn + " of: " + n) {}                  
    private:
        NoSuchProp();
    };

    class Properties {

    friend class Config;

    public:
        string getName() const
        {
            return name;
        }
        string operator[]( const string&)
                               const throw (NoSuchProp, runtime_error);
        string findRecursively( const string&, set< string>)
                               const throw (NoSuchProp, runtime_error);

    private:
        string& operator[]( const string&);
        map< string, string> prop;
        string name;
    };

    Config( istream&);
    ~Config();

    static const Config *config; //XXX

    list< string> objects() const;
    const Properties& operator[]( const string&) const throw(runtime_error);

private:
    Properties& operator[]( const string&) throw(runtime_error);
    typedef map< string, Properties > Objects;
    Objects prop;
};

#endif
