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

typedef struct 
{
	char *str;
} string;

typedef struct
{

	char *str;

} identifier;

%}


%union
{

	ast_node *ast;
	int iconst;
	float rconst;
	char *sconst;
	int data_type;
		
};

%token <sconst> _str
       <iconst> _int
       <rconst> _real
       <sconst> _id

%token  SEMI
        PLUOP
        MINOP
        MULOP
        DIVOP
        ASS
        _FUNC
        _IF
        _ELSE
        _WHILE
        INCR
        DECR
        _PRINT
        LPAREN
        RPAREN
        LBRAC
        RBRAC
        LCURLI
        RCURLI
       _GETINT
       _GETREAL
       _INT
       _REAL
       _STRING

%type  <ast> EXPRESSION FUNCTIONS FUNCTION EXPRESSIONS
%type  <data_type> TYPE

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
	  | EXPRESSION ASS   EXPRESSION        { $$ = new_node(ASSIGN); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | _PRINT LPAREN EXPRESSION RPAREN { $$ = new_node(PRINT); $$->childNodes[0] = $3; } 
	  | TYPE _id                    { $$ = new_node(DECLARATION); $$->val.m_id = $2; $$->val.m_flag = $1; }
	  | _int                        { $$ = new_node(INT); $$->val.m_int = $1; $$->val.m_flag = intType; }
          | _real                       { $$ = new_node(REAL); $$->val.m_real = $1; $$->val.m_flag = realType; }
          | _str                        { $$ = new_node(STRING); $$->val.m_string = $1; $$->val.m_flag = stringType; } 
	  | _id                         { $$ = new_node(ID); $$->val.m_id = $1; } //gotta somehow find out about the variable type
	  
TYPE:       _INT                        { $$ = intType; } 
	  | _STRING			{ $$ = stringType; }
	  | _REAL			{ $$ = realType; }

%%



int main(int argc, char **argv)
{
	yyin = fopen(argv[1], "r");
	yyparse();


}

