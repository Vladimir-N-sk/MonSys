//  $Id: Config.cc,v 1.21 2002/11/21 05:18:55 denis Exp $

#include "common.h"
#include "globals.h"
#include "utils.h"

#include <istream>
#include <string>
#include <set>
#include <sstream>

#include "Config.h"

const Config *Config::config; //XXX

string& Config::Properties::operator[]( const string& pname)
{
    return prop[pname];
}

string Config::Properties::findRecursively( const string& pname,
                                            set< string> sawList)
                               const throw (Config::NoSuchProp, runtime_error)
{

//        MOND_DEBUG << "!1 Call findRecursively" <<endl;

    const string current = getName();


    if ( sawList.find( current) != sawList.end())
        throw runtime_error( "loop in config detected: " + current);
    map< string, string>::const_iterator p;

    if ( prop.end() != ( p=prop.find(pname)))
        return trim( p->second);

    if ( prop.end() != ( p=prop.find("extends"))) {

        sawList.insert( current);
        stringstream parents( p->second);
        string val;
        while ( parents >> ws >> val) {
            try {
                return (*Config::config)[val].findRecursively( pname, sawList);
            }
            catch (Config::NoSuchProp e) {
            }
        }
    }

     throw Config::NoSuchProp( pname, current);
}

string Config::Properties::operator[]( const string& pname)
                               const throw (Config::NoSuchProp, runtime_error)
{
    set< string> sawList;
    return findRecursively( pname, sawList);
}

Config::Properties& Config::operator[]( const string& oname)
                                              throw (runtime_error)
{
//MOND_DEBUG << "!3 Call Config::Properties& Config::operator[]" <<endl;
    Objects::iterator p;
    if ( prop.end() == ( p=prop.find(oname)))
        throw runtime_error( "unknown object: " + oname);
    return p->second;
}

const Config::Properties& Config::operator[]( const string& oname) const
                                              throw (runtime_error)
{
//MOND_DEBUG << "!4 Call const Config::Properties& Config::operator[]" <<endl;
    Objects::const_iterator p;
    if ( prop.end() == (p=prop.find(oname)))
        throw runtime_error( "unknown object: " + oname);
    return p->second;
}

list< string> Config::objects() const
{
MOND_DEBUG << "!5 Call list< string> Config::objects()" <<endl;
    list< string> l;
    for ( Objects::const_iterator b = prop.begin(); b != prop.end(); ++b)
        l.push_back( b->first);

    return l;
}

Config::Config( istream& f)
{
    config = this; //XXX

    string current = "global";
    string s;
    while ( f >> ws >> s, f ) {
//    MOND_DEBUG << "config string:"<< s << endl;
        if ( s[0] == '<' && s[s.size()-1] == '>') {
//            MOND_DEBUG << "<> string from cfg: " << s << endl;
            if ( s[1] == '/' ) {
                current = "global";
            } else {
                if ( current != "global" )
                    throw runtime_error( "not closed config tag: " + current);
                current = s.substr( 1, s.size()-2);
                prop[current].name = trim( current);
//            MOND_DEBUG << "prop["<<current<<"].name: " << trim( current) << endl;
            }
        } else {
            string buf;
//            MOND_DEBUG << "string from cfg: " << s << endl;
            if ( s[0] == '#' ) {
//                MOND_DEBUG << "comment, ignored" << endl;
                getline( f, buf);
                continue;
            }
            if ( s[s.size()-1] == ':' ) {
                f >> ws;
                getline( f, buf);
                prop[current][s.substr( 0, s.size()-1)] = trim( buf);
//            MOND_DEBUG << "prop["<<current<<"]["<<s.substr( 0, s.size()-1)<<"]:" << trim( buf) << endl;
            } else {
                MOND_ERROR << s << ": noname property, ignored" << endl;
            }
        }
    }
    MOND_DEBUG << "config parsed ok!" << endl;

/*    for ( Objects::const_iterator b = prop.begin(); b != prop.end(); ++b)
    {
//        MOND_DEBUG << "prop-first:" << b->first << " prop-second:" << b->second<< endl;
//        MOND_DEBUG << "prop-first:" << b->first  <<endl;
    }
*/

    MOND_DEBUG << "End read config" << endl;
}

Config::~Config()
{
}

