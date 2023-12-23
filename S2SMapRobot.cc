

#include "common.h"
#include "globals.h"
#include "utils.h"

#include "Message.h"
#include "Message_Body.h"
#include "S2SMapRobot.h"

S2SMapRobot::S2SMapRobot( const string& name, const Address& a,
                            Network* net, const S2SMapRobot::Map& m)
 : Robot(name,a,net),
   i2smap(m)
{
}

void S2SMapRobot::automat(Message* m)
{
    setPStatus( m->getStatus() & WEDGED);

    if ( m->getStatus() & WEDGED ) {
      S_BAD();
    } else {
      SOK();
    }


    try {
        const string i = m->getTextValue();

        const S2SMapRobot::Map::const_iterator ip = i2smap.find(i);

        if ( i2smap.end() == ip) {
            MOND_DEBUG << "can not translate " << i << " to string" << endl;
            Message * const n = new Message(this, "No data");
            n->post();
	}
        else {
            Message * const n = new Message(this, ip->second);
            n->setAlarm( m->getAlarm());
            n->post();
MOND_DEBUG << "S2S MsgName:"<<n->getParameterName() << " MsgText:"<<  n->getTextValue() << " MsgAlarm:"<<  n->getAlarm() <<endl;

        }
    }
    catch ( Message::empty& e) {
      Message * const n = new Message(this);
      n->post();
//        new Message( this)->post();
    }
}

D2DMapRobot::D2DMapRobot( const string& name, const Address& a,
                            Network* net, const D2DMapRobot::Map& m)
 : Robot(name,a,net),
   d2dmap(m)
{
}

void D2DMapRobot::automat(Message* m)
{
    setPStatus( m->getStatus() & WEDGED);

    if ( m->getStatus() & WEDGED ) {
      S_BAD();
    } else {
      SOK();
    }

    try {
        const string i = double2str(m->getDoubleValue());
//            MOND_DEBUG << "D2D i: " << i  << " getInt:"<< m->getIntValue()<< " getDouble"<< m->getDoubleValue()<<endl;
        const D2DMapRobot::Map::const_iterator ip = d2dmap.find(i);

        if ( d2dmap.end() == ip) {
            MOND_DEBUG << "can not translate " << i << " to string" << endl;
            Message * const n = new Message(this, "no data");
            n->post();
        } else {
            Message * const n = new Message(this, dec2<double>(ip->second));
            n->setAlarm( m->getAlarm());
            n->post();
        }
    }
    catch ( Message::empty& e) {
      Message * const n = new Message(this);
      n->post();
    }
}


//Timeticks: (486000) 1:21:00.00
Timeticks2TimeStringRobot::Timeticks2TimeStringRobot( const string& name, const Address& a, Network* net)
 : Robot(name,a,net)
{
}

void Timeticks2TimeStringRobot::automat(Message* m)
{
    setPStatus( m->getStatus() & WEDGED);

    if ( m->getStatus() & WEDGED ) {
      S_BAD();
    } else {
      SOK();
    }

    try {
         int tt = m->getIntValue();
         int hour =  tt/360000 ;
         int minute = tt/6000 - hour*60;
         int seconds = tt/100 - hour*3600 - minute*60;
         string str = int2str(hour,2,'0')+":"+int2str(minute,2,'0')+":"+int2str(seconds,2,'0');

            Message * const n = new Message(this, str);
            n->setAlarm( m->getAlarm());
            n->post();

    }
    catch ( Message::empty& e) {
      Message * const n = new Message(this);
      n->post();
    }
}
