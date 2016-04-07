%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "marco.h"
#define YYSTYPE node_t*
#include "syntax.tab.h"
node_t* newNode();
extern int is_error;
// printf("Line:%d Token:%s Symbol:%s\n",yylineno,toArray(str(token)),yytext); 
#define handle(token) do { \
   yylval = newToken(token); \
   yylval->name=toArray(str(token)); \
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
{NUM_16} {checkNUM_16(yytext,yylineno); handle(INT);}
{NUM_8} {checkNUM_8(yytext,yylineno);handle(INT);}
{FLOAT_NoE} {handle(FLOAT);}
{SPACE} {}
. {
printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);
}

%%

int checkNUM_8(char* num,int lineno)
{
	int len=strlen(num);
	int i;
	for(i=1;i<len;i++)
	{
		if(num[i]=='8'||num[i]=='9')
		{ 
			printf("Error type A at Line %d: Illegal octal number '%s'\n", lineno, num);
			return 0;
		}
	}
	return 1;
}

int checkNUM_16(char* num,int lineno)
{
	int len=strlen(num);
	int i;
	for(i=1;i<len;i++)
	{
		if(num[i]>='g'&& num[i]<='z')
		{
			is_error=1;
			printf("Error type A at Line %d: Illegal hexadecimal number '%s'\n", lineno, num); 
			return 0;
		}
		if(num[i]>='G'&& num[i]<='Z') 
		{
			is_error=1;
			printf("Error type A at Line %d: Illegal octal number '%s'\n", lineno, num);
			return 0;
		}
	}
	return 1;
}