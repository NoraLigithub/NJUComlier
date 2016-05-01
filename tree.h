#ifndef TREE_H
#define TREE_H

typedef struct Node
{
	int lineno;
	union{
		int valInt;
		double valDouble;
		char* valStr;
		void* varAny;
	} value;
	int terminalFlag;
	char* name;
	//This is the name of nonterminal symbol
	// I will use marcon get this string.
	char* text;
	int token;
	struct Node *child,*nextSibling;
} node_t;

extern node_t* root;

node_t* newNode();
node_t* NodeAddChild (node_t* root,node_t* child);
node_t* createTree(int airty, ...);
node_t* treeLastChild(node_t* root);
node_t* treeFirstChild(node_t* r);
node_t* treeKthChild(node_t* root,int k);
#endif
