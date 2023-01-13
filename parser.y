%{
#include <assert.h>
//#include "ast.h"
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

%token  _RETURN
	_FOR
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
       _ARRAY
       _OR
       _AND
       _NOT	

%type  <ast> ARRAY_DECLARATION OPTIONAL_RVALUE GETFUNCTIONS INCDEC INSTRUCTIONS OPTIONAL_ELSE CONTROLL LOGICAL EXPRESSION FUNCTIONS FUNCTION VARIABLE DECLARATION PRINT DEFINITION ARITHMETIC RVALUE CONSTANT CALLPARAMS FUNCTIONCALL FUNCPARAMS 
%type  <data_type> TYPE


%right ASS _PRINT
%left PLUOP MINOP _MORE _LESS _EQU _LESSEQU _MOREEQU _NOTEQU _AND _OR MODULO
%left MULOP DIVOP _NOT


%start START

%%

START: FUNCTIONS { startOptimization($1);  print($1); if(mainFunction){ execute(mainFunction);} else { printf("No main function detected\n"); } /*printf("Last dump\n"); var_dump();*/  freeAll($1); }
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
 
CONTROLL: _IF LPAREN LOGICAL RPAREN LCURLI INSTRUCTIONS RCURLI OPTIONAL_ELSE { $$ = new_node(IF); $$->childNodes[0] = $3; $$->childNodes[1] = $6; $$->childNodes[2] = $8; }
         | _WHILE LPAREN LOGICAL RPAREN LCURLI INSTRUCTIONS RCURLI            { $$ = new_node(WHILE); $$->childNodes[0] = $3; $$->childNodes[1] = $6; }
	 | _FOR LPAREN DEFINITION SEMI LOGICAL SEMI INCDEC RPAREN LCURLI INSTRUCTIONS RCURLI { $$ = new_node(FOR); $$->childNodes[0] = $3; $$->childNodes[1] = $5; $$->childNodes[2] = $7; $$->childNodes[3] = $10; }


OPTIONAL_ELSE: _ELSE LCURLI INSTRUCTIONS RCURLI { $$ = new_node(ELSE); $$->childNodes[0] = $3; }
	     | { $$ = NULL; }

EXPRESSION: DEFINITION SEMI   { $$ = $1; }  
	  | DECLARATION SEMI  { $$ = $1; }
	  | PRINT SEMI        { $$ = $1; }
	  | FUNCTIONCALL SEMI { $$ = $1; } 	
	  | INCDEC SEMI       { $$ = $1; }
          | _RETURN OPTIONAL_RVALUE SEMI { $$ = new_node(RETURN); $$->childNodes[0] = $2; }
	  | ARRAY_DECLARATION SEMI { $$ = $1; }

OPTIONAL_RVALUE: ARITHMETIC { $$ = $1; }
		| { $$ = NULL; }
PRINT:      _PRINT LPAREN ARITHMETIC RPAREN { $$ = new_node(PRINT); $$->childNodes[0] = $3; }

DEFINITION: VARIABLE ASS ARITHMETIC     { $$ = new_node(DEFINITION); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
          | DECLARATION ASS ARITHMETIC  { $$ = new_node(DEFINITION); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
	  


ARITHMETIC: ARITHMETIC PLUOP ARITHMETIC  { $$ = new_node(ARITHMETIC); $$->childNodes[0] = $1; $$->childNodes[1] = $3; $$->op = strdup("+"); }
	  | ARITHMETIC MINOP ARITHMETIC  { $$ = new_node(ARITHMETIC); $$->childNodes[0] = $1; $$->childNodes[1] = $3; $$->op = strdup("-"); }
	  | ARITHMETIC MULOP ARITHMETIC  { $$ = new_node(ARITHMETIC); $$->childNodes[0] = $1; $$->childNodes[1] = $3; $$->op = strdup("*"); }
	  | ARITHMETIC DIVOP ARITHMETIC  { $$ = new_node(ARITHMETIC); $$->childNodes[0] = $1; $$->childNodes[1] = $3; $$->op = strdup("/"); }
	  | ARITHMETIC MODULO ARITHMETIC { $$ = new_node(ARITHMETIC); $$->childNodes[0] = $1; $$->childNodes[1] = $3; $$->op = strdup("%"); }
	  | LPAREN ARITHMETIC RPAREN    { $$ = $2; }
	  | RVALUE   

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
	  | FUNCTIONCALL         
          | INCDEC
	  | GETFUNCTIONS
	  
	  
GETFUNCTIONS: _GETINT  LPAREN RPAREN  { $$ = new_node(GETINT);  } 
	    | _GETREAL LPAREN RPAREN  { $$ = new_node(GETREAL); }
	    | _GETRAND LPAREN RPAREN  { $$ = new_node(GETRAND); }
	  	 
INCDEC:   VARIABLE INCR          { $$ = new_node(INCDEC); $$->childNodes[0] = $1; $$->op = strdup("++"); }
	  | VARIABLE DECR	 { $$ = new_node(INCDEC); $$->childNodes[0] = $1; $$->op = strdup("--"); }

CONSTANT:   _int                 { $$ = new_node(INT); $$->val.m_int = $1; $$->val.m_flag = intType; $$->val.boolean = $1 != 0; }
          | _real                { $$ = new_node(REAL); $$->val.m_real = $1; $$->val.m_flag = realType; $$->val.boolean = $1 != 0; }
          | _str                 { $$ = new_node(STRING); $$->val.m_string = strdup($1); $$->val.m_flag = stringType; $$->val.boolean = strlen($1) != 0; } 
	 	

VARIABLE:   _id                         { $$ = new_node(ID); $$->val.m_id = $1; }
	 | _id LBRAC ARITHMETIC RBRAC       { $$ = new_node(ARR_ID); $$->val.m_id = strdup($1); $$->childNodes[0] = $3; }


DECLARATION: TYPE _id                   { $$ = new_node(DECLARATION); $$->val.m_id = $2; $$->val.m_flag = $1; } 
//	    | ARRAY_DECLARATION


	      
ARRAY_DECLARATION: TYPE LBRAC RBRAC _id { $$ = new_node(DECLARATION); $$->val.m_id = $4; $$->val.m_flag = $1; }


TYPE:       _INT                        { $$ = intType; } 
	  | _STRING			{ $$ = stringType; }
	  | _REAL			{ $$ = realType; }
	  | _INT _ARRAY                 { $$ = intArrayType; }

FUNCTIONCALL: _id LPAREN CALLPARAMS RPAREN { $$ = new_node(FUNCTIONCALL); $$->val.m_id = strdup($1); $$->childNodes[0] = $3; }

CALLPARAMS: CALLPARAMS COMMA ARITHMETIC { $$ = new_node(CALLPARAMS); $$->childNodes[0] = $1; $$->childNodes[1] = $3; }
      	   | ARITHMETIC { $$ = new_node(CALLPARAMS); $$->childNodes[0] = $1; }
	   | { $$ = NULL; } 	
FUNCPARAMS: FUNCPARAMS COMMA DECLARATION      { $$ = new_node(FUNCPARAMS); $$->childNodes[0] = $1; $$->childNodes[1] = $3; } 
	   | DECLARATION { $$ = $1; }
	   | FUNCPARAMS COMMA ARRAY_DECLARATION { $$ = new_node(FUNCPARAMS); $$->childNodes[0] = $1; $$->childNodes[1] = $3; } 
	   | ARRAY_DECLARATION                  { $$ = $1; }
	   | { $$ = NULL; }

%%

int checkFileName(const char *fileName)
{

	const char *dot = strrchr(fileName, '.');
	if(strcmp(dot, ".frail") == 0)
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
		fprintf(stderr, "This interpreter only handels frail files\n");
	}

	return 0;

}

