#include "symbol_table.h"
#include "ast.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

const char* getNodeType(ASTNode* node, SymbolTable* symbolTable);
const char* getNodeTypeString(ASTNodeType type) {
    switch (type) {
	case AST_PROGRAM: return "AST_PROGRAM";
        case AST_CLASS: return "AST_CLASS";
        case AST_FUNCTION: return "AST_FUNCTION";
        case AST_DECLARATION: return "AST_DECLARATION";
        case AST_ASSIGNMENT: return "AST_ASSIGNMENT";
        case AST_BLOCK: return "AST_BLOCK";
        case AST_IF: return "AST_IF";
        case AST_WHILE: return "AST_WHILE";
        case AST_RETURN: return "AST_RETURN";
        case AST_PRINT: return "AST_PRINT";
        case AST_EXPRESSION: return "AST_EXPRESSION";
        case AST_VARIABLE: return "AST_VARIABLE";
        case AST_LITERAL: return "AST_LITERAL";
        case AST_BINARY_OP: return "AST_BINARY_OP";
        case AST_UNARY_OP: return "AST_UNARY_OP";
        case AST_FUNCTION_CALL: return "AST_FUNCTION_CALL";
        case AST_NEW: return "AST_NEW";
        case AST_MEMBER_ACCESS: return "AST_MEMBER_ACCESS";
        case AST_METHOD_CALL: return "AST_METHOD_CALL";
        case AST_IDENTIFIER_LIST: return "AST_IDENTIFIER_LIST";
        case AST_STRING_LITERAL: return "AST_STRING_LITERAL";
        case AST_SUPER: return "AST_SUPER";
        case AST_TYPE_CAST: return "AST_TYPE_CAST";
        case AST_THIS: return "AST_THIS";
        default:
	    printf("Unknown node type encountered: %d\n", type);  // Agregado para imprimir el valor numérico
            return "Unknown";
    }
}

