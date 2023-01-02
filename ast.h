#include <stdio.h>
#include <stdlib.h>
#include <graphviz/gvc.h>
#include <string.h>

int vars[256];
char *strings[256];

enum types {ID = 0, INT = 1, REAL = 2, STRING = 3, PRINT = 4, PLUS = 5, MIN = 6, MUL = 7, DIV = 8, ASSIGN = 9, FUNC = 10, FUNCS = 11, EXPRESSIONS = 12, UNKNOWN = 13};
typedef enum types types;

enum dataType{ typeInt, typeReal, typeString};
typedef enum dataType dataType;

GVC_t *gvc;

struct value{

	int INR;
	float FNR;
	char *ID;
	char *STR;
	dataType dType;

};

typedef struct value value_t;

#define MAXCHILDREN 5
struct ast_node {

	types type;	
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
int getvar(char *id);
void setvar(char *id, int val);
int searchvar(char *id);
value_t plusOperation(value_t op1, value_t op2);
void printExpression(value_t val);
value_t getExpression(char *id);
void assignExpression(char *id, value_t val);
void setString(char *id, char *string);
char *getString(char *id);
int searchString(char *id);



