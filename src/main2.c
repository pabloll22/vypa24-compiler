  GNU nano 4.9                                                                                                                                          src/main.c
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"

// Prototipo para la función generada por Bison
extern int yyparse();
extern FILE* yyin;


// Función para imprimir el AST (puedes extenderla según lo que quieras mostrar)
void printAST(ASTNode* node, int indent) {
    if (!node) {
        for (int i = 0; i < indent; i++) printf("  ");
        printf("(null)\n");
        return;
    }

    for (int i = 0; i < indent; i++) {
        printf("  "); // Indentación para visualizar la jerarquía
    }

    switch (node->type) {
        case AST_PROGRAM:
            printf("Program Node\n");
            printAST(((ASTProgramNode*)node)->classes, indent + 1);
            printAST(((ASTProgramNode*)node)->functions, indent + 1);
            break;
        case AST_CLASS:
            printf("Class Node: %s\n", ((ASTClassNode*)node)->name);
            printAST(((ASTClassNode*)node)->members, indent + 1);
            break;
        case AST_FUNCTION:
            printf("Function Node: %s\n", ((ASTFunctionNode*)node)->name);
            printAST(((ASTFunctionNode*)node)->parameters, indent + 1);
            printAST(((ASTFunctionNode*)node)->body, indent + 1);
            break;
        case AST_DECLARATION:
            printf("Declaration Node: %s %s\n",
                   ((ASTDeclarationNode*)node)->type,
                   ((ASTDeclarationNode*)node)->name);
            if (((ASTDeclarationNode*)node)->init) {
                printf("Initialization:\n");
                printAST(((ASTDeclarationNode*)node)->init, indent + 1);
            }
            break;
        /*case AST_BLOCK:
            printf("Block Node\n");
            printAST(((ASTBlockNode*)node)->statements, indent + 1);
            break;*/
        default:
            printf("Unknown Node Type\n");
            break;
    }

    // Recorrer nodos hermanos si los hay
    if (node->next) {
        printAST(node->next, indent);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    yyin = inputFile;

    // Parse the input file
    if (yyparse() == 0) {
        printf("Parsing completed successfully.\n");
    } else {
        printf("Parsing failed.\n");
        return EXIT_FAILURE;
    }

    fclose(inputFile);
/*
    // Check if the AST was constructed
    if (!root) {
        fprintf(stderr, "Error: AST root is NULL.\n");
        return EXIT_FAILURE;
    }

    // Print the AST
    printf("Abstract Syntax Tree (AST):\n");
    printAST(root, 0);  // Assuming printAST takes the root and an indent level
    return EXIT_SUCCESS;*/
}

