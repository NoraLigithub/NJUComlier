%{
 #include "lex.yy.c"
 #include <stdio.h>
 #include "tree.h"
 #include "marco.h"
 #define YYSTYPE node_t*
 int is_error=0;
 int yylex();
 node_t* createTree(int airty, ...);

 #define handleS(root,token,arity, ...) \
 do { \
 	root=createTree(arity,__VA_ARGS__); \
 	root->name=toArray(str(token)); \
 } while(0)

 #define errorhandle(errorToken) \
 do{ \
 	char msg[50]; \
 	sprintf(msg,"missing %s",toArray(errorToken)); \
 	is_error=1; \
 }while(0)

 extern void yyerror(const char* msg);
%}

%define parse.error verbose

/*declared tokens*/

%token INT FLOAT ID
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS 
%left STAR DIV
%right NOT
%left LP RP DOT LB RB
%token LC RC
%token TYPE STRUCT RETURN IF WHILE
%token SEMI COMMA
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%

Program: ExtDefList
	{
	handleS($$,Program,1,$1);
	if(is_error==0)
		printTree($$,0);
	}
	;
ExtDefList: ExtDef ExtDefList
	{
	handleS($$,ExtDefList,2,$1,$2);
	}
	|{$$=NULL;}
	;
ExtDef: Specifier ExtDecList SEMI
	{
	handleS($$,ExtDef,3,$1,$2,$3);
	}
	| Specifier SEMI
	{
	handleS($$,ExtDef,2,$1,$2);
	}
	| Specifier FunDec CompSt
	{
	handleS($$,Extdef,3,$1,$2,$3);
	}
	| Specifier error {errorhandle(";");}
	| Specifier ExtDecList error {errorhandle(";");}
	;
ExtDecList: VarDec
	{
	handleS($$,ExtDecList,1,$1);
	}
	| VarDec COMMA ExtDecList
	{
	handleS($$,ExtDecList,3,$1,$2,$3);
	}
	;

/*Specifiers*/

Specifier: TYPE
	{
	handleS($$,Specifier,1,$1);
	}
	| StructSpecifier
	{
	handleS($$,Specifier,1,$1);
	}
	;
StructSpecifier: STRUCT OptTag LC DefList RC
	{
	handleS($$,StructSpecifier,5,$1,$2,$3,$4,$5);
	}
	| STRUCT Tag
	{
	handleS($$,StructSpecifier,2,$1,$2);
	}
	;
OptTag: ID
	{
	handleS($$,OptTag,1,$1);
	}
	|{$$=NULL;}
	;
Tag: ID
	{
	handleS($$,Tag,1,$1);
	}
	;

/*declarators*/

VarDec: ID
	{
	handleS($$,VarDec,1,$1);
	}
	| VarDec LB INT RB
	{
	handleS($$,VarDec,4,$1,$2,$3,$4);
	}
	| VarDec LB error RB
	
	;
FunDec: ID LP VarList RP
	{
	handleS($$, FunDec, 4, $1, $2, $3, $4); 
	}
	| ID LP RP
	{
	handleS($$, FunDec, 3, $1, $2, $3); 
	}
	| ID LP error RP
	;
VarList: ParamDec COMMA VarList
	{
	handleS($$, VarList, 3, $1, $2, $3);
	}
	| ParamDec
	{
	handleS($$, VarList, 1, $1);
	}
	;
ParamDec: Specifier VarDec
	{
	handleS($$, ParamDec, 2, $1, $2);
	}
	;

/*statements*/

CompSt: LC DefList StmtList RC
	{
	handleS($$,CompSt,4,$1,$2,$3,$4);
	}
	;
StmtList: Stmt StmtList
	{
	handleS($$, StmtList, 2, $1, $2);
	}
	|{$$=NULL;}
	;
Stmt: Exp SEMI
	{
	handleS($$, Stmt, 2, $1, $2);
	}
	| CompSt
	{
	handleS($$, Stmt, 1, $1);
	}
	| RETURN Exp SEMI
	{
	handleS($$, Stmt, 3, $1, $2, $3);
	}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
	{
	handleS($$, Stmt, 5, $1, $2, $3, $4, $5);
	}
	| IF LP Exp RP Stmt ELSE Stmt
	{
	handleS($$, Stmt, 7, $1, $2, $3, $4, $5, $6, $7);
	}
	| WHILE LP Exp RP Stmt
	{
	handleS($$, Stmt, 5, $1, $2, $3, $4, $5); 
	}
	| Exp error {errorhandle(";");}
	| RETURN Exp error {errorhandle(";");}
	| error SEMI {is_error=1;}
	| error ELSE {is_error=1;}
	| error RETURN {is_error=1;}
	| error IF {is_error=1;}
	| error WHILE {is_error=1;}

	;

/*Local Definitions*/

DefList: Def DefList
	{
	handleS($$,DefList,2,$1,$2);
	}
	|
	{$$=NULL;}
	;
Def: Specifier DecList SEMI
	{
	handleS($$,Def,3,$1,$2,$3);
	}
	;
DecList: Dec
	{
	handleS($$,DecList,1,$1);
	}
	| Dec COMMA DecList
	;
Dec: VarDec
	{
	handleS($$,Dec,1,$1);
	}
	| VarDec ASSIGNOP Exp
	{
	handleS($$,Dec,3,$1,$2,$3);
	}
	;

/*Expressions*/

Exp: Exp ASSIGNOP Exp { handleS($$, Exp, 3, $1, $2, $3); }
   | Exp AND Exp { handleS($$, Exp, 3, $1, $2, $3); }
   | Exp OR Exp { handleS($$, Exp, 3, $1, $2, $3); }
   | Exp RELOP Exp { handleS($$, Exp, 3, $1, $2, $3); }
   | Exp PLUS Exp { handleS($$, Exp, 3, $1, $2, $3); }
   | Exp MINUS Exp { handleS($$, Exp, 3, $1, $2, $3); }
   | Exp STAR Exp { handleS($$, Exp, 3, $1, $2, $3); }
   | Exp DIV Exp { handleS($$, Exp, 3, $1, $2, $3); }
   | Exp LB Exp RB { handleS($$, Exp, 4, $1, $2, $3, $4); }
   | Exp DOT ID { handleS($$, Exp, 3, $1, $2, $3); }
   | LP Exp RP { handleS($$, Exp, 3, $1, $2, $3); }
   | MINUS Exp { handleS($$, Exp, 2, $1, $2); }
   | NOT Exp { handleS($$, Exp, 2, $1, $2); }
   | ID LP Args RP { handleS($$, Exp, 4, $1, $2, $3, $4); }
   | ID LP RP { handleS($$, Exp, 3, $1, $2, $3); }
   | ID { handleS($$, Exp, 1, $1); }
   | INT { handleS($$, Exp, 1, $1); }
   | FLOAT { handleS($$, Exp, 1, $1); }

   ;
Args: Exp COMMA Args { handleS($$, Args, 3, $1, $2, $3); }
	| Exp { handleS($$, Args, 1, $1); }
	;

%%
void yyerror(const char* msg)
{
	is_error=1;
	int lineno=yylineno;
	fprintf(stderr,"Error type B at Line %d :%s\n",lineno,msg);
}