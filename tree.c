#include "tree.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h> 

node_t* root=NULL;
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
	//printf("enter NodeAddChild root=%s,child=%s\n",root->name,child->name );
	node_t* nd=root->child;
	//printf("enter NodeAddChild root=%s,child=%s  2\n",root->name,child->name );
	if(nd==NULL)
		root->child=child;
	else
	{
		while(nd->nextSibling!=NULL)
		{
			//printf("enter NodeAddChild root=%s,child=%s  3\n",root->name,child->name);
			nd=nd->nextSibling;
		}
			//printf("enter NodeAddChild root=%s,child=%s  4\n",root->name,child->name );
		nd->nextSibling=child;	
			//printf("enter NodeAddChild root=%s,child=%s  5\n",root->name,child->name );

	}
	return root;
}
node_t* createTree(int airty, ...)
{
	va_list ap;
	//printf("create tree:airty=%d,1\n",airty);
	va_start(ap,airty);
	//printf("create tree:airty=%d,2\n",airty);
	node_t* root=newNode(); 
	//printf("create tree:airty=%d,3\n",airty);
	int i;
	for(i=0;i<airty;i++)
	{
		//printf("create tree:airty=%d,4,cirtcle:%d\n",airty,i);
		node_t* p = va_arg(ap,node_t*);
		//printf("create tree:airty=%d,5,cirtcle:%d\n",airty,i);
		if(p!=NULL)
		{
			//printf("create tree:airty=%d,6,cirtcle:%d\n",airty,i);
			if(i==0) root->lineno=p->lineno;
			//printf("this is in the process of createTree,adding %s\n",p->name );
			root=NodeAddChild(root,p);
			//printf("create tree:airty=%d,7,cirtcle:%d\n",airty,i);
		}
	}
	//printf("create tree:airty=%d,8,cirtcle:%d\n",airty,i);
	va_end(ap);
	//printf("create tree:airty=%d,9,cirtcle:%d\n",airty,i);
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
node_t* treeKthChild(node_t* root,int k)
{
	assert(root!=NULL);
	node_t* p=root->child;
	while(k!=1)
	{
		if(p==NULL) return p;
		p=p->nextSibling;
		k--;
	}
	return p;
}

node_t* treeLastChild(node_t* root)
{
	assert(root!=NULL);
	root=root->child;
	node_t* child=root;
	while(root!=NULL)
	{
		child=root;
		root=root->nextSibling;
	}
	return child;
}
node_t* treeFirstChild(node_t* r)
{
	assert(r!=NULL);
	return r->child;
}