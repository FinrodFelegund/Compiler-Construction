#include "ast.h"

int nodeCounter = 0;
int l = 0;
function_stack f_stack;
callParams p_stack;
value_t returnVal;
int returnSignal = 0;
int deadCode = 0;
int deads = 0;
int folds = 0;
int arranges = 0;


ast_node *new_node(int type)
{
        ast_node *node = (ast_node*)malloc(sizeof(ast_node));
        node->type = type;
	node->funcName = NULL;
	node->blockScoped = 0;
	node->val = createEmpty();
	for(int i = 0; i < MAXCHILDREN; i++)
	{

		node->childNodes[i] = NULL;

	}
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
	l = 0;
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
	
//	printf("Node: %d ", l++); 
//	printf("%d\n", root->type);
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
		case ARITHMETIC: name = concatenateString("ARITHMETIC", root->op); break;
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
		case RETURN: name = concatenateString("RETURN", ""); break;
		case ARR_ID: name = concatenateString("ARRAY ID", ""); break;
		case RANGE_FOR: name = concatenateString("RANGE FOR", ""); break;
		default: name = "Unknown Node";printf("Unknown node in printing\n"); return; 
	}

//	printf("%s\n", name);	
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
//	printf("Node type: %d %d\n",l++, root->type);
		switch(root->type)
		{
			case INSTRUCTIONS: 
			{    
				if(returnSignal) break;
				execute(root->childNodes[0]); 
				if(returnSignal) break;
				if(checkScope(root->childNodes[1])){ enter_block(); } execute(root->childNodes[1]); if(checkScope(root->childNodes[1])){ leave_block(); } break;
			}

			case INCDEC: val = execute(root->childNodes[0]); char *id = strdup(val.m_id); val = incdec(root->op, val); var_set(val, id); break; 
			
			case LOGICAL: val = determineLogical(root->op, execute(root->childNodes[0]), execute(root->childNodes[1])); return val;
			
			case IF: val = execute(root->childNodes[0]); /*printVariable(val);*/  if(val.boolean){ execute(root->childNodes[1]); } else if(root->childNodes[2]) { execute(root->childNodes[2]); } break;
			
			case ELSE: execute(root->childNodes[0]); break;
			
			case FOR: 
			{ 
				execute(root->childNodes[0]); val = execute(root->childNodes[1]); 
				if(val.boolean) { while(val.boolean) { enter_block(); execute(root->childNodes[3]); execute(root->childNodes[2]); 
				val = execute(root->childNodes[1]); leave_block(); } } break;
			}

			case WHILE: 
			{ 
				val = execute(root->childNodes[0]); 
				if(val.boolean)
				{
					while(val.boolean)
					{

						enter_block();
						execute(root->childNodes[1]);
						val = execute(root->childNodes[0]);
						leave_block();

					}
				} 
				break;
			}

			case RANGE_FOR:
			{

				execute(root->childNodes[0]);
				execute(root->childNodes[1]);
				val = execute(root->childNodes[2]);
				if(val.boolean)
				{

					while(val.boolean)
					{

						enter_block();
						execute(root->childNodes[4]);
						execute(root->childNodes[5]);
						execute(root->childNodes[3]);
						val = execute(root->childNodes[2]);
					//	var_dump();
						leave_block();						

					}

				}
				break;
	
			}

			case FUNC: 
			{ 
				enter_func(); execute(root->childNodes[0]); /*var_dump();*/ setParams(); resetParams(); 
				execute(root->childNodes[1]); /*var_dump();*/ if(!returnSignal){ leave_func(); } break;	   
			}

			case DECLARATION: val = createEmpty(); val.m_flag = root->val.m_flag; val.m_id = strdup(root->val.m_id); var_declare(val); return val;
			
			case DEFINITION:
			{     
				value_t dest = execute(root->childNodes[0]); value_t source = execute(root->childNodes[1]); 
				val = assignement(dest, source); var_set(val, dest.m_id); return val; 
			}			

			case RVALUE: val = execute(root->childNodes[0]); return val;
			
			case ID: val = var_get(root->val.m_id); return val;
			
			case INT: root->val.boolean = root->val.m_int != 0; return root->val; 
			
			case ARITHMETIC: 
			{
				value_t op1 = execute(root->childNodes[0]); value_t op2 = execute(root->childNodes[1]); val = arithmeticOperation(root->op, op1, op2); return val; 
			}  
			
			case PRINT: val = execute(root->childNodes[0]); printExpression(val); break; 
			
			case STRING: root->val.boolean = strlen(root->val.m_string) != 0; return root->val;
			
			case REAL: root->val.boolean = root->val.m_real != 0; return root->val;
			
			case FUNCTIONCALL: 
			{    
				/*dumpFunctions();*/ ast_node *node = getFunction(root->val.m_id); 
				execute(root->childNodes[0]); /*dumpParams();*/ execute(node); if(returnSignal){ leave_func(); returnSignal = 0; } 
				val = getReturnVal(); return val; 
			}
			
			case FUNCPARAMS: execute(root->childNodes[0]); execute(root->childNodes[1]); break;
			
			case CALLPARAMS: 
			{ 
				if(root->childNodes[0]) { val = execute(root->childNodes[0]); pushParam(val); }  
				if(root->childNodes[1]) { val = execute(root->childNodes[1]); pushParam(val); } 
				break; 
			} 
			
			case GETINT: val.m_flag = intType; val.m_int = getInt(); return val;
			
			case GETREAL: val.m_flag = realType; val.m_real = getReal(); return val;
	
			case GETRAND: val.m_flag = intType; val.m_int = getRand(); return val;
	
			case RETURN: if(root->childNodes[0]) { setReturnVal(execute(root->childNodes[0])); /*var_dump();*/ } returnSignal = 1; break; 

			case ARR_ID: 
			{ 
				val = var_get(root->val.m_id); value_t tmp = execute(root->childNodes[0]); if(tmp.m_flag != intType) { fprintf(stderr, "Index has to be of type int"); exit(1); }
				val.m_intArray.current = tmp.m_int; 
//				printf("%s %s\n", val.m_id, tmp.m_id);
				return val;
				     
			}

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

		case intType: printf("%d", val.m_int); break;
		case realType: printf("%f", val.m_real); break;
		case stringType: printf("%s", val.m_string); break;
		case intArrayType:
		{
			if(val.m_intArray.current == -1)
			{
				for(int i = 0; i < val.m_intArray.size; i++)
				{
					printf("%d ", val.m_intArray.array[i]);
				}	
			} else
			{
				printf("%d", lookUpIntArray(&val.m_intArray, val.m_intArray.current));	
			}
		}
		default: break;
	}
}

