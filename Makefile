# Compiler and options
CC = gcc
CFLAGS = -Wall -Wextra -g

# Main files
EXEC = vypcomp
SRC = src
LEXER_SRC = $(SRC)/lexer.l
PARSER_SRC = $(SRC)/parser.y
MAIN_SRC = $(SRC)/main.c
AST_SRC = $(SRC)/ast.c
SYMBOL_TABLE_SRC = $(SRC)/symbol_table.c
SEMANTIC_SRC = $(SRC)/semantic_analysis.c

# Generated files
LEXER_GEN = $(SRC)/lexer.c
PARSER_GEN = $(SRC)/parser.c
PARSER_HEADER = $(SRC)/parser.h

# Objects
OBJS = parser.o lexer.o main.o ast.o symbol_table.o semantic_analysis.o

# Main rule
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) -lfl

# Object for the parser
parser.o: $(PARSER_GEN) $(PARSER_HEADER) $(SRC)/ast.h $(SRC)/symbol_table.h
	$(CC) $(CFLAGS) -c -o parser.o $(PARSER_GEN)

# Object for the lexer
lexer.o: $(LEXER_GEN) $(PARSER_HEADER) $(SRC)/ast.h
	flex -o $(LEXER_GEN) $(LEXER_SRC)
	$(CC) $(CFLAGS) -c -o lexer.o $(LEXER_GEN)

# Object for the main file
main.o: $(MAIN_SRC) $(SRC)/ast.h $(SRC)/symbol_table.h $(SRC)/semantic_analysis.h
	$(CC) $(CFLAGS) -c -o main.o $(MAIN_SRC)

# Object for AST
ast.o: $(AST_SRC) $(SRC)/ast.h
	$(CC) $(CFLAGS) -c -o ast.o $(AST_SRC)

# Object for the symbols table
symbol_table.o: $(SYMBOL_TABLE_SRC) $(SRC)/symbol_table.h
	$(CC) $(CFLAGS) -c -o symbol_table.o $(SYMBOL_TABLE_SRC)

# Object for semantic analysis
semantic_analysis.o: $(SEMANTIC_SRC) $(SRC)/semantic_analysis.h $(SRC)/symbol_table.h
	$(CC) $(CFLAGS) -c -o semantic_analysis.o $(SEMANTIC_SRC)

# PARSER GENERATION
$(PARSER_GEN) $(PARSER_HEADER): $(PARSER_SRC)
	bison -d -o $(PARSER_GEN) $(PARSER_SRC)

# Cleaning
clean:
	rm -f $(EXEC) $(LEXER_GEN) $(PARSER_GEN) $(PARSER_HEADER) $(OBJS)
