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
    node->base.next = NULL;         // Sin siguiente nodo
    node->name = strdup(name);      // Copia el nombre de la función
    node->returnType = strdup(returnType); // Copia el tipo de retorno
    node->parameters = parameters; // Lista de parámetros
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
    node->type = strdup(type);
    return node;
}

ASTVariableNode* createVariableNode(const char* name) {
    ASTVariableNode* node = (ASTVariableNode*)malloc(sizeof(ASTVariableNode));
    if (!node) {
        fprintf(stderr, "Error: no se pudo asignar memoria para ASTVariableNode.\n");
        exit(EXIT_FAILURE);
    }
    node->base.type = AST_VARIABLE;
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

