#include "ast.h"

int nodeCounter = 0;
int l = 0;
function_stack f_stack;
callParams p_stack;

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
	char *name = NULL;
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
//		case EXPRESSIONS: name = concatenateString("EXPRESSIONS", ""); break;
		case DEFINITION: name = concatenateString("DEFINITION", ""); break;
		case RVALUE: name = concatenateString("RVALUE", ""); break;
		case DECLARATION: name = concatenateString("DECLARATION", getDataType(root->val)); name = concatenateString(name, root->val.m_id); break;
		case FUNCTIONCALL: name = concatenateString("FUNCTIONCALL", root->val.m_id); break;
		case FUNCPARAMS: name = concatenateString("FUNCTIONSPARAMS", ""); break;
		case CALLPARAMS: name = concatenateString("CALLPARAMS", ""); break;
		case INSTRUCTIONS: name = concatenateString("INSTRUCTIONS", ""); break;
		case IF: name = concatenateString("IF", ""); break;
		case ELSE: name = concatenateString("ELSE", ""); break;
		case LOGICAL: name = concatenateString("LOGICAL", root->op); break;
		case WHILE: name = concatenateString("WHILE", ""); break;
		case INCDEC: name = concatenateString("INCDEC", root->op); break;
		case FOR: name = concatenateString("FOR", ""); break;
		case GETINT: name = concatenateString("GETINT", ""); break;
		case GETREAL: name = concatenateString("GETREAL", ""); break;
		case GETRAND: name = concatenateString("GETRAND", ""); break;
		default: name = "Unknown Node";printf("Unknown node in printing\n"); return; 
	}

	//printf("%s\n", name);	
	char buffer[10];
	sprintf(buffer, "%d", nodeCounter++);	
	Agnode_t *n = agnode(graph, buffer, 1);
	agsafeset(n, "label", name, "");
	if(name)
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
	value_t val = createEmpty();
	if(root != NULL)
	{	
//	printf("Node type: %d\n", root->type);
		switch(root->type)
		{
			case INSTRUCTIONS: { 
						execute(root->childNodes[0]); 
						if(checkScope(root->childNodes[1])){ enter_block(); } execute(root->childNodes[1]); if(checkScope(root->childNodes[1])){ leave_block(); } break;
					   }

			case INCDEC: val = execute(root->childNodes[0]); char *id = strdup(val.m_id); val = incdec(root->op, val); var_set(val, id); break; 
			
			case LOGICAL: val.m_flag = root->childNodes[0]->val.m_flag; val.m_int = determineLogical(root->op, execute(root->childNodes[0]), execute(root->childNodes[1])); return val;
			
			case IF: val = execute(root->childNodes[0]); if(val.m_int){ execute(root->childNodes[1]); } else if(root->childNodes[3]) { execute(root->childNodes[3]); } break;
			
			case ELSE: execute(root->childNodes[0]); break;
			
			case FOR: execute(root->childNodes[0]); val = execute(root->childNodes[1]); if(val.m_int) { while(val.m_int) { enter_block(); execute(root->childNodes[3]); execute(root->childNodes[2]); val = execute(root->childNodes[1]); leave_block(); } } break;
	
			case WHILE: do { enter_block(); execute(root->childNodes[1]); val = execute(root->childNodes[0]); leave_block(); } while(val.m_int); break;
			
			case FUNC: enter_func(); execute(root->childNodes[0]); var_dump(); setParams(); resetParams(); execute(root->childNodes[1]); var_dump(); leave_func(); break;
			
			case DECLARATION: val = createEmpty(); val.m_flag = root->val.m_flag; val.m_id = strdup(root->val.m_id); var_declare(val); return val;
			
			case DEFINITION: {     
					 	value_t dest = execute(root->childNodes[0]); value_t source = execute(root->childNodes[1]); 
					 	val = assignement(dest, source); var_set(val, dest.m_id); break; 
					 }			

			case RVALUE: val = execute(root->childNodes[0]); return val;
			
			case ID:  val = var_get(root->val.m_id); return val;
			
			case INT: return root->val;
			
			case PLUS: {
					value_t op1 = execute(root->childNodes[0]); value_t op2 = execute(root->childNodes[1]); val = plusOperation(op1, op2); return val; 
				   
				   }  
			
			case PRINT: val = execute(root->childNodes[0]); printExpression(val); break; 
			
			case STRING: return root->val;
			
			case REAL: return root->val;
			
			case FUNCTIONCALL: { /*dumpFunctions();*/ ast_node *node = getFunction(root->val.m_id); execute(root->childNodes[0]); dumpParams(); execute(node); break; }
			
			case FUNCPARAMS: execute(root->childNodes[0]); execute(root->childNodes[1]); break;
			
			case CALLPARAMS: { 
					   if(root->childNodes[0]) { val = execute(root->childNodes[0]); printf("Adding variable\n"); pushParam(val); }  
					   if(root->childNodes[1]) { val = execute(root->childNodes[1]); printf("Adding variable\n"); pushParam(val); } 
					   break; 
					 } 
			
			case GETINT: val.m_flag = intType; val.m_int = getInt(); return val;
			
			case GETREAL: val.m_flag = realType; val.m_real = getReal(); return val;
	
			case GETRAND: val.m_flag = intType; val.m_int = getRand(); return val;
	
			default: printf("Unknown Node in executing\n"); break;

		}
	}

	val.empty = 1;	
	return val;	

}

