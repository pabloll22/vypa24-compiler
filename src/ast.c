#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Función para crear un nodo de programa
ASTProgramNode* createProgramNode(ASTNode* classes, ASTNode* functions) {
    ASTProgramNode* node = (ASTProgramNode*)malloc(sizeof(ASTProgramNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTProgramNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_PROGRAM; // Asigna el tipo de nodo
    node->base.next = NULL;        // El nodo raíz no tiene siguiente
    node->classes = classes;       // Lista de clases
    node->functions = functions;   // Lista de funciones
    return node;
}

ASTClassNode* createClassNode(const char* name, const char* parent, ASTNode* members) {
    ASTClassNode* node = (ASTClassNode*)malloc(sizeof(ASTClassNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTClassNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_CLASS;
    node->name = strdup(name);  // Copiar el nombre de la clase
    node->parent = parent ? strdup(parent) : NULL;  // Si tiene clase base
    node->members = members;    // Miembros de la clase (funciones o atributos)
    return node;
}

ASTFunctionNode* createFunctionNode(const char* name, const char* returnType, ASTNode* parameters, ASTNode* body) {
    ASTFunctionNode* node = (ASTFunctionNode*)malloc(sizeof(ASTFunctionNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTFunctionNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_FUNCTION; // Asigna el tipo de nodo
    node->name = strdup(name);      // Copia el nombre de la función
    node->returnType = strdup(returnType); // Copia el tipo de retorno
    node->parameters = parameters ? parameters : NULL; // Lista de parámetros
    node->body = body;             // Cuerpo de la función
    return node;
}

// Función para crear un nodo de declaración
ASTDeclarationNode* createDeclarationNode(const char* type, const char* name, ASTNode* init) {
    ASTDeclarationNode* node = (ASTDeclarationNode*)malloc(sizeof(ASTDeclarationNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTDeclarationNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_DECLARATION;  // Asigna el tipo de nodo
    node->base.next = NULL;             // Sin siguiente nodo
    node->type = strdup(type);          // Copia el tipo de la variable
    node->name = strdup(name);          // Copia el nombre de la variable
    node->init = init;                  // Expresión de inicialización (puede ser NULL)
    return node;
}

// Función para crear un nodo de lista de declaraciones
ASTDeclarationListNode* createDeclarationListNode(ASTNode* declaration, ASTNode* rest) {
    ASTDeclarationListNode* node = (ASTDeclarationListNode*)malloc(sizeof(ASTDeclarationListNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTDeclarationListNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_DECLARATION;  // Asigna el tipo de nodo
    node->base.next = rest;             // Apunta al siguiente nodo en la lista
    node->declaration = declaration;    // Declaración actual
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
        return node;  // Si la lista está vacía, el nodo se convierte en la lista
    }

    ASTNode* current = list;
    while (current->next) {
        current = current->next;  // Encuentra el último nodo de la lista
    }
    current->next = node;  // Agrega el nodo al final
    return list;
}

// Función para crear un nodo de bloque
ASTBlockNode* createBlockNode(ASTNode* statements) {
    ASTBlockNode* node = (ASTBlockNode*)malloc(sizeof(ASTBlockNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTBlockNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_BLOCK;   // Asigna el tipo de nodo como bloque
    node->base.next = NULL;        // Inicializa el puntero 'next' a NULL
    node->statements = statements; // Asigna la lista de sentencias o declaraciones
    return node;
}

// Función para crear un nodo New
ASTNewNode* createNewNode(const char* className, ASTNode* arguments) {
    ASTNewNode* node = (ASTNewNode*)malloc(sizeof(ASTNewNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTNewNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_NEW;
    node->className = strdup(className); // Copia del nombre de la clase
    node->arguments = arguments;        // Lista de argumentos
    return node;
}

// Función para crear el nodo de if
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

// Función para crear el nodo de while
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

// Función para crear el nodo de return
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

// Función para crear el nodo de print
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

//Lectura de funcion
ASTFunctionCallNode* createFunctionCallNode(const char* functionName, ASTNode* arguments) {
    ASTFunctionCallNode* node = (ASTFunctionCallNode*)malloc(sizeof(ASTFunctionCallNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTFunctionCallNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_FUNCTION_CALL;
    node->functionName = strdup(functionName); // Copia del nombre de la función
    node->arguments = arguments;               // Lista de argumentos
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
    node->memberName = strdup(memberName);  // Copiar el nombre del miembro
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
    node->methodName = strdup(methodName);  // Copiar el nombre del método
    node->arguments = arguments;            // Asignar los argumentos
    return node;
}

//Función para crear lista de identificadores

ASTNode* createIdentifierListNode(ASTNode* first, ASTNode* second) {
    ASTIdentifierListNode* node = (ASTIdentifierListNode*)malloc(sizeof(ASTIdentifierListNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTIdentifierListNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_IDENTIFIER_LIST;
    node->identifiers = first;  // El primer identificador
    if (second) {
        // Si hay un segundo identificador, lo agregamos a la lista
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

//Cast tipo
ASTTypeCastNode* createTypeCastNode(const char* typeName, ASTNode* expression) {
    ASTTypeCastNode* node = (ASTTypeCastNode*)malloc(sizeof(ASTTypeCastNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTTypeCastNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_TYPE_CAST;
    node->typeName = strdup(typeName);  // Copia el nombre del tipo
    node->expression = expression;     // La expresión a convertir
    return node;
}

ASTFunctionCallNode* createFunctionCallWithContextNode(ASTNode* context, ASTNode* arguments) {
    ASTFunctionCallNode* node = (ASTFunctionCallNode*)malloc(sizeof(ASTFunctionCallNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTFunctionCallNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_FUNCTION_CALL;
    node->context = context;     // Nodo antes del paréntesis (contexto)
    node->functionName = NULL;   // Esto puede ser NULL si solo usamos contexto
    node->arguments = arguments; // Lista de argumentos
    return node;
}

ASTThisNode* createThisNode() {
    ASTThisNode* node = (ASTThisNode*)malloc(sizeof(ASTThisNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTThisNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_THIS;  // Asegúrate de tener un tipo AST_THIS
    return node;
}