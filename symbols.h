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

} value_t;

typedef struct 
{

	value_t *vals;
	int used;
	int size;


} var_stack;

int vars[256];

void setvar(char *id, int nr);
int getvar(char *id);



