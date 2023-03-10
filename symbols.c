#include "symbols.h"

var_stack stack_l;
var_stack stack_g;


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
        val.m_intArray = createEmptyIntArray();
	val.boolean = -1; 
	val.constqual = 0;
        return val;
 
}


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

		case intType: printf("%d Boolean: %d\n", val.m_int, val.boolean); break;
		case realType: printf("%f\n", val.m_real); break;
		case stringType: printf("%s\n", val.m_string); break;
		case intArrayType: printf("Int Array\n"); break;
		default: return;
	}

}

void stack_push(value_t val)
{	
	stack_l.vals = realloc(stack_l.vals, (stack_l.size + 1) * sizeof(value_t));
	stack_l.vals[stack_l.used++] = val;
	stack_l.size++;
}

void enter_func()
{	
//	printf("Entering function\n");
	value_t val = createEmpty();
	val.scopeBorder = func_border;
	stack_push(val);

}


void leave_func()
{
//	printf("Leaving function\n");
	int i = stack_l.used - 1;
	for(; i >= 0; i--)
	{

		if(stack_l.vals[i].scopeBorder == func_border)
			break;

	}
	stack_l.used = i;
}

void enter_block()
{
//	printf("Entering block\n");
	value_t val = createEmpty();
	val.scopeBorder = block_border;
	stack_push(val);

}

void leave_block()
{
//	printf("Leaving block\n");
	int i = stack_l.used - 1;
	for(; i >= 0; i--)
	{

		if(stack_l.vals[i].scopeBorder == block_border)
			break;

	}
	stack_l.used = i;


}

value_t *lookUp(char *id)
{
	
	int i = stack_l.used - 1;
	for(; i >= 0; i--)
	{
//		printf("Stack pos: %d:%s:%s\n", i, id, stack_l.vals[i].m_id);
		if(stack_l.vals[i].scopeBorder == func_border)
		{
			break;
		}

		if(stack_l.vals[i].scopeBorder == block_border)
		{

			continue;
		
		}

		if(strcmp(stack_l.vals[i].m_id, id) == 0)
		{
			return &stack_l.vals[i];
		}

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

			case intType: 
			{
				v->m_int = val.m_int; 
				v->boolean = val.m_int != 0;
				break;
			}
			case realType: 
			{
				v->m_real = val.m_real; 
				v->boolean = val.m_real != 0;
				break;
			}

			case stringType: 
			{
			//	if(v->m_string) { free(v->m_string); v->m_string = NULL; }
				v->m_string = strdup(val.m_string); 
				v->boolean = strlen(val.m_string) != 0;
				break;
			}
			case intArrayType: 
			{
				if(v->m_intArray.array)
				{

			//		free(v->m_intArray.array);
					v->m_intArray.array = NULL;

				}
				v->m_intArray = val.m_intArray; break;
			}
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
	int i = stack_l.used - 1;
	for(; i >= 0; i--)
	{
		if(stack_l.vals[i].scopeBorder == 1)
		{

			printf("%d: End of function: %d\n", i, stack_l.vals[i].scopeBorder);
		
		} else if(stack_l.vals[i].scopeBorder == 2)
		{

			printf("%d: End of block: %d\n", i, stack_l.vals[i].scopeBorder);

		} 
		else  
		{
			printf("%d: ", i);
			printVariable(stack_l.vals[i]);
		}
	}
	printf("-- Bottom --\n\n");


}

int getTopFunctionSize()
{

	int i = stack_l.used - 1;
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

	int top = stack_l.used - 1;
	int i = size - 1;
	for(; i >= 0; i--)
	{
		if(stack_l.vals[top].m_flag != vals[i].m_flag)
		{

			fprintf(stderr, "Calling parameter is different than function paramter\n");
			exit(1);
		
		}
		switch(vals[i].m_flag)
		{

			case intType: stack_l.vals[top].m_int = vals[i].m_int; break;
			case realType: stack_l.vals[top].m_real = vals[i].m_real; break;
			case stringType: stack_l.vals[top].m_string = strdup(vals[i].m_string); break;
			case intArrayType: stack_l.vals[top].m_intArray = copyIntArray(&vals[i].m_intArray); break;
			default: break;
		}
		top--;
	} 	

}

void freeStack()
{
	if(stack_l.vals)
	{
		int i = stack_l.size - 1;
		for(; i >= 0; i--)
		{
//			printf("Free\n");
			if(stack_l.vals[i].m_id)
				free(stack_l.vals[i].m_id);
			if(stack_l.vals[i].m_string)
				free(stack_l.vals[i].m_string);	
		}
	}
	stack_l.size = stack_l.used = 0;
	if(stack_l.vals)
	{
		free(stack_l.vals);
	}
}


void pushIntArray(intArray *arr, int pos, int val)
{	
//	printf("%d : %d\n", pos, val);
	if(arr->size == 0)
	{
		arr->size = 1;
		arr->array = realloc(arr->array, sizeof(int));
	//	printf("Initialized\n");	
	}

	if(pos >= arr->size)
	{
	//	printf("T\n");
		int a[arr->size];
		memcpy(a, arr->array, arr->size * sizeof(int));
	//	for(int i = 0; i < arr->size; i++)
	//		printf("%d ", a[i]);
	//	printf("\n");
		arr->array = realloc(arr->array, (pos + 1) * sizeof(int));
		memset(arr->array, 0, (pos + 1) * sizeof(int));
	//	for(int i = 0; i < pos + 1; i++)
	//		printf("%d ", arr->array[i]);
	//	printf("\n"); 
		memcpy(arr->array, a, arr->size * sizeof(int));
	//	for(int i = 0; i < pos + 1; i++)
	//		printf("%d ", arr->array[i]);
	//	printf("\n"); 
		arr->size = pos + 1;
	//	printf("T\n");
	}

	arr->array[pos] = val;
}

int lookUpIntArray(intArray *arr, int pos)
{
//	printf("%d \n", pos);
	if(arr->size <= pos)
	{
		if(arr->size == 0)
		{

			arr->size = 1;
			arr->array = realloc(arr->array, sizeof(int));
			arr->array[0] = 0;

		}
		return arr->array[arr->size - 1];

	}
	return arr->array[pos];


}

intArray createEmptyIntArray()
{

	intArray val;
	val.size = 0;
	val.current = -1;
	val.array = NULL;
	return val;

}

intArray copyIntArray(intArray *arr)
{

	intArray val = createEmptyIntArray();
	val.array = malloc(arr->size * sizeof(int));
	memcpy(val.array, arr->array, arr->size * sizeof(int));
	val.size = arr->size;
	return val;
	
}

void printArray(intArray *arr)
{

	int size = arr->size;
	for(int i = 0; i < size; i++)
	{

		printf("%d ", arr->array[i]);

	}
	printf("\n");


}