char *concate(char *op1, char *op2)
{
	
	int size = strlen(op1) + strlen(op2) + 1;
        char *dst = malloc(size);
        strcpy(dst, op1);
        strcat(dst, op2);
        return dst;

}

value_t arithmeticOperation(char *op, value_t op1, value_t op2)
{
	//printf("Arithmetic operation: %s\n", op);
	value_t val = createEmpty();

	if((op1.m_flag == intArrayType) && (op2.m_flag == intType))
	{	
		if(op1.m_intArray.current == -1)
		{
		
			fprintf(stderr, "array name not availbale for arithmetic operations\n");
			exit(1);

		}
		val.m_flag = intType;
		if(strcmp(op, "+") == 0)
		{

			val.m_int = lookUpIntArray(&op1.m_intArray, op1.m_intArray.current) + op2.m_int;


		}
		if(strcmp(op, "-") == 0)
		{

			val.m_int = lookUpIntArray(&op1.m_intArray, op1.m_intArray.current) - op2.m_int;

		
		}

		if(strcmp(op, "*") == 0)
		{

			val.m_int = lookUpIntArray(&op1.m_intArray, op1.m_intArray.current) * op2.m_int;


		}
	
		if(strcmp(op, "/") == 0)
		{

			val.m_int = lookUpIntArray(&op1.m_intArray, op1.m_intArray.current) / op2.m_int;

	
		}
	
		if(strcmp(op, "%") == 0)
		{

			val.m_int = lookUpIntArray(&op1.m_intArray, op1.m_intArray.current) % op2.m_int;


		}
		val.boolean = val.m_int != 0;
		return val;
	}

	if((op1.m_flag == intType) && (op2.m_flag == intArrayType))
	{


		if(op2.m_intArray.current == -1)
		{

			fprintf(stderr, "array name not availbale for arithmetic operations\n");
			exit(1);
		
		}
		val.m_flag = intType;
	
		if(strcmp(op, "+") == 0)
		{

			val.m_int = op1.m_int + lookUpIntArray(&op2.m_intArray, op2.m_intArray.current);


		}

		if(strcmp(op, "-") == 0)
		{

			val.m_int = op1.m_int - lookUpIntArray(&op2.m_intArray, op2.m_intArray.current);


		}

		if(strcmp(op, "*") == 0)
		{

			val.m_int = op1.m_int * lookUpIntArray(&op2.m_intArray, op2.m_intArray.current);

		
		}

		if(strcmp(op, "/") == 0)
		{

			val.m_int = op1.m_int / lookUpIntArray(&op2.m_intArray, op2.m_intArray.current);


		}
	
		if(strcmp(op, "%") == 0)
		{

			val.m_int = op1.m_int % lookUpIntArray(&op2.m_intArray, op2.m_intArray.current);


		}
		val.boolean = val.m_int != 0;
		return val;
	}

	if(((op1.m_flag == stringType) && (op2.m_flag != stringType)) || ((op1.m_flag != stringType) && (op2.m_flag == stringType)))
	{

		fprintf(stderr, "For operations on strings, both operands have to be of type string\n");
		exit(1);

	}

	if((op1.m_flag == intType) && (op2.m_flag == realType))
	{

		val.m_flag = intType;
		val.m_int = op1.m_int + op2.m_real;
		val.boolean = val.m_int != 0;
		return val;

	}

	if((op1.m_flag == realType) && (op2.m_flag == intType))
	{

		val.m_flag = realType;
		val.m_real = op1.m_real + op2.m_int;
		val.boolean = val.m_real != 0;
		return val;

	}


	if(op1.m_flag == op2.m_flag)
	{

		if(op1.m_flag == stringType)
        	{
			val.m_flag = stringType;
                	if(strcmp(op, "+") == 0)
			{ 
				val.m_string = concate(op1.m_string, op2.m_string);
				return val;
          		} else 
			{
				fprintf(stderr, "Operations other than + are not allowed on strings\n");
				exit(1);
			}        
		}
	
		if(op1.m_flag == intType)
		{
		
			val.m_flag = intType;
			if(strcmp(op, "+") == 0) val.m_int = op1.m_int + op2.m_int;
			if(strcmp(op, "-") == 0) val.m_int = op1.m_int - op2.m_int; 
			if(strcmp(op, "*") == 0) val.m_int = op1.m_int * op2.m_int;
			if(strcmp(op, "/") == 0) val.m_int = op1.m_int / op2.m_int; 
			if(strcmp(op, "%") == 0) val.m_int = op1.m_int % op2.m_int;
			val.boolean = val.m_int != 0;
		}


		if(op1.m_flag == realType)
		{
			val.m_flag = realType;
			if(strcmp(op, "+") == 0) val.m_real = op1.m_real + op2.m_real;
			if(strcmp(op, "-") == 0) val.m_real = op1.m_real - op2.m_real; 
			if(strcmp(op, "*") == 0) val.m_real = op1.m_real * op2.m_real;
			if(strcmp(op, "/") == 0) val.m_real = op1.m_real / op2.m_real;
			if(strcmp(op, "%") == 0) fprintf(stderr, "Modulo operand on real types is illegal\n"); exit(1);
			val.boolean = val.m_real != 0;
		}

		if(op1.m_flag == intArrayType)
		{	

			if((op1.m_intArray.current == -1) && (op2.m_intArray.current == -1))
			{

				val.m_flag = intArrayType;
				int size = op1.m_intArray.size + op2.m_intArray.size;
				val.m_intArray.array = malloc(size * sizeof(int));
				val.m_intArray.size = size;
				memcpy(val.m_intArray.array, op1.m_intArray.array, op1.m_intArray.size * sizeof(int));
				memcpy(val.m_intArray.array + op1.m_intArray.size, op2.m_intArray.array, op2.m_intArray.size * sizeof(int));	
				val.boolean = val.m_intArray.size != 0;
				return val;
			}

			if((op1.m_intArray.current != -1) && (op2.m_intArray.current != -1))
			{	
				val.m_flag = intType;
				val.m_int = lookUpIntArray(&op1.m_intArray, op1.m_intArray.current) + lookUpIntArray(&op2.m_intArray, op2.m_intArray.current);
				val.boolean = val.m_int != 0;
				return val;

			}

	
		}
	}	

	return val;
}

