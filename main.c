#include <stdio.h>
#include <string.h>

extern FILE* yyin;
extern int yylex(); 
extern int yyparse();
extern int yyrestart();
// extern int yydebug;
extern int yylineno;
int main(int argc,char** argv)
{
	if(argc <= 1) return 1;
	FILE* f=fopen(argv[1],"r");
	if(!f)
	{
		perror(argv[1]);
		return 1;
	}
	yylineno=1;
	yyrestart(f);
//	yydebug=1;
	yyparse();
	return 0;
/*	if(argc>1)
	{
		if(!(yyin=fopen(argv[1],"r")))
		{
			perror(argv[1]);
			return 1;
		}
	}
	while(yylex()!=0);
	return 0;
*/
}
