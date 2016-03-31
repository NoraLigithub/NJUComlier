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
	r->lineno=1;
	return r;
}
node_t* NodeAddChild(node_t* root,node_t* child)
{
	if(root==NULL) return NULL;
	if(child==NULL) return root;
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
		if(p!=NULL)
		{
			if(i==0) root->lineno=p->lineno;
			root=NodeAddChild(root,p);
		}
	}
	va_end(ap);
	return root;
}

void printTree(node_t* root,int blankLen)
{
	int i;
	for(i=0;i<blankLen;i++)
	{
		printf("  ");
	}
	printf("%s",root->name);
	if(root->terminalFlag==1)
	{
		if(strcmp(root->name,"INT")==0)
		{
			printf(": %d",atoi(root->text));
		}
		else if(strcmp(root->name,"FLOAT")==0)
		{
			printf(": %f",atof(root->text));
		}
		else if(strcmp(root->name,"TYPE")==0||strcmp(root->name,"ID")==0)
		{
			printf(": %s",root->text);
		}
	}
	else
	{
		printf(" (%d) ",root->lineno);
	}
	printf("\n");
	if(root->child!=NULL)
	{
		printTree(root->child,blankLen+1);
		node_t* p=root->child;
		while(p->nextSibling!=NULL)
		{
			p=p->nextSibling;
			printTree(p,blankLen+1);
		}
	}	
}
