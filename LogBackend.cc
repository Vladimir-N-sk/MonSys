
#include "LogBackend.h"

const string errmsg = "Not open file ";

JsonBackend::JsonBackend( const string& fName, const string& params)
: fn (fName),
par (params)
{
    stringstream s(par);
    string sens;
    while ( s >> ws >> sens, s) {
        pn.push_back(sens);
    }
}

void JsonBackend::thread()
{

    Message* const msg = wait();

    file.open( fn.c_str(), ios::out|ios::trunc);
    if( !file) throw string(errmsg+fn.c_str());

try {
    file << "echo -n \"{\"" << endl;

    size_t pnsize = pn.size();

    for (size_t i = 0; i < pnsize; i++)
    {
        s_msg = MSGBackend::getMessage(Parameter::getByName(pn.at(i)));

        if (s_msg.size() > 20)
        {
            alarm = hex2<unsigned short>( s_msg.substr( 0, 2));
            status = hex2<unsigned short>( s_msg.substr( 2, 2));
            ts =  hex2<unsigned long long>( s_msg.substr( 5, 16)) ;
            ttt =  nanosec2timespec( hex2<unsigned long long>( s_msg.substr( 5, 16)) ) ;        
            v_msg = trim(s_msg.substr( 21));

            file << "echo -n " <<"\"\\\"" << pn[i].substr(0,pn[i].find('.')) <<"Value"<<"\\\"" << ": "
                <<"\\\"" << v_msg<< "\\\",\""<< endl;
            file << "echo -n " <<"\"\\\"" << pn[i].substr(0,pn[i].find('.'))<<"Alarm"<< "\\\"" << ": " 
                << "\\\"" << alarm<< "\\\",\""<< endl;
            file << "echo -n " <<"\"\\\"" << pn[i].substr(0,pn[i].find('.'))<<"Status"<< "\\\"" << ": "
                << "\\\"" << status<< "\\\",\""<< endl;            
            file << "echo -n " <<"\"\\\"" << pn[i].substr(0,pn[i].find('.'))<<"Date"<< "\\\"" << ": " 
                << "\\\"" << ttt<< "\\\",\""<< endl;
            
            if ( (i+1) != pnsize)
            {
                file << "echo -n " <<"\"\\\"" << pn[i].substr(0,pn[i].find('.'))<<"Time"<< "\\\"" << ": " 
                    << "\\\"" << ts<< "\\\",\""<< endl;
            }
            else
            {
                file << "echo -n " <<"\"\\\"" << pn[i].substr(0,pn[i].find('.'))<<"Time"<< "\\\"" << ": " 
                    << "\\\"" << ts<< "\\\"\""<< endl;
            }
        }
    }

    file << "echo -n \"}\""<< endl;
    file.close();        
    } catch ( Message::empty& e) {}


    int res=chmod(fn.c_str(), S_IRWXO| S_IRWXU | S_IRWXG );

    if (res != 0)
    {
        res = errno;
        MONSYS_ERROR << "Chmod file " << fn.c_str()<<" error: " << fn << endl;
    }
    msg->decUses();
}

JsonBackend::~JsonBackend()
{}

LogBackend::LogBackend( const string& fName)
{
    file.open( fName.c_str(), ios::out|ios::app);
    if( !file) throw string(errmsg+fName.c_str());
    MONSYS_DEBUG << "Write to: " << fName << endl;
}

void LogBackend::thread()
{
    Message* const msg = wait();

    if( !file) throw errmsg;

    try {
        file << msg->getParameterName() << "\tat ";
        file << msg->getTextTime() << " : ";
        file << msg->getAlarm() << " : ";
        file << msg->getStatus() << " :";
        file << msg->getTextValue();
    }
    catch ( Message::empty& e) {
    }
    file << endl;

    msg->decUses();
}
