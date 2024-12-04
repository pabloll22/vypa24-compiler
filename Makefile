# Compilador y opciones
CC = gcc
CFLAGS = -Wall -Wextra -g

# Archivos principales
EXEC = vypcomp
SRC = src
LEXER_SRC = $(SRC)/lexer.l
PARSER_SRC = $(SRC)/parser.y
MAIN_SRC = $(SRC)/main.c
AST_SRC = $(SRC)/ast.c
SYMBOL_TABLE_SRC = $(SRC)/symbol_table.c

# Archivos generados
LEXER_GEN = $(SRC)/lexer.c
PARSER_GEN = $(SRC)/parser.c
PARSER_HEADER = $(SRC)/parser.h

# Objetos
OBJS = symbol_table.o parser.o lexer.o main.o ast.o

# Regla principal
$(EXEC): $(OBJS)
        $(CC) $(CFLAGS) -o $(EXEC) $(OBJS) -lfl

# Objeto para el parser
parser.o: $(PARSER_GEN) $(PARSER_HEADER) $(SRC)/ast.h $(SRC)/symbol_table.h
        $(CC) $(CFLAGS) -c -o parser.o $(PARSER_GEN)

# Objeto para el lexer
lexer.o: $(LEXER_GEN) $(PARSER_HEADER) $(SRC)/ast.h $(SRC)/symbol_table.h
        flex -o $(LEXER_GEN) $(LEXER_SRC)
        $(CC) $(CFLAGS) -c -o lexer.o $(LEXER_GEN)

# Objeto para el archivo principal
main.o: $(MAIN_SRC) $(SRC)/ast.h $(SRC)/symbol_table.h
        $(CC) $(CFLAGS) -c -o main.o $(MAIN_SRC)

# Objeto para el AST
ast.o: $(AST_SRC) $(SRC)/ast.h
        $(CC) $(CFLAGS) -c -o ast.o $(AST_SRC)

# Objeto para la tabla de símbolos
symbol_table.o: $(SYMBOL_TABLE_SRC) $(SRC)/symbol_table.h
        $(CC) $(CFLAGS) -c -o symbol_table.o $(SYMBOL_TABLE_SRC)

# Generación del parser
$(PARSER_GEN) $(PARSER_HEADER): $(PARSER_SRC)
        bison -d -o $(PARSER_GEN) $(PARSER_SRC)

$(LEXER_GEN): $(PARSER_HEADER) $(LEXER_SRC)
        flex -o $(LEXER_GEN) $(LEXER_SRC)

# Limpieza
clean:
        rm -f $(EXEC) $(LEXER_GEN) $(PARSER_GEN) $(PARSER_HEADER) $(OBJS)
        rm -f *.output *.pre
