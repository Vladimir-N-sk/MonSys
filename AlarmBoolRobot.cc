#ifdef EXPRESSIONS


#include "AlarmBoolRobot.h"

void PoolRobot::automat( Message* m)
{

    pool.put( m);

    setPStatus( pool.status());

    if (pool.status()) {
      S_BAD();
    } else {
      SOK();
    }

    try {
        sendMessage();
    }
    catch ( Expr::UnfindedVar& e) {  }
    catch ( Message::empty& e) {
// Original  17/07/2015        new Message( this)->post();//pool.center() XXX

                if ( 1 == getProperties()->type ) {
                   Message* const n = new Message(this);
                   n->post();                    
                }
                else {
                   Message* const n = new Message(this, "");
                   n->post();
                }
    }
}

ArithmeticRobot::ArithmeticRobot( const string& name, const Address& a,
                                Network* net, const string& str_expr)
                                throw ( Parameter::FindError, runtime_error)
  : PoolRobot( name,a,net), 
   expr( Expression<double>::make( &pool, str_expr))
{
}

void ArithmeticRobot::sendMessage()
{
    Message * const n = new Message( this, expr->evaluate(), pool.center());
    n->post();
  MONSYS_DEBUG << "ArithmeticRobot name: "<< n->getParameterName()<< " value: "<< n->getTextValue() <<  endl;    
}

void AlarmBoolRobot::sendMessage()
{
 unsigned ca = alarms.examine();

    ca = (ca == MAXALARM) ? NOALARM:ca;

    Message* const tmpm
            = new Message( this, alarmMessages[ca], pool.center());
    tmpm->setAlarm( ca);
    tmpm->post();
}


void AlarmBoolRobot::pushAlarm( unsigned n, const string& s, const string& e)
                                 throw ( Parameter::FindError, runtime_error)
{
    alarms.push( n, Expression<bool>::make( &pool, e));
    alarmMessages[n] = s;
  }

AlarmBoolRobot::AlarmBoolRobot( const string& name, const Address& a,
                                Network* net, const string& ns)
 : PoolRobot( name,a,net)
{
    alarmMessages[NOALARM] = ns;
}

#endif