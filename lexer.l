%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
typedef struct ast_node ast_node;
#include "parser.tab.h"
int lineno = 1;
char *str;


typedef struct string
{
	char *str;
	int used; 
	int size;

} string;

string *st = NULL;

string *init(int size)
{

	if(size < 5)
		size = 5;

	string *ret = malloc(sizeof(string));
	ret->used = 0;
	ret->size = 5;
	ret->str = malloc(ret->size);
	ret->str[0] = '\0';
	return ret; 

}

void push(char ch)
{

	if(st->used == st->size - 1)
	{
		st->size = st->size * 2;
		st->str = realloc(st->str, st->size);
			
	}
	st->str[st->used] = ch;
	st->str[++st->used] = '\0';
	
}

void insert(char *text)
{
	int size = strlen(text);
	for(int i = 0; i < size; i++)
	{

		push(text[i]);	

	}

}

void del()
{

	if(st)
	{

		st->size = st->used = 0;
		if(st->str)
			free(st->str);

	}

}

void printMessage(char *msg)
{
	
	
	int size = strlen(msg);
	for(int i = 0; i < size; i++)
	{
		printf("%c", msg[i]);
	}
	printf("\n");

}

%}

letter [a-zA-Z]
digit [0-9]
letordig {letter}|{digit}

ID {letter}{letordig}*
INT "0"|[1-9][0-9]*
REAL "0"|[0-9]+"."[0-9]+

%x Comment String
%option noinput nounput yylineno
%%

"//".*  { printf("Comment in line: %d\n", lineno); }

"/*"    { printf("Comment from line %d ", lineno); BEGIN(Comment); }
<Comment>"*/" { printf("to line %d\n", lineno); BEGIN(INITIAL); }
<Comment>"\n" { lineno++; }
<Comment>. {;}

"\""     { if(st == NULL) st = init(5); 
	   BEGIN(String);}
<String>"\"" { yylval.STR = malloc(st->used), strcpy(yylval.STR, st->str); del(); st = NULL;
	       BEGIN(INITIAL); return _str; }
<String>"\\x" { insert("\""); }
<String>"\\nl" { insert("\n"); lineno++; }
<String>[^\"\\x\\n]*   { insert(yytext); }

"func"    { return _FUNC; }
"if"      { return _IF; }
"else"    { return _ELSE; }
"while"   { return _WHILE; }
"+"       { return PLUOP; }
"-"       { return MINOP; }
"*"       { return MULOP; }
"/"       { return DIVOP; }
"++"      { return INCR; }
"--"      { return DECR; }
"print"   { return _PRINT; }

"("       { return LPAREN; }
")"       { return RPAREN; }
"["       { return LBRAC; }
"]"       { return RBRAC; }
"{"       { return LCURLI; }
"}"       { return RCURLI; }
";"       { return SEMI; }
"="       { return ASS; }

"getInt"  { return _GETINT; }
"getReal" { return _GETREAL; }

{ID}      { yylval.STR = malloc(yyleng); strcpy(yylval.STR, yytext); return _id; }
{INT}     { yylval.INT = atoi(yytext); return _int; }
{REAL}    { yylval.FLOAT = atof(yytext); return _real; }


"\n"      { lineno++; }
[ \t\r\f]+ { ; }	
.       { fprintf(stderr, "%s\n", "Unrecognized character"); }

%%

int yywrap(void)
{
	return 1;
}

