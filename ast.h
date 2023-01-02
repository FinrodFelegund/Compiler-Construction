#include <stdio.h>
#include <stdlib.h>
#include <graphviz/gvc.h>
#include <string.h>
#include "symbols.h"



enum node_types {ID = 0, INT = 1, REAL = 2, STRING = 3, PRINT = 4, PLUS = 5, MIN = 6, MUL = 7, DIV = 8, ASSIGN = 9, FUNC = 10, FUNCS = 11, EXPRESSIONS = 12, DECLARATION = 13, UNKNOWN = 14};
typedef enum node_types node_type;


GVC_t *gvc;
var_stack stack;



#define MAXCHILDREN 5
struct ast_node {

	node_type type;	
	value_t val;
	struct ast_node *childNodes[MAXCHILDREN];
	//for functions
	char *funcName;
	int scope;
};

typedef struct ast_node ast_node;

ast_node *new_node(int type);

void print(ast_node *root);
void printTree(ast_node *root, Agraph_t *graph, Agnode_t *node);
value_t execute(ast_node *root);



value_t plusOperation(value_t op1, value_t op2);
void printExpression(value_t val);







