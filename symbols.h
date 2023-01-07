#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { intType, realType, stringType } types;

typedef struct
{
	char *m_id;
	int m_int;
	float m_real;
	char *m_string;
	types m_flag;
	int scopeBorder;
	int empty;
	int logical;

} value_t;

typedef struct 
{

	value_t *vals;
	int size;

} var_stack;

void stack_push(var_stack *st, value_t val);
void enter_func(var_stack *st);
void leave_func(var_stack *st);
value_t *lookUp(var_stack *st, char *id);
void var_declare(var_stack *st, value_t val, char *id);
void var_set(var_stack *st, value_t val, char *id);
value_t var_get(var_stack *st, char *id);
void var_dump(var_stack *st);
void printVariable(value_t val);
int getTopFunctionSize(var_stack *st);
void var_set_function(var_stack *st, value_t *vals, int size);






