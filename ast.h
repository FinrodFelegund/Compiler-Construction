#include <stdio.h>
#include <stdlib.h>
#include <graphviz/gvc.h>
#include <string.h>
#include <time.h>
#include "symbols.h"



enum node_types {ID = 0, INT = 1, REAL = 2, STRING = 3, PRINT = 4, PLUS = 5, MIN = 6, MUL = 7, DIV = 8, FUNC = 9, FUNCS = 10, 
	         DECLARATION = 12, DEFINITION = 13, RVALUE = 14, FUNCTIONCALL = 15, FUNCPARAMS = 16, CALLPARAMS = 17, MORE = 18, LESS = 19, EQU = 20, 
                 LESSEQU = 21, MOREEQU = 22, IF = 23, INSTRUCTIONS = 24, NOTEQU = 25, LOGICAL = 26, ELSE = 27, WHILE = 28, INCDEC = 29, FOR = 30, 
	         GETINT = 31, GETREAL = 32, GETRAND = 33, ARITHMETIC = 34, RETURN = 35, ARRAY_DEC = 36, ARR_ID = 37 };
typedef enum node_types node_type;


GVC_t *gvc;


#define MAXCHILDREN 5
struct ast_node {

	node_type type;	
	value_t val;
	struct ast_node *childNodes[MAXCHILDREN];
	//for functions
	char *funcName;
	int blockScoped;
	char *op;
};

typedef struct ast_node ast_node;

ast_node *new_node(int type);

void print(ast_node *root);
void printTree(ast_node *root, Agraph_t *graph, Agnode_t *node);
value_t execute(ast_node *root);
void freeAll(ast_node *root);
void freeTree(ast_node *root);
int checkScope(ast_node *root);
char *getDataType(value_t val);

value_t assignement(value_t dest, value_t source);
char *concate(char *op1, char *op2);
value_t arithmeticOperation(char *op, value_t op1, value_t op2);
value_t incdec(char *op, value_t op1);
value_t determineLogical(char *op, value_t op1, value_t op2);
void printExpression(value_t val);

// --- FUNCTIONS ---

typedef struct
{
	char *id;
	ast_node *node;

} function_node;

typedef struct
{

	function_node *nodes;
	int size;
	int used;

} function_stack;

void pushFunction(char *id, ast_node *node);
ast_node *getFunction(char *id);
void dumpFunctions();
void freeFunctions();
// --- FUNCTIONS ---

// --- CALLING PARAMETER ---

typedef struct 
{

	value_t *vals;
	int size;

} callParams;

void pushParam(value_t val);
void resetParams();
void dumpParams();
void setParams();
int getParamsSize();

// --- CALLING PARAMETER

// --- GET FUNCTIONS ---

int getInt();
float getReal();
int getRand();
void setRand();

// --- GET FUNCTIONS ---

// --- RETURN ---

void setReturnVal(value_t val);
value_t getReturnVal();

// --- RETURN ---

// --- OPTIMIZATION ---

void startOptimization(ast_node *node);
void constantFolding(ast_node *node);
void deadCodeElimination(ast_node *prev, int index, ast_node *node);
