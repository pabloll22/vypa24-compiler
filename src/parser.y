%{
#include "semantic_analysis.h"
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
    class_definitions class_definition {
        $$ = (ASTNode*)appendNode($1, $2);  // Agrega la clase actual a la lista
    }
    | class_definition {
        $$ = $1;  // Primera clase en la lista
    }
;

class_definition:
    CLASS IDENTIFIER ':' IDENTIFIER '{' class_body '}' {
        // Añadir la clase a la tabla de símbolos
        if (find_symbol(&symbol_table, $2) == -1) {
	    printf("Processing class_body for class '%s': initial node type=%d\n", $2, $6->type);

	    //Crear la clase y registrar en la tabla de símbolos
            char** attributes = extractAttributesFromClassBody($6); // Extrae atributos del cuerpo
            char** methods = extractMethodsFromClassBody($6);       // Extrae métodos del cuerpo

            int attr_count = countAttributes($6); // Cuenta atributos
            int method_count = countMethods($6);  // Cuenta métodos

            // Indicamos que este es un símbolo de tipo "clase"
            add_symbol(&symbol_table, $2, "class", false, true, false, NULL, 0, $4, attributes, attr_count, methods, method_count);  // $2 es el nombre de la clase
            // Now we process class members (attributes and methods)
            $$ = (ASTNode*)createClassNode($2, $4, $6);  // Crear nodo de clase con herencia
//	    program.classes = appendNode(program.classes, $$);  // Agrega la clase al programa
        } else {
            yyerror("Class already declared");  // Error si la clase ya está declarada
            $$ = NULL;
        }
    }
    | CLASS IDENTIFIER '{' class_body '}' {
        // Add the class to the symbols table without inheritance
        if (find_symbol(&symbol_table, $2) == -1) {
	    char** attributes = extractAttributesFromClassBody($4);
            char** methods = extractMethodsFromClassBody($4);
            int attr_count = countAttributes($4);
            int method_count = countMethods($4);

            add_symbol(&symbol_table, $2, "class", false, true, false, NULL, 0, NULL, attributes, attr_count, methods, method_count);  // $2 es el nombre de la clase
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
    |class_member{
	$$ = $1; // Primera declaración o método en la lista
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

        // Verify if the function is already in the symbols table
        ASTFunctionNode* funcNode = (ASTFunctionNode*)$2;
        if (find_symbol(&symbol_table, funcNode->name) == -1) {
            printf("Adding function: %s\n", funcNode->name);
	            // Agregar la función a la tabla de símbolos
            add_symbol(&symbol_table, funcNode->name, funcNode->returnType, true, false, false, funcNode->parameters, funcNode->param_count, NULL, NULL, 0, NULL, 0);  // Parámetros NULL por ahora
        } else {
            yyerror("Function already declared");
        }
    }
    | function_definition {
        $$ = $1;  // The initial list is simply the first node
        ASTFunctionNode* funcNode = (ASTFunctionNode*)$1;
        if (find_symbol(&symbol_table, funcNode->name) == -1) {
            printf("Adding function: %s\n", funcNode->name);
            // Add the function to the symbols table
            add_symbol(&symbol_table, funcNode->name, funcNode->returnType, true, false, false, funcNode->parameters, funcNode->param_count, NULL, NULL, 0, NULL, 0);
        } else {
            yyerror("Function already declared");
        }
    }
;


function_definition:
    type IDENTIFIER '(' parameter_list ')' block {
        // Create the function node
        $$ = (ASTNode*)createFunctionNode($2, $1, $4, $6);  // Crear nodo de función
        // Count the parameter number
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
            // If the function is not declared, add it to the symbols table
            printf("Adding function: %s with %d parameters\n", $2, param_count);
            add_symbol(&symbol_table, $2, $1, true, false, false, $4, param_count, NULL, NULL, 0, NULL, 0);  // Agregar función a la tabla de símbolos
        } else {
            // If the function is already declared, report an error
            yyerror("Function already declared");
            $$ = NULL;  // Indicate that the function should not be added to the tree
        }
    }
;


parameter_list:
    /* vacío */ {
        $$ = NULL; // No parameters, the list will be null
    }
    |VOID {
        $$ = $1; // Parameter list
    }
    | parameter_declaration_list {
        $$ = $1; // Parameter list
    }
;

parameter_declaration_list:
    parameter_declaration {
        $$ = $1; // A single parameter
    }
    | parameter_declaration_list ',' parameter_declaration {
        $$ = (ASTNode*)appendNode($1, $3); // Combined parameter list
    }
;

parameter_declaration:
    type IDENTIFIER {
        printf("Creating parameter: type=%s, name=%s\n", $1, $2);
        // Add the parameter to the symbols table
        if (find_symbol(&symbol_table, $2) == -1) {
            add_symbol(&symbol_table, $2, $1, false, false, false, NULL, 0, NULL, NULL, 0, NULL, 0);  // Añadir parámetro a la tabla
            $$ = (ASTNode*)createDeclarationNode($1, $2, NULL);  // Crear nodo de parámetro sin inicialización
        } else {
            yyerror("Parameter already declared");
            $$ = NULL;  // Indicate that the parameter should not be added
        }
    }
;


// Statements
declaration:
    |type IDENTIFIER ';' {
        printf("Creating declaration: type=%s, name=%s\n", $1, $2);

        // Verify if the variable is already declared
        int found = find_symbol(&symbol_table, $2);
        if (found == -1) {
            printf("Symbol not found, adding: %s\n", $2);
            // If the variable is not declared, add it to the symbols table
            add_symbol(&symbol_table, $2, $1, false, false, false, NULL, 0, NULL, NULL, 0, NULL, 0);  // Agregar variable
            $$ = (ASTNode*)createDeclarationNode($1, $2, NULL);  // Crear nodo de declaración sin inicialización
        } else {
            // If the variable is already declared, report an error
            yyerror("Variable already declared");
            $$ = NULL;
        }
    }
    | type IDENTIFIER '=' expression ';' {
	printf("Creating declaration with initialization: type=%s, name=%s\n", $1, $2);
        // Verify if the variable is already declared
        int found = find_symbol(&symbol_table, $2);
        if (found == -1) {
            printf("Symbol not found, adding: %s\n", $2);
            // If the variable is not declared, add it to the symbols table
            add_symbol(&symbol_table, $2, $1, false, false, false, NULL, 0, NULL, NULL, 0, NULL, 0);  // Agregar variable
            $$ = (ASTNode*)createDeclarationNode($1, $2, $4);  // Crear nodo de declaración con inicialización
        } else {
            // If the variable is already declared, report an error
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
        // Create a node for the identifier and assign it to $$.
        $$ = (ASTNode*)createVariableNode($1);
    }
    | IDENTIFIER_LIST ',' IDENTIFIER {
        // Create a node for the list and add the new identifier.
        $$ = (ASTNode*)createIdentifierListNode($1, (ASTNode*)createVariableNode($3));
    }
;

type:
    simple_type {
	printf("Parsed simple_type: %s\n", $1);
        $$ = $1; // Pass the type value directly
    }
    | user_type {
	printf("Parsed user_type: %s\n", $1);
        $$ = $1; // Pass the name of the user type
    }
;

simple_type:
    INT {
        $$ = $1; // "int", "string" or "void" are already defined in the lexer
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
        $$ = $1; // User -defined class or type identifier
    }
;


// Code blocks
block:
    '{' declaration_or_statement_list '}' {
        $$ = (ASTNode*)createBlockNode($2); // Create a block node with the list of statements or sentences
    }
;

declaration_or_statement_list:
    /* vacío */ {
        $$ = NULL;  // If the list is empty, the $$ value is null
    }
    | declaration_or_statement {
        $$ = $1;  // If there is only one statement or sentence, that node simply passes
    }
    | declaration_or_statement_list declaration_or_statement {
        $$ = (ASTNode*)appendNode($1, $2);  // If there are multiple statements or sentences, we add them
    }
;


declaration_or_statement:
    declaration {
        $$ = $1;  // If it is a statement, we assign the declaration node
    }
    | statement {
        $$ = $1;  // If it is a sentence, we assign the node of the sentence
    }
;


// Sentences
statement:
    IF '(' expression ')' block ELSE block {
        $$ = (ASTNode*)createIfNode($3, $5, $7);  // Create 'if' node with the condition and blocks
    }
    | IF '(' expression ')' block {
        $$ = (ASTNode*)createIfNode($3, $5, NULL);  // Create 'if' node without block 'else'
    }
    | WHILE '(' expression ')' block {
        $$ = (ASTNode*)createWhileNode($3, $5);  // Create 'While' node with the condition and block
    }
    | RETURN expression ';' {
        $$ = (ASTNode*)createReturnNode($2);  // Create 'return' node with expression
    }
    | PRINT '(' print_arguments ')' ';' {
        $$ = (ASTNode*)createPrintNode($3);  // Create 'print' node with arguments
    }
    | IDENTIFIER '=' expression ';' {
        $$ = (ASTNode*)createBinaryOpNode(OP_ASSIGN,createVariableNode($1), $3);  // Create allocation node
    }
    | block {
	$$ = $1;  // The block is a list of sentences
    }
    | ';' {
        $$ = NULL; // Empty sentence
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
        $$ = (ASTNode*)createLiteralNode($1, "string");  // Create chain literal node
    }
    | expression { 
        $$ = $1;  // The argument is an expression
    }
    | print_arguments ',' STRING_LITERAL {
        $$ = (ASTNode*)appendNode($1, (ASTNode*)createLiteralNode($3, "string"));  // Add literal to the list of arguments
    }
    | print_arguments ',' expression {
        $$ = (ASTNode*)appendNode($1, $3);  // Add expression to the list of arguments
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
  | expression '.' IDENTIFIER {
        // Access to attribute or method
        $$ = (ASTNode*)createMemberAccessNode($1, $3);
  }
  | expression '.' IDENTIFIER '(' argument_list ')'  / * Method call */ {
        $$ = (ASTNode*)createMethodCallNode($1, $3, $5);
  }
  | IDENTIFIER '.' IDENTIFIER '=' expression ';' {
    $$ = (ASTNode*)createBinaryOpNode(OP_ASSIGN,
                                       (ASTNode*)createMemberAccessNode(createVariableNode($1), $3),
                                       $5);
  }
  | IDENTIFIER '.' IDENTIFIER / * Access to attribute */ {
        // Access to attribute
        $$ = (ASTNode*)createMemberAccessNode(createVariableNode($1), $3);
  }
  | IDENTIFIER '.' IDENTIFIER '(' argument_list ')'  /* Method call */ {
        // Llamada a método
        $$ = (ASTNode*)createMethodCallNode(createVariableNode($1), $3, $5);
  }
  | SUPER '.' IDENTIFIER '(' argument_list ')'{
        // Method call from Super
        $$ = (ASTNode*)createMethodCallNode(createSuperNode(), $3, $5);
  }
  | NEW IDENTIFIER '(' argument_list ')'  /* Constructor */ {
        $$ = (ASTNode*)createNewNode($2, $4); // Node for 'new IDENTIFIER()'
  }
  | NEW IDENTIFIER  /* Instancia sin argumentos */ {
        $$ = (ASTNode*)createNewNode($2, NULL); // Node for 'new IDENTIFIER'
  }
  | THIS '.' IDENTIFIER  /* Acceso a atributo/método de la clase */ {
    $$ = (ASTNode*)createMemberAccessNode(createThisNode(), $3);  // Node for access to attribute/method from 'this'
  }
  | THIS '.' IDENTIFIER '(' argument_list ')' {
    $$ = (ASTNode*)createMethodCallNode(createThisNode(), $3, $5);  // Node for call to method from 'this'
  }
  | expression '(' argument_list ')'  /* Llamada a función */ {
      $$ = (ASTNode*)createFunctionCallWithContextNode($1, $3);  // Node with context
  }
  | IDENTIFIER '(' argument_list ')'  /* Llamada a función */ {
	if (strcmp($1, "subStr") == 0) {
		printf("Creating subStr function call\n");
        	$$ = createFunctionCallNode($1, $3);  // Subr call with your arguments
        } else {
		printf("Creating general function call\n");
        	$$ = (ASTNode*)createFunctionCallNode($1, $3);  // CALL TO GENERAL FUNCTION
        }
  }
  | READ_INT '(' ')'  /* Leer entero */{
	printf("Adding predefined function: readInt\n");
        char* readIntParams[] = {NULL};  // No parameters
        add_symbol(&symbol_table, "readInt", "int", true, false, false, readIntParams, 0, NULL, NULL, 0, NULL, 0);
        $$ = (ASTNode*)createFunctionCallNode("readInt", NULL);  // Create node for call to readInt()
  }
  | READ_STRING '(' ')'  /* Leer string */ {
        $$ = createFunctionCallNode("readString", NULL); // Call to Readstring function without arguments
  }
  | '(' expression ')'  /* Expression in parentheses */ {
        $$ = $2;  // Simply return the expression within parentheses
  }
  | '(' type ')' expression  /* Type conversion */ {
      printf("Creating type cast: (%s)\n", $2);
        $$ = (ASTNode*)createTypeCastNode($2, $4);
  }
  | expression '+' expression  {
        $$ = (ASTNode*)createBinaryOpNode(OP_ADD, $1, $3);  // Addition
  }
  | expression '-' expression  {
        $$ = (ASTNode*)createBinaryOpNode(OP_SUB, $1, $3);  // Subtraction
  }
  | expression '*' expression  {
        $$ = (ASTNode*)createBinaryOpNode(OP_MUL, $1, $3);  // Multiplication
  }
  | expression '/' expression  {
        $$ = (ASTNode*)createBinaryOpNode(OP_DIV, $1, $3);  // Division
  }
  | expression '<' expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_LT, $1, $3);  // Minor than
  }
  | expression '>' expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_GT, $1, $3);  // Greater than
  }
  | expression LE expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_LE, $1, $3);  // Smaller the equal
  }
  | expression GE expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_GE, $1, $3);  // Greater than or equal
  }
  | expression EQ expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_EQ, $1, $3);  // Just like
  }
  | expression NE expression {
        $$ = (ASTNode*)createBinaryOpNode(OP_NE, $1, $3);  // Unlike
  }
  /*| subStr(expression, expression, expression) {  // Rule for subStr
        $$ = (ASTNode*)createFunctionCallNode("subStr", $3);
  }*/
;

argument_list:
    /* empty */  { $$ = NULL; }
    | expression { $$ = $1; }
    | argument_list ',' expression { $$ = appendNode($1, $3); }
;


%%

// Function to handle errors
void yyerror(const char* msg) {
    fprintf(stderr, "Error: %s\n", msg);
}
