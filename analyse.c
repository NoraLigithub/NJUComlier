#include "common.h"
#include "symbol-table.h"
#include "tree.h"
#include "stdarg.h"


static Type* retType;
static Type* analyseSpecifier(node_t*);
static void analyseExtDef(node_t*);
static void analyseExtDecList(node_t *, Type*);
static Arg* analyseDefList(node_t*, Arg*);
static Arg* analyseDef(node_t*, Arg*);
static Arg* analyseDecList(node_t*, Type*, Arg*);
static Arg* analyseDec(node_t*, Type*, Arg*);
static Dec* analyseVarDec(node_t*, Type*);
static Func* analyseFunDec(node_t*, Type*);
static Arg* analyseVarList(node_t*, Arg*);
static Arg* analyseParamDec(node_t*);
static void analyseStmtList(node_t*);
static void analyseStmt(node_t*);
static Arg* analyseArgs(node_t*, Arg*);
static void analyseCompSt(node_t*, Func*);
typedef struct Val {
	Type* type;
	bool isVar;
} Val;
typedef struct FunhtNode FunhtNode;
static Val analyseExp(node_t*);

const char* errorMsg[] = {
		"Undefined variable \"%s\"",
		"Undefined function \"%s\"",
		"Redefined variable \"%s\"",
		"Redefined function \"%s\"",
		"Type mismatched for assignment",
		"The left-hand side of an assignment must be a variable",
		"Type mismatched for operands",
		"Type mismatched for return",
		"Function \"%s(%s)\" is not applicable for arguments \"(%s)\"",
		"\"%s\" is not an array",
		"\"%s\" is not a function",
		"\"%s\" is not an integer",
		"Illegal use of \".\"",
		"Non-existent field \"%s\"",
		"Redefined or initialized field \"%s\"",
		"Duplicated name \"%s\"",
		"Undefined structure \"%s\"",
		"Undefined function \"%s\"",
		"Inconsistent declaration of function \"%s\"",
};
#define printerror(errno,lineno, ...) \
do{ \
	printf("Error Type %d at line %d: ", errno,lineno); \
	printf(errorMsg[errno-1], __VA_ARGS__); \
	puts(".\n");\
} while(0);
static void analyseExtDefList(node_t* p) {
	assert(isSyntax(p, ExtDefList));
	if(p->child!=NULL) analyseExtDef(p->child);
	node_t *rest = treeLastChild(p);
	if (isSyntax(rest, ExtDefList))
		analyseExtDefList(rest);
}
static void analyseExtDef(node_t *p) {
	//printf("%d,%s\n",p==NULL,p->name);
	assert(isSyntax(p, ExtDef));
	Type *type = analyseSpecifier(p->child);
	node_t *second = treeKthChild(p, 2);
	node_t *last = treeKthChild(p,3);
	if (isSyntax(second, ExtDecList)) {
		analyseExtDecList(second, type);
	} else if (isSyntax(second, FunDec)) {
		bool isDef = isSyntax(last, CompSt);
		Func* func = analyseFunDec(second, type);
		if (!func) return;
		retType = func->retType;
		if (isDef) {
			analyseCompSt(last, func);
			//func->defined = true;
		}
	}
}
static void analyseExtDecList(node_t *p, Type *type) {
	assert(isSyntax(p, ExtDecList));
	node_t *first = p->child;
	node_t *rest = treeKthChild(p,3);
	Dec *varDec = analyseVarDec(first, type);
	htNode *symbol = (htNode*)malloc(sizeof(htNode));
	symbol->name = toArray(varDec->name);
	symbol->kind = VAR;
	symbol->type = varDec->type;
	if (!node_insert(symbol))
		printerror(3, first->lineno, symbol->name);
	if (rest!=NULL&&isSyntax(rest, ExtDecList))
		analyseExtDecList(rest, type);
}

