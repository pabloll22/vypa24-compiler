%{
#include "symbol_table.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#define YYDEBUG 1

extern int yydebug;
extern int yylex();
extern char* yytext;

void yyerror(const char* msg);

extern SymbolTable symbol_table;
extern ASTNode* root;

ASTNode* root = NULL;

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
%type <astNode> print_arguments

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
        // Añadir la clase a la tabla de símbolos
        if (find_symbol(&symbol_table, $2) == -1) {
            // Indicamos que este es un símbolo de tipo "clase"
            add_symbol(&symbol_table, $2, "class", false, true, false, NULL, 0);  // $2 es el nombre de la clase
            // Ahora procesamos los miembros de la clase (atributos y métodos)
            $$ = (ASTNode*)createClassNode($2, $4, $6);  // Crear nodo de clase con herencia
        } else {
            yyerror("Class already declared");  // Error si la clase ya está declarada
            $$ = NULL;
        }
    }
    | CLASS IDENTIFIER '{' class_body '}' {
        // Añadir la clase a la tabla de símbolos sin herencia
        if (find_symbol(&symbol_table, $2) == -1) {
            add_symbol(&symbol_table, $2, "class", false, true, false, NULL, 0);  // $2 es el nombre de la clase
            $$ = (ASTNode*)createClassNode($2, NULL, $4);  // Crear nodo de clase sin clase base
        } else {
            yyerror("Class already declared");
            $$ = NULL;
        }
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

        // Verificar si la función ya está en la tabla de símbolos
        ASTFunctionNode* funcNode = (ASTFunctionNode*)$2;
        if (find_symbol(&symbol_table, funcNode->name) == -1) {
            printf("Adding function: %s\n", funcNode->name);
                    // Agregar la función a la tabla de símbolos
            add_symbol(&symbol_table, funcNode->name, funcNode->returnType, true, false, false, NULL, 0);  // Parámetros NULL por ahora
        } else {
            yyerror("Function already declared");
        }
    }
    | function_definition {
        $$ = $1;  // La lista inicial es simplemente el primer nodo
        ASTFunctionNode* funcNode = (ASTFunctionNode*)$1;
        if (find_symbol(&symbol_table, funcNode->name) == -1) {
            printf("Adding function: %s\n", funcNode->name);
            // Agregar la función a la tabla de símbolos
            add_symbol(&symbol_table, funcNode->name, funcNode->returnType, true, false, false, NULL, 0);  // Parámetros NULL por ahora
        } else {
            yyerror("Function already declared");
        }
    }
;


function_definition:
    type IDENTIFIER '(' parameter_list ')' block {
        // Crear el nodo de la función
        $$ = (ASTNode*)createFunctionNode($2, $1, $4, $6);  // Crear nodo de función
        // Contar el número de parámetros
        printf("Parameters: ");
        int param_count = 0;
        ASTNode* param_node = $4;
        while (param_node) {
            param_count++;
            param_node = param_node->next;
        }

        // Verificar si la función ya está en la tabla de símbolos
        int found = find_symbol(&symbol_table, $2);  // Buscar la función por su nombre
        if (found == -1) {
            // Si la función no está declarada, agregarla a la tabla de símbolos
            printf("Adding function: %s with %d parameters\n", $2, param_count);
            add_symbol(&symbol_table, $2, $1, true, false, false, NULL, param_count);  // Agregar función a la tabla de símbolos
        } else {
            // Si la función ya está declarada, reportar un error
            yyerror("Function already declared");
            $$ = NULL;  // Indicar que la función no se debe agregar al árbol
        }
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
        printf("Creating parameter: type=%s, name=%s\n", $1, $2);
        // Añadir el parámetro a la tabla de símbolos
        if (find_symbol(&symbol_table, $2) == -1) {
            add_symbol(&symbol_table, $2, $1, false, false, false, NULL, 0);  // Añadir parámetro a la tabla
            $$ = (ASTNode*)createDeclarationNode($1, $2, NULL);  // Crear nodo de parámetro sin inicialización
        } else {
            yyerror("Parameter already declared");
            $$ = NULL;  // Indicar que el parámetro no debe ser añadido
        }
    }
;


