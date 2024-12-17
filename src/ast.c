#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Function to create a program node
ASTProgramNode* createProgramNode(ASTNode* classes, ASTNode* functions) {
    ASTProgramNode* node = (ASTProgramNode*)malloc(sizeof(ASTProgramNode));
    if (!node) {
        fprintf(stderr, "Error: You could not assign memory for astprogramnode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_PROGRAM; // Assign the node type
    node->base.next = NULL;        // The root node has no next
    node->classes = classes;       // Class list
    node->functions = functions;   // Function list
    return node;
}

ASTClassNode* createClassNode(const char* name, const char* parent, ASTNode* members) {
    ASTClassNode* node = (ASTClassNode*)malloc(sizeof(ASTClassNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTClassNode.\n");
        exit(EXIT_FAILURE);
    }
    //node->base.next = NULL;
    node->base.type = AST_CLASS;
    node->name = strdup(name);  // copy the name of the class
    node->parent = parent ? strdup(parent) : NULL;  // If you have a base class
    node->members = members;    // class members (functions or attributes)
    return node;
}

ASTFunctionNode* createFunctionNode(const char* name, const char* returnType, ASTNode* parameters, ASTNode* body) {
    ASTFunctionNode* node = (ASTFunctionNode*)malloc(sizeof(ASTFunctionNode));
    if (!node) {
        fprintf(stderr, "Error: You could not assign memory for astfunctionnode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_FUNCTION; // Assign the node type
    node->name = strdup(name);      // Copy the name of the function
    node->returnType = strdup(returnType); // Copy the type of return
    node->parameters = parameters ? parameters : NULL; // Parameter list
    node->body = body;             // Body of the function

    // Count the parameters
    int count = 0;
    ASTNode* current = parameters;
    while (current) {
        count++;
        current = current->next;
    }
    node->param_count = count;  // Assign the parameter count

    return node;
}

// Function to create a declaration node
ASTDeclarationNode* createDeclarationNode(const char* type, const char* name, ASTNode* init) {
    ASTDeclarationNode* node = (ASTDeclarationNode*)malloc(sizeof(ASTDeclarationNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTDeclarationNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_DECLARATION;  // Assign the node type
    node->base.next = NULL;             // Without next node
    node->type = strdup(type);          // Copy the type of variable
    node->name = strdup(name);          // Copy the name of the variable
    node->init = init;                  // Initialization expression (it can be null)
    return node;
}

// Function to create a statement list node
ASTDeclarationListNode* createDeclarationListNode(ASTNode* declaration, ASTNode* rest) {
    ASTDeclarationListNode* node = (ASTDeclarationListNode*)malloc(sizeof(ASTDeclarationListNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTDeclarationListNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_DECLARATION;  // Assign the node type
    node->base.next = rest;             // Points to the next node on the list
    node->declaration = declaration;    // Current statement
    return node;
}

//Op Nodes

ASTBinaryOpNode* createBinaryOpNode(BinaryOperator op, ASTNode* left, ASTNode* right) {
    ASTBinaryOpNode* node = (ASTBinaryOpNode*)malloc(sizeof(ASTBinaryOpNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTBinaryOpNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_BINARY_OP;
    node->left = left;
    node->right = right;
    node->op = op;
    return node;
}

ASTUnaryOpNode* createUnaryOpNode(UnaryOperator op, ASTNode* operand) {
    ASTUnaryOpNode* node = (ASTUnaryOpNode*)malloc(sizeof(ASTUnaryOpNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTUnaryOpNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_UNARY_OP;
    node->operand = operand;
    node->op = op;
    return node;
}

ASTLiteralNode* createLiteralNode(const char* value, const char* type) {
    ASTLiteralNode* node = (ASTLiteralNode*)malloc(sizeof(ASTLiteralNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTLiteralNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_LITERAL;
    node->value = strdup(value);
    node->literalType = strdup(type);
    return node;
}

ASTVariableNode* createVariableNode(const char* name) {
    ASTVariableNode* node = (ASTVariableNode*)malloc(sizeof(ASTVariableNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTVariableNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_VARIABLE;
    node->base.next = NULL;
    node->name = strdup(name);
    return node;
}

ASTNode* appendNode(ASTNode* list, ASTNode* node) {
    if (!list) {
        return node;  // If the list is empty, the node becomes the list
    }

    ASTNode* current = list;
    while (current->next) {
        current = current->next;  // Find the last node on the list
    }
    current->next = node;  // Add the node to the end
    return list;
}

// Function to create a block node
ASTBlockNode* createBlockNode(ASTNode* statements) {
    ASTBlockNode* node = (ASTBlockNode*)malloc(sizeof(ASTBlockNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTBlockNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_BLOCK;   // Assign the type of node as a block
    node->base.next = NULL;        // Initializes the 'Next' pointer
    node->statements = statements; // Assign the list of sentences or statements
    return node;
}

// Function to create a new node
ASTNewNode* createNewNode(const char* className, ASTNode* arguments) {
    ASTNewNode* node = (ASTNewNode*)malloc(sizeof(ASTNewNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTNewNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_NEW;
    node->className = strdup(className); // Copy of the class name
    node->arguments = arguments;        // List of Arguments
    return node;
}

// Function to create the IF node
ASTIfNode* createIfNode(ASTNode* condition, ASTNode* trueBlock, ASTNode* falseBlock) {
    ASTIfNode* node = (ASTIfNode*)malloc(sizeof(ASTIfNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTIfNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_IF;
    node->condition = condition;
    node->trueBlock = trueBlock;
    node->falseBlock = falseBlock;
    return node;
}

// Function to create the While node
ASTWhileNode* createWhileNode(ASTNode* condition, ASTNode* body) {
    ASTWhileNode* node = (ASTWhileNode*)malloc(sizeof(ASTWhileNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTWhileNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_WHILE;
    node->condition = condition;
    node->body = body;
    return node;
}

// Function to create the return node
ASTReturnNode* createReturnNode(ASTNode* expression) {
    ASTReturnNode* node = (ASTReturnNode*)malloc(sizeof(ASTReturnNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTReturnNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_RETURN;
    node->expression = expression;
    return node;
}

// Function to create the print node
ASTPrintNode* createPrintNode(ASTNode* arguments) {
    ASTPrintNode* node = (ASTPrintNode*)malloc(sizeof(ASTPrintNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTPrintNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_PRINT;
    node->arguments = arguments;
    return node;
}

//FUNCTION READING
ASTFunctionCallNode* createFunctionCallNode(const char* functionName, ASTNode* arguments) {
    ASTFunctionCallNode* node = (ASTFunctionCallNode*)malloc(sizeof(ASTFunctionCallNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTFunctionCallNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_FUNCTION_CALL;
    node->functionName = strdup(functionName); // Copy of the function name
    node->arguments = arguments;               // List of Arguments
    return node;
}

//
ASTMemberAccessNode* createMemberAccessNode(ASTNode* expression, const char* memberName) {
    ASTMemberAccessNode* node = (ASTMemberAccessNode*)malloc(sizeof(ASTMemberAccessNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTMemberAccessNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_MEMBER_ACCESS;
    node->expression = expression;
    node->memberName = strdup(memberName);  // Copy the member's name
    return node;
}

ASTMethodCallNode* createMethodCallNode(ASTNode* expression, const char* methodName, ASTNode* arguments) {
    ASTMethodCallNode* node = (ASTMethodCallNode*)malloc(sizeof(ASTMethodCallNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTMethodCallNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_METHOD_CALL;
    node->expression = expression;
    node->methodName = strdup(methodName);  // Copy the name of the method
    node->arguments = arguments;            // Assign the arguments
    return node;
}

//Function to create identifiers list

ASTNode* createIdentifierListNode(ASTNode* first, ASTNode* second) {
    ASTIdentifierListNode* node = (ASTIdentifierListNode*)malloc(sizeof(ASTIdentifierListNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTIdentifierListNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_IDENTIFIER_LIST;
    node->identifiers = first;  // The first identifier
    if (second) {
        // If there is a second identifier, we add it to the list
        ASTNode* temp = first;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = second;
    }
    return (ASTNode*)node;
}

ASTNode* createStringLiteralNode(const char* value) {
    ASTStringLiteralNode* node = (ASTStringLiteralNode*)malloc(sizeof(ASTStringLiteralNode));
    if (!node) {
        fprintf(stderr, "Error allocating memory for string literal node.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_STRING_LITERAL;
    node->value = strdup(value);
    return (ASTNode*)node;
}

//Función super
ASTNode* createSuperNode() {
    ASTSuperNode* node = (ASTSuperNode*)malloc(sizeof(ASTSuperNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTSuperNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_SUPER;
    return (ASTNode*)node;
}

//castTipo
ASTTypeCastNode* createTypeCastNode(const char* typeName, ASTNode* expression) {
    ASTTypeCastNode* node = (ASTTypeCastNode*)malloc(sizeof(ASTTypeCastNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTTypeCastNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_TYPE_CAST;
    node->typeName = strdup(typeName);  // Copy the name of the type
    node->expression = expression;     // The expression to convert
    return node;
}

ASTFunctionCallNode* createFunctionCallWithContextNode(ASTNode* context, ASTNode* arguments) {
    ASTFunctionCallNode* node = (ASTFunctionCallNode*)malloc(sizeof(ASTFunctionCallNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTFunctionCallNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_FUNCTION_CALL;
    node->context = context;     // Node before parentheses (context)
    node->functionName = NULL;   // This can be null if we only use context
    node->arguments = arguments; // List of Arguments
    return node;
}

ASTThisNode* createThisNode() {
    ASTThisNode* node = (ASTThisNode*)malloc(sizeof(ASTThisNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTThisNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_THIS;  // Be sure to have an Ast_This type
    return node;
}