static Type* analyseSpecifier(node_t* p )
{
	assert(isSyntax(p,Specifier));
	node_t* child=p->child;
	if(isSyntax(child,TYPE))
	{
		if(strcmp(child->text,"int")==0)
			return TypeINT; 
		else if(strcmp(child->text,"float")==0)
		    return TypeFLOAT;
	}
	else{
		assert(isSyntax(child,StructSpecifier));
		assert(isSyntax(child->child,STRUCT));
		node_t* tag=child->child->nextSibling;
		if(isSyntax(tag,Tag))
		{
			node_t* id=tag->child;
			assert(isSyntax(id,ID));
			htNode* symbol=node_seek(id->text);
			if((symbol==NULL)||(symbol->kind!=STRUCT)){
				printerror(17,id->lineno,id->text);
				//need return ?	
			}
			//printf("%d:%d\n", __LINE__,symbol->type->kind);
			return symbol->type;

		}else
		{
			int defListIndex = (isSyntax(tag, OptTag))? 4: 3;
			Type *type = (Type*)malloc(sizeof(Type));
			type->kind = STRUCTURE;
			//listInit(&type->structure);
			type->structure=(Arg*)malloc(sizeof(Arg));
			type->structure->name=NULL;
			type->structure->next=NULL;
			type->structure=analyseDefList(treeKthChild(child, defListIndex), type->structure);
			if (defListIndex == 4) {
				node_t* id = treeFirstChild(tag);
				assert(isSyntax(id, ID));
				htNode *symbol = (htNode*)malloc(sizeof(htNode));
				symbol->name = toArray(id->text);
				symbol->kind = STRUCT;
				symbol->type = type;
				if (!node_insert(symbol))
					printerror(16, id->lineno, id->text);
			}
			return type;
			/*
			assert(isSyntax(tag,OptTag));
			node_t* id = tag->child;
			Type *type=(Type*)malloc(sizeof(Type));
			type->kind=STRUCTURE;
			printf("type->kind=%d\n",type->kind);
			assert(isSyntax(id, ID));
				htNode *symbol = (htNode*)malloc(sizeof(htNode));
				symbol->name = toArray(id->text);
				symbol->kind = STRUCT;
				//printf("symbol->kind\n");
				symbol->type = type;
				if (!node_insert(symbol))
					printerror(16, id->lineno, id->text);
				return type;*/
		}
	}
	return NULL;
}
static Dec* analyseVarDec(node_t* p, Type* type) {
	assert(isSyntax(p, VarDec));
	node_t* first = p->child;
	if (isSyntax(first, ID)) {
		Dec *dec = (Dec*)malloc(sizeof(Dec));
		dec->name = toArray(first->text);
		dec->type = type;
		//printf("%d\n", type->kind);
		return dec;
	} else {
		node_t *size = treeKthChild(p, 3);
		assert(isSyntax(first, VarDec));
		assert(isSyntax(size, INT));
		Type *newType = (Type*)malloc(sizeof(Type));
		newType->kind = ARRAY;
		newType->array.elem = type;
		newType->array.size = atoi(size->text);
		return analyseVarDec(first, newType);
	}
}
static Arg* analyseDecList(node_t* p, Type* type, Arg* list) {
	assert(isSyntax(p, DecList));
	node_t *dec = treeFirstChild(p);
	node_t *rest = treeLastChild(p);
	list=analyseDec(dec, type, list);
	if (isSyntax(rest, DecList))
		list=analyseDecList(rest, type, list);
	return list;
}