value_t assignement(value_t dest, value_t source)
{
	
	value_t val = createEmpty();

	if(dest.constqual)
	{

		fprintf(stderr, "Assigning to const qualified ID is illegal\n");
		exit(1);

	}

	if(dest.m_flag == intArrayType && source.m_flag == intType)
	{		
		if(dest.m_intArray.current == -1)
		{

			fprintf(stderr, "Assigning int to array is illegal\n");
			exit(1);

		}
		pushIntArray(&dest.m_intArray, dest.m_intArray.current, source.m_int);
		dest.m_intArray.current = -1;
		return dest;
	}
	
	if(dest.m_flag == intType && source.m_flag == intArrayType)
	{	
		
		if(source.m_intArray.current == -1)
		{

			fprintf(stderr, "Assigning array to int is illegal\n");
			exit(1);

		}
		val.m_int = lookUpIntArray(&source.m_intArray, source.m_intArray.current);
		val.m_flag = intType;
		return val;
	}

	if(((source.m_flag == stringType) && (dest.m_flag != stringType)) || ((source.m_flag != stringType) && (dest.m_flag == stringType)))
	{

		fprintf(stderr, "Assignements with strings are restricted to strings\n");
		exit(1);

	}
	
	if(source.m_flag == intType && dest.m_flag == realType)
	{
			
		val.m_flag = realType;
		val.m_real = source.m_int;
		return val;

	}

	if(source.m_flag == realType && dest.m_flag == intType)
	{
		val.m_flag = intType;
		val.m_int = source.m_real;
		return val;

	}

	if(dest.m_flag == source.m_flag)
	{
	
		switch(dest.m_flag)
		{

			case intType: val.m_int = source.m_int; val.m_flag = intType; return val;
			case realType: val.m_real = source.m_real; val.m_flag = realType; return val;
			case stringType: val.m_string = strdup(source.m_string); val.m_flag = stringType; return val;
			case intArrayType:
			{

				if((dest.m_intArray.current == -1) && (source.m_intArray.current == -1))
				{ 	

					val.m_intArray = copyIntArray(&source.m_intArray); val.m_flag = intArrayType; 
					return val;
				
				} else if((dest.m_intArray.current != -1) && (source.m_intArray.current != -1))
				{	
	//				printf("%d : %d\n", dest.m_intArray.current, source.m_intArray.current);
					int tmp = lookUpIntArray(&source.m_intArray, source.m_intArray.current);
					pushIntArray(&dest.m_intArray, dest.m_intArray.current, tmp);
					dest.m_intArray.current = -1;
					return dest;	
				} 	
				else if((dest.m_intArray.current != -1) && (source.m_intArray.current == -1)) 
				{

					fprintf(stderr, "Assigning array to member of array is illegal\n");
					exit(1);
			
				}
				break;
			}
			default: break;
		}

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
			case intType: op1.m_int = op1.m_int + 1; op1.boolean = op1.m_int != 0; return op1;
			case realType: op1.m_real = op1.m_real + 1; op1.boolean = op1.m_real != 0; return op1;
			default: return op1;
		}
	
	}
	if(strcmp(op, "--") == 0)
	{
		switch(op1.m_flag)
		{
			case stringType: fprintf(stderr, "%s\n", "Incrementation or Decrementation on strings is not yet alloweed"); exit(1);
                        case intType: op1.m_int = op1.m_int - 1; op1.boolean = op1.m_int != 0; return op1;
                        case realType: op1.m_real = op1.m_real - 1; op1.boolean = op1.m_real != 0; return op1;
                        default: return op1;
		}
	}
	
	return op1;
}

value_t determineLogical(char *op, value_t op1, value_t op2)
{
	
//	printf("Types: %s %d  %s %d\n", op1.m_id, op1.m_flag, op2.m_id, op2.m_flag);	
	
	value_t val = createEmpty();

	if(op1.m_flag == intType && op2.m_flag == intArrayType)
	{	

		if(op2.m_intArray.current == -1)
		{
			if(strcmp(op, "!=") == 0)
			{
//				printf("%s %d : %s %d\n", op1.m_id, op1.m_int, op2.m_id, op2.m_intArray.size);
				val.m_flag = intType;
				val.m_int = op1.m_int != op2.m_intArray.size;
				val.boolean = op1.m_int != op2.m_intArray.size;
				return val;

			} else 
			{
			
				fprintf(stderr, "No operations between integer type and integer array type allowed\n");
				exit(1);
			}
		}


		val.m_flag = intType;
		int tmp = lookUpIntArray(&op2.m_intArray, op2.m_intArray.current);
		if(strcmp(op, "<") == 0)
		{
			val.m_int = op1.m_int < tmp;
			val.boolean = op1.m_int < tmp; 
		}

		if(strcmp(op, ">") == 0)
		{
			val.m_int = op1.m_int > tmp;
			val.boolean = op1.m_int > tmp;
		}

		if(strcmp(op, "<=") == 0)
		{
			val.m_int = op1.m_int <= tmp;
			val.boolean = op1.m_int <= tmp;
		}

		if(strcmp(op, ">=") == 0)
		{

			val.m_int = op1.m_int >= tmp;
			val.boolean = op1.m_int >= tmp;
		}

		if(strcmp(op, "!=") == 0)
		{
			val.m_int = op1.m_int != tmp;
			val.boolean = op1.m_int != tmp;
		}
	
		if(strcmp(op, "==") == 0)
		{
			val.m_int = op1.m_int == tmp;
			val.m_int = op1.m_int == tmp;
		}

		if(strcmp(op, "&&") == 0)
		{
			val.m_int = op1.m_int && tmp;
			val.boolean = op1.m_int && tmp;
		}

		if(strcmp(op, "||") == 0)
		{
			val.m_int = op1.m_int || tmp;
			val.boolean = op1.m_int || tmp;
		}

		if(strcmp(op, "!=") == 0)
		{	
			printf("TODO\n");
		}
		return val;
	}
	
	if((op1.m_flag == intArrayType) && (op2.m_flag == intType))
        {	
		if(op1.m_intArray.current == -1)
		{

			fprintf(stderr, "No operations between integer type and integer array type allowed\n");
			exit(1);
		}


                 val.m_flag = intType;
                 int tmp = lookUpIntArray(&op1.m_intArray, op1.m_intArray.current);
                 if(strcmp(op, "<") == 0)
                 {
                         val.m_int = tmp < op2.m_int;
                         val.boolean = tmp < op2.m_int;
                 }
 
                 if(strcmp(op, ">") == 0)
                 {
                         val.m_int = tmp > op2.m_int;
                         val.boolean = tmp > op2.m_int;
                 }
 
                 if(strcmp(op, "<=") == 0)
                 {
                         val.m_int = tmp <= op2.m_int;
                         val.boolean = tmp <= op2.m_int;
                 }

                 if(strcmp(op, ">=") == 0)
                 { 
                         val.m_int = tmp >= op2.m_int;
                         val.boolean = tmp >= op2.m_int;
                 }
 
                 if(strcmp(op, "!=") == 0)
                 {
                         val.m_int = op2.m_int != tmp;
                         val.boolean = op2.m_int != tmp;
                 }
 
                 if(strcmp(op, "==") == 0)
                 {
                        val.m_int = tmp == op2.m_int;
                        val.boolean = op2.m_int == tmp;
                //	printf("%d : %d, VAL: %d : %d\n", tmp, op2.m_int, val.m_int, val.boolean); 
		}
 
                 if(strcmp(op, "&&") == 0)
                 {
                         val.m_int = op2.m_int && tmp;
                         val.boolean = op2.m_int && tmp;
                 }
 
                 if(strcmp(op, "||") == 0)
                 {
                         val.m_int = op2.m_int || tmp;
                         val.boolean = op2.m_int || tmp;
                 }
 
                 if(strcmp(op, "!=") == 0)
                 {
                         printf("TODO\n");
                 }
                 return val;
         }



	if(op1.m_flag != op2.m_flag)
	{

		fprintf(stderr,"%s", "Comparison between different types is illegal\n");
		exit(1);

	}

//	if(op1.boolean == 0 || op2.boolean == 0)
//	{
//
//		val.boolean = 0;
//		val.m_flag = op1.m_flag;
//		return val;
//
//	}
	
	if(op1.m_flag == stringType)
	{	
		val.m_flag = stringType;
		if(strcmp(op, "<") == 0)
		{ 
			val.m_int =  strlen(op1.m_string) < strlen(op2.m_string) ? strlen(op2.m_string) : strlen(op1.m_string);
			val.boolean = strlen(op1.m_string) < strlen(op2.m_string);
		}
		if(strcmp(op, ">") == 0)
		{

			val.m_int =  strlen(op1.m_string) > strlen(op2.m_string) ? strlen(op1.m_string) : strlen(op2.m_string);
			val.boolean = strlen(op1.m_string) > strlen(op2.m_string);
			
		}
		if(strcmp(op, "<=") == 0) 
		{

			val.m_int =  strlen(op1.m_string) <= strlen(op2.m_string) ? strlen(op2.m_string) : strlen(op1.m_string);
			val.boolean = strlen(op1.m_string) <= strlen(op2.m_string);
		}
		if(strcmp(op, ">=") == 0)
		{

			val.m_int =  strlen(op1.m_string) >= strlen(op2.m_string) ? strlen(op1.m_string) : strlen(op2.m_string);
			val.boolean = strlen(op1.m_string) >= strlen(op2.m_string);
		}
		if(strcmp(op, "!=") == 0) 
		{

			val.m_int =  strlen(op1.m_string) != strlen(op2.m_string) ? strlen(op1.m_string) : strlen(op2.m_string);
			val.m_int = strlen(op1.m_string) != strlen(op2.m_string);
		}
		if(strcmp(op, "==") == 0)
		{
				
			val.m_int = strlen(op1.m_string) == strlen(op2.m_string) ? strlen(op1.m_string) : strlen(op2.m_string);
			val.boolean = strlen(op1.m_string) == strlen(op2.m_string);
		}

		if(strcmp(op, "&&") == 0) 
		{

			val.m_int = strlen(op1.m_string) && strlen(op2.m_string) ? strlen(op1.m_string) : strlen(op2.m_string);
			val.boolean = strlen(op1.m_string) && strlen(op2.m_string);
		}

		if(strcmp(op, "||") == 0)
		{

			val.m_int =  strlen(op1.m_string) || strlen(op2.m_string) ? strlen(op1.m_string) : strlen(op2.m_string);
			val.boolean = strlen(op1.m_string) || strlen(op2.m_string);
		}
		if(strcmp(op, "!") == 0)
		{

			printf("TODO\n");

		}
	}

	if(op1.m_flag == intType)
	{	
		val.m_flag = intType;
//		printf("Operants: %d %d : %d %d\n", op1.m_int, op1.boolean, op2.m_int, op2.boolean);
		if(strcmp(op, "<") == 0) 
		{

			val.m_int = op1.m_int < op2.m_int;
			val.boolean = op1.m_int < op2.m_int;

		}
                if(strcmp(op, ">") == 0)
		{

			val.m_int = op1.m_int > op2.m_int;
			val.boolean = op1.m_int > op2.m_int;

		} 
                if(strcmp(op, "<=") == 0)
		{

			val.m_int = op1.m_int <= op2.m_int;
			val.boolean = op1.m_int <= op2.m_int;

		}
                if(strcmp(op, ">=") == 0)
		{

			val.m_int = op1.m_int >= op2.m_int;
			val.boolean = op1.m_int >= op2.m_int;

		} 
                if(strcmp(op, "!=") == 0)
		{

			val.m_int = op1.m_int != op2.m_int;
			val.boolean = op1.m_int != op2.m_int;

		} 
                if(strcmp(op, "==") == 0)
		{

			val.m_int = op1.m_int == op2.m_int;
			val.boolean = op1.m_int == op2.m_int;

		}
		if(strcmp(op, "&&") == 0)
		{
		//	printf("Here\n");			
			val.m_int = op1.m_int && op2.m_int;
			val.boolean = op1.m_int && op2.m_int;
		}
		if(strcmp(op, "||") == 0)
		{

			val.m_int = op1.m_int || op2.m_int;
			val.boolean = op1.m_int || op2.m_int;

		}
		if(strcmp(op, "!") == 0)
		{

			printf("TODO\n");

		}
	}

	if(op1.m_flag == realType)
	{


          
                 val.m_flag = realType;
                  //printf("Operants: %d %d\n", op1.m_int, op2.m_int);
                 if(strcmp(op, "<") == 0)                 {
 
                         val.m_real = op1.m_real < op2.m_real;
                         val.boolean = op1.m_real < op2.m_real;
 
                 }
                 if(strcmp(op, ">") == 0)
                 {
 
                         val.m_real = op1.m_real > op2.m_real;
                         val.boolean = op1.m_real > op2.m_real;
 
                 }
                 if(strcmp(op, "<=") == 0)
                 {
 
                         val.m_real = op1.m_real <= op2.m_real;
                        val.boolean = op1.m_real <= op2.m_real;
  
                  }
                  if(strcmp(op, ">=") == 0)
                  {
  
                          val.m_real = op1.m_real >= op2.m_real;
                          val.boolean = op1.m_real >= op2.m_real;
  
                  }
                  if(strcmp(op, "!=") == 0)
                  {
  
                          val.m_real = op1.m_real != op2.m_real;
                          val.boolean = op1.m_real != op2.m_real;
  
                  }
                  if(strcmp(op, "==") == 0)
                  {
  
                          val.m_real = op1.m_real == op2.m_real;
                          val.boolean = op1.m_real == op2.m_real;
  
                  }
                  if(strcmp(op, "&&") == 0)
                  {
  
                          val.m_real = op1.m_real && op2.m_real;
                          val.boolean = op1.m_real && op2.m_real;
                  }
                  if(strcmp(op, "||") == 0)
                  {
  
                          val.m_real = op1.m_real || op2.m_real;
                          val.boolean = op1.m_real || op2.m_real;
  
                  }
		if(strcmp(op, "!") == 0)
		{

			printf("TODO\n");
		
		}
	}	
	  

	return val;

}


