#include "symbol-table.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
Type *TypeINT;
Type *TypeFLOAT;
void typeIni()
{
	TypeINT=(Type*)malloc(sizeof(Type));
	TypeINT->basic=0; 
	TypeFLOAT=(Type*)malloc(sizeof(Type));
	TypeFLOAT->basic=1;
}
bool typeEqual(Type* a, Type* b) {
	//if(a!=b) return false; 
	
	assert(a != NULL);
	assert(b != NULL);
	if (a == b) return true;
	if (a->kind != b->kind) return false;
	switch (a->kind) {
		case BASIC:
			return (a->basic == b->basic);
		case ARRAY:
			return typeEqual(a->array.elem, b->array.elem);
		case STRUCTURE:
			return (a->structure->name== b->structure->name);
	}
	return false;
}
bool argsEqual(Arg* a, Arg* b) {
	if(a==NULL&&b!=NULL) return false;
	if(a!=NULL&&b==NULL) return false;
	if(a==NULL&&b==NULL) return true;
	assert(a != NULL);
	assert(b != NULL);
	while (a!=NULL&&b!=NULL) {
		if (!typeEqual(a->type, b->type))
			return false;
		a = a->next; b = b->next;
	}
	return (a==b);
}
bool funcEqual(Func* a, Func* b) {
	assert(a != NULL);
	assert(b != NULL);
//	printf("%d\n",a->args==NULL );
	return (typeEqual(a->retType, b->retType) &&
			argsEqual(a->args, b->args));
}

void argsRelease(Arg *args) {
//!!!	assert(args != NULL);
	while (args!=NULL) {
		Arg* t=args->next;
		if (args->name != NULL) free(args->name);
		free(args);
		args=t;
	}
}
void funcRelease(Func *func) {
	assert(func != NULL);
	argsRelease(func->args);
	free(func);
}
void typeRelease(Type *type) {
	if(type==NULL) return;
	assert(type != NULL);
	if (type->kind == ARRAY) {
		Type *baseType = type->array.elem;
		if (baseType->kind == ARRAY) typeRelease(baseType);
		free(type);
	} else if (type->kind == STRUCTURE) {
		Field* field=type->structure;
		while(field!=NULL){
			Field* t=field->next;
			typeRelease(field->type);
			free(field->name);
			free(field);
			field=t;
		}
		free(type);
	}
}

static void typeArrayToStr(Type* type, char* s) {
	assert(type != NULL);
	assert(s != NULL);
	if (type->kind == ARRAY) {
		sprintf(s, "[%d]", type->array.size);
		s += strlen(s);
		typeArrayToStr(type->array.elem, s);
	}
}
static Type* baseType(Type* type) {
	assert(type != NULL);
	if (type->kind != ARRAY) return type;
	return baseType(type->array.elem);
}

void typeToStr(Type* type, char* s) {
	if(type==NULL) return;
	assert(type != NULL);
	assert(s != NULL);
	if (typeEqual(type, TypeINT)) {
		strcpy(s, "int");
	} else if (typeEqual(type, TypeFLOAT)) {
		strcpy(s, "float");
	} else if (type->kind == ARRAY) {
		typeToStr(baseType(type), s);
		s += strlen(s);
		typeArrayToStr(type, s);
	} else if (type->kind == STRUCTURE) {
		strcpy(s, "struct");
	}
}
void argsToStr(Arg* list, char* s) {
	if(list==NULL) return;
	assert(list != NULL);
	assert(s != NULL);
	while(list!=NULL) {
		typeToStr(list->type, s);
		s += strlen(s);
		list=list->next;
		if (list != NULL) {
			strcpy(s, ", ");
			s += 2;
		}
	}
	
}



int typeSize(Type *type) {
	assert(type != NULL);
	int size = 0;
	fieldList *field;
	switch (type->kind) {
		case BASIC:
			return 4;
		case ARRAY:
			return typeSize((type->array).elem)*(type->array).size;
		case STRUCTURE:
			field=type->structure;
			while(field!=NULL) {
				size += typeSize(field->type);
				field=field->next;
			}
			return size;
	}
	return 0;
}