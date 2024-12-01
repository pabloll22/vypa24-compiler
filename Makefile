vypcomp: parser.o lexer.o main.o
        gcc -o vypcomp parser.o lexer.o main.o -lfl

parser.o: src/parser.c
        gcc -c -o parser.o src/parser.c

lexer.o: src/lexer.l src/parser.h
        flex -o src/lexer.c src/lexer.l
        gcc -c -o lexer.o src/lexer.c

src/parser.c src/parser.h: src/parser.y
        bison -d -o src/parser.c src/parser.y

main.o: src/main.c
        gcc -c -o main.o src/main.c

clean:
        rm -f vypcomp src/parser.c src/parser.h src/lexer.c parser.o lexer.o main.o