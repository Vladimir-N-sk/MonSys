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
    return p->second;
}

Message* PHolder::getLastMessage() const throw()
{
    return param->getLM();
}

bool AlarmExpression::evaluate() const
                        throw (Expr::UnfindedVar, Message::empty)
{

	try {
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
        return getLastMessage()->getDoubleValue();
}

#endif //EXPRESSIONS
