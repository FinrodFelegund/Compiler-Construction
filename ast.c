#include "ast.h"

int nodeCounter = 0;
int scopeCounter = 0;
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
		case ID: name = concatenateString("ID", root->val.ID); break;
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
		default: name = "Unknown Node";printf("Unknown node in printing\n"); return; 
	}
	printf("%s\n", name);
	
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
			case ASSIGN: val = execute(root->childNodes[1]); assignExpression(root->childNodes[0]->val.ID, val); break;
			case ID: val = getExpression(root->val.ID); return val;
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
	switch(val.dType)
	{

		case typeInt: printf("> %d\n", val.INR); break;
		case typeString: printf(">> %s\n", val.STR); break;
		default: break;
	}


}

value_t plusOperation(value_t op1, value_t op2)
{

	value_t val;
	val.dType = op1.dType;
	val.INR = op1.INR + op2.INR;
	return val;


}

value_t getExpression(char *id)
{
	value_t val;
	if(searchvar(id))
	{

		val.dType = typeInt;
		val.INR = getvar(id);
		return val;
	} else if(searchString(id))
	{

		val.dType = typeString;
		val.STR = getString(id);
		return val;

	} else 
	{ 

		fprintf(stderr, "Use of undeclared identifier %s\n", id);
		exit(0);
	} 

	return val;
	

}

void assignExpression(char *id, value_t val)
{
	
	
	switch(val.dType)
	{
		case typeInt: setvar(id, val.INR); break;
		case typeString: setString(id, val.STR); break;
		default: printf("Defaulting\n"); break;

	}	

}

void setString(char *id, char *string)
{

	strings[(int)(*id)] = string;

}

char *getString(char *id)
{

	return strings[(int)(*id)];

}

int searchString(char *id)
{

	if(strings[(int)(*id)] != NULL)
		return 1;

	return 0;

}

void setvar(char *id, int val)
{
	
	vars[(int)(*id)] = val;

}

int getvar(char *id)
{

		return vars[(int)(*id)];

}

int searchvar(char *id)
{

	if(vars[(int)(*id)] != 0)
		return 1;

	return 0;

}





	
