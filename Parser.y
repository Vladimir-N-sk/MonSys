%token BOOL DOUBLE DVARIABLE BVARIABLE

%left OR
%left AND
%nonassoc '>' '<' EQ NEQ NL NG
%left '+' '-'
%left '*' '/'

%{

// $Id: Parser.y,v 1.5 2002/12/16 05:48:58 denis Exp $

#ifdef EXPRESSIONS

#include "common.h"
#include "globals.h"
#include "Parser.h"
#include "Expression.h"
#include "Operator.h"
#include <functional>

#ifdef FreeBSD
    int yylex();
    void yyerror( const char*);
#endif

extern Expr* newexpr;
//#define YYDEBUG 1

#endif //EXPRESSIONS
%}

%%

statement:
    expr                      {
#ifdef EXPRESSIONS
    newexpr = $1; }
    ;

expr:
    bexpr
    | dexpr
    ;

dexpr:
    DVARIABLE
    | DOUBLE
    | '-' dexpr %prec '*'       { $$ =
                new UExpression< double, negate<double>, double>( $2); }
    | dexpr '+' dexpr           { $$ =
                new BExpression< double, plus<double>, double>( $1, $3); }
    | dexpr '-' dexpr           { $$ =
                new BExpression< double, minus<double>, double>( $1, $3); }
    | dexpr '*' dexpr           { $$ =
                new BExpression< double, multiplies<double>, double>( $1, $3); }
    | dexpr '/' dexpr           { $$ =
                new BExpression< double, divides<double>, double>( $1, $3); }
    | '(' dexpr ')'             { $$ = $2; }
    ;

bexpr:
    BVARIABLE
    | BOOL
    | '!' bexpr %prec '*'       { $$ =
                 new UExpression< bool, logical_not<bool>, bool>( $2); }
    | bexpr OR bexpr            { $$ =
                 new BExpression< bool, logical_or<bool>, bool>( $1, $3); }
    | bexpr AND bexpr           { $$ =
                 new BExpression< bool, logical_and<bool>, bool>( $1, $3); }
    | dexpr '>' dexpr           { $$ =
                 new BExpression< bool, greater<double>, double>( $1, $3); }
    | dexpr '<' dexpr           { $$ =
                 new BExpression< bool, less<double>, double>( $1, $3); }
    | dexpr EQ dexpr            { $$ =
                 new BExpression< bool, equal_to<double>, double>( $1, $3); }
    | dexpr NEQ dexpr           { $$ =
                 new BExpression< bool, not_equal_to<double>, double>( $1, $3); }
    | dexpr NL dexpr            { $$ =
                    new BExpression< bool, greater_equal<double>, double>( $1, $3); }
    | dexpr NG dexpr            { $$ =
                    new BExpression< bool, less_equal<double>, double>( $1, $3); }
    | '(' bexpr ')'             {
    $$ = $2;
#endif //EXPRESSIONS
                                }
    ;

%%

