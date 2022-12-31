#include <stdio.h>
#include <stdlib.h>
#include <graphviz/gvc.h>
#include <string.h>

enum types {ID, INT, REAL, STRING, PRINT, PLUS, MIN, MUL, DIV, ASSIGN, FUNC, FUNCS, EXPRESSIONS, UNKNOWN};
typedef enum types types;

GVC_t *gvc;

struct value{

	int INR;
	float FNR;
	char *ID;
	char *STR;

};

typedef struct value value_t;

#define MAXCHILDREN 5
struct ast_node {

	types type;	
	value_t val;
	struct ast_node *childNodes[MAXCHILDREN];
	//for functions
	char *funcName;
};

typedef struct ast_node ast_node;

ast_node *new_node(int type);

void print(ast_node *root);
void printTree(ast_node *root, Agraph_t *graph, Agnode_t *node);
