#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H

#define TABLE_SIZE 0x3fff

typedef int bool;
#define true 1;
#define false 0; 

typedef struct fieldList_ fieldList;
typedef struct Type_ Type;
typedef struct htNode_ htNode;
typedef struct fieldList_
{
	char* name;
	Type* type;
	fieldList* next; 
} fieldList;
typedef struct Type_
{
	enum {BASIC,ARRAY,STRUCTURE} kind;
	union
	{
		int basic;
		struct {Type* elem;int size;} array;
		fieldList* structure;
	};
} Type;
extern Type* TypeINT;
extern Type* TypeFLOAT;
extern void typeIni();
typedef struct func_
{
	Type* retType;
	char* name;
	fieldList* args;
} func;

typedef struct htNode_
{
	char* name;
	enum {VAR,STRUCT,FUNC} kind;
	union{
		func* func;
		Type* type;
	};
	int pos,depth;
	htNode* next;
	htNode* snext;//the same depth
} htNode;

typedef fieldList Dec;
typedef fieldList Field;
typedef fieldList Arg;
typedef func Func;

htNode* htTable[TABLE_SIZE];
htNode* stack[TABLE_SIZE];
//SIZE is casual

int hashpjw(const char* name);

extern void htTable_ini();

htNode* node_seek(const char* name);
bool node_insert(htNode* h);
fieldList* fieldFind(fieldList*  structure, const char* fieldName);
fieldList* fieldInsert(fieldList* list,fieldList*  structure);
bool typeEqual(Type*, Type*);
bool argsEqual(Arg*, Arg*);
bool funcEqual(Func*, Func*);
void argsRelease(Arg *args);
void funcRelease(Func *);
void typeRelease(Type *);
void typeToStr(Type*, char*);
void argsToStr(Arg*, char*);
Field* fieldFind(Arg*, const char*);
int typeSize(Type *);
void symbolsStackPop();
void symbolsStackPush();
#endif