#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"

// Prototipo para la función generada por Bison
extern int yyparse();
extern int yydebug;
extern FILE* yyin;
ASTNode* root = NULL;

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
            printf("Function: %s\n", ((ASTFunctionNode*)node)->name);
            printf("  Return Type: %s\n", ((ASTFunctionNode*)node)->returnType);
            printf("  Parameters:\n");
            printAST(((ASTFunctionNode*)node)->parameters, indent + 1);
            printf("  Body:\n");
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
        case AST_BINARY_OP:
            ASTBinaryOpNode* binaryNode = (ASTBinaryOpNode*)node;
            printf("Binary Operation: ");
            switch (binaryNode->op) {
                case OP_ASSIGN: printf("="); break;
                case OP_ADD: printf("+"); break;
                case OP_SUB: printf("-"); break;
                case OP_MUL: printf("*"); break;
                case OP_DIV: printf("/"); break;
                default: printf("unknown"); break;
            }
           printf("\n");
           printf("  Left:\n");
           printAST(binaryNode->left, indent + 1);
           printf("  Right:\n");
           printAST(binaryNode->right, indent + 1);
           break;
        case AST_BLOCK:
            printf("Block Node\n");
            if (((ASTBlockNode*)node)->statements) {
                printAST(((ASTBlockNode*)node)->statements, indent + 1);
            }else {
                printf("(empty block)\n");
            }
            break;

        case AST_VARIABLE: {
           ASTVariableNode* varNode = (ASTVariableNode*)node;
           printf("Variable Node: %s\n", varNode->name);
           break;
        }
        case AST_LITERAL: {
           ASTLiteralNode* litNode = (ASTLiteralNode*)node;
           printf("Literal Node: %s (%s)\n", litNode->value, litNode->literalType);
           break;
        }
        case AST_NEW:
           printf("New Node: %s\n", ((ASTNewNode*)node)->className);
           if (((ASTNewNode*)node)->arguments) {
                printf("  Arguments:\n");
                printAST(((ASTNewNode*)node)->arguments, indent + 2);
           }
           break;
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

    //yydebug = 1;

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

    // Check if the AST was constructed
    if (!root) {
        fprintf(stderr, "Error: AST root is NULL.\n");
        return EXIT_FAILURE;
    }

    // Print the AST
    printf("Abstract Syntax Tree (AST):\n");
    printAST(root, 0);  // Assuming printAST takes the root and an indent level
    return EXIT_SUCCESS;
}