static Arg* analyseDec(node_t* p, Type* type, Dec* list) {
	assert(isSyntax(p, Dec));
	node_t *first = p->child;
	node_t *last = treeLastChild(p);
	Dec *varDec = analyseVarDec(first, type);
	if (list) {
		if (fieldFind(list, varDec->name) != NULL) {
			printerror(15, p->lineno, varDec->name);
		} else {
			list=fieldInsert(list, varDec);
		}
		if (isSyntax(last, Exp))
			printerror(15, p->lineno, varDec->name);
	} else {
		htNode *symbol = (htNode*)malloc(sizeof(htNode));
		symbol->name = varDec->name;
		symbol->kind = VAR;
		symbol->type = varDec->type;
		if (!node_insert(symbol)) {
			printerror(3, p->lineno, symbol->name);
		} else if (isSyntax(last, Exp)) {
			Val val = analyseExp(last);
			if ((val.type != NULL)&&(!typeEqual(val.type, symbol->type)))
				printerror(5, treeKthChild(p, 2)->lineno, "");
		}
		free(varDec);
	}
	return list;
}static Val makeVar(Type* type) {
	Val val;
	val.type = type;
	val.isVar = true;
	return val;
}
static Val makeVal(Type* type) {
	Val val;
	val.type = type;
	val.isVar = false;
	return val;
}
static Val requireBasic(node_t* p, int errorNo) {
	assert(isSyntax(p, Exp));
	Val val = analyseExp(p);
	if ((val.type != NULL)&&(val.type->kind != BASIC)) {
		printerror(errorNo, p->lineno, p->text);
	}
	return val;
}
static Val requireType(node_t *p, Type *type, int errorNo) {
	assert(isSyntax(p, Exp));
	Val val = analyseExp(p);
	if ((val.type != NULL)&&(!typeEqual(val.type, type))) {
		printerror(errorNo, p->lineno, p->text);
	}
	return val;
}
#define check(val) do { if (val.type == NULL) return makeVal(NULL); } while (0)
static Val analyseExp(node_t* p) {
	assert(isSyntax(p, Exp));
	node_t *first = treeKthChild(p,1);
	node_t *second = treeKthChild(p, 2);
	node_t *last = treeLastChild(p);
	if (isSyntax(first, ID)) {
		if (isSyntax(last, RP)) { // ID LP Args RP | ID LP RP
			node_t *id = first;
			assert(isSyntax(id, ID));
			htNode *symbol = node_seek(id->text);
			if (!symbol) {
				printerror(2, id->lineno, id->text);
			} else if (symbol->kind != FUNC) {
				printerror(11, id->lineno, id->text);
			} else {
				node_t *args = treeKthChild(p, 3);
				Arg* list=NULL;
				//list->type=(Type*)malloc(sizeo)
				if (isSyntax(args, Args))
					list=analyseArgs(args,list);
				//printf("%d\n",list->type->kind );
				if (!argsEqual(list, symbol->func->args)) {

					char paramsStr[32], argsStr[32];
					argsToStr(symbol->func->args, paramsStr);
					argsToStr(list, argsStr);
					printerror(9, id->lineno, symbol->name, paramsStr, argsStr);
				}
				argsRelease(list);
				return makeVal(symbol->func->retType);
			}
		} else { // ID
			htNode *symbol = node_seek(first->text);
			p->text=first->text;
			if (!symbol) {
				printerror(1, first->lineno, first->text);
			} else {
				//printf("%d\n",symbol->type->kind );
				return makeVar(symbol->type);
			}
		}
	} else if (isSyntax(first, INT)) {
		p->text=first->text;
		return makeVal(TypeINT);
	} else if (isSyntax(first, FLOAT)) {
		p->text=first->text;
		return makeVal(TypeFLOAT);
	} else if (isSyntax(last, RB)) { // EXP LB EXP RB
		node_t *third = treeKthChild(p, 3);
		Val base = analyseExp(first);
		Val index = requireType(third, TypeINT, 12);
		check(base);
		check(index);
		if (base.type->kind != ARRAY) {
			//printf("llll  %s",first->text);
			printerror(10, first->lineno, first->text);
		} else {
			base.type = base.type->array.elem;
			return base;
		}
	} else if (isSyntax(last, ID)) { // EXP DOT ID
		assert(isSyntax(second, DOT));
		Val base = analyseExp(first);
		check(base);
		//printf("%d:%d\n",__LINE__,base.type->kind );
		char *fieldName = last->text;
		if (base.type->kind != STRUCTURE) {
			printerror(13, second->lineno, "");
		} else {
			Field *field = fieldFind((base.type)->structure, fieldName);
			if (field == NULL) {
				printerror(14, last->lineno, fieldName);
			} else {
				base.type = field->type;
				return base;
			}
		}
	} else if (isSyntax(second, ASSIGNOP)) {
		Val left = analyseExp(first);
		check(left);
		if (!left.isVar) {
			printerror(6, first->lineno, "");
		} else {
			Val right = requireType(last, left.type, 5);
			check(right);
			return left;
		}
	} else if (isSyntax(first, LP) && isSyntax(last, RP)) {
		return analyseExp(second);
	} else if (last == second) {
		Val val;
		if (isSyntax(first, NOT)) {
			val = requireType(second, TypeINT, 7);
		} else {
			assert(isSyntax(first, MINUS));
			val = requireBasic(second, 7);
		}
		check(val);
		return val;
	} else if (isSyntax(second, AND)||isSyntax(second, OR)) {
		Val left = requireType(first, TypeINT, 7);
		Val right = requireType(last, TypeINT, 7);
		check(left);
		check(right);
		return makeVal(TypeINT);
	} else {
		Val left = requireBasic(first, 7);
		check(left);
		Val right = requireType(last, left.type, 7);
		check(right);
		return makeVal(left.type);
	}
	return makeVal(NULL);
}
static Arg* analyseArgs(node_t* p, Arg* list) {
//	assert(list != NULL);
	assert(isSyntax(p, Args));
	node_t *exp = treeFirstChild(p);
	node_t *rest = treeLastChild(p);
	Arg *arg = (Arg*)malloc(sizeof(Arg));
	arg->type = analyseExp(exp).type;
	//arg->name = v.name;
	//list=fieldInsert(list,arg);
	if(list==NULL) list=arg;
	else {
		Arg* p=list->next;
		list->next=arg;
		arg->next=p;
	}
	if (isSyntax(rest, Args))
		list=analyseArgs(rest, list);
	return list;
}

