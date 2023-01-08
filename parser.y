%{
#include <assert.h>
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

%token  _FOR
	_NOTEQU
	_EQU
	_LESS
	_MORE
	_LESSEQU
	_MOREEQU
	COMMA
	 SEMI
        PLUOP
        MINOP
        MULOP
        DIVOP
	MODULO
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
       _GETRAND
       _INT
       _REAL
       _STRING
       _OR
       _AND
       _NOT	

%type  <ast> GETFUNCTIONS INCDEC INSTRUCTIONS OPTIONAL_ELSE CONTROLL LOGICAL EXPRESSION FUNCTIONS FUNCTION VARIABLE DECLARATION PRINT DEFINITION ARITHMETIC RVALUE CONSTANT CALLPARAMS FUNCTIONCALL FUNCPARAMS 
%type  <data_type> TYPE


%right ASS _PRINT
%left PLUOP MINOP _MORE _LESS _EQU _LESSEQU _MOREEQU _NOTEQU _AND _OR
%left MULOP DIVOP _NOT


%start START

%%

START: FUNCTIONS {  print($1); if(mainFunction){ execute(mainFunction);} else { printf("No main function detected\n"); } freeTree($1); }
       | { printf("No input provided\n"); }

FUNCTIONS: FUNCTIONS FUNCTION { /*printf("Creating funcs\n");*/ $$ = new_node(FUNCS); $$->childNodes[0] = $1; $$->childNodes[1] = $2; } 
	 | FUNCTION { $$ = $1; /*printf("Creating func\n");*/ } 

FUNCTION: _FUNC _id LPAREN FUNCPARAMS RPAREN LCURLI INSTRUCTIONS RCURLI { $$ = new_node(FUNC); $$->childNodes[0] = $4; $$->funcName = strdup($2); $$->childNodes[1] = $7;
                                                                          if((strcmp($2, "main") == 0) && mainFunction == NULL){ mainFunction = $$; } else { pushFunction($2, $$); }
                                                                        }
   	
INSTRUCTIONS:  INSTRUCTIONS EXPRESSION    { $$ = new_node(INSTRUCTIONS); $$->childNodes[0] = $1; $$->childNodes[1] = $2; }
	     | INSTRUCTIONS CONTROLL      { $$ = new_node(INSTRUCTIONS); $$->childNodes[0] = $1; $$->childNodes[1] = $2; } 
	     | { $$ = NULL; }
	     | INSTRUCTIONS LCURLI INSTRUCTIONS RCURLI { $$ = new_node(INSTRUCTIONS); $$->childNodes[0] = $1; if($3){ $3->blockScoped = 1; } $$->childNodes[1] = $3; }
 
CONTROLL: _IF LPAREN LOGICAL RPAREN LCURLI INSTRUCTIONS RCURLI OPTIONAL_ELSE { $$ = new_node(IF); $$->childNodes[0] = $3; $$->childNodes[1] = $6; $$->childNodes[3] = $8; }
         | _WHILE LPAREN LOGICAL RPAREN LCURLI INSTRUCTIONS RCURLI            { $$ = new_node(WHILE); $$->childNodes[0] = $3; $$->childNodes[1] = $6; }
	 | _FOR LPAREN DEFINITION SEMI LOGICAL SEMI INCDEC RPAREN LCURLI INSTRUCTIONS RCURLI { $$ = new_node(FOR); $$->childNodes[0] = $3; $$->childNodes[1] = $5; $$->childNodes[2] = $7; $$->childNodes[3] = $10; }


OPTIONAL_ELSE: _ELSE LCURLI INSTRUCTIONS RCURLI { $$ = new_node(ELSE); $$->childNodes[0] = $3; }
	     | { $$ = NULL; }

EXPRESSION: DEFINITION SEMI   { $$ = $1; }  
	  | DECLARATION SEMI  { $$ = $1; }
	  | PRINT SEMI        { $$ = $1; }
	  | FUNCTIONCALL SEMI { $$ = $1; } 	
	  | INCDEC SEMI       { $$ = $1; }

PRINT:      _PRINT LPAREN RVALUE RPAREN { $$ = new_node(PRINT); $$->childNodes[0] = $3; }

DEFINITION: VARIABLE ASS RVALUE     { $$ = new_node(DEFINITION); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
          | DECLARATION ASS RVALUE  { $$ = new_node(DEFINITION); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }

ARITHMETIC: RVALUE PLUOP RVALUE  { $$ = new_node(PLUS); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | RVALUE MINOP RVALUE  { $$ = new_node(MIN); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | RVALUE MULOP RVALUE  { $$ = new_node(MUL); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | RVALUE DIVOP RVALUE  { $$ = new_node(DIV); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }


LOGICAL:    LOGICAL _MORE LOGICAL    { $$ = new_node(LOGICAL); $$->op = strdup(">"); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
          | LOGICAL _LESS LOGICAL    { $$ = new_node(LOGICAL); $$->op = strdup("<"); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | LOGICAL _EQU LOGICAL     { $$ = new_node(LOGICAL); $$->op = strdup("=="); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | LOGICAL _LESSEQU LOGICAL { $$ = new_node(LOGICAL); $$->op = strdup("<="); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | LOGICAL _MOREEQU LOGICAL { $$ = new_node(LOGICAL); $$->op = strdup(">="); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }	
	  | LOGICAL _NOTEQU LOGICAL  { $$ = new_node(LOGICAL); $$->op = strdup("!="); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | LOGICAL _AND LOGICAL     { $$ = new_node(LOGICAL); $$->op = strdup("&&"); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | LOGICAL _OR LOGICAL      { $$ = new_node(LOGICAL); $$->op = strdup("||"); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  | LPAREN LOGICAL RPAREN    { $$ = $2; }
	  | _NOT LOGICAL             { $$ = new_node(LOGICAL); $$->op = strdup("!"); $$->childNodes[0] = $2; }
	  | RVALUE	   

RVALUE:     CONSTANT	       
	  | VARIABLE	
	  | ARITHMETIC 
	  | FUNCTIONCALL         
          | INCDEC
	  | GETFUNCTIONS
	  
GETFUNCTIONS: _GETINT  LPAREN RPAREN  { $$ = new_node(GETINT);  } 
	    | _GETREAL LPAREN RPAREN  { $$ = new_node(GETREAL); }
	    | _GETRAND LPAREN RPAREN  { $$ = new_node(GETRAND); }
	  	 
INCDEC:   VARIABLE INCR          { $$ = new_node(INCDEC); $$->childNodes[0] = $1; $$->op = strdup("++"); }
	  | VARIABLE DECR	 { $$ = new_node(INCDEC); $$->childNodes[0] = $1; $$->op = strdup("--"); }

CONSTANT:   _int                 { $$ = new_node(INT); $$->val.m_int = $1; $$->val.m_flag = intType; }
          | _real                { $$ = new_node(REAL); $$->val.m_real = $1; $$->val.m_flag = realType; }
          | _str                 { $$ = new_node(STRING); $$->val.m_string = strdup($1); $$->val.m_flag = stringType; } 
	 	

VARIABLE:   _id                         { $$ = new_node(ID); $$->val.m_id = $1; }

DECLARATION: TYPE _id                   { $$ = new_node(DECLARATION); $$->val.m_id = $2; $$->val.m_flag = $1; } 
	   
TYPE:       _INT                        { $$ = intType; } 
	  | _STRING			{ $$ = stringType; }
	  | _REAL			{ $$ = realType; }

FUNCTIONCALL: _id LPAREN CALLPARAMS RPAREN { $$ = new_node(FUNCTIONCALL); $$->val.m_id = strdup($1); $$->childNodes[0] = $3; }

CALLPARAMS: CALLPARAMS COMMA RVALUE { $$ = new_node(CALLPARAMS); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
      	   | RVALUE { $$ = new_node(CALLPARAMS); $$->childNodes[0] = $1; }
	   | { $$ = NULL; } 	
FUNCPARAMS: FUNCPARAMS COMMA DECLARATION      { $$ = new_node(FUNCPARAMS); $$->childNodes[0] = $1; $$->childNodes[1] = $3; } 
	   | DECLARATION { $$ = $1; }
	   | { $$ = NULL; }

%%

int checkFileName(const char *fileName)
{

	const char *dot = strrchr(fileName, '.');
	if(strcmp(dot, ".frog") == 0)
	{
		return 1;
        }

	return 0;

}

int main(int argc, char **argv)
{
	assert(argc > 1);
	setRand();
	if(checkFileName(argv[1]))
	{
		FILE *file = fopen(argv[1], "r");
		yyin = file;
		yyparse();
		fclose(file);
	} else {
		fprintf(stderr, "This interpreter only handels frogs! Quack\n");
	}

	return 0;

}

