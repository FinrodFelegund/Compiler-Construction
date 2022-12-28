CC=gcc
CFLAGS=-O2 -Wall -ggdb

exec: lexer.l lexer.yy.c
	gcc lexer.yy.c -o exec

lexer.yy.c: lexer.l
	flex -o lexer.yy.c lexer.l

clean:
	rm -rf lexer.yy.* exec
