
#ifdef EXPRESSIONS

#ifndef Expression_h
#define Expression_h

class Parameter;
class MessagePool;

#include "Message.h"
// orig 
#include "Parser.h"

class Expr {
public:
    virtual ~Expr() {}
    class UnfindedVar {};
};

#include "Parser.h"

template <typename R>
class Expression
 : public virtual Expr {

public:

    template<typename I>
    static Expression<R>* make( I i, const string& s)
    {
        Expression<R>* e = dynamic_cast<Expression<R>*>( ::makeExpr( i, s));
        if ( NULL == e)
            throw runtime_error( "wrong expression type: " + s);
        return e;
    }
    virtual R evaluate() const throw (Expr::UnfindedVar, Message::empty) =0;
};

template <typename R>
class CExpression
 : public Expression<R> {

public:
    CExpression( const R i)
        : c(i) {}
    R  evaluate() const throw ()
    {
        return c;
    }

private:
    const R c;
};

template <typename R, typename OP, typename A>
class UExpression
 : public Expression<R> {

public:
    UExpression( Expression<A>*a)
        : arg (a) {}
    UExpression( Expr*a)
        : arg ( dynamic_cast<Expression<A>*>(a)) {}

    R  evaluate() const throw (Expr::UnfindedVar, Message::empty)
    {
        return op( arg->evaluate());
    }


private:
    UExpression();
    const auto_ptr< Expression<A> > arg;
    OP op;
};

template <typename R, typename OP, typename A>
class BExpression
 : public Expression<R> {

public:
    BExpression( Expression<A>*a, Expression<A>*b)
         : larg(a), rarg(b) {}
    BExpression( Expr*a, Expr*b)
         : larg(dynamic_cast<Expression<A>*>(a)),
           rarg(dynamic_cast<Expression<A>*>(b)) {}

    R evaluate() const throw (Expr::UnfindedVar, Message::empty)
    {
        return op( larg->evaluate(), rarg->evaluate());
    }

private:
    BExpression();
    const auto_ptr< Expression<A> > larg, rarg;
    OP op;
};

class Holder {
protected:
    virtual Message* getLastMessage() const =0;
};

class VHolder
 : public virtual Holder {
public:
    VHolder( MessagePool* s, Parameter* n)
        : param(n), store (s) {}

protected:
    Message* getLastMessage() const throw (Expr::UnfindedVar);

private:
    VHolder();
    Parameter *const param;
    MessagePool *const store;
};

class PHolder
 : public virtual Holder {
public:
    PHolder( Parameter* p)
        : param(p) {}

protected:
    Message* getLastMessage() const throw();

private:
    PHolder();
    Parameter* param;
};

class AlarmExpression
 : public Expression<bool>, virtual Holder {
public:
    AlarmExpression( unsigned l)
        : level( l) {}
//    bool evaluate() const throw (Expr::UnfindedVar);
    bool evaluate() const throw (Expr::UnfindedVar, Message::empty);
private:
    unsigned level;
};

class ValueExpression
 : public Expression<double>, virtual Holder {
public:
    double evaluate() const throw (Expr::UnfindedVar, Message::empty);
};

    /***
     *    last message of parameter experssion
     */

class ValuePExpression
 : public ValueExpression, virtual PHolder {
public:
    ValuePExpression( Parameter* n)
        : ValueExpression(),
          PHolder( n) {}
private:
    ValuePExpression();
};

    /***
     *    double variable address based expression
     */

class ValueCExpression
 : public Expression<double> {
public:
    ValueCExpression( double* n)
        : ptr( n) {}
    virtual double evaluate() const throw ()
    {
        return *ptr;
    }
private:
    ValueCExpression();
    double * const ptr;
};

    /***
     *    MessagePool based expressions
     */

class ValueVExpression
 : public ValueExpression, virtual VHolder {
public:
    ValueVExpression( MessagePool* s, Parameter* n)
        : ValueExpression(),
          VHolder( s, n) {}
private:
    ValueVExpression();
};

class AlarmVExpression
 : public AlarmExpression, virtual VHolder {
public:
    AlarmVExpression( unsigned l, MessagePool* s, Parameter* n)
        : AlarmExpression( l),
          VHolder( s, n) {}
private:
    AlarmVExpression();
};


#endif
#endif //EXPRESSIONS
