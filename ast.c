#include "ast.h"

int nodeCounter = 0;
var_stack stack;
int l = 0;
function_stack f_stack;

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
	
	//printf("Node: %d ", l++); 
	//printf("%d\n", root->type);
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
		case FUNC: name = concatenateString("FUNC", root->funcName); break;
		case FUNCS: name = concatenateString("FUNCS", ""); break;
		case EXPRESSIONS: name = concatenateString("EXPRESSIONS", ""); break;
		case DEFINITION: name = concatenateString("DEFINITION", ""); break;
		case RVALUE: name = concatenateString("RVALUE", ""); break;
		case DECLARATION: name = concatenateString("DECLARATION", getDataType(root->val)); name = concatenateString(name, root->val.m_id); break;
		case FUNCTIONCALL: name = concatenateString("FUNCTIONCALL", root->val.m_id); break; 
		default: name = "Unknown Node";printf("Unknown node in printing\n"); return; 
	}

	//printf("%s\n", name);	
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
//		printf("Node type: %d\n", root->type);
		switch(root->type)
		{
			case FUNC: enter_func(&stack); execute(root->childNodes[0]); var_dump(&stack); leave_func(&stack); break;
			case EXPRESSIONS:  execute(root->childNodes[0]); execute(root->childNodes[1]); break;	
			case DECLARATION: val = createEmpty(); val.m_flag = root->val.m_flag; var_declare(&stack, val, root->val.m_id); break;
			case DEFINITION: execute(root->childNodes[0]); val = execute(root->childNodes[1]); var_set(&stack, val, root->childNodes[0]->val.m_id); break; 
			case RVALUE: val = execute(root->childNodes[0]); return val;
			case ID:  val = var_get(&stack, root->val.m_id); return val;
			case INT: return root->val;
			case PLUS: {
					value_t op1 = execute(root->childNodes[0]); value_t op2 = execute(root->childNodes[1]); val = plusOperation(op1, op2); return val; 
				   }  
			case PRINT: val = execute(root->childNodes[0]); printExpression(val); break; 
			case STRING: return root->val;
			case REAL: return root->val;
			case FUNCTIONCALL: { ast_node *node = getFunction(root->val.m_id); execute(node); break; }
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
		case realType: printf("> %f\n", val.m_real); break;
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

value_t createEmpty()
{

	value_t val;
	val.m_int = 0;
	val.m_real = 0;
	val.m_string = NULL;
	val.m_flag = 0;
	val.scopeBorder = 0;
	return val;

}

char *getDataType(value_t val)
{
	
	switch(val.m_flag)
	{
		case intType: return "int";
		case realType: return "real";
		case stringType: return "string";
		default: break;
	}
	return "";

}

void pushNode(char *id, ast_node *node)
{

	function_node n;
	n.id = strdup(id);
	n.node = node;
	f_stack.nodes = realloc(f_stack.nodes, (f_stack.size + 1) * sizeof(function_node));
	f_stack.nodes[f_stack.size++] = n;
	

}

ast_node *getFunction(char *id)
{

	for(int i = 0; i < f_stack.size; i++)
	{

		if(strcmp(id, f_stack.nodes[i].id) == 0)
		{

			return f_stack.nodes[i].node;

		}
	
	}

	fprintf(stderr, "%s\n", "Unrecognized function");
	exit(1);
}


