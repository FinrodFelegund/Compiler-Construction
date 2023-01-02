%{
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int lineno;
extern int yylex();
extern FILE *yyin;

void yyerror(char *msg)
{
	fprintf(stderr, "Error type: %s in line %d\n", msg, lineno);
	exit(1);
}

ast_node *mainFunction = NULL;

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
       <op> _NL

%type  <ast> EXPRESSION FUNCTIONS FUNCTION EXPRESSIONS

%right ASS _PRINT
%left PLUOP MINOP
%left MULOP DIVOP


%start START

%%

START: FUNCTIONS { $1->funcName = "begin"; print($1); execute(mainFunction); }

FUNCTIONS: FUNCTIONS FUNCTION { $$ = new_node(FUNCS); $$->childNodes[0] = $1; $$->childNodes[1] = $2; } 
           | FUNCTION           { $$ = $1; }
	   

FUNCTION: _FUNC _id LPAREN RPAREN LCURLI EXPRESSIONS RCURLI { $$ = new_node(FUNC); $$->childNodes[0] = $6; $$->funcName = $2;
							      if((strcmp($2, "main") == 0) && mainFunction == NULL){ mainFunction = $$; }   
							    }

EXPRESSIONS: EXPRESSIONS EXPRESSION SEMI { $$ = new_node(EXPRESSIONS); $$->childNodes[0] = $1; $$->childNodes[1] = $2; } 
		| EXPRESSION SEMI { $$ = new_node(EXPRESSIONS); $$->childNodes[0] = $1; }
		 

EXPRESSION: EXPRESSION PLUOP EXPRESSION { $$ = new_node(PLUS); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
          | EXPRESSION MINOP EXPRESSION { $$ = new_node(MIN); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | EXPRESSION MULOP EXPRESSION { $$ = new_node(MUL); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | EXPRESSION DIVOP EXPRESSION { $$ = new_node(DIV); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | EXPRESSION ASS   EXPRESSION        { $$ = new_node(ASSIGN); $$->childNodes[0] = $1; $$->childNodes[1] = $3; $1->val.dType = $3->val.dType; }
	  | _PRINT LPAREN EXPRESSION RPAREN { $$ = new_node(PRINT); $$->childNodes[0] = $3; } 
	  | _int                        { $$ = new_node(INT); $$->val.INR = $1; $$->val.dType = typeInt; }
	  | _id				{ $$ = new_node(ID); $$->val.ID = $1; }
          | _real                       { $$ = new_node(REAL); $$->val.FNR = $1; $$->val.dType = typeReal;}
          | _str                        { $$ = new_node(STRING); $$->val.STR = $1; $$->val.dType = typeString; } 


%%



int main(int argc, char **argv)
{
	yyin = fopen(argv[1], "r");
	yyparse();


}

