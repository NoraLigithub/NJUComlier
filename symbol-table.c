#include "symbol-table.h"
#include "common.h"

int hashpjw(char* name)
{
	unsigned val = 0, i;
	for(; *name; ++name) {
		val = (val << 2) + *name;
		if(i = val & ~TABLE_SIZE)
		  val = (val ^ (i >> 12)) & TABLE_SIZE;
	}
	return (int)val;
}

void htTable_ini()
{
	int i=0;
	for(;i<TABLE_SIZE;i++)
	{
		htTable[i]=NULL;
	}
	typeIni();
}

htNode* node_seek(char* name)
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
		if(htTable[pos]==NULL)
		{
			htTable[pos]=h;
			return true;			
		}
		else
		{
			if(node_seek(h->name)!=NULL)
			{
				return false;
				//node exsits
			}
			else
			{
				htNode* temp=htTable[pos]->next;
				htTable[pos]->next=h;
				h->next=temp;
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
		if (strcmp(structure->name, fieldName) == 0)
			return structure;
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