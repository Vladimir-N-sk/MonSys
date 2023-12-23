
#include "common.h"
#include "globals.h"
#include "utils.h"
#include "Time.h"

#include "RndSensor.h"
#include "Message.h"
#include "Message_Body.h"
//#include "S2SMapRobot.h"
#include "Parameter.h"
//#include "ICPSensor.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

Message* RndSensor::getSensorData() throw(exception)
{

    double val;
    Message *m = new Message( this, val=drand48());
MONSYS_DEBUG << "RndSensor:"<< val  <<  endl;    
    return m;
}

RndSensor::RndSensor( const string& n, const Address& a,
                                    Network *m, const timespec& d)
 : RTSensor(n,a,m,ZEROTIME,d)
{
}

/*
Message* RndIntSensor::getSensorData() throw(exception)
{

    int val;
    Message *m = new Message( this, rand() % 10 );

    return m;
}

RndIntSensor::RndIntSensor( const string& n, const Address& a,
                                    Network *m, const timespec& d)
 : RTSensor(n,a,m,ZEROTIME,d)
{
}


Message* ScriptSensor::getSensorData() throw( runtime_error)
{

             Message *msg;
             char* buf = new char[100];
             FILE *ptr;
              int pcl;
//MONSYS_DEBUG << "PCLOSE 1"  <<  endl;
try {

             if ((ptr = popen(s.c_str(), "r")) != NULL)
                     while (fgets(buf, 99, ptr) != NULL)

//                    MONSYS_DEBUG << this->getName() << "Before PCLOSE:"<<  endl;

                pcl =  pclose(ptr);

//                    MONSYS_DEBUG << this->getName() << " After PCLOSE:"<< pcl <<  endl;

               if ( pcl != 0 ) {

               MONSYS_DEBUG << this->getName() << " PCLOSE not NULL: "<< pcl <<  endl;
               exit(1);
             }

     if ( ptr != NULL ) (void) pclose(ptr);

MONSYS_DEBUG << this->getName()<< " Command:" << s << " Result:"<< buf<< endl;

    sss="";
    for (int i=0;buf[i]!=NULL;i++) sss=sss+buf[i];
    delete[] buf;
    sss=trim(sss);

                if ( 1 == getProperties()->type ) {
//                    MONSYS_DEBUG << this->getName() << " Before dec2double: "<< sss< endl;

                    if ( !ICPSensor::checkHEX(sss)) {
                      sss = this->getName()+" Error check HEX answer:"+sss ;
                      throw runtime_error( sss );
                    }

//                    MONSYS_DEBUG << this->getName() << " Before dec2double HEX" << sss <<  endl;

                    msg = new Message( this, dec2<double>(sss));
//                    MONSYS_DEBUG << this->getName() << " After dec2double"<<  endl;
                } else {
//                    MONSYS_DEBUG << this->getName() << " Before new Msg"<<  endl;
                    msg = new Message( this, sss);
//                    MONSYS_DEBUG << this->getName() << " After new Msg"<<  endl;
                }

    }
    catch ( exception& e) {
       throw runtime_error( "Erorr ScriptSensor: " + error());
//        return;
    }
    catch (...) {
//        MONSYS_DEBUG << "unknown exception" << endl;
        throw runtime_error( "unknown error ScriptSensor");
    }

    return msg;
}

ScriptSensor::ScriptSensor( const string& name, const Address& a,
                                    Network *n, const timespec& d, const timespec& t, string r)
 : RTSensor(name,a,n,t,d),
   s(r)
{
}

Message* FileReadSensor::getSensorData() throw( runtime_error)
{

             Message *msg;

             infile.open( s.c_str(), ios::in);

               if ( !infile ) {
               sss = "Error open file: " + s ;
              throw runtime_error( sss );
             }

              try {
                sss="";
                while ( infile.get(ch) )   sss +=ch;
                infile.close();
MONSYS_DEBUG << "Read file " << s << ": Date:"<< sss<< endl;

    sss=trim(sss);

                if ( 1 == getProperties()->type )
                    msg = new Message( this, dec2<double>(sss));
                else
                    msg = new Message( this, sss);

    }
    catch ( exception& e) {
       throw runtime_error( error());
    }
    catch (...) {
        throw runtime_error( "unknown error");
    }

    return msg;
}

FileReadSensor::FileReadSensor( const string& name, const Address& a,
                                    Network *n, const timespec& d, const timespec& t, string r)
 : RTSensor(name,a,n,t,d),
   s(r)
{
}


Message* ConstSensor::getSensorData() throw(exception)
{
//MONSYS_DEBUG << this->getName() << " Const:" << n <<endl;
    return new Message( this, n);
}

ConstSensor::ConstSensor( const string& n, const Address& a,
                                    Network *m, const timespec& d, double r)
 : RTSensor(n,a,m,ZEROTIME,d),
   n( r)
{
}

Message* ConstStringSensor::getSensorData() throw(exception)
{
    return new Message( this, s);
}

ConstStringSensor::ConstStringSensor( const string& n, const Address& a,
                                    Network *m, const timespec& d, string r)
 : RTSensor(n,a,m,ZEROTIME,d),
   s( r)
{
}

//Message* ScriptSensor::getSensorData() throw(exception)

Message* TmpSensor::getSensorData() throw(exception)
{
BBB:
    if ( first) {           // slow start
        first = false;
    } else goto BBB;

    return new Message( this, t);
}

TmpSensor::TmpSensor( const string& n, const Address& a,
                                    Network *m, const timespec& d, string r)
 : RTSensor(n,a,m,ZEROTIME,d),
   t( r),
   first(true)
{
}

//06/12/2011
Message* ConstDiscrSensor::getSensorData() throw(exception)
{

	unsigned D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12,D13,D14,D15;
	unsigned A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15;
	string S0,S1,S2,S3,S4,S5,S6,S7,S8,S9,S10,S11,S12,S13,S14,S15;
	unsigned mask=1;

    const unsigned readed = hex2<unsigned>(str);

//MONSYS_DEBUG << "STR:" << str << "END"<<endl;
//    MONSYS_DEBUG << "From DI :" << readed << endl;

for ( int bit=0; bit<16; bit++  ) {

     switch ( bit) {
    case 0:
        D0 = 0;
	S0 = zeroString;
	A0 = zeroAlarm;
	if (readed & mask) {
		D0 = 1;
		S0 = oneString;
		A0 = oneAlarm;
	}
        break;
    case 1:
        D1 = 0;
	S1 = zeroString;
	A1 = zeroAlarm;
	if (readed & mask) {
		D1 = 1;
		S1 = oneString;
		A1 = oneAlarm;
	}
        break;
    case 2:
        D2 = 0;
	S2 = zeroString;
	A2 = zeroAlarm;
	if (readed & mask) {
		D2= 1;
		S2 = oneString;
		A2 = oneAlarm;
	}
        break;
    case 3:
        D3 = 0;
			S3 = zeroString;
			A3 = zeroAlarm;
			if (readed & mask) {
				D3= 1;
				S3 = oneString;
				A3 = oneAlarm;
			}
        break;
    case 4:
        D4 = 0;
			S4 = zeroString;
			A4 = zeroAlarm;
			if (readed & mask) {
				D4= 1;
				S4 = oneString;
				A4 = oneAlarm;
			}
        break;
    case 5:
        D5 = 0;
			S5 = zeroString;
			A5 = zeroAlarm;
			if (readed & mask) {
				D5= 1;
				S5 = oneString;
				A5 = oneAlarm;
			}
        break;
    case 6:
        D6 = 0;
			S6 = zeroString;
			A6 = zeroAlarm;
			if (readed & mask) {
				D6= 1;
				S6 = oneString;
				A6 = oneAlarm;
			}
        break;
    case 7:
        D7 = 0;
			S7 = zeroString;
			A7 = zeroAlarm;
			if (readed & mask) {
				D7= 1;
				S7 = oneString;
				A7 = oneAlarm;
			}
        break;
    case 8:
        D8 = 0;
			S8 = zeroString;
			A8 = zeroAlarm;
			if (readed & mask) {
				D8=1;
				S8 = oneString;
				A8 = oneAlarm;
			}
        break;
    case 9:
        D9 = 0;
			S9 = zeroString;
			A9 = zeroAlarm;
			if (readed & mask) {
				D9=1;
				S9 = oneString;
				A9 = oneAlarm;
			}
        break;
    case 10:
        D10 = 0;
			S10 = zeroString;
			A10 = zeroAlarm;
			if (readed & mask) {
				D10 = 1;
				S10 = oneString;
				A10 = oneAlarm;
			}
        break;
    case 11:
        D11 = 0;
			S11 = zeroString;
			A11 = zeroAlarm;
			if (readed & mask) {
				D11= 1;
				S11 = oneString;
				A11 = oneAlarm;
			}
        break;
    case 12:
        D12 = 0;
			S12 = zeroString;
			A12 = zeroAlarm;
			if (readed & mask) {
				D12= 1;
				S12 = oneString;
				A12 = oneAlarm;
			}
        break;
    case 13:
        D13 = 0;
			S13 = zeroString;
			A13 = zeroAlarm;
			if (readed & mask) {
				D13=1;
				S13 = oneString;
				A13 = oneAlarm;
			}
        break;
    case 14:
        D14 = 0;
			S14 = zeroString;
			A14 = zeroAlarm;
			if (readed & mask) {
				D14= 1;
				S14 = oneString;
				A14 = oneAlarm;
			}
        break;
    case 15:
        D15 = 0;
			S15 = zeroString;
			A15 = zeroAlarm;
			if (readed & mask) {
				D15= 1;
				S15 = oneString;
				A15 = oneAlarm;
			}
        break;
    default:
        MONSYS_DEBUG << "End of discret number: "<< bit << endl;
        throw runtime_error("unknown discretnumber");
    }

			mask = mask*2;

}

			const int DD0=D0;
			const int DD1=D1;
			const int DD2=D2;
			const int DD3=D3;
			const int DD4=D4;
			const int DD5=D5;
			const int DD6=D6;
			const int DD7=D7;
			const int DD8=D8;
			const int DD9=D9;
			const int DD10=D10;
			const int DD11=D11;
			const int DD12=D12;
			const int DD13=D13;
			const int DD14=D14;
			const int DD15=D15;

			const string SS0=S0;
			const string SS1=S1;
			const string SS2=S2;
			const string SS3=S3;
			const string SS4=S4;
			const string SS5=S5;
			const string SS6=S6;
			const string SS7=S7;
			const string SS8=S8;
			const string SS9=S9;
			const string SS10=S10;
			const string SS11=S11;
			const string SS12=S12;
			const string SS13=S13;
			const string SS14=S14;
			const string SS15=S15;

if ( zeroString == "0" || zeroString == "1" || oneString == "0" || oneString == "1") {

Message* const tmp0 = new Message(&d0, DD0);
tmp0->setAlarm(A0);
tmp0->post();
Message* const tmp1 = new Message(&d1, DD1);
tmp1->setAlarm(A1);
tmp1->post();
Message* const tmp2 = new Message(&d2, DD2);
tmp2->setAlarm(A2);
tmp2->post();
Message* const tmp3 = new Message(&d3, DD3);
tmp3->setAlarm(A3);
tmp3->post();
Message* const tmp4 = new Message(&d4, DD4);
tmp4->setAlarm(A4);
tmp4->post();
Message* const tmp5 = new Message(&d5, DD5);
tmp5->setAlarm(A5);
tmp5->post();
Message* const tmp6 = new Message(&d6, DD6);
tmp6->setAlarm(A6);
tmp6->post();
Message* const tmp7 = new Message(&d7, DD7);
tmp7->setAlarm(A7);
tmp7->post();
Message* const tmp8 = new Message(&d8, DD8);
tmp8->setAlarm(A8);
tmp8->post();
Message* const tmp9 = new Message(&d9, DD9);
tmp9->setAlarm(A9);
tmp9->post();
Message* const tmp10 = new Message(&d10, DD10);
tmp10->setAlarm(A10);
tmp10->post();
Message* const tmp11 = new Message(&d11, DD11);
tmp11->setAlarm(A11);
tmp11->post();
Message* const tmp12 = new Message(&d12, DD12);
tmp12->setAlarm(A12);
tmp12->post();
Message* const tmp13 = new Message(&d13, DD13);
tmp13->setAlarm(A13);
tmp13->post();
Message* const tmp14 = new Message(&d14, DD14);
tmp14->setAlarm(A14);
tmp14->post();
Message* const tmp15 = new Message(&d15, DD15);
tmp15->setAlarm(A15);
tmp15->post();

} else {

Message* const tmp0 = new Message(&d0, SS0);
tmp0->setAlarm(A0);
tmp0->post();
Message* const tmp1 = new Message(&d1, SS1);
tmp1->setAlarm(A1);
tmp1->post();
Message* const tmp2 = new Message(&d2, SS2);
tmp2->setAlarm(A2);
tmp2->post();
Message* const tmp3 = new Message(&d3, SS3);
tmp3->setAlarm(A3);
tmp3->post();
Message* const tmp4 = new Message(&d4, SS4);
tmp4->setAlarm(A4);
tmp4->post();
Message* const tmp5 = new Message(&d5, SS5);
tmp5->setAlarm(A5);
tmp5->post();
Message* const tmp6 = new Message(&d6, SS6);
tmp6->setAlarm(A6);
tmp6->post();
Message* const tmp7 = new Message(&d7, SS7);
tmp7->setAlarm(A7);
tmp7->post();
Message* const tmp8 = new Message(&d8, SS8);
tmp8->setAlarm(A8);
tmp8->post();
Message* const tmp9 = new Message(&d9, SS9);
tmp9->setAlarm(A9);
tmp9->post();
Message* const tmp10 = new Message(&d10, SS10);
tmp10->setAlarm(A10);
tmp10->post();
Message* const tmp11 = new Message(&d11, SS11);
tmp11->setAlarm(A11);
tmp11->post();
Message* const tmp12 = new Message(&d12, SS12);
tmp12->setAlarm(A12);
tmp12->post();
Message* const tmp13 = new Message(&d13, SS13);
tmp13->setAlarm(A13);
tmp13->post();
Message* const tmp14 = new Message(&d14, SS14);
tmp14->setAlarm(A14);
tmp14->post();
Message* const tmp15 = new Message(&d15, SS15);
tmp15->setAlarm(A15);
tmp15->post();

}

    stringstream buf;
  buf << str << " " << "["<<D15 << D14 <<  D13 <<  D12 << " "<< D11 << D10 << D9 << D8 << " "<< D7 << D6 << D5 << D4 << " "<< D3 << D2 << D1 << D0<<"]" ;

        return new Message( this, buf.str());
}

ConstDiscrSensor::ConstDiscrSensor( const string& n, const Address& a,
                                    	Network *m, const timespec& d, string r,
					const string& zero, const string& one,
					unsigned za, unsigned oa)
 : RTSensor(n,a,m,ZEROTIME,d),
   str( r),
   zeroString(zero),
   oneString(one),
   zeroAlarm(za),
   oneAlarm(oa),
   d0( "d0." + n, a + "0", m),
   d1( "d1." + n, a + "1", m),
   d2( "d2." + n, a + "2", m),
   d3( "d3." + n, a + "3", m),
   d4( "d4." + n, a + "4", m),
   d5( "d5." + n, a + "5", m),
   d6( "d6." + n, a + "6", m),
   d7( "d7." + n, a + "7", m),
   d8( "d8." + n, a + "8", m),
   d9( "d9." + n, a + "9", m),
   d10( "d10." + n, a + "A", m),
   d11( "d11." + n, a + "B", m),
   d12( "d12." + n, a + "C", m),
   d13( "d13." + n, a + "D", m),
   d14( "d14." + n, a + "E", m),
   d15( "d15." + n, a + "F", m)

{
}
*/