// Realiza el análisis semántico del árbol AST
int performSemanticAnalysis(ASTNode* root, SymbolTable* symbolTable) {
    if (!root) return 0; // No hay nada que analizar
    printf("ROOT type: %s\n", getNodeTypeString(root->type));

    switch (root->type) {
        case AST_PROGRAM: {
            ASTProgramNode* program = (ASTProgramNode*)root;

	    // Analizamos las clases (recorrer lista terminada en NULL)
            ASTNode* classNode = program->classes;
            while (classNode != NULL) {
                performSemanticAnalysis(classNode, symbolTable);  // Llamamos a performSemanticAnalysis para cada clase
                classNode = classNode->next;  // Avanzamos al siguiente nodo de clase
            }

            // Analizamos las funciones (recorrer lista terminada en NULL)
            ASTNode* functionNode = program->functions;
            while (functionNode != NULL) {
                performSemanticAnalysis(functionNode, symbolTable);  // Llamamos a performSemanticAnalysis para cada función
                functionNode = functionNode->next;  // Avanzamos al siguiente nodo de función
            }

            break;
        }
        case AST_FUNCTION: {
            ASTFunctionNode* function = (ASTFunctionNode*)root;

            // Llamar recursivamente para los parámetros y cuerpo
	    if (function->parameters) {
	        performSemanticAnalysis(function->parameters, symbolTable);
	    }
	    if (function->body) {
	        performSemanticAnalysis(function->body, symbolTable);
	     }

            break;
        }
	case AST_VARIABLE: {
            ASTVariableNode* varNode = (ASTVariableNode*)root;
            if (find_symbol(symbolTable, varNode->name) == -1) {
                fprintf(stderr, "Error: Variable '%s' no declarada.\n", varNode->name);
                return -1;
            }
            break;
        }
        case AST_DECLARATION: {
	    ASTDeclarationNode* decl = (ASTDeclarationNode*)root;
	    // Verificar si el tipo es una clase definida
	    if (!isValidType(decl->type) && !isDefinedClass(decl->type, symbolTable)) {
	        fprintf(stderr, "Error: Tipo desconocido '%s' para la variable '%s'.\n", decl->type, decl->name);
	        return -1;
	    }
	    // Verificar si la variable ya está declarada
	    if (check_variable_declaration(symbolTable, decl->name) == -1) {
	        return -1; // Error ya reportado
	    }

	    // Agregar la variable a la tabla de símbolos
	    //add_symbol(symbolTable, decl->name, decl->type, false, false, false, NULL, 0);

	    // Si hay inicialización, realiza el análisis semántico de la expresión
	    if (decl->init) {
	        const char* initType = getNodeType(decl->init, symbolTable);
	        if (!initType) {
	            fprintf(stderr, "Error: Tipo de inicialización no válido para '%s'.\n", decl->name);
	            return -1;
	        }

	        // Verifica si el tipo de la inicialización es compatible con el tipo declarado
	        if (!check_types_compatibility(decl->type, initType, "declaración")) {
	            fprintf(stderr, "Error: Tipos incompatibles al inicializar '%s'. Se esperaba '%s', pero se obtuvo '%s'.\n",
	                    decl->name, decl->type, initType);
	            return -1;
	        }
	    }
	    break;
        }
	case AST_BINARY_OP: {
	    ASTBinaryOpNode* binOp = (ASTBinaryOpNode*)root;

	    // Analizar el lado derecho de la operación binaria
	    performSemanticAnalysis(binOp->right, symbolTable);

	    // Comprobar si es una asignación (OP_ASSIGN)
	    if (binOp->op == OP_ASSIGN) {
	        const char* leftType = NULL;

	        // Verificar si el lado izquierdo es un acceso a miembro
	        if (binOp->left->type == AST_MEMBER_ACCESS) {
	            ASTMemberAccessNode* memberAccess = (ASTMemberAccessNode*)binOp->left;

	            // Verificar el objeto (ejemplo: 'r' en 'r.id')
	            if (memberAccess->expression->type != AST_VARIABLE) {
	                fprintf(stderr, "Error: Solo se pueden asignar miembros de objetos.\n");
	                return -1;
	            }

	            ASTVariableNode* objectNode = (ASTVariableNode*)memberAccess->expression;
	            int objectIndex = find_symbol(symbolTable, objectNode->name);

	            if (objectIndex == -1) {
	                fprintf(stderr, "Error: Objeto '%s' no declarado.\n", objectNode->name);
	                return -1;
	            }

	            const char* objectType = symbolTable->symbols[objectIndex].type;

	            // Obtener el tipo del miembro usando `getMemberType`
	            const char* memberName = memberAccess->memberName;
	            leftType = getMemberType(objectType, memberName, symbolTable);
	            if (!leftType) {
	                fprintf(stderr, "Error: '%s' no tiene un miembro llamado '%s'.\n", objectType, memberName);
	                return -1;
	            }
	        } else {
	            // Si no es un acceso a miembro, analizar como tipo general
	            leftType = getNodeType(binOp->left, symbolTable);
	            if (!leftType) {
	                fprintf(stderr, "Error: No se pudo determinar el tipo del lado izquierdo de la asignación.\n");
	                return -1;
	            }
	        }

	        // Obtener el tipo del lado derecho de la asignación
	        const char* rightType = getNodeType(binOp->right, symbolTable);
	        if (!rightType) {
	            fprintf(stderr, "Error: No se pudo determinar el tipo del lado derecho de la asignación.\n");
	            return -1;
	        }

	        printf("Left-hand side type: %s\n", leftType);
	        printf("Right-hand side type: %s\n", rightType);

	        // Verificar compatibilidad entre tipos básicos y clases
	        if (!check_types_compatibility(leftType, rightType, "asignación") &&
	            !areCompatibleClasses(leftType, rightType, symbolTable)) {
	            fprintf(stderr, "Error: Tipos incompatibles al asignar '%s' a '%s'.\n", rightType, leftType);
	            return -1;
	        }
	    } else {
	        // Manejo de otras operaciones binarias (+, -, *, /, etc.)
	        const char* leftType = getNodeType(binOp->left, symbolTable);
	        const char* rightType = getNodeType(binOp->right, symbolTable);

	        if (!leftType || !rightType) {
	            fprintf(stderr, "Error: No se pudo determinar los tipos en la operación binaria.\n");
	            return -1;
	        }

	        printf("Binary operation types: %s %s\n", leftType, rightType);

	        // Verificar compatibilidad en operaciones binarias
	        if (!check_types_compatibility(leftType, rightType, "operación binaria")) {
	            fprintf(stderr, "Error: Tipos incompatibles en operación binaria entre '%s' y '%s'.\n", leftType, rightType);
	            return -1;
	        }
	    }

	    break;
	}

	case AST_BLOCK: {
	    printf("Analizando un bloque de código...\n");
	    ASTBlockNode* blockNode = (ASTBlockNode*)root;

	    // Recorre todas las declaraciones o sentencias dentro del bloque
	    ASTNode* statement = blockNode->statements;
	    while (statement) {
	        performSemanticAnalysis(statement, symbolTable);
	        statement = statement->next;  // Pasa al siguiente nodo en el bloque
	    }
	    break;
	}
	case AST_CLASS: {
	    ASTClassNode* classNode = (ASTClassNode*)root;
	    printf("Analizando la clase: %s\n", classNode->name);

	    if (find_symbol(symbolTable, "Object") == -1) {
	        add_symbol(symbolTable, "Object", "class", false, true, false, NULL, 0, NULL, NULL, 0, NULL, 0);
	        printf("Clase 'Object' agregada a la tabla de símbolos.\n");
	    }

	    // Verificar si la clase está en la tabla de símbolos
	    int classIndex = find_symbol(symbolTable, classNode->name);
	    if (classIndex == -1) {
	        fprintf(stderr, "Error: Clase '%s' no está definida en la tabla de símbolos.\n", classNode->name);
	        return -1;
	    }

	    // Verificar la clase base (si existe)
	    if (classNode->parent) {
	        int parentIndex = find_symbol(symbolTable, classNode->parent);
	        if (parentIndex == -1) {
	            fprintf(stderr, "Error: Clase base '%s' de la clase '%s' no está definida.\n",
	                    classNode->parent, classNode->name);
	            return -1;
	        }
	    }

	    // Analizar atributos y métodos
	    printf("Analizando atributos y métodos de la clase '%s'\n", classNode->name);
	    ASTNode* member = classNode->members;
	    while (member) {
	        performSemanticAnalysis(member, symbolTable);  // Analizar cada miembro
	        member = member->next;  // Avanzar al siguiente miembro
	    }

	    break;
	}
	case AST_ASSIGNMENT: {
	    ASTBinaryOpNode* assignment = (ASTBinaryOpNode*)root;

	    // Lado derecho de la asignación
	    performSemanticAnalysis(assignment->right, symbolTable);

	    // Lado izquierdo de la asignación (la variable)
	    ASTVariableNode* varNode = (ASTVariableNode*)assignment->left;
	    int index = find_symbol(symbolTable, varNode->name);
	    if (index == -1) {
	        fprintf(stderr, "Error: Variable '%s' no declarada antes de asignar.\n", varNode->name);
	        return -1;
	    }

	    // Verifica el tipo
	    const char* varType = symbolTable->symbols[index].type;
	    const char* exprType = getNodeType(assignment->right, symbolTable);
	    if (!check_types_compatibility(varType, exprType, "asignación")) {
	        fprintf(stderr, "Error: Tipos incompatibles al asignar '%s' a '%s'.\n", exprType, varType);
	        return -1;
	    }
	    break;
        }
	case AST_IF: {
            ASTIfNode* ifNode = (ASTIfNode*)root;
            performSemanticAnalysis(ifNode->condition, symbolTable);
            performSemanticAnalysis(ifNode->trueBlock, symbolTable);
            if (ifNode->falseBlock) {
                performSemanticAnalysis(ifNode->falseBlock, symbolTable);
            }
            break;
        }
        case AST_WHILE: {
            ASTWhileNode* whileNode = (ASTWhileNode*)root;
            performSemanticAnalysis(whileNode->condition, symbolTable);
            performSemanticAnalysis(whileNode->body, symbolTable);
            break;
        }
        case AST_RETURN: {
            ASTReturnNode* returnNode = (ASTReturnNode*)root;
            if (returnNode->expression) {
                performSemanticAnalysis(returnNode->expression, symbolTable);
            }
            break;
        }
        case AST_PRINT: {
            ASTPrintNode* printNode = (ASTPrintNode*)root;
            performSemanticAnalysis(printNode->arguments, symbolTable);
            break;
        }
	case AST_FUNCTION_CALL: {
	    printf("Analizando llamada a función...\n");

	    ASTFunctionCallNode* funcCall = (ASTFunctionCallNode*)root;

	    // Verificar si la función está declarada en la tabla de símbolos
	    int index = find_symbol(symbolTable, funcCall->functionName);
	    if (index == -1) {
	        fprintf(stderr, "Error: Función '%s' no declarada.\n", funcCall->functionName);
	        return -1;
	    }

	    // Obtener información de los parámetros esperados
	    Symbol* functionSymbol = &symbolTable->symbols[index];
	    if (!functionSymbol->is_function) {
	        fprintf(stderr, "Error: '%s' no es una función.\n", funcCall->functionName);
	        return -1;
	    }

	    // Verificar que el número de argumentos coincida
	    int argCount = 0;
	    ASTNode* arg = funcCall->arguments;
	    while (arg) {
	        argCount++;
	        // Analizar cada argumento recursivamente
	        if (performSemanticAnalysis(arg, symbolTable) == -1) {
	            return -1; // Error en uno de los argumentos
	        }
	        arg = arg->next;
	    }

	    if (argCount != functionSymbol->func.param_count) {
	        fprintf(stderr, "Error: La función '%s' esperaba %d argumentos, pero recibió %d.\n",
	                funcCall->functionName, functionSymbol->func.param_count, argCount);
	        return -1;
	    }

	    // Verificar tipos de los argumentos
	    arg = funcCall->arguments;
	    for (int i = 0; i < functionSymbol->func.param_count; i++) {
	        const char* expectedType = functionSymbol->func.parameters[i];
	        const char* actualType = getNodeType(arg, symbolTable);
	        if (!check_parameter_type(expectedType, actualType)) {
	            fprintf(stderr, "Error: Argumento %d en la llamada a '%s': se esperaba '%s', pero se obtuvo '%s'.\n",
	                    i + 1, funcCall->functionName, expectedType, actualType);
	            return -1;
	        }
	        arg = arg->next;
	    }

	    break;
	}
	case AST_STRING_LITERAL: {
	    ASTLiteralNode* literal = (ASTLiteralNode*)root;
	    printf("Literal de cadena encontrado: valor=%s\n", literal->value);
	    return "string";
	}
	case AST_MEMBER_ACCESS: {
	    ASTMemberAccessNode* accessNode = (ASTMemberAccessNode*)root;
	    printf("Analizando acceso a miembro: %s\n", accessNode->memberName);

	    // Verificar si el objeto es "this"
	    if (accessNode->expression->type == AST_THIS) {
	        // Determinar la clase de "this" (puede estar en un campo global o en la tabla de símbolos)
	        const char* currentClassName = getNodeType(accessNode->expression, symbolTable);
	        if (!currentClassName) {
	            fprintf(stderr, "Error: 'this' fuera del contexto de una clase.\n");
	            return -1;
	        }

	        // Verificar si el miembro existe en la clase actual o en las clases base
	        if (!hasClassMember(currentClassName, accessNode->memberName, symbolTable)) {
	            fprintf(stderr, "Error: 'this' no tiene un miembro llamado '%s'.\n", accessNode->memberName);
	            return -1;
	        }
	    } else {
	        // Si no es "this", realiza el análisis semántico del objeto
	        if (performSemanticAnalysis(accessNode->expression, symbolTable) == -1) {
	            return -1;
	        }

	        // Verificar el tipo del objeto
	        const char* objectType = getNodeType(accessNode->expression, symbolTable);
	        if (!objectType || find_symbol(symbolTable, objectType) == -1) {
	            fprintf(stderr, "Error: Tipo de objeto desconocido o no definido para '%s'.\n", accessNode->memberName);
	            return -1;
	        }

	        // Verificar si el miembro existe en la clase del objeto
	        if (!hasClassMember(objectType, accessNode->memberName, symbolTable)) {
	            fprintf(stderr, "Error: '%s' no tiene un miembro llamado '%s'.\n", objectType, accessNode->memberName);
	            return -1;
	        }
	    }

	    break;
	}
        default:
            printf("Unknown node type encountered: %d\n", root->type);
            break;
    }
    return 0; // Análisis exitoso
}


