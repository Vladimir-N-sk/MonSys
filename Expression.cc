#ifdef EXPRESSIONS

#include "common.h"
#include "globals.h"

#include <map>
#include "Expression.h"
#include "Parameter.h"
#include "MessagePool.h"
#include "Message.h"

Message* VHolder::getLastMessage() const throw (Expr::UnfindedVar)
{
    const MessagePool::iterator p =  store->find(param);

    if ( store->end() == p) throw Expr::UnfindedVar();
//    MOND_ERROR << "STRING 3"  << endl;
    return p->second;
}

Message* PHolder::getLastMessage() const throw()
{
//    MOND_ERROR << "STRING 2"  << endl;
    return param->getLM();
}

bool AlarmExpression::evaluate() const
//                        throw (Expr::UnfindedVar)
                        throw (Expr::UnfindedVar, Message::empty)
{

	try {
//MOND_DEBUG << "12 Name: "<<  getLastMessage()->getParameterName()<< "AlarmExpression return getLastMessage()->getAlarm: "<<  getLastMessage()->getAlarm()
//<< " getLastMessage()->Type:"<<  getLastMessage()->getSource()->getProperties()->type
//<< endl;

		if (getLastMessage()->getSource()->getProperties()->type < 4 ) return level == getLastMessage()->getAlarm();
        return level == 2;

	    }
    	catch ( Message::empty& e) {
          return 0;
        }
return 0;
}

double ValueExpression::evaluate() const
                        throw (Expr::UnfindedVar, Message::empty)
{
//    MOND_ERROR << "STRING 4"  << endl;
        return getLastMessage()->getDoubleValue();
}

#endif //EXPRESSIONS
