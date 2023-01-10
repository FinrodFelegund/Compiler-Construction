#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define func_border 1
#define block_border 2

typedef enum { intType, realType, stringType, intArrayType } types;

typedef struct
{
	int *array;
	int current;
	int size;
} intArray;

typedef struct
{
	
	char *m_id;
	intArray m_intArray;
	int m_int;
	float m_real;
	char *m_string;
	types m_flag;
	int scopeBorder;
	int empty;


} value_t;

typedef struct 
{

	value_t *vals;
	int size;
	int used;

} var_stack;

void stack_push(value_t val);
void enter_func();
void leave_func();
void enter_block();
void leave_block();
value_t *lookUp(char *id);
void var_declare(value_t val);
void var_set(value_t val, char *id);
value_t var_get(char *id);
void var_dump();
void printVariable(value_t val);
int getTopFunctionSize();
void var_set_function(value_t *vals, int size);
void freeStack();



void pushIntArray(intArray *arr, int pos, int val);
int lookUpIntArray(intArray *arr, int pos);
void freeIntArray(intArray *arr);
intArray createEmptyIntArray();
intArray copyIntArray(intArray *arr);