// Verificar si una variable ya está declarada
int check_variable_declaration(SymbolTable* table, const char* name) {
    int aux=find_symbol(table, name);
    //printf("Indice '%i para la función '%s'. \n",aux,name);
    if (find_symbol(table, name) == -1) {
        fprintf(stderr, "Error: Variable '%s' ya declarada.\n", name);
        return -1;  // Ya está declarada
    }
    return 1;  // No está declarada
}

// Verificar si una función ya está declarada
int check_function_redefinition(SymbolTable* table, const char* name) {
    int aux=find_symbol(table, name);
    //printf("Indice '%i para la función '%s'. \n",aux,name);
    if (find_symbol(table, name) == -1) {
        fprintf(stderr, "Error: Función '%s' ya declarada.\n", name);
        return -1;  // Ya existe
    }
    return 1;  // No existe
}

// Verificación de compatibilidad de tipos (en operaciones)
int check_types_compatibility(const char* type1, const char* type2, const char* context) {
    printf("Checking compatibility: %s vs %s in %s\n", type1, type2, context);
    if (strcmp(type1, type2) == 0) {
        return 1;  // Tipos compatibles
    }

    fprintf(stderr, "Error: Tipos incompatibles en %s entre '%s' y '%s'.\n", context, type1, type2);
    return 0;  // Tipos incompatibles
}