typedef struct FunhtNode {
	htNode *symbol;
	int lineno;
	FunhtNode* next;
} FunhtNode;
FunhtNode* funhtNodes;
void FunhtNode_ini(){
	//funhtNodes=(FunhtNode*)malloc(sizeof(FunhtNode));
	funhtNodes=NULL;

}
//FunhtNode_ini(funhtNodes); //move it
static Func* analyseFunDec(node_t* p, Type* type) {
	assert(isSyntax(p, FunDec));
	assert(type != NULL);
	Func *func = (Func*)malloc(sizeof(Func));
	func->retType = type;
//	func->args=(Arg*)malloc(sizeof(Arg));
	//func->args->type=(Type*)malloc(sizeof(Type));
	//func->defined = false;
	//listInit(&func->args);
	node_t* id = treeFirstChild(p);
	assert(isSyntax(id, ID));
	htNode *symbol = node_seek(id->text);
	if ((symbol != NULL)) {
		printerror(4, id->lineno, symbol->name);
	} else {
		node_t* varList = treeKthChild(p, 3);
		if (symbol == NULL) {
			symbol = (htNode*)malloc(sizeof(htNode));
			symbol->name = toArray(id->text);
			symbol->kind = FUNC;
			symbol->func = func;
		//	printf("line 374 %d\n",func==NULL );
			if (node_insert(symbol)) {
				FunhtNode *funhtNode = (FunhtNode*)malloc(sizeof(FunhtNode));
				funhtNode->symbol = symbol;
				funhtNode->lineno = p->lineno;
				funhtNode->next=NULL;
				funhtNode->next=funhtNodes;
				funhtNodes=funhtNode;
			}
		}
		//printf("line 384 %d\n",symbol->func->args==NULL );
		func->args=NULL;
		if (isSyntax(varList, VarList))
			func->args=analyseVarList(varList, func->args);
		/*if (funcEqual(symbcol->func, func)) {
			if (symbol->func != func) {
				//func->defined = symbol->func->defined;
				funcRelease(symbol->func);
				symbol->func = func;
			}
			return symbol->func;
		} else {
			//printerror(19, p->lineno, symbol->name);
		}*/
		return symbol->func;
	}
	//funcRelease(func);
	return NULL;
}