int checkScope(ast_node *root)
{

	if(root && (root->blockScoped == 1))
	{

		return 1;

	}
	return 0;

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

value_t assignement(value_t dest, value_t source)
{

	value_t val = createEmpty();
	if(dest.m_flag != source.m_flag)
	{

		fprintf(stderr, "Assignement between different data types is illegal\n");
		exit(1);	

	}
	switch(dest.m_flag)
	{

		case intType: val.m_int = source.m_int; val.m_flag = intType; return val;
		case realType: val.m_real = source.m_real; val.m_flag = realType; return val;
		case stringType: val.m_string = strdup(source.m_string); val.m_flag = stringType; return val;
		default: return val;
	}

	return val;


}

value_t incdec(char *op, value_t op1)
{
	
	if(strcmp(op, "++") == 0)
	{
		switch(op1.m_flag)
		{
			case stringType: fprintf(stderr, "%s\n", "Incrementation or Decrementation on strings is not yet alloweed"); exit(1);
			case intType: op1.m_int = op1.m_int + 1; return op1;
			case realType: op1.m_real = op1.m_real + 1; return op1;
			default: return op1;
		}
	}
	if(strcmp(op, "--") == 0)
	{
		switch(op1.m_flag)
		{
			case stringType: fprintf(stderr, "%s\n", "Incrementation or Decrementation on strings is not yet alloweed"); exit(1);
                        case intType: op1.m_int = op1.m_int - 1; return op1;
                        case realType: op1.m_real = op1.m_real - 1; return op1;
                        default: return op1;
		}
	}
	
	return op1;
}

int determineLogical(char *op, value_t op1, value_t op2)
{
	
//	printf("Types: %s %d  %s %d\n", op1.m_id, op1.m_flag, op2.m_id, op2.m_flag);	
	if(op1.m_flag != op2.m_flag)
	{

		fprintf(stderr,"%s", "Comparison between different types is illegal\n");
		exit(1);

	}
	
	if(op1.m_flag == stringType)
	{
		if(strcmp(op, "<") == 0) return strlen(op1.m_string) < strlen(op2.m_string);
		if(strcmp(op, ">") == 0) return strlen(op1.m_string) > strlen(op2.m_string);
		if(strcmp(op, "<=") == 0) return strlen(op1.m_string) <= strlen(op2.m_string);
		if(strcmp(op, ">=") == 0) return strlen(op1.m_string) >= strlen(op2.m_string);
		if(strcmp(op, "!=") == 0) return strlen(op1.m_string) != strlen(op2.m_string);
		if(strcmp(op, "==") == 0) return strlen(op1.m_string) == strlen(op2.m_string);
		if(strcmp(op, "&&") == 0) return strlen(op1.m_string) && strlen(op2.m_string);
		if(strcmp(op, "||") == 0) return strlen(op1.m_string) || strlen(op2.m_string);	
		if(strcmp(op, "!") == 0) return !strlen(op1.m_string); 
	}

	if(op1.m_flag == intType)
	{
		//printf("Operants: %d %d\n", op1.m_int, op2.m_int);
		if(strcmp(op, "<") == 0) return op1.m_int < op2.m_int;
                if(strcmp(op, ">") == 0) return op1.m_int > op2.m_int; 
                if(strcmp(op, "<=") == 0) return op1.m_int <= op2.m_int;
                if(strcmp(op, ">=") == 0) return op1.m_int >= op2.m_int;
                if(strcmp(op, "!=") == 0) return op1.m_int != op2.m_int;
                if(strcmp(op, "==") == 0) return op1.m_int == op2.m_int;
		if(strcmp(op, "&&") == 0) return op1.m_int && op2.m_int;
		if(strcmp(op, "||") == 0) return op1.m_int || op2.m_int;
		if(strcmp(op, "!") == 0) return !op1.m_int; 
	}

	if(op1.m_flag == realType)
	{

		if(strcmp(op, "<") == 0) return op1.m_real < op2.m_real;
                if(strcmp(op, ">") == 0) return op1.m_real > op2.m_real;
                if(strcmp(op, "<=") == 0) return op1.m_real <= op2.m_real;
                if(strcmp(op, ">=") == 0) return op1.m_real >= op2.m_real;
                if(strcmp(op, "!=") == 0) return op1.m_real != op2.m_real;
                if(strcmp(op, "==") == 0) return op1.m_real == op2.m_real;
		if(strcmp(op, "&&") == 0) return op1.m_real && op2.m_real;
		if(strcmp(op, "||") == 0) return op1.m_real || op2.m_real;
		if(strcmp(op, "!") == 0) return !op1.m_real; 
	}	
	  

	return 0;

}

value_t createEmpty()
{

	value_t val;
	val.m_id = NULL;
	val.m_int = 0;
	val.m_real = 0;
	val.m_string = NULL;
	val.m_flag = 0;
	val.scopeBorder = 0;
	val.empty = 0;
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

void pushFunction(char *id, ast_node *node)
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

void dumpFunctions()
{

	printf("--- Functions Top ---\n");
	for(int i = 0; i < f_stack.size; i++)
	{

		printf("Function: %s\n", f_stack.nodes[i].id);

	}
	printf("--- Functions Bottom ---\n\n");


}

void pushParam(value_t val)
{
	if(val.empty)
	{
		printf("Empty val: %d\n", val.empty);
		return;
	}
	p_stack.vals = realloc(p_stack.vals, (p_stack.size + 1) * sizeof(value_t));
	p_stack.vals[p_stack.size++] = val;

}

void resetParams()
{		
	if(p_stack.vals)
	{

		free(p_stack.vals);
		p_stack.vals = NULL;

	}
	p_stack.size = 0;
}

void dumpParams()
{
	
	printf("--- Call Variable Top ---\n");
	int i = p_stack.size - 1;
	for(; i >= 0; i--)
	{

		printVariable(p_stack.vals[i]);

	}
	printf("--- Call Variable Bottom ---\n\n");

}


void setParams()
{

	int j = getTopFunctionSize();
	int k = getParamsSize();
//	printf("Amount func stack: %d Amount param stack %d\n", j, k);	
	if( j != k)
	{
	
		fprintf(stderr, "Function call does not match function definition\n" );
		exit(1);

	}
	var_set_function(p_stack.vals, p_stack.size);
}

int getParamsSize()
{

	return p_stack.size;

}


int getInt()
{

	char buffer[100];
	printf("< ");
	char *s = fgets(buffer, sizeof(buffer), stdin);
  	if (s == NULL) 
	{
    		
		printf("EOF\n");
    		exit(1);
  	
	}
  	
	return atoi(s);

}

float getReal()
{
	srand(time(NULL));
	char buffer[100];
	printf("< ");
	char *s = fgets(buffer, sizeof(buffer), stdin);
  	if (s == NULL) 
	{
    
		printf("EOF\n");
    		exit(1);
  
	}
  	return atof(s);

}


int getRand()
{
	
	return rand();

}

void setRand()
{

	srand(time(NULL));

}


void freeTree(ast_node *root)
{	
	
	for(int i = 0; i < MAXCHILDREN; i++)
	{
	
		if(root->childNodes[i])
		{

			freeTree(root->childNodes[i]);

		}

	}

	if(root->funcName)
		free(root->funcName);
	if(root->op)
		free(root->op);

	if(root->val.m_string)
		free(root->val.m_string);
	
	if(root->val.m_id)
		free(root->val.m_id);

	free(root);
	

}
