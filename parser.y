%{
#include "header.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int lineno;
extern int yylex();
extern FILE *yyin;

void yyerror(char *msg)
{
	fprintf(stderr, "%s\n", msg);
}

int vars[256];
void setvar(char *id, int nr)
{
	vars[(int)(*id)] = nr;

}

int getvar(char *id)
{
	int var = vars[(int)*id];
	return var;
}
ast_node *mainFunction = NULL;
ast_node *functions[10];
int functionIndex = 10;
%}

%union
{

	ast_node *ast;
	char *op;
	int INT;
	float FLOAT;
	char *STR;	
};

%token <STR> _str
       <INT> _int
       <FLOAT> _real
       <STR> _id

%token <op> SEMI
       <op> PLUOP
       <op> MINOP
       <op> MULOP
       <op> DIVOP
       <op> ASS
       <op> _FUNC
       <op> _IF
       <op> _ELSE
       <op> _WHILE
       <op> INCR
       <op> DECR
       <op> _PRINT
       <op> LPAREN
       <op> RPAREN
       <op> LBRAC
       <op> RBRAC
       <op> LCURLI
       <op> RCURLI
       <op> _GETINT
       <op> _GETREAL

%type  <ast> EXPRESSION FUNCTIONS FUNCTION EXPRESSIONS

%right ASS
%right _PRINT
%left PLUOP MINOP
%left MULOP DIVOP


%start START

%%

START: FUNCTIONS { $1->funcName = "begin"; print($1); }

FUNCTIONS: FUNCTIONS FUNCTION { $$ = new_node(FUNCS); $$->childNodes[0] = $1; $$->childNodes[1] = $2; } 
         | FUNCTION           { $$ = $1; } 

FUNCTION: _FUNC _id LPAREN RPAREN LCURLI EXPRESSIONS RCURLI { $$ = new_node(FUNC); $$->childNodes[0] = $6; $$->funcName = $2;
							      if((strcmp($2, "main") == 0) && mainFunction == NULL){ mainFunction = $$; }   
							    }

EXPRESSIONS: EXPRESSIONS EXPRESSION SEMI { $$ = new_node(EXPRESSIONS); $$->childNodes[0] = $1; $$->childNodes[1] = $2; } 
		| EXPRESSION SEMI { $$ = new_node(EXPRESSIONS); $$->childNodes[0] = $1; }
		| error SEMI { yyerrok; }

EXPRESSION: EXPRESSION PLUOP EXPRESSION { $$ = new_node(PLUS); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
          | EXPRESSION MINOP EXPRESSION { $$ = new_node(MIN); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | EXPRESSION MULOP EXPRESSION { $$ = new_node(MUL); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | EXPRESSION DIVOP EXPRESSION { $$ = new_node(DIV); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | EXPRESSION ASS   EXPRESSION        { $$ = new_node(ASSIGN); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | _PRINT LPAREN EXPRESSION RPAREN { $$ = new_node(PRINT); $$->childNodes[0] = $3; } 
	  | _int                        { $$ = new_node(INT); $$->val.INR = $1; }
	  | _id				{ $$ = new_node(ID); $$->val.ID = $1; }
          | _real                       { $$ = new_node(REAL); $$->val.FNR = $1;}
     


%%



int main(int argc, char **argv)
{
	yyin = fopen(argv[1], "r");
	yyparse();


}

