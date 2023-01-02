#include "ast.h"

int nodeCounter = 0;


ast_node *new_node(int type)
{
        ast_node *node = (ast_node*)malloc(sizeof(ast_node));
        node->type = type;
        return node;
 
}

void print(ast_node *root)
{

//	printf("Arrived at printing!\n");
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

	int size = strlen(str1) + strlen(str2) + 2;
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
		case ID: name = concatenateString("ID", root->val.m_id); break;
		case INT: name = concatenateString("INT", ""); break;
		case REAL: name = concatenateString("REAL", ""); break;
		case STRING: name = concatenateString("STRING", ""); break;
		case PRINT: name = concatenateString("PRINT", ""); break;
		case PLUS: name = concatenateString("PLUS", ""); break;
		case MIN: name = concatenateString("MIN", ""); break;
		case MUL: name = concatenateString("MUL", ""); break;
		case DIV: name = concatenateString("DIV", ""); break;
		case ASSIGN: name = concatenateString("ASSIGN", ""); break;
		case FUNC: name = concatenateString("FUNC", root->funcName); break;
		case FUNCS: name = concatenateString("FUNCS", root->funcName); break;
		case EXPRESSIONS: name = concatenateString("EXPRESSIONS", ""); break;
		case DECLARATION: name = concatenateString("DECLARATION", root->val.m_id); break;
		default: name = "Unknown Node";printf("Unknown node in printing\n"); return; 
	}

	
	char buffer[10];
	sprintf(buffer, "%d", nodeCounter++);	
	Agnode_t *n = agnode(graph, buffer, 1);
	agsafeset(n, "label", name, "");
	free(name);	
	if(node != NULL)
	{

		agedge(graph, node, n, 0, 1);

	}

	for(int i = 0; i < MAXCHILDREN; i++)
	{

		printTree(root->childNodes[i], graph, n);

	}

}

value_t execute(ast_node *root)
{	
	value_t val;
	if(root != NULL)
	{	
		//printf("Node type: %d\n", root->type);
		switch(root->type)
		{
			case FUNC: execute(root->childNodes[0]); break;
			case EXPRESSIONS:  execute(root->childNodes[0]); execute(root->childNodes[1]); break;
			case ASSIGN: val = execute(root->childNodes[1]); setvar(root->childNodes[0]->val.m_id, val.m_int);  break;
			case DECLARATION: setvar(root->val.m_id, 0); break;
			case ID:  val.m_int = getvar(root->val.m_id); return val;
			case INT: return root->val;
			case PLUS: {
					value_t op1 = execute(root->childNodes[0]); value_t op2 = execute(root->childNodes[1]); val = plusOperation(op1, op2); return val; 
				   }  
			case PRINT: val = execute(root->childNodes[0]); printExpression(val); break; 
			case STRING: return root->val;
			default: printf("Unknown Node in executing\n"); break;

		}
	}
	return val;	

}

void printExpression(value_t val)
{
	switch(val.m_flag)
	{

		case intType: printf("> %d\n", val.m_int); break;
		case stringType: printf(">> %s\n", val.m_string); break;
		default: break;
	}


}

value_t plusOperation(value_t op1, value_t op2)
{

	value_t val;
	val.m_flag = op1.m_flag;
	val.m_int = op1.m_int + op2.m_int;
	return val;


}































	





































	

























	
