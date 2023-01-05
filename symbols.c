#include "symbols.h"

void printVariable(value_t val)
{
	
	if(val.scopeBorder)
	{
		printf("%d ", val.scopeBorder);
		printf("Hit scopeborder\n");
		return;

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

void stack_push(var_stack *st, value_t val)
{	
	st->vals = realloc(st->vals, (st->size + 1) * sizeof(value_t));
	st->vals[st->size++] = val;
}

void enter_func(var_stack *st)
{	
//	printf("Entering function\n");
	value_t val;
	val.m_id = "";
	val.scopeBorder = 1;
	stack_push(st, val);

}


void leave_func(var_stack *st)
{
//	printf("Leaving function\n");
	int i = st->size - 1;
	for(; i >= 0; i--)
	{

		if(st->vals[i].scopeBorder == 1)
			break;

	}
	st->size = i;
}

value_t *lookUp(var_stack *st, char *id)
{
	
	int i = st->size - 1;
	for(; i >= 0; i--)
	{
		if(st->vals[i].scopeBorder == 1)
		{

			return NULL;

		}
		if(strcmp(st->vals[i].m_id, id) == 0)
			return &st->vals[i];

	}

	return NULL;

}

void var_declare(var_stack *st, value_t val, char *id)
{
	value_t *v = lookUp(st, id);
	if(v)
	{

		fprintf(stderr, "Redeclaration of variable \"%s\" not allowed\n", id);
		var_dump(st);
		exit(1);

	}
	
//	printf("Assigning %d to %s\n", val.m_int, id);	
	val.m_id = strdup(id);
	stack_push(st, val);
	
}

void var_set(var_stack *st, value_t val, char *id)
{

	value_t *v = lookUp(st, id);
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

value_t var_get(var_stack *st, char *id)
{

	value_t *val = lookUp(st, id);
	if(val)
	{

		return *val;
	
	}

	fprintf(stderr, "Unrecognized variable \"%s\"\n", id);
	exit(1);
	

}

void var_dump(var_stack *st)
{
	printf("-- Top -- \n");
	int i = st->size - 1;
	for(; i >= 0; i--)
	{
		if(st->vals[i].scopeBorder)
		{

			printf("%d: End of function\n", i);
		
		} else  
		{
			printf("%d: ", i);
			printVariable(st->vals[i]);
		}
	}
	printf("-- Bottom --\n\n");


}

int getTopFunctionSize(var_stack *st)
{

	int i = st->size - 1;
	int counter = 0;
	for(; i >= 0; i--)
	{

		if(st->vals[i].scopeBorder)
		{
			return counter;
		}
		counter++;
	}
	return counter;

}

void var_set_function(var_stack *st, value_t *vals, int size)
{

	int top = st->size - 1;
	int i = size - 1;
	for(; i >= 0; i--)
	{

		switch(vals[i].m_flag)
		{

			case intType: st->vals[top].m_int = vals[i].m_int; break;
			case realType: st->vals[top].m_real = vals[i].m_real; break;
			case stringType: st->vals[top].m_string = strdup(vals[i].m_string); break;
			default: break;

		}
		top--;
	} 	

}