// Verificar que el tipo de parámetro coincide con el tipo esperado
int check_parameter_type(const char* expected_type, const char* actual_type) {
    if (strcmp(expected_type, actual_type) != 0) {
        fprintf(stderr, "Error: Se esperaba '%s' pero se obtuvo '%s' como tipo de parámetro.\n", expected_type, actual_type);
        return 0;
    }
    return 1;  // Tipos coinciden
}

const char* getNodeType(ASTNode* node, SymbolTable* symbolTable) {
    if (!node) return NULL;

    switch (node->type) {
        case AST_LITERAL: {
            ASTLiteralNode* literal = (ASTLiteralNode*)node;
	    printf("Literal encontrado: tipo=%s, valor=%s\n", literal->literalType, literal->value);
            return literal->literalType; // Tipo del literal (e.g., "int", "string")
        }
        case AST_VARIABLE: {
            ASTVariableNode* var = (ASTVariableNode*)node;
            int index = find_symbol(symbolTable, var->name);
            if (index != -1) {
                return symbolTable->symbols[index].type; // Tipo de la variable
            } else {
                fprintf(stderr, "Error: Variable '%s' no declarada.\n", var->name);
                return NULL;
            }
	    printf("Variable encontrada: nombre=%s, tipo=%s\n", var->name, symbolTable->symbols[index].type);
            return symbolTable->symbols[index].type;
        }
	case AST_BINARY_OP: {
            ASTBinaryOpNode* binOp = (ASTBinaryOpNode*)node;
            // Se evalúan los tipos de los operandos de la operación binaria
            const char* leftType = getNodeType(binOp->left, symbolTable);
            const char* rightType = getNodeType(binOp->right, symbolTable);
            if (!leftType || !rightType) {
                return NULL; // Si no se puede determinar alguno de los tipos, no es posible continuar
            }
            // Aquí se podría agregar lógica para manejar operaciones específicas
            return leftType; // Asumimos que los dos operandos tienen el mismo tipo para simplificar
        }
        case AST_UNARY_OP: {
            ASTUnaryOpNode* unaryOp = (ASTUnaryOpNode*)node;
            // Evaluar el tipo del operando de la operación unaria
            const char* operandType = getNodeType(unaryOp->operand, symbolTable);
            return operandType;
        }
        case AST_FUNCTION_CALL: {
            ASTFunctionCallNode* funcCall = (ASTFunctionCallNode*)node;
            int index = find_symbol(symbolTable, funcCall->functionName);
            if (index != -1) {
                return symbolTable->symbols[index].type; // Devuelve el tipo de la función llamada
            } else {
                fprintf(stderr, "Error: Función '%s' no declarada.\n", funcCall->functionName);
                return NULL;
            }
        }
        case AST_ASSIGNMENT: {
            ASTBinaryOpNode* assignment = (ASTBinaryOpNode*)node;
            // El tipo de la asignación es el tipo del valor en el lado derecho
            return getNodeType(assignment->right, symbolTable);
        }
        case AST_BLOCK: {
            ASTBlockNode* block = (ASTBlockNode*)node;
            // Aquí se debe manejar los tipos de las sentencias dentro del bloque, si es necesario
            return "block"; // El tipo es un bloque de código
        }
        case AST_IF: {
            ASTIfNode* ifNode = (ASTIfNode*)node;
            // El tipo sería determinado por la expresión condicional
            return getNodeType(ifNode->condition, symbolTable);
        }
        case AST_WHILE: {
            ASTWhileNode* whileNode = (ASTWhileNode*)node;
            // El tipo sería determinado por la expresión condicional
            return getNodeType(whileNode->condition, symbolTable);
        }
        case AST_RETURN: {
            ASTReturnNode* returnNode = (ASTReturnNode*)node;
            if (returnNode->expression) {
                return getNodeType(returnNode->expression, symbolTable);
            }
            return "void"; // Si no hay expresión, el tipo es void
        }
        case AST_PRINT: {
            ASTPrintNode* printNode = (ASTPrintNode*)node;
            // Evaluar los tipos de los argumentos
            return getNodeType(printNode->arguments, symbolTable);
        }
        case AST_FUNCTION: {
            ASTFunctionNode* function = (ASTFunctionNode*)node;
            return function->returnType; // Tipo de retorno de la función
        }
        case AST_CLASS: {
            ASTClassNode* classNode = (ASTClassNode*)node;
            return "class"; // El tipo es "class"
        }
        case AST_MEMBER_ACCESS: {
            ASTMemberAccessNode* memberAccess = (ASTMemberAccessNode*)node;
            return getNodeType(memberAccess->expression, symbolTable); // Tipo de la propiedad o método
        }
        case AST_METHOD_CALL: {
            ASTMethodCallNode* methodCall = (ASTMethodCallNode*)node;
            return getNodeType(methodCall->expression, symbolTable); // Tipo del objeto que hace la llamada
        }
        case AST_IDENTIFIER_LIST: {
            ASTIdentifierListNode* idList = (ASTIdentifierListNode*)node;
            return "identifier_list"; // El tipo sería una lista de identificadores
        }
        case AST_STRING_LITERAL: {
            ASTStringLiteralNode* stringLiteral = (ASTStringLiteralNode*)node;
            return "string"; // Literal de tipo string
        }
        case AST_NEW: {
            ASTNewNode* newNode = (ASTNewNode*)node;
            return newNode->className; // Tipo de objeto que se crea
        }
        case AST_SUPER: {
            return "super"; // Se refiere a la clase base en una jerarquía de clases
        }
        case AST_TYPE_CAST: {
            ASTTypeCastNode* typeCastNode = (ASTTypeCastNode*)node;
            return typeCastNode->typeName; // Tipo al que se convierte
        }
        case AST_THIS: {
            return "this"; // Se refiere al objeto actual en una clase
        }
        default:
            return NULL; // No se puede determinar el tipo
    }
}

