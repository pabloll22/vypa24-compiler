#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// Operadores binarios
typedef enum {
    OP_ASSIGN,  //=
    OP_ADD,     // +
    OP_SUB,     // -
    OP_MUL,     // *
    OP_DIV,     // /
    OP_LT,      // <
    OP_GT,      // >
    OP_LE,      // <=
    OP_GE,      // >=
    OP_EQ,      // ==
    OP_NE,      // !=
} BinaryOperator;

// Operadores unarios
typedef enum {
    OP_NEG,     // -
    OP_NOT,     // !
} UnaryOperator;

// Tipo de nodo del AST
typedef enum {
    AST_PROGRAM, // Nodo raíz para el programa
    AST_CLASS,      // Nodo para una clase
    AST_FUNCTION,   // Nodo para una función
    AST_DECLARATION, // Nodo para declaraciones
    AST_ASSIGNMENT,  // Nodo para asignaciones
    AST_BLOCK,      // Nodo para bloques
    AST_IF,         // Nodo para sentencias if
    AST_WHILE,      // Nodo para sentencias while
    AST_RETURN,     // Nodo para sentencias return
    AST_PRINT,      // Nodo para sentencias print
    AST_EXPRESSION, // Nodo para expresiones
    AST_VARIABLE,   // Nodo para variables
    AST_LITERAL,    // Nodo para literales
    AST_BINARY_OP,  // Nodo para operadores binarios
    AST_UNARY_OP,   // Nodo para operadores unarios
    AST_FUNCTION_CALL,
    AST_NEW,
    AST_MEMBER_ACCESS,
    AST_METHOD_CALL,
    AST_IDENTIFIER_LIST,
    AST_STRING_LITERAL,
    AST_SUPER,
    AST_TYPE_CAST,
    AST_THIS,
} ASTNodeType;

// Nodo genérico del AST
typedef struct ASTNode {
    ASTNodeType type;              // Tipo del nodo
    struct ASTNode* next;          // Apuntador al siguiente nodo en una lista
} ASTNode;

extern ASTNode* root;

// Nodo para el programa
typedef struct {
    ASTNode base;                  // Nodo base
    ASTNode* classes;              // Lista de clases
    ASTNode* functions;            // Lista de funciones
} ASTProgramNode;

// Nodo para una clase
typedef struct {
    ASTNode base;
    char* name;                    // Nombre de la clase
    char* parent;                  // Nombre de la clase padre (puede ser NULL)
    ASTNode* members;              // Lista de declaraciones y funciones
} ASTClassNode;

// Nodo para una función
typedef struct {
    ASTNode base;
    char* name;                    // Nombre de la función
    char* returnType;              // Tipo de retorno
    ASTNode* parameters;           // Lista de parámetros
    int param_count;
    ASTNode* body;                 // Cuerpo de la función
} ASTFunctionNode;

// Nodo para una declaración (variables o atributos)
typedef struct {
    ASTNode base;
    char* type;                    // Tipo de la variable
    char* name;                    // Nombre de la variable
    ASTNode* init;                 // Expresión de inicialización (puede ser NULL)
} ASTDeclarationNode;

// Nodo para una lista de declaraciones
typedef struct {
    ASTNode base;
    ASTNode* declaration;          // Declaración actual
    ASTNode* next;                 // Siguiente declaración en la lista
} ASTDeclarationListNode;

typedef struct {
    ASTNode base;  // Nodo base
    ASTNode* left;  // Expresión izquierda
    ASTNode* right; // Expresión derecha
    BinaryOperator op;  // Operador binario
} ASTBinaryOpNode;


typedef struct {
    ASTNode base;  // Nodo base
    ASTNode* operand;  // Operando (en el caso de operadores unarios)
    UnaryOperator op;  // Operador unario
} ASTUnaryOpNode;

typedef struct {
    ASTNode base;  // Nodo base
    char* value;   // Valor literal (entero, cadena, etc.)
    char* literalType;    // Tipo de literal (int, string, etc.)
} ASTLiteralNode;

typedef struct {
    ASTNode base;  // Nodo base
    char* name;    // Nombre de la variable o función
} ASTVariableNode;

typedef struct ASTFunctionCallNode {
    ASTNode base;          // Nodo base
    char* functionName;    // Nombre de la función
    ASTNode* context;
    ASTNode* arguments;    // Lista de argumentos de la función
} ASTFunctionCallNode;

// Nodo para el bloque
typedef struct ASTBlockNode {
    ASTNode base;            // Nodo base
    ASTNode* statements;     // Lista de declaraciones o sentencias en el bloque
} ASTBlockNode;

//Nodo de new
typedef struct ASTNewNode {
    ASTNode base;         // Nodo base
    char* className;      // Nombre de la clase que se está instanciando
    ASTNode* arguments;   // Lista de argumentos del constructor (si existen)
} ASTNewNode;

// Nodo de if
typedef struct ASTIfNode {
    ASTNode base;
    ASTNode* condition; // Condición del if
    ASTNode* trueBlock; // Bloque de código del if
    ASTNode* falseBlock; // Bloque de código del else (si existe)
} ASTIfNode;

// Nodo de while
typedef struct ASTWhileNode {
    ASTNode base;
    ASTNode* condition; // Condición del while
    ASTNode* body;      // Cuerpo del while
} ASTWhileNode;

// Nodo de return
typedef struct ASTReturnNode {
    ASTNode base;
    ASTNode* expression; // Expresión a retornar
} ASTReturnNode;

// Nodo de print
typedef struct ASTPrintNode {
    ASTNode base;
    ASTNode* arguments;  // Lista de argumentos para print
} ASTPrintNode;

// Nodo para acceder a un atributo o método
typedef struct ASTMemberAccessNode {
    ASTNode base;          // Nodo base
    ASTNode* expression;   // Expresión antes del punto (e.g., obj en obj.field)
    char* memberName;      // El nombre del atributo o método
} ASTMemberAccessNode;

// Nodo para una llamada a un método
typedef struct ASTMethodCallNode {
    ASTNode base;          // Nodo base
    ASTNode* expression;   // Expresión antes del punto (e.g., obj en obj.method())
    char* methodName;      // El nombre del método
    ASTNode* arguments;    // Argumentos de la llamada al método
} ASTMethodCallNode;

//Lista de identificadores
typedef struct ASTIdentifierListNode {
    ASTNode base;          // Nodo base (común a todos los nodos AST)
    ASTNode* identifiers;  // Lista de identificadores (nodos AST_VARIABLE)
} ASTIdentifierListNode;

//Strings
typedef struct ASTStringLiteralNode {
    ASTNode base;          // Nodo base
    char* value;           // Valor de la cadena literal
} ASTStringLiteralNode;

//Super
typedef struct ASTSuperNode {
    ASTNode base;          // Nodo base
} ASTSuperNode;

typedef struct ASTTypeCastNode {
    ASTNode base;          // Nodo base
    char* typeName;        // Tipo al que se está convirtiendo
    ASTNode* expression;   // Expresión a la que se aplica la conversión
} ASTTypeCastNode;

typedef struct ASTThisNode {
    ASTNode base; // Nodo base
} ASTThisNode;

// Funciones para crear nodos
ASTProgramNode* createProgramNode(ASTNode* classes, ASTNode* functions);
ASTClassNode* createClassNode(const char* name, const char* parent, ASTNode* members);
ASTFunctionNode* createFunctionNode(const char* name, const char* returnType, ASTNode* parameters, ASTNode* body);
ASTDeclarationNode* createDeclarationNode(const char* type, const char* name, ASTNode* init);
ASTDeclarationListNode* createDeclarationListNode(ASTNode* declaration, ASTNode* rest);

//ASTAssignmentNode* createAssignmentNode(ASTNode* var, ASTNode* value);
ASTVariableNode* createVariableNode(const char* name);
//ASTLiteralNode* createLiteralNode(int value);

ASTBinaryOpNode* createBinaryOpNode(BinaryOperator op, ASTNode* left, ASTNode* right);
ASTUnaryOpNode* createUnaryOpNode(UnaryOperator op, ASTNode* operand);
ASTLiteralNode* createLiteralNode(const char* value, const char* type);
ASTVariableNode* createVariableNode(const char* name);
ASTFunctionCallNode* createFunctionCallNode(const char* functionName, ASTNode* arguments);
ASTNode* appendNode(ASTNode* list, ASTNode* node);
ASTBlockNode* createBlockNode(ASTNode* statements);
ASTNewNode* createNewNode(const char* className, ASTNode* arguments);
ASTIfNode* createIfNode(ASTNode* condition, ASTNode* trueBlock, ASTNode* falseBlock);
ASTWhileNode* createWhileNode(ASTNode* condition, ASTNode* body);
ASTReturnNode* createReturnNode(ASTNode* expression);
ASTPrintNode* createPrintNode(ASTNode* arguments);
ASTMemberAccessNode* createMemberAccessNode(ASTNode* expression, const char* memberName);
ASTMethodCallNode* createMethodCallNode(ASTNode* expression, const char* methodName, ASTNode* arguments);
ASTNode* createIdentifierListNode(ASTNode* first, ASTNode* second);
ASTNode* createStringLiteralNode(const char* value);
ASTNode* createSuperNode();
ASTTypeCastNode* createTypeCastNode(const char* typeName, ASTNode* expression);
ASTFunctionCallNode* createFunctionCallWithContextNode(ASTNode* context, ASTNode* arguments);
ASTThisNode* createThisNode();

#endif // AST_H
