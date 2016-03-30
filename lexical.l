%{
#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "marco.h"
#define YYSTYPE node_t*
#include "syntax.tab.h"
node_t* newNode();
#define handle(token) do { \
   yylval = newToken(token); \
   printf("Line:%d Token:%s Symbol:%s\n",yylineno,toArray(str(token)),yytext); \
   yylval->name = toArray(str(token)); \
   return token; \
} while(0)
node_t* newToken(int token)
{
	node_t* r=newNode();
	r->lineno=yylineno;
	r->token=token;
	r->terminalFlag=1;
	//terminal symbol
	r->text=toArray(yytext);
	return r;
}
%}

%option yylineno

digit [0-9]
digitNoZ [1-9]
letter [a-zA-Z]
letterg _|{letter}
INT 0|({digitNoZ}{digit}*)
NUM_8 0[0-9]+
NUM_16 0[xX]({digit}|{letter})+
FLOAT_NoE {digit}+\.{digit}+
ID ({letterg})({letterg}|{digit})*
SPACE [ \t\n]
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while

%%

{SEMI} {handle(SEMI);}
{COMMA} {handle(COMMA);}
{ASSIGNOP} {handle(ASSIGNOP);}
{RELOP} {handle(RELOP);}
{PLUS} {handle(PLUS);}
{MINUS} {handle(MINUS);}
{STAR} {handle(STAR);}
{DIV} {handle(DIV);}
{AND} {handle(AND);}
{OR} {handle(OR);}
{DOT} {handle(DOT);}
{NOT} {handle(NOT);}
{TYPE} {handle(TYPE);}
{LP} {handle(LP);}
{RP} {handle(RP);}
{LB} {handle(LB);}
{RB} {handle(RB);}
{LC} {handle(LC);}
{RC} {handle(RC);}
{STRUCT} {handle(STRUCT);}
{RETURN} {handle(RETURN);}
{IF} {handle(IF);}
{ELSE} {handle(ELSE);}
{WHILE} {handle(WHILE);}
{ID} {handle(ID);}
{INT} {handle(INT);}
{NUM_16} {handle(INT);}
{NUM_8} {handle(INT);}
{FLOAT_NoE} {handle(FLOAT);}
{SPACE} {}
. {
printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);
}

%%
