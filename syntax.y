%{
 #include "lex.yy.c"
 #include <stdio.h>
%}

/*declared tokens*/
%token INT FLOAT ID
%token PLUS MINUS 
%token STAR DIV
%token SEMI COMMA ASSIGNOP RELOP AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE
%%

Program: ExtDefList
	;
ExtDefList: ExtDef ExtDefList
	|
	;
ExtDef: Specifier ExtDecList SEMI
	| Specifier SEMI
	| Specifier FunDec CompSt
	;
ExtDecList: VarDec
	| VarDec COMMA ExtDecList
	;

/*Specifiers*/

Specifier: TYPE
	| StructSpecifier
	;
StructSpecifier: STRUCT OpTag LC DefList RC
	| STRUCT Tag
	;
OpTag: ID
	|
	;
Tag: ID
	;

/*declarators*/

VarDec: ID
	| VarDec LB INT RB
	;
FunDec: ID LP VarList RP
	| ID LP RP
	;
VarList: ParamDec COMMA VarList
	| ParamDec
	;
ParamDec: Specifier VarDec
	;

/*statements*/

CompSt: LC DefList StmtList RC
	;
StmtList: Stmt StmtList
	|
	;
Stmt: Exp SEMI
	| CompSt
	| RETURN Exp SEMI
	| IF LP Exp RP Stmt ELSE Stmt
	| WHILE LP Exp RP Stmt
	;

/*Local Definitions*/

DefList: Def DefList;
	|
	;
Def: Specifier DecList SEMI
	;
DecList: Dec
	|
	;
Dec: VarDec
	| VarDec ASSIGNOP Exp
	;

/*Expressions*/

Exp: Exp ASSIGNOP Exp
	| Exp AND Exp
	| Exp OR Exp
	| Exp RELOP Exp
	| Exp PLUS Exp
	| Exp MINUS Exp
	| Exp STAR Exp
	| Exp DIV Exp
	| LP Exp RP
	| MINUS Exp
	| NOT Exp
	| ID LP Args RP
	| ID LP RP
	| Exp LB Exp RB
	| Exp DOT ID
	| ID
	| INT
	| FLOAT
	;
Args: Exp COMMA Args
	| Exp
	;