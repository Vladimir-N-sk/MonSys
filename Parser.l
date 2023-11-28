PNAME [_a-zA-Z]+([_.0-9a-zA-Z])*
BLANK [ \t]
INTEGER [0-9]+

%{

    #include "common.h"
    #include "globals.h"
    #include "utils.h"
    #include "Parser.tab.h"
    #include "Parser.h"
    #include "Parameter.h"
    #include "MessagePool.h"
    #include "Expression.h"

    extern YYSTYPE yylval;

    void yyerror( const char*);
    void yyerror( const string& s);
    int yyparse();
    #ifdef EXPRESSIONS
    Expr* newexpr;
    MessagePool* store;
    Parameter* param;
    double* pp;
    #endif //EXPRESSIONS
%}

%%

"true" {
    #ifdef EXPRESSIONS
    yylval = new CExpression<bool>( true);
    #endif //EXPRESSIONS
    return BOOL;
    }

"false" {
    #ifdef EXPRESSIONS
    yylval = new CExpression<bool>( false);
    #endif //EXPRESSIONS
    return BOOL;
    }

{INTEGER}_{PNAME} {
    #ifdef EXPRESSIONS
    const string vn = string(yytext);
    if ( NULL == store )
        throw runtime_error( "variable: " + vn + " can't be used");

    const unsigned level = dec2<unsigned>( cut( 0, "_", vn));

    if ( MAXALARM <= level)
        throw runtime_error( "MAXALARM overflow: " + vn);

    yylval = new AlarmVExpression( level, store,
                           Parameter::get( cut( 1, "_", vn)));
    #endif //EXPRESSIONS
    return BVARIABLE;
    }

{PNAME} {
    #ifdef EXPRESSIONS
    yylval = new ValueVExpression( store, Parameter::get(yytext));
    #endif //EXPRESSIONS
    return DVARIABLE;
    }

\$ {
    #ifdef EXPRESSIONS
    if ( param != NULL)
        yylval = new ValuePExpression( param);
    else
        yylval = new ValueCExpression( pp);
    #endif //EXPRESSIONS
    return DVARIABLE;
    }

[0-9]+\.?[0-9]*(e[+-][0-9]+)? {
    #ifdef EXPRESSIONS
    yylval = new CExpression<double>( dec2<double>( yytext));
    #endif //EXPRESSIONS
    return DOUBLE;
    }

"||" { return OR; }

"&&" { return AND; }

"==" { return EQ; }

"!=" { return NEQ; }

"<=" { return NG; }

">=" { return NL; }

[-+()/*<>!] { return *yytext; }

{BLANK}     { ;}

.   yyerror( string("invalid character :") + yytext);

%%

int yywrap(void)
{
    return 1;
}

void yyerror( const char *s)
{
    throw runtime_error( s);
}

void yyerror( const string& s)
{
    throw runtime_error( s);
}

#ifdef EXPRESSIONS
Expr* makeExpr( MessagePool* initstore, string s)
{
    store = initstore;
    YY_BUFFER_STATE buf = yy_scan_string( s.c_str()); //XXX lack of efficiency
    yy_switch_to_buffer( buf);
    yyparse();
    yy_delete_buffer( buf);
    return newexpr;
}

Expr* makeExpr( Parameter* p, string s)
{
    store = NULL;
    param = p;
    YY_BUFFER_STATE buf = yy_scan_string( s.c_str()); //XXX lack of efficiency
    yy_switch_to_buffer( buf);
    yyparse();
    yy_delete_buffer( buf);
    return newexpr;
}

Expr* makeExpr( double* p, const string& s)
{
    store = NULL;
    param = NULL;
    pp = p;
    YY_BUFFER_STATE buf = yy_scan_string( s.data()); //XXX lack of efficiency
    yy_switch_to_buffer( buf);
    yyparse();
    yy_delete_buffer( buf);
    return newexpr;
}

#endif //EXPRESSIONS
