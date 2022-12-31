#include "header.h"

int nodeCounter = 0;
void test()
{

value_t val;
val.INR = 5;
printf("%d\n", val.INR);

}

void test_astnode()
{
	
}

ast_node *new_node(int type)
{
        ast_node *node = (ast_node*)malloc(sizeof(ast_node));
        node->type = type;
        return node;
 
}

void print(ast_node *root)
{

	printf("Arrived at printing!\n");
	printf("Root Node: %s\n", root->funcName);
	gvc = gvContext();
	Agraph_t *g = agopen("g", Agdirected, 0);
        printTree(root, g, NULL);
        
        
	
	gvLayout (gvc, g, "dot");
	gvRenderFilename (gvc, g, "png", "out.png");
	gvFreeLayout(gvc, g);
	agclose(g);
	gvFreeContext(gvc);
	nodeCounter = 0;
}

char *concatenateString(char *str1, char *str2)
{
	int size = strlen(str1) + strlen(str2) + 1;
	char *dst = malloc(size);
	strcpy(dst, str1);
	strcat(dst, " ");
	strcat(dst, str2);
	return dst; 

}

void printTree(ast_node *root, Agraph_t *graph, Agnode_t *node)
{
	//base case
	if(root == NULL)
		return;

	char *name;
	switch(root->type)
	{
		case ID: name = root->val.ID; break;
		case INT: name = "INT"; break;
		case REAL: name = "REAL"; break;
		case STRING: name = "STRING"; break;
		case PRINT: name = "PRINT"; break;
		case PLUS: name = "PLUS"; break;
		case MIN: name = "MIN"; break;
		case MUL: name = "MUL"; break;
		case DIV: name = "DIV"; break;
		case ASSIGN: name = "ASSIGN"; break;
		case FUNC: name = concatenateString("FUNC", root->funcName); break;
		case FUNCS: name = "FUNCS"; break;
		case EXPRESSIONS: name = "EXPRESSIONS"; break;
		default: name = "Unknown Node"; return; 
	}
	
	char buffer[10];
	sprintf(buffer, "%d", nodeCounter++);	
	Agnode_t *n = agnode(graph, buffer, 1);
	agsafeset(n, "label", name, "");

	if(node != NULL)
	{

		agedge(graph, node, n, 0, 1);

	}

	for(int i = 0; i < MAXCHILDREN; i++)
	{

		printTree(root->childNodes[i], graph, n);

	}

}
