#include "symbol-table.h"
#include "common.h"

int hashpjw(const char* name)
{
	unsigned val = 0, i;
	for(; *name; ++name) {
		val = (val << 2) + *name;
		if(i = val & ~TABLE_SIZE)
		  val = (val ^ (i >> 12)) & TABLE_SIZE;
	}
	return (int)val;
}
unsigned stackTop;
void htTable_ini()
{
	int i=0;
	stackTop=0;
	for(;i<TABLE_SIZE;i++)
	{
		htTable[i]=NULL;
		stack[i]=NULL;
	}
	typeIni();
}
void symbolRelease(htNode* h)
{
	assert(h!=NULL);
	if(((h->kind==VAR)&&(h->type->kind==ARRAY))||(h->kind=STRUCT)){
		htNode* bef=htTable[h->pos];
		if(bef!=NULL&&bef!=h)
		{
			while(bef!=NULL&&bef->next!=h){
				bef=bef->next;
				//assert(bef!=NULL);
			}
			bef->next=h->next;
		}else{
			htTable[h->pos]=h->next;
		}
		typeRelease(h->type);
	}
	free(h->name);
	free(h);
}
void symbolsStackPush(){
	stackTop++;
}
void symbolsStackPop(){
	assert(stackTop>=0);
	htNode *p=stack[stackTop],*q;
	while(p!=NULL){
		q=p->snext;
		symbolRelease(p);
		//free(p);
		p=q;
	}
	stackTop--;
}
bool symbolAtStackTop(const char* name){
	htNode* h=node_seek(name);
	return (h!=NULL)&&(h->depth==stackTop);
}
void symbolAddStack(htNode* h)
{
	assert(h!=NULL);
	h->depth=stackTop;
	if(stack[stackTop]==NULL){ 
		stack[stackTop]=h;
		return;
	}
	htNode* p=stack[stackTop];
	//htNode* q=p->snext;
	stack[stackTop]=h;
	h->snext=p;
}
htNode* node_seek(const char* name)
{
	int pos=hashpjw(name);
	htNode* root=htTable[pos];
	while(root!=NULL)
	{
		if(strcmp(root->name,name)==0)
			return root;
		root=root->next;
	}
	return NULL;
}	
bool node_insert(htNode* h)
{
	if(h!=NULL){
		int pos=hashpjw(h->name);
		h->pos=pos;
		if(htTable[pos]==NULL)
		{
			htTable[pos]=h;
			symbolAddStack(h);
			return true;			
		}
		else
		{
			if(symbolAtStackTop(h->name))
			{
				return false;
				//node exsits
			}
			else
			{
				htNode* temp=htTable[pos]->next;
				htTable[pos]->next=h;
				h->next=temp;
				symbolAddStack(h);
				return true;
			}
		}
	}
	return false;
}
fieldList* fieldFind(fieldList*  structure, const char* fieldName) {
	//!!!assert(structure != NULL);
	assert(fieldName != NULL);
	//ListHead *p;
	while(structure!=NULL) {
		if(structure->name!=NULL){ 
			if (strcmp(structure->name, fieldName) == 0)
				return structure;
		}
		structure=structure->next;
	}
	return NULL;
}

fieldList* fieldInsert(fieldList* list,fieldList*  structure)
{
	//assert(list!=NULL);
	assert(structure!=NULL);
	if(fieldFind(list,structure->name)) return false;
	if(list==NULL){
		list=structure;
		return list;
	}
	fieldList* p=list->next;
	list->next=structure;
	structure->next=p;
	return list;
}