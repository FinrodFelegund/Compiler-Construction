#include "symbols.h"

var_stack stack_l;
var_stack stack_g;

void printVariable(value_t val)
{
	
	if(val.scopeBorder)
	{	
		switch(val.scopeBorder)
		{
			case func_border: printf("%s \n", "Function Border"); return;
			case block_border: printf("%s \n", "Block Border"); return;
			default: return;
		}
	}
		
	if(val.m_id == NULL)
	{
		printf("Constant Value: ");
	} else {
		printf("ID: %s Value: ", val.m_id);
	}
	switch(val.m_flag)
	{

		case intType: printf("%d\n", val.m_int); break;
		case realType: printf("%f\n", val.m_real); break;
		case stringType: printf("%s\n", val.m_string); break;

	}

}

void stack_push(value_t val)
{	
	stack_l.vals = realloc(stack_l.vals, (stack_l.size + 1) * sizeof(value_t));
	stack_l.vals[stack_l.size++] = val;
}

void enter_func()
{	
//	printf("Entering function\n");
	value_t val;
	val.m_id = "";
	val.scopeBorder = func_border;
	stack_push(val);

}


void leave_func()
{
//	printf("Leaving function\n");
	int i = stack_l.size - 1;
	for(; i >= 0; i--)
	{

		if(stack_l.vals[i].scopeBorder == func_border)
			break;

	}
	stack_l.size = i;
}

void enter_block()
{

	value_t val;
	val.m_id = "";
	val.scopeBorder = block_border;
	stack_push(val);

}

void leave_block()
{

	int i = stack_l.size - 1;
	for(; i >= 0; i--)
	{

		if(stack_l.vals[i].scopeBorder == block_border)
			break;

	}
	stack_l.size = i;


}

value_t *lookUp(char *id)
{
	
	int i = stack_l.size - 1;
	for(; i >= 0; i--)
	{
		if(stack_l.vals[i].scopeBorder == 1)
		{

			return NULL;

		}
		if(strcmp(stack_l.vals[i].m_id, id) == 0)
			return &stack_l.vals[i];

	}

	return NULL;

}

void var_declare(value_t val)
{
	value_t *v = lookUp(val.m_id);
	if(v)
	{

		fprintf(stderr, "Redeclaration of variable \"%s\" not allowed\n", val.m_id);
		var_dump();
		exit(1);

	}
	
//	printf("Assigning %d to %s\n", val.m_int, id);	
	stack_push(val);
	
}

void var_set(value_t val, char *id)
{

	value_t *v = lookUp(id);
	//printf("Type: %d\n", val.m_flag);
	if(v)
	{
		switch(val.m_flag)
		{

			case intType: v->m_int = val.m_int; break;
			case realType: v->m_real = val.m_real; break;
			case stringType: v->m_string = (val.m_string); break;
		
		}
	}

}

value_t var_get(char *id)
{

	value_t *val = lookUp(id);
	if(val)
	{

		return *val;
	
	}

	fprintf(stderr, "Unrecognized variable \"%s\"\n", id);
	exit(1);
	

}

void var_dump()
{
	printf("-- Top -- \n");
	int i = stack_l.size - 1;
	for(; i >= 0; i--)
	{
		if(stack_l.vals[i].scopeBorder)
		{

			printf("%d: End of function\n", i);
		
		} else  
		{
			printf("%d: ", i);
			printVariable(stack_l.vals[i]);
		}
	}
	printf("-- Bottom --\n\n");


}

int getTopFunctionSize()
{

	int i = stack_l.size - 1;
	int counter = 0;
	for(; i >= 0; i--)
	{

		if(stack_l.vals[i].scopeBorder)
		{
			return counter;
		}
		counter++;
	}
	return counter;

}

void var_set_function(value_t *vals, int size)
{

	int top = stack_l.size - 1;
	int i = size - 1;
	for(; i >= 0; i--)
	{

		switch(vals[i].m_flag)
		{

			case intType: stack_l.vals[top].m_int = vals[i].m_int; break;
			case realType: stack_l.vals[top].m_real = vals[i].m_real; break;
			case stringType: stack_l.vals[top].m_string = strdup(vals[i].m_string); break;
			default: break;

		}
		top--;
	} 	

}