// Declaraciones
declaration:
    |type IDENTIFIER ';' {
        printf("Creating declaration: type=%s, name=%s\n", $1, $2);

        // Verificar si la variable ya está declarada
        int found = find_symbol(&symbol_table, $2);
        if (found == -1) {
            printf("Symbol not found, adding: %s\n", $2);
            // Si la variable no está declarada, agregarla a la tabla de símbolos
            add_symbol(&symbol_table, $2, $1, false, false, false, NULL, 0);  // Agregar variable
            $$ = (ASTNode*)createDeclarationNode($1, $2, NULL);  // Crear nodo de declaración sin inicialización
        } else {
            // Si la variable ya está declarada, reportar un error
            yyerror("Variable already declared");
            $$ = NULL;
        }
    }
    | type IDENTIFIER '=' expression ';' {
        printf("Creating declaration with initialization: type=%s, name=%s\n", $1, $2);
        // Verificar si la variable ya está declarada
        int found = find_symbol(&symbol_table, $2);
        if (found == -1) {
            printf("Symbol not found, adding: %s\n", $2);
            // Si la variable no está declarada, agregarla a la tabla de símbolos
            add_symbol(&symbol_table, $2, $1, false, false, false, NULL, 0);  // Agregar variable
            $$ = (ASTNode*)createDeclarationNode($1, $2, $4);  // Crear nodo de declaración con inicialización
        } else {
            // Si la variable ya está declarada, reportar un error
            yyerror("Variable already declared");
            $$ = NULL;
        }
    }
    |type IDENTIFIER ',' IDENTIFIER_LIST ';' {
        printf("Creating declaration list: type=%s, name=%s\n", $1, $2);
        $$ = (ASTNode*)createDeclarationListNode($2, $4);  // Múltiples declaraciones
    }
;

IDENTIFIER_LIST:
    IDENTIFIER {
        // Crear un nodo para el identificador y asignarlo a $$.
        $$ = (ASTNode*)createVariableNode($1);
    }
    | IDENTIFIER_LIST ',' IDENTIFIER {
        // Crear un nodo para la lista y agregar el nuevo identificador.
        $$ = (ASTNode*)createIdentifierListNode($1, (ASTNode*)createVariableNode($3));
    }
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
    IF '(' expression ')' block ELSE block {
        $$ = (ASTNode*)createIfNode($3, $5, $7);  // Crear nodo de 'if' con la condición y los bloques
    }
    | IF '(' expression ')' block {
        $$ = (ASTNode*)createIfNode($3, $5, NULL);  // Crear nodo de 'if' sin bloque 'else'
    }
    | WHILE '(' expression ')' block {
        $$ = (ASTNode*)createWhileNode($3, $5);  // Crear nodo de 'while' con la condición y el bloque
    }
    | RETURN expression ';' {
        $$ = (ASTNode*)createReturnNode($2);  // Crear nodo de 'return' con la expresión
    }
    | PRINT '(' print_arguments ')' ';' {
        $$ = (ASTNode*)createPrintNode($3);  // Crear nodo de 'print' con los argumentos
    }
    | IDENTIFIER '=' expression ';' {
        $$ = (ASTNode*)createBinaryOpNode(OP_ASSIGN,createVariableNode($1), $3);  // Crear nodo de asignación
    }
    | block {
        $$ = $1;  // El bloque es una lista de sentencias
    }
    | ';' {
        $$ = NULL; // Sentencia vacía
    }
    |IDENTIFIER '.' IDENTIFIER '=' expression ';' {
        printf("Parsing: %s.%s = ...\n", $1, $3);  // Depuración
        $$ = (ASTNode*)createBinaryOpNode(OP_ASSIGN,
                                           (ASTNode*)createMemberAccessNode(createVariableNode($1), $3),
                                           $5);
    }
;

print_arguments:
    STRING_LITERAL {
        $$ = (ASTNode*)createLiteralNode($1, "string");  // Crear nodo de literal de cadena
    }
    | expression {
        $$ = $1;  // El argumento es una expresión
    }
    | print_arguments ',' STRING_LITERAL {
        $$ = (ASTNode*)appendNode($1, (ASTNode*)createLiteralNode($3, "string"));  // Agregar literal a la lista de argumentos
    }
    | print_arguments ',' expression {
        $$ = (ASTNode*)appendNode($1, $3);  // Agregar expresión a la lista de argumentos
    }
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
        $$ = (ASTNode*)createStringLiteralNode($1);
  }
  | IDENTIFIER {
        printf("Creating variable node: %s\n", $1);
        $$ = (ASTNode*)createVariableNode($1);
  }
  | expression '.' IDENTIFIER  /* Acceso a atributo o método */ {
        // Acceso a atributo o método
        $$ = (ASTNode*)createMemberAccessNode($1, $3);
  }
  | expression '.' IDENTIFIER '(' argument_list ')'  /* Llamada a método */ {
        // Llamada a método
        $$ = (ASTNode*)createMethodCallNode($1, $3, $5);
  }
  | IDENTIFIER '.' IDENTIFIER '=' expression ';' {
    $$ = (ASTNode*)createBinaryOpNode(OP_ASSIGN,
                                       (ASTNode*)createMemberAccessNode(createVariableNode($1), $3),
                                       $5);
  }
  | IDENTIFIER '.' IDENTIFIER /* Acceso a atributo */ {
        // Acceso a atributo
        $$ = (ASTNode*)createMemberAccessNode(createVariableNode($1), $3);
  }
  | IDENTIFIER '.' IDENTIFIER '(' argument_list ')'  /* Llamada a método */ {
        // Llamada a método
        $$ = (ASTNode*)createMethodCallNode(createVariableNode($1), $3, $5);
  }
  | SUPER '.' IDENTIFIER '(' argument_list ')'  /* Llamada a método desde super */ {
        // Llamada a método desde super
        $$ = (ASTNode*)createMethodCallNode(createSuperNode(), $3, $5);
  }
  | NEW IDENTIFIER '(' argument_list ')'  /* Constructor */ {
        $$ = (ASTNode*)createNewNode($2, $4); // Nodo para 'new IDENTIFIER()'
  }
  | NEW IDENTIFIER  /* Instancia sin argumentos */ {
        $$ = (ASTNode*)createNewNode($2, NULL); // Nodo para 'new IDENTIFIER'
  }
  | THIS '.' IDENTIFIER  /* Acceso a atributo/método de la clase */ {
    $$ = (ASTNode*)createMemberAccessNode(createThisNode(), $3);  // Nodo para acceso a atributo/método desde 'this'
  }
  | THIS '.' IDENTIFIER '(' argument_list ')' {
    $$ = (ASTNode*)createMethodCallNode(createThisNode(), $3, $5);  // Nodo para llamada a método desde 'this'
  }
  | expression '(' argument_list ')'  /* Llamada a función */ {
      $$ = (ASTNode*)createFunctionCallWithContextNode($1, $3);  // Nodo con contexto
  }
  | IDENTIFIER '(' argument_list ')'  /* Llamada a función */ {
        if (strcmp($1, "subStr") == 0) {
                printf("Creating subStr function call\n");
                $$ = createFunctionCallNode($1, $3);  // Llamada a subStr con sus argumentos
        } else {
                printf("Creating general function call\n");
                $$ = (ASTNode*)createFunctionCallNode($1, $3);  // Llamada a función en general
        }
  }
  | READ_INT '(' ')'  /* Leer entero */{
        $$ = (ASTNode*)createFunctionCallNode("readInt", NULL);  // Crear nodo para la llamada a readInt()
  }
  | READ_STRING '(' ')'  /* Leer string */ {
        $$ = createFunctionCallNode("readString", NULL); // Llamada a la función readString sin argumentos
  }
  | '(' expression ')'  /* Expresión entre paréntesis */ {
        $$ = $2;  // Simplemente devolver la expresión dentro de los paréntesis
  }
  | '(' type ')' expression  /* Conversión de tipo */ {
      printf("Creating type cast: (%s)\n", $2);
        $$ = (ASTNode*)createTypeCastNode($2, $4);
  }
  | expression '+' expression  {
        $$ = (ASTNode*)createBinaryOpNode(OP_ADD, $1, $3);  // Suma
  }
  | expression '-' expression  {
        $$ = (ASTNode*)createBinaryOpNode(OP_SUB, $1, $3);  // Resta
  }
  | expression '*' expression  {
        $$ = (ASTNode*)createBinaryOpNode(OP_MUL, $1, $3);  // Multiplicación
  }
  | expression '/' expression  {
        $$ = (ASTNode*)createBinaryOpNode(OP_DIV, $1, $3);  // División
  }
  | expression '<' expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_LT, $1, $3);  // Menor que
  }
  | expression '>' expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_GT, $1, $3);  // Mayor que
  }
  | expression LE expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_LE, $1, $3);  // Menor o igual que
  }
  | expression GE expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_GE, $1, $3);  // Mayor o igual que
  }
  | expression EQ expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_EQ, $1, $3);  // Igual que
  }
  | expression NE expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_NE, $1, $3);  // Diferente que
  }
  /*| subStr(expression, expression, expression) {  // Regla para subStr
        $$ = (ASTNode*)createFunctionCallNode("subStr", $3);
  }*/
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