char *getDataType(value_t val)
{
	
	switch(val.m_flag)
	{
		case intType: return "int";
		case realType: return "real";
		case stringType: return "string";
		case intArrayType: return "Int Array";
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

void freeFunctions()
{

	if(f_stack.nodes)
	{
		for(int i = 0; i < f_stack.size; i++)
		{

			if(f_stack.nodes[i].id)
			{
				free(f_stack.nodes[i].id);
			}
		}
		free(f_stack.nodes);
	}

}

void pushParam(value_t val)
{
	if(val.empty)
	{

		return;

	}
	p_stack.vals = realloc(p_stack.vals, (p_stack.size + 1) * sizeof(value_t));
	p_stack.vals[p_stack.size++] = val;

}

void resetParams()
{		
	if(p_stack.vals)
	{
		
		for(int i = 0; i < p_stack.size; i++)
		{
//			if(p_stack.vals[i].m_id)
//				free(p_stack.vals[i].m_id);
//			if(p_stack.vals[i].m_string)
//				free(p_stack.vals[i].m_string);	
	
		}	

//	free(p_stack.vals);
	p_stack.size = 0;
	}
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

void freeAll(ast_node *root)
{

	freeTree(root);
	printf("Tree deleted\n");
//	freeStack();
//	freeFunctions();


}

void freeTree(ast_node *root)
{	

	for(int i = 0; i < MAXCHILDREN; i++)
	{
	
		if(root->childNodes[i] != NULL)
		{

			freeTree(root->childNodes[i]);

		}

	}
	
	freeNode(root);

}

void freeNode(ast_node *node)
{

	if(node->funcName)
	{

		free(node->funcName);

	}
	if(node->op)
	{

		free(node->op);

	}
	if(node->val.m_string)
	{

		free(node->val.m_string);

	}

	if(node->val.m_id)
	{

		free(node->val.m_id);
	
	}

	free(node);
	node = NULL;

}


void setReturnVal(value_t val)
{

	returnVal = val;

}


value_t getReturnVal()
{

	return returnVal;

}



int deleted = 0;

void startOptimization(int argc, char **argv, ast_node *node)
{

	printf("Starting optimization\n");
	
	for(int i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "-c") == 0)
		{
			printf("Constant Folding\n");
			constantFolding(node);
		}
		if(strcmp(argv[i], "-d") == 0)
		{
			printf("Dead Code Elimination\n");
			deadCodeElimination(NULL, 0, node);
		}
		if(strcmp(argv[i], "-r") == 0)
		{
			printf("Rearranging nodes\n");
			reArrangeNodes(NULL, -1, node);
		}
	}

	if(deads || folds || arranges)
	{
		printf("Eliminated %d blocks of dead code, %d operations were folded and %d nodes were rearranged\n", deads, folds, arranges);
	} else
	{

		printf("No optimization specified\n");
	
	}

}
	
void deadCodeElimination(ast_node *prev, int index, ast_node *node)
{


	if(node == NULL)
	{

		return;
	
	}

	switch(node->type)
	{

		case FUNCS:
		{

			deadCodeElimination(node, 0, node->childNodes[0]); 
			deadCode = 0;
			deadCodeElimination(node, 1, node->childNodes[1]);
			deadCode = 0;
			break;		
		}

		case FUNC:
		{

			deadCodeElimination(node, 0, node->childNodes[1]);
			deadCode = 0;
			break;
		}
	
		case INSTRUCTIONS:
		{

			deadCodeElimination(node, 0, node->childNodes[0]);
			if(deleted)
			{

				deleted = 0;
				node->childNodes[0] = NULL;

			}
			deadCodeElimination(node, 1, node->childNodes[1]);
			if(deleted)
			{

				deleted = 0;
				node->childNodes[1] = NULL;

			}
			break;
		}

		case RETURN:
		{

			deadCode = 1;
			break;
		}
		
		case IF:
		{
			
			if(node->childNodes[0]->type != LOGICAL && node->childNodes[0] != ID)
			{

			
				//check if the expression evaluates to true
				if(node->childNodes[0]->val.boolean)
				{


					//swap the if statement with instructions and delete it
					freeTree(node->childNodes[0]);
					node->childNodes[0] = NULL;
					prev->childNodes[index] = node->childNodes[1];	
					
					if(node->childNodes[2])
					{
						// remove else branch if existing
						freeTree(node->childNodes[2]);
						node->childNodes[2] = NULL;

					}
					free(node);
					node = NULL;
					deads++;		

				} else 
				{

					//make the else branch the instruction
					freeTree(node->childNodes[0]);
					node->childNodes[0] = NULL;
					freeTree(node->childNodes[1]);
					node->childNodes[1] = NULL;
					prev->childNodes[index] = node->childNodes[2]->childNodes[0];
					free(node->childNodes[2]);
					free(node);
					node = NULL;
					deads++;
				
				}

			}	

			//just check if node was deleted
			if(node != NULL)
			{

				deadCodeElimination(node, 1, node->childNodes[1]);
				deadCode = 0;				
			
			} else 
			{
//				printf("Going through old instructions\n");
				deadCodeElimination(prev, index, prev->childNodes[index]);

			}

			break;
		}



		default:
		{	
			if(deadCode)
			{	
//				printf("Deleting %d\n", node->type);
				freeTree(node);
				deleted = 1;
				deads++;
				node = NULL;
			}
			break;

		}
	}



}


void constantFolding(ast_node *node)
{

	for(int i = 0; i < MAXCHILDREN; i++)
	{
		if(node->childNodes[i])
		{
			constantFolding(node->childNodes[i]);
		}
	}

	if(node->type == LOGICAL && node->childNodes[0] && node->childNodes[1])
	{

		if(!node->childNodes[0]->val.m_id && !node->childNodes[1]->val.m_id)
		{

			node->val = determineLogical(node->op, node->childNodes[0]->val, node->childNodes[1]->val);
			node->type = INT;
			freeTree(node->childNodes[0]);
			freeTree(node->childNodes[1]);
			node->childNodes[0] = NULL;
			node->childNodes[1] = NULL;
			folds++;
		} else 
		{

			node->val.m_id = strdup("Not foldable");

		}

	}



	if(node->type == ARITHMETIC && node->childNodes[0] && node->childNodes[1])
	{

		if(!node->childNodes[0]->val.m_id && !node->childNodes[1]->val.m_id)
		{

			node->val = arithmeticOperation(node->op, node->childNodes[0]->val, node->childNodes[1]->val);
			switch(node->val.m_flag)
			{

				case intType: node->type = INT; break;
				case realType: node->type = REAL; break;
				default: break;

			}
			freeTree(node->childNodes[0]);
			freeTree(node->childNodes[1]);
			node->childNodes[0] = NULL;
			node->childNodes[1] = NULL;
			folds++;
		} else 
		{

			node->val.m_id = strdup("Not foldable");

		}

	}
}


void reArrangeNodes(ast_node *prev, int index, ast_node *node)
{
	
	if(node == NULL)
	{

		return;

	}

//	printf("Nodetype: %d\n", node->type);
	switch(node->type)
	{
		case INSTRUCTIONS:
		{
			reArrangeNodes(node, 0, node->childNodes[0]);
			reArrangeNodes(node, 1, node->childNodes[1]);
			
			if(node->childNodes[0] != NULL && node->childNodes[0]->type == INSTRUCTIONS && node->childNodes[1] == NULL)
			{

				//printf("Empty Instruction\n");
				prev->childNodes[index] = node->childNodes[0];
				freeNode(node);
				arranges++;	
		
			}
			break;
		}

		case FUNC:
		{

			reArrangeNodes(node, 1, node->childNodes[1]);
			break;
		
		}

		case FUNCS:
		{

			reArrangeNodes(node, 0, node->childNodes[0]);
			reArrangeNodes(node, 1, node->childNodes[1]);
			break;
		}

		case IF:
		{

			reArrangeNodes(node, 1, node->childNodes[1]);
			break;
		
		}

		case ELSE:
		{

			reArrangeNodes(node, 0, node->childNodes[0]);
			break;
		}

		default:
		{
			
			break;

		}	
	}
}
