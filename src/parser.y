                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  %{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#define YYDEBUG 1
extern int yydebug;
extern int yylex();
void yyerror(const char* msg);
%}

%union {
    int ival;
    char* sval;
//    ASTNode* astNode;
}

%token <sval> CLASS INT STRING VOID IDENTIFIER STRING_LITERAL
%token <ival> INTEGER_LITERAL
%token IF WHILE ELSE RETURN PRINT READ_INT READ_STRING
%token LE GE EQ NE
%token SUPER NEW THIS
%token '.'

%left '.'
%left '+' '-'
%left '*' '/'
%nonassoc '<' '>' LE GE EQ NE

%%

// Gramática principal
program:
    class_definitions
    | function_definitions
    | class_definitions function_definitions
;

class_definitions:
    class_definitions class_definition
    | class_definition
;

class_definition:
    CLASS IDENTIFIER ':' IDENTIFIER '{' class_body '}'
        { printf("Class %s inherits from %s\n", $2, $4); }
    | CLASS IDENTIFIER '{' class_body '}'
        { printf("Class %s with no inheritance\n", $2); }
;

class_body:
    /* vacío */
    | class_body class_member
;

class_member:
    declaration
    | function_definition
;

function_definitions:
    function_definitions function_definition
    | function_definition
;

function_definition:
    type IDENTIFIER '(' parameter_list ')' block
;

parameter_list:
    /* vacío */
    |VOID
    | parameter_declaration_list
;

parameter_declaration_list:
    parameter_declaration
    | parameter_declaration_list ',' parameter_declaration
;

parameter_declaration:
    type IDENTIFIER
;

// Declaraciones
declaration:
    type IDENTIFIER ';'
    | type IDENTIFIER '=' expression ';'
    | type IDENTIFIER ',' IDENTIFIER_LIST ';'
;

IDENTIFIER_LIST:
    IDENTIFIER
    | IDENTIFIER_LIST ',' IDENTIFIER
;

type:
    simple_type
    |user_type

simple_type:
    INT
    | STRING
    | VOID

user_type:
    IDENTIFIER;


// Bloques de código
block:
    '{' declaration_or_statement_list '}'
;

declaration_or_statement_list:
    /* vacío */
    | declaration_or_statement
    | declaration_or_statement_list declaration_or_statement
;

declaration_or_statement:
    declaration
    | statement
;

// Sentencias
statement:
    IF '(' expression ')' block ELSE block
    | IF '(' expression ')' block
    | WHILE '(' expression ')' block
    | RETURN expression ';'
    | PRINT '(' print_arguments ')' ';'
    | IDENTIFIER '=' expression ';'
    | block
    | ';'
;

print_arguments:
    STRING_LITERAL
    | STRING_LITERAL ',' print_arguments
    | expression
    | expression ',' print_arguments
;

expression:
  INTEGER_LITERAL
  | STRING_LITERAL
  | IDENTIFIER
  | expression '.' IDENTIFIER  /* Acceso a atributo o método */
  | expression '.' IDENTIFIER '(' argument_list ')'  /* Llamada a método */
  | IDENTIFIER '.' IDENTIFIER /* Acceso a atributo */
  | IDENTIFIER '.' IDENTIFIER '(' argument_list ')'  /* Llamada a método */
  | SUPER '.' IDENTIFIER '(' argument_list ')'  /* Llamada a método desde super */
  | NEW IDENTIFIER '(' argument_list ')'  /* Constructor */
  | NEW IDENTIFIER  /* Instancia sin argumentos */
  | THIS '.' IDENTIFIER  /* Acceso a atributo/método de la clase */
  | THIS '.' IDENTIFIER '(' argument_list ')'
  | expression '(' argument_list ')'  /* Llamada a función */
  | IDENTIFIER '(' argument_list ')'  /* Llamada a función */
  | READ_INT '(' ')'  /* Leer entero */
  | READ_STRING '(' ')'  /* Leer string */
  | '(' expression ')'  /* Expresión entre paréntesis */
  | '(' type ')' expression  /* Conversión de tipo */
  | expression '+' expression
  | expression '-' expression
  | expression '*' expression
  | expression '/' expression
  | expression '<' expression
  | expression '>' expression
  | expression LE expression
  | expression GE expression
  | expression EQ expression
  | expression NE expression
;

argument_list:
    /* vacío */  /* Para funciones sin argumentos */
    | expression
    | argument_list ',' expression
;

%%

// Función para manejar errores
void yyerror(const char* msg) {
    fprintf(stderr, "Error: %s\n", msg);
}
