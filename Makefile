CC=gcc
CFLAGS=-O2 -Wall -ggdb
LDLIBS=`pkg-config libgvc --libs`

exec: parser.tab.o lexer.yy.o ast.o symbols.o 
	$(CC) -o $@ parser.tab.o lexer.yy.o ast.o $(LDLIBS)
 
parser.tab.c parser.tab.h: parser.y 
	bison --defines -v  parser.y
 
ast.o: ast.c ast.h
	$(CC) -c ast.c 

symbols.o: symbols.c symbols.h
	$(CC) -c symbols.c

parser.tab.o: parser.tab.c parser.tab.h
 
lexer.yy.c: lexer.l
	flex -o lexer.yy.c lexer.l
 
lexer.yy.o: lexer.yy.c parser.tab.h
 
clean:
	rm -rf ast.o parser.tab.* lexer.yy.* exec parser.output out.png
