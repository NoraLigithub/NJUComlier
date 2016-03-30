#include "tree.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

node_t* newNode()
{
	node_t* r=(node_t*)malloc(sizeof(node_t));
	r->terminalFlag=0;
	//not terminal by default 
	r->child=NULL;
	r->nextSibling=NULL;
	r->lineno=0;
	return r;
}
node_t* NodeAddChild(node_t* root,node_t* child)
{
	node_t* nd=root->child;
	if(nd==NULL)
		root->child=child;
	else
	{
		while(nd->nextSibling!=NULL)
		{
			nd=nd->nextSibling;
		}
		nd->nextSibling=child;	
	}
	return root;
}
node_t* createTree(int airty, ...)
{
	va_list ap;
	va_start(ap,airty);
	node_t* root=newNode();
	int i;
	for(i=0;i<airty;i++)
	{
		node_t* p = va_arg(ap,node_t*);
		if(i==0) root->lineno=p->lineno;
		if(p!=NULL) NodeAddChild(root,p);
	}
	va_end(ap);
	return root;
}