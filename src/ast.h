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
    AST_CALL,
    AST_NEW,
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

typedef struct {
    ASTNode base;  // Nodo base
    char* functionName;  // Nombre de la función
    ASTNode* arguments;   // Lista de argumentos
} ASTCallNode;

// Nodo para el bloque
typedef struct ASTBlockNode {
    ASTNode base;            // Nodo base
    ASTNode* statements;     // Lista de declaraciones o sentencias en el bloque
} ASTBlockNode;

typedef struct ASTNewNode {
    ASTNode base;         // Nodo base
    char* className;      // Nombre de la clase que se está instanciando
    ASTNode* arguments;   // Lista de argumentos del constructor (si existen)
} ASTNewNode;

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
ASTCallNode* createCallNode(const char* functionName, ASTNode* arguments);
ASTNode* appendNode(ASTNode* list, ASTNode* node);
ASTBlockNode* createBlockNode(ASTNode* statements);
ASTNewNode* createNewNode(const char* className, ASTNode* arguments);

#endif // AST_H