int isValidType(const char* type) {
    const char* validTypes[] = {"int", "float", "char", "void", "string"};
    for (size_t i = 0; i < sizeof(validTypes) / sizeof(validTypes[0]); i++) {
        if (strcmp(type, validTypes[i]) == 0) {
            return 1; // Tipo válido
        }
    }
    return 0; // Tipo no válido
}

int isDefinedClass(const char* className, SymbolTable* symbolTable) {
    int index = find_symbol(symbolTable, className);
    if (index != -1 && symbolTable->symbols[index].is_class) {
        return 1; // Es una clase definida
    }
    return 0; // No está definida
}

int areCompatibleClasses(const char* parent, const char* child, SymbolTable* symbolTable) {
    if (strcmp(parent, child) == 0) return 1; // Mismo tipo

    int index = find_symbol(symbolTable, child);
    while (index != -1) {
        const char* parentClass = symbolTable->symbols[index].class.parentClass;
        if (parentClass && strcmp(parentClass, parent) == 0) {
            return 1; // Encontró un ancestro común
        }
        index = find_symbol(symbolTable, parentClass);
    }
    return 0; // No son compatibles
}

int hasClassMember(const char* className, const char* memberName, SymbolTable* symbolTable) {
    int index = find_symbol(symbolTable, className);
    if (index == -1 || !symbolTable->symbols[index].is_class) return 0;

    Symbol* classSymbol = &symbolTable->symbols[index];

    // Buscar en los atributos de la clase actual
    for (int i = 0; i < classSymbol->class.attr_count; i++) {
        char* attributeEntry = classSymbol->class.attributes[i];
        char* delimiter = strchr(attributeEntry, ':');
        if (!delimiter) continue;

        size_t nameLength = delimiter - attributeEntry;
        if (strncmp(attributeEntry, memberName, nameLength) == 0 && strlen(memberName) == nameLength) {
            return 1; // Atributo encontrado
        }
    }

    // Buscar en los métodos de la clase actual
    for (int i = 0; i < classSymbol->class.method_count; i++) {
        if (strcmp(classSymbol->class.methods[i], memberName) == 0) {
            return 1; // Método encontrado
        }
    }

    // Si no se encuentra en la clase actual, buscar en la clase base
    if (classSymbol->class.parentClass) {
        return hasClassMember(classSymbol->class.parentClass, memberName, symbolTable);
    }

    return 0; // Miembro no encontrado
}