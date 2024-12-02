%{
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#define YYDEBUG 1

extern int yydebug;
extern int yylex();
extern char* yytext;

void yyerror(const char* msg);
extern ASTNode* root;

%}

%union {
    int ival;
    char* sval;
    ASTNode* astNode;
}

%token <sval> CLASS INT STRING VOID IDENTIFIER STRING_LITERAL
%token <ival> INTEGER_LITERAL
%token IF WHILE ELSE RETURN PRINT READ_INT READ_STRING
%token LE GE EQ NE
%token SUPER NEW THIS
%token '.'

%type <sval> type simple_type user_type

%left '.'
%left '+' '-'
%left '*' '/'
%nonassoc '<' '>' LE GE EQ NE

%type <astNode> program
%type <astNode> class_definitions
%type <astNode> class_body
%type <astNode> class_member
%type <astNode> function_definitions
%type <astNode> class_definition
%type <astNode> function_definition
%type <astNode> declaration
%type <astNode> parameter_list
%type <astNode> expression
%type <astNode> statement
%type <astNode> IDENTIFIER_LIST
%type <astNode> block
%type <astNode> declaration_or_statement_list
%type <astNode> declaration_or_statement
%type <astNode> parameter_declaration_list
%type <astNode> parameter_declaration
%type <astNode> argument_list

%%

// Gramática principal
program:
    class_definitions {
        root = (ASTNode*)createProgramNode($1, NULL);
    }
    | function_definitions {
        root = (ASTNode*)createProgramNode(NULL, $1);
    }
    | class_definitions function_definitions {
        root = (ASTNode*)createProgramNode($1, $2);
    }
;

class_definitions:
    class_definitions class_definition
    | class_definition
;

class_definition:
    CLASS IDENTIFIER ':' IDENTIFIER '{' class_body '}' {
        $$ = (ASTNode*)createClassNode($2, $4, $6);  // Crear nodo de clase
    }
    | CLASS IDENTIFIER '{' class_body '}' {
        $$ = (ASTNode*)createClassNode($2, NULL, $4);  // Sin herencia
    }
;

class_body:
    /* vacío */{
        $$ = NULL;
    }
    | class_body class_member {
        $$ = (ASTNode*)appendNode($1, $2);  // Agregar el nuevo miembro al cuerpo de la clase
    }
;

class_member:
    declaration {
        $$ = $1;  // El miembro es una declaración
    }
    | function_definition {
        $$ = $1;  // El miembro es una función
    }
;

function_definitions:
    function_definitions function_definition {
        $$ = (ASTNode*)appendNode($1, $2);  // Combina la lista de funciones con una nueva función
    }
    | function_definition {
        $$ = $1;  // La lista inicial es simplemente el primer nodo
    }
;

function_definition:
    type IDENTIFIER '(' parameter_list ')' block {
        $$ = (ASTNode*)createFunctionNode($2, $1, $4, $6);  // Crear nodo de función
    }
;
parameter_list:
    /* vacío */ {
        $$ = NULL; // Sin parámetros, la lista será NULL
    }
    |VOID {
        $$ = $1; // Lista de parámetros
    }
    | parameter_declaration_list {
        $$ = $1; // Lista de parámetros
    }
;

parameter_declaration_list:
    parameter_declaration {
        $$ = $1; // Un único parámetro
    }
    | parameter_declaration_list ',' parameter_declaration {
        $$ = (ASTNode*)appendNode($1, $3); // Lista de parámetros combinada
    }
;

parameter_declaration:
    type IDENTIFIER {
        $$ = (ASTNode*)createDeclarationNode($1, $2, NULL); // Crear nodo para un parámetro
    }
;

// Declaraciones
declaration:
    type IDENTIFIER ';' {
        printf("Creating declaration: type=%s, name=%s\n", $1, $2);
        $$ = (ASTNode*)createDeclarationNode($1, $2, NULL);  // Declaración sin inicialización
    }
    | type IDENTIFIER '=' expression ';' {
        printf("Creating declaration with initialization: type=%s, name=%s\n", $1, $2);
        $$ = (ASTNode*)createDeclarationNode($1, $2, $4);  // Declaración con inicialización
    }
    | type IDENTIFIER ',' IDENTIFIER_LIST ';' {
        printf("Creating declaration list: type=%s, name=%s\n", $1, $2);
        $$ = (ASTNode*)createDeclarationListNode($2, $4);  // Múltiples declaraciones
    }
;

IDENTIFIER_LIST:
    IDENTIFIER
    | IDENTIFIER_LIST ',' IDENTIFIER
;

type:
    simple_type {
        printf("Parsed simple_type: %s\n", $1);
        $$ = $1; // Pasar directamente el valor del tipo
    }
    | user_type {
        printf("Parsed user_type: %s\n", $1);
        $$ = $1; // Pasar el nombre del tipo de usuario
    }
;

simple_type:
    INT {
        $$ = $1; // "int", "string" o "void" ya están definidos en el lexer
    }
    | STRING {
        $$ = $1;
    }
    | VOID {
        $$ = $1;
    }
;

user_type:
    IDENTIFIER {
        $$ = $1; // El identificador de la clase o tipo definido por el usuario
    }
;


// Bloques de código
block:
    '{' declaration_or_statement_list '}' {
        $$ = (ASTNode*)createBlockNode($2); // Crear un nodo de bloque con la lista de declaraciones o sentencias
    }
;

declaration_or_statement_list:
    /* vacío */ {
        $$ = NULL;  // Si la lista está vacía, el valor de $$ es NULL
    }
    | declaration_or_statement {
        $$ = $1;  // Si hay solo una declaración o sentencia, simplemente pasa ese nodo
    }
    | declaration_or_statement_list declaration_or_statement {
        $$ = (ASTNode*)appendNode($1, $2);  // Si hay múltiples declaraciones o sentencias, las agregamos
    }
;


declaration_or_statement:
    declaration {
        $$ = $1;  // Si es una declaración, asignamos el nodo de declaración
    }
    | statement {
        $$ = $1;  // Si es una sentencia, asignamos el nodo de la sentencia
    }
;


// Sentencias
statement:
    IF '(' expression ')' block ELSE block
    | IF '(' expression ')' block
    | WHILE '(' expression ')' block
    | RETURN expression ';'
    | PRINT '(' print_arguments ')' ';'
    | IDENTIFIER '=' expression ';' {
        $$ = (ASTNode*)createBinaryOpNode(OP_ASSIGN,createVariableNode($1), $3);  // Crear nodo de asignación
    }
    | block {
        $$ = $1;  // El bloque es una lista de sentencias
    }
    | ';' {
        $$ = NULL; // Sentencia vacía
    }
;

print_arguments:
    STRING_LITERAL
    | STRING_LITERAL ',' print_arguments
    | expression
    | expression ',' print_arguments
;

expression:
  INTEGER_LITERAL{
        printf("Creating literal: %d\n", $1);
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%d", $1);  // Convierte el entero a cadena
        $$ = (ASTNode*)createLiteralNode(buffer, "int");
  }
  | STRING_LITERAL {
        printf("Creating string literal: %s\n", $1);
        $$ = (ASTNode*)createLiteralNode($1, "string");
  }
  | IDENTIFIER {
        printf("Creating variable node: %s\n", $1);
        $$ = (ASTNode*)createVariableNode($1);
  }
  | expression '.' IDENTIFIER  /* Acceso a atributo o método */
  | expression '.' IDENTIFIER '(' argument_list ')'  /* Llamada a método */
  | IDENTIFIER '.' IDENTIFIER /* Acceso a atributo */
  | IDENTIFIER '.' IDENTIFIER '(' argument_list ')'  /* Llamada a método */
  | SUPER '.' IDENTIFIER '(' argument_list ')'  /* Llamada a método desde super */
  | NEW IDENTIFIER '(' argument_list ')'  /* Constructor */ {
        $$ = (ASTNode*)createNewNode($2, $4); // Nodo para 'new IDENTIFIER()'
  }
  | NEW IDENTIFIER  /* Instancia sin argumentos */ {
        $$ = (ASTNode*)createNewNode($2, NULL); // Nodo para 'new IDENTIFIER'
  }
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
    /* vacío */  { $$ = NULL; }
    | expression { $$ = $1; }
    | argument_list ',' expression { $$ = appendNode($1, $3); }
;


%%

// Función para manejar errores
void yyerror(const char* msg) {
    fprintf(stderr, "Error: %s\n", msg);
}
