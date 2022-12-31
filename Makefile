CC=gcc
CFLAGS=-O2 -Wall -ggdb
LDLIBS=`pkg-config libgvc --libs`

exec: parser.tab.o lexer.yy.o header.c header.h 
	$(CC) -o $@ parser.tab.o lexer.yy.o $(LDLIBS)
 
parser.tab.c parser.tab.h: parser.y
	bison --defines -v  parser.y
 
parser.tab.o: parser.tab.c parser.tab.h
 
lexer.yy.c: lexer.l
	flex -o lexer.yy.c lexer.l
 
lexer.yy.o: lexer.yy.c parser.tab.h
 
clean:
	rm -rf parser.tab.* lexer.yy.* exec parser.output out.png
