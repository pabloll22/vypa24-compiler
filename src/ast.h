#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// Binary operators
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

// Unario operators
typedef enum {
    OP_NEG,     // -
    OP_NOT,     // !
} UnaryOperator;

// AST node type
typedef enum {
    AST_PROGRAM, // Root node for the program
    AST_CLASS,      // Class node
    AST_FUNCTION,   // Node for a function
    AST_DECLARATION, // Node for statements
    AST_ASSIGNMENT,  // Node for assignments
    AST_BLOCK,      // Node for blocks
    AST_IF,         // Node for IF sentences
    AST_WHILE,      // While sentences node
    AST_RETURN,     // Return sentences node
    AST_PRINT,      // Print sentences node
    AST_EXPRESSION, // Node for expressions
    AST_VARIABLE,   // Node for variables
    AST_LITERAL,    // Node for literals
    AST_BINARY_OP,  // Node for binary operators
    AST_UNARY_OP,   // Node for Unarium Operators
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

// Generic AST node
typedef struct ASTNode {
    ASTNodeType type;              // Type of the knot
    struct ASTNode* next;          // Pointed to the next node on a list
} ASTNode;

extern ASTNode* root;

// Program node
typedef struct {
    ASTNode base;                  // Base knot
    ASTNode* classes;              // Class list
    ASTNode* functions;            // Functions list
} ASTProgramNode;

// Class node
typedef struct {
    ASTNode base;
    char* name;                    // Class name
    char* parent;                  // Father class name (it can be null)
    ASTNode* members;              // List of statements and functions
} ASTClassNode;

// Node for a function
typedef struct {
    ASTNode base;
    char* name;                    // Function name
    char* returnType;              // Return type
    ASTNode* parameters;           // Parameter list
    int param_count;
    ASTNode* body;                 // Body of the function
} ASTFunctionNode;

// Node for a statement (variables or attributes)
typedef struct {
    ASTNode base;
    char* type;                    // Variable type
    char* name;                    // Variable name
    ASTNode* init;                 // Initialization expression (it can be null)
} ASTDeclarationNode;

// Node for a list of statements
typedef struct {
    ASTNode base;
    ASTNode* declaration;          // Current statement
    ASTNode* next;                 // Next statement in the list
} ASTDeclarationListNode;

typedef struct {
    ASTNode base;  // Base knot
    ASTNode* left;  // Left expression
    ASTNode* right; // Right expression
    BinaryOperator op;  // Binary operator
} ASTBinaryOpNode;


typedef struct {
    ASTNode base;  // Base knot
    ASTNode* operand;  // Operating (in the case of unarium operators)
    UnaryOperator op;  // Unario operator
} ASTUnaryOpNode;

typedef struct {
    ASTNode base;  // Base knot
    char* value;   // Literal value (whole, chain, etc.)
    char* literalType;    // tipoDeLiteral (int,String,Etc.)
} ASTLiteralNode;

typedef struct {
    ASTNode base;  // Base knot
    char* name;    // Variable or function name
} ASTVariableNode;

typedef struct ASTFunctionCallNode {
    ASTNode base;          // Base knot
    char* functionName;    // Function name
    ASTNode* context;
    ASTNode* arguments;    // List of arguments of the function
} ASTFunctionCallNode;

// Nodo para el bloque
typedef struct ASTBlockNode {
    ASTNode base;            // Nodo base
    ASTNode* statements;     // List of statements or sentences in the block
} ASTBlockNode;

//Nodo de new
typedef struct ASTNewNode {
    ASTNode base;         // Nodo base
    char* className;      // Class name that is instance
    ASTNode* arguments;   // Builder's argument list (if they exist)
} ASTNewNode;

// Nodo de if
typedef struct ASTIfNode {
    ASTNode base;
    ASTNode* condition; // IF condition
    ASTNode* trueBlock; // IF code block
    ASTNode* falseBlock; // Else code block (if it exists)
} ASTIfNode;

// Nodo de while
typedef struct ASTWhileNode {
    ASTNode base;
    ASTNode* condition; // CWhile condition
    ASTNode* body;      // While body
} ASTWhileNode;

// Nodo de return
typedef struct ASTReturnNode {
    ASTNode base;
    ASTNode* expression; // Expression to return
} ASTReturnNode;

// Nodo de print
typedef struct ASTPrintNode {
    ASTNode base;
    ASTNode* arguments;  // List of arguments for print
} ASTPrintNode;

// Node to access an attribute or method
typedef struct ASTMemberAccessNode {
    ASTNode base;          // Base knot
    ASTNode* expression;   // Expression before the point (E.G., Obj in Obj.field)
    char* memberName;      // The name of the attribute or method
} ASTMemberAccessNode;

// Node for a call to a method
typedef struct ASTMethodCallNode {
    ASTNode base;          // Base knot
    ASTNode* expression;   // Expression before the point (E.G., Obj in Obj.method ())
    char* methodName;      // The name of the method
    ASTNode* arguments;    // Arguments of the call call
} ASTMethodCallNode;

//List of identifiers
typedef struct ASTIdentifierListNode {
    ASTNode base;          // Base node (common to all AST nodes)
    ASTNode* identifiers;  // List of identifiers (Ast_variable nodes)
} ASTIdentifierListNode;

//Strings
typedef struct ASTStringLiteralNode {
    ASTNode base;          // Base knot
    char* value;           // Literal chain value
} ASTStringLiteralNode;

//Super
typedef struct ASTSuperNode {
    ASTNode base;          // Nodo base
} ASTSuperNode;

typedef struct ASTTypeCastNode {
    ASTNode base;          // Nodo base
    char* typeName;        // Type to which it is becoming
    ASTNode* expression;   // Expression to which conversion is applied
} ASTTypeCastNode;

typedef struct ASTThisNode {
    ASTNode base; // Nodo base
} ASTThisNode;

// Functions to create nodes
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
