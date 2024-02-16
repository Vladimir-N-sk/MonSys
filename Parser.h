
#ifndef Parser_h
#define Parser_h

class Expr;

typedef Expr *expr_ptr;

#define YYSTYPE expr_ptr

class MessagePool;
class Parameter;
class Message;

Expr* makeExpr( MessagePool* , string);
Expr* makeExpr( Parameter* , string);
Expr* makeExpr( double* , const string&);

#endif
