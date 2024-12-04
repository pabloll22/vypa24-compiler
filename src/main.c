#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "ast.h"
#include "parser.h"
#include "string.h"

// Prototipo para la función generada por Bison
extern int yyparse();
extern int yydebug;
extern FILE* yyin;
//ASTNode* root = NULL;
SymbolTable symbol_table;

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
        case AST_BINARY_OP: {
            ASTBinaryOpNode* binaryNode = (ASTBinaryOpNode*)node;
            printf("Binary Operation: ");

            // Imprimir el tipo de operación
            switch (binaryNode->op) {
                case OP_ASSIGN: printf("="); break;
                case OP_ADD: printf("+"); break;
                case OP_SUB: printf("-"); break;
                case OP_MUL: printf("*"); break;
                case OP_DIV: printf("/"); break;
                case OP_LT: printf("<"); break;
                case OP_GT: printf(">"); break;
                case OP_EQ: printf("=="); break;
                case OP_NE: printf("!="); break;
                case OP_LE: printf("<="); break;
                case OP_GE: printf(">="); break;
                default: printf("unknown"); break;
            }

            // Imprimir los operandos izquierdo y derecho
            printf("\n");
            printf("%*s  Left: ",indent+2, "");
            printAST(binaryNode->left,0);
            printf("%*s  Right: ", indent+2, "");
            printAST(binaryNode->right, 0);
            break;
        }
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
        case AST_IF:
            printf("If Statement\n");
            printf("  Condition:\n");
            printAST(((ASTIfNode*)node)->condition, indent + 1);
            printf("  True Block:\n");
            printAST(((ASTIfNode*)node)->trueBlock, indent + 1);
            if (((ASTIfNode*)node)->falseBlock) {
                printf("  False Block:\n");
                printAST(((ASTIfNode*)node)->falseBlock, indent + 1);
            }
            break;

        case AST_WHILE:
            printf("While Loop\n");
            printf("  Condition:\n");
            printAST(((ASTWhileNode*)node)->condition, indent + 1);
            printf("  Body:\n");
            printAST(((ASTWhileNode*)node)->body, indent + 1);
            break;

        case AST_RETURN:
            printf("Return Statement\n");
            if (((ASTReturnNode*)node)->expression) {
                printf("  Expression:\n");
                printAST(((ASTReturnNode*)node)->expression, indent + 1);
            }
            break;

        case AST_PRINT:
            printf("Print Statement\n");
            if (((ASTPrintNode*)node)->arguments) {
                printf("  Arguments:\n");
                printAST(((ASTPrintNode*)node)->arguments, indent + 1);
            }
            break;
        case AST_FUNCTION_CALL:
            printf("Function Call: %s\n", ((ASTFunctionCallNode*)node)->functionName);
            if (((ASTFunctionCallNode*)node)->arguments) {
                printf("  Arguments:\n");
                printAST(((ASTFunctionCallNode*)node)->arguments, indent + 1);
            }
            break;
        case AST_IDENTIFIER_LIST: {
            ASTIdentifierListNode* listNode = (ASTIdentifierListNode*)node;
            printf("Identifier List: ");
            // Recorrer la lista de identificadores e imprimirlos
            ASTNode* current = listNode->identifiers;
            while (current) {
                printf("%s ", ((ASTVariableNode*)current)->name);
                current = current->next;
           }
           printf("\n");
           break;
        }
        case AST_MEMBER_ACCESS: {
           ASTMemberAccessNode* memberAccessNode = (ASTMemberAccessNode*)node;
           printf("Member Access: %s\n", memberAccessNode->memberName);
           printf("  Object:\n");
           printAST(memberAccessNode->expression, indent + 1);
           break;
        }
        case AST_METHOD_CALL: {
           ASTMethodCallNode* methodCallNode = (ASTMethodCallNode*)node;
           printf("Method Call: %s\n", methodCallNode->methodName);
           printf("  Object:\n");
           printAST(methodCallNode->expression, indent + 1);
           printf("  Arguments:\n");
           printAST(methodCallNode->arguments, indent + 1);
           break;
        }
        case AST_STRING_LITERAL: {
            ASTStringLiteralNode* stringLiteralNode = (ASTStringLiteralNode*)node;
            printf("String Literal Node: \"%s\"\n", stringLiteralNode->value);
            break;
        }
        case AST_TYPE_CAST: {
            ASTTypeCastNode* castNode = (ASTTypeCastNode*)node;
            printf("Type Cast Node: (%s)\n", castNode->typeName);
            printf("  Expression:\n");
            printAST(castNode->expression, indent + 1);
            break;
        }
        case AST_THIS:
            printf("This Node\n");
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

    init_symbol_table(&symbol_table);

//    yydebug = 1;

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

    // Muestra el contenido de la tabla de símbolos
    printf("\nSymbol Table:\n");
    print_symbol_table(&symbol_table);  // Función que imprimirá la tabla de símbolos
    return EXIT_SUCCESS;
}