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

%type  <ast> EXPRESSION FUNCTIONS FUNCTION EXPRESSIONS VARIABLE DECLARATION PRINT DEFINITION ARITHMETIC RVALUE CONSTANT PARAMS FUNCTIONCALL
%type  <data_type> TYPE

%right ASS _PRINT
%left PLUOP MINOP
%left MULOP DIVOP


%start START

%%

START: FUNCTIONS {  print($1);  execute(mainFunction); }
       | { printf("No input provided\n"); }

FUNCTIONS: FUNCTIONS FUNCTION { /*printf("Creating funcs\n");*/ $$ = new_node(FUNCS); $$->childNodes[0] = $1; $$->childNodes[1] = $2; } 
	 | FUNCTION { $$ = $1; /*printf("Creating func\n");*/ } 
	
	 	    

FUNCTION: _FUNC _id LPAREN RPAREN LCURLI EXPRESSIONS RCURLI { /*printf("Creating actual functions\n");*/ $$ = new_node(FUNC); $$->childNodes[0] = $6; $$->funcName = strdup($2);
							      if((strcmp($2, "main") == 0) && mainFunction == NULL){ mainFunction = $$; } else { pushNode($2, $$); }
							    }   
							    	  
EXPRESSIONS: EXPRESSIONS EXPRESSION SEMI { $$ = new_node(EXPRESSIONS); $$->childNodes[0] = $1; $$->childNodes[1] = $2; } 
		| { $$ = NULL; }	

EXPRESSION: DEFINITION   { $$ = $1; }  
	  | DECLARATION  { $$ = $1; }
	  | PRINT        { $$ = $1; }
	  | FUNCTIONCALL { $$ = $1; } 
	  

PRINT:      _PRINT LPAREN RVALUE RPAREN { $$ = new_node(PRINT); $$->childNodes[0] = $3; }

DEFINITION: VARIABLE ASS RVALUE     { $$ = new_node(DEFINITION); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
          | DECLARATION ASS RVALUE  { $$ = new_node(DEFINITION); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }

ARITHMETIC: RVALUE PLUOP RVALUE  { $$ = new_node(PLUS); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | RVALUE MINOP RVALUE  { $$ = new_node(MIN); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | RVALUE MULOP RVALUE  { $$ = new_node(MUL); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | RVALUE DIVOP RVALUE  { $$ = new_node(DIV); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	
RVALUE:     CONSTANT	         { $$ = new_node(RVALUE); $$->childNodes[0] = $1; }
	  | VARIABLE		 { $$ = new_node(RVALUE); $$->childNodes[0] = $1; }
	  | ARITHMETIC           { $$ = new_node(RVALUE); $$->childNodes[0] = $1; }
	  | FUNCTIONCALL         { ; }

CONSTANT:   _int                 { $$ = new_node(INT); $$->val.m_int = $1; $$->val.m_flag = intType; }
          | _real                { $$ = new_node(REAL); $$->val.m_real = $1; $$->val.m_flag = realType; }
          | _str                 { $$ = new_node(STRING); $$->val.m_string = strdup($1); $$->val.m_flag = stringType; } 
	 	

VARIABLE:   _id                         { $$ = new_node(ID); $$->val.m_id = $1; }

DECLARATION: TYPE _id                   { $$ = new_node(DECLARATION); $$->val.m_id = $2; $$->val.m_flag = $1; } 
 
TYPE:       _INT                        { $$ = intType; } 
	  | _STRING			{ $$ = stringType; }
	  | _REAL			{ $$ = realType; }

FUNCTIONCALL: _id LPAREN PARAMS RPAREN { $$ = new_node(FUNCTIONCALL); $$->val.m_id = strdup($1); }

PARAMS: PARAMS RVALUE { ; }
      | { ; }
	
%%



int main(int argc, char **argv)
{
	yyin = fopen(argv[1], "r");
	yyparse();


}