static Arg* analyseVarList(node_t* p, Arg* list) {
	assert(isSyntax(p, VarList));
	Arg *arg = analyseParamDec(treeFirstChild(p));
	//printf("%s\n", arg->name);
	list=fieldInsert(list, arg);
	node_t *rest = treeLastChild(p);
	if (isSyntax(rest, VarList))
		list=analyseVarList(rest, list);
	return list;
}

static Arg* analyseParamDec(node_t* p) {
	assert(isSyntax(p, ParamDec));
	Type *type = analyseSpecifier(treeFirstChild(p));
	//printf("%d  %d\n", type->kind,type->basic);
	return analyseVarDec(treeLastChild(p), type);
}

static void analyseCompSt(node_t* p, Func* func) {
	assert(isSyntax(p, CompSt));
	symbolsStackPush();
	if (func != NULL) {
		Arg* arg=func->args;
		while(arg!=NULL) {
			htNode *symbol = (htNode*)malloc(sizeof(htNode));
			symbol->name = toArray(arg->name);
			symbol->kind = VAR;
			symbol->type = arg->type;
			node_insert(symbol);
			arg=arg->next;
		}
	}
	node_t *next = treeKthChild(p, 2);
	if (isSyntax(next, DefList)) {
		analyseDefList(next, NULL);
		next = treeKthChild(p, 3);
	}
	if (isSyntax(next, StmtList))
		analyseStmtList(next);
	symbolsStackPop();
}

static void analyseStmtList(node_t* p) {
	assert(isSyntax(p, StmtList));
	node_t *rest = treeLastChild(p);
	analyseStmt(treeFirstChild(p));
	if (isSyntax(rest, StmtList))
		analyseStmtList(rest);
}

static void analyseStmt(node_t* p) {
	assert(isSyntax(p, Stmt));
	node_t *first = treeFirstChild(p);
	if (isSyntax(first, RETURN)) {
		Type *type = analyseExp(treeKthChild(p, 2)).type;
		if ((type != NULL)&&(!typeEqual(type, retType)))
			printerror(8, p->lineno, "");
	} else if (isSyntax(first, Exp)) {
		analyseExp(first);
	} else {
		node_t* r=p->child;
		while(r!=NULL) {
			//node_t *r = listEntry(q, node_t, list);
			if (isSyntax(r, Stmt)) analyseStmt(r);
			else if (isSyntax(r, CompSt)) analyseCompSt(r, NULL);
			else if (isSyntax(r, Exp)) {
				requireType(r, TypeINT, 7);
			}
			r=r->nextSibling;
		}
	}
}

static Arg* analyseDefList(node_t* p, Arg* list) {
	assert(isSyntax(p, DefList));
	list=analyseDef(treeFirstChild(p), list);
	node_t *rest = treeLastChild(p);
	if (isSyntax(rest, DefList))
		list=analyseDefList(rest, list);
	return list;
}
static Arg* analyseDef(node_t* p, Arg* list) {
	assert(isSyntax(p, Def));
	node_t *specifier = treeFirstChild(p);
	node_t *decList = treeKthChild(p, 2);
	Type *type = analyseSpecifier(specifier);
	list=analyseDecList(decList, type, list);
	return list;
}

void analyseProgram(node_t* p) {
	assert(isSyntax(p, Program));
	FunhtNode_ini(funhtNodes);
	//listInit(&funhtNodes);
	node_t *extDefList = treeFirstChild(p);
	analyseExtDefList(extDefList);
	while (funhtNodes!=NULL) {
		//FunhtNode *funhtNode = listEntry(funhtNodes.next, FunhtNode, list);
		//assert(funhtNodes->symbol->kind == FUNC);
		//if (!funhtNode->symbol->func->defined)
		//	printerror(18, funhtNode->lineno, funhtNode->symbol->name);
		FunhtNode* t=funhtNodes->next;
		free(funhtNodes);
		funhtNodes=t;
	}
}