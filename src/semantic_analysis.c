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
	    printf("Unknown node type encountered: %d\n", type);  // Aggregate to print the numerical value
            return "Unknown";
    }
}

// Perform the semantic analysis of the AST tree
int performSemanticAnalysis(ASTNode* root, SymbolTable* symbolTable) {
    if (!root) return 0; // There is nothing to analyze
    printf("ROOT type: %s\n", getNodeTypeString(root->type));

    switch (root->type) {
        case AST_PROGRAM: {
            ASTProgramNode* program = (ASTProgramNode*)root;

	    // We analyze the classes (tour Null finished list)
            ASTNode* classNode = program->classes;
            while (classNode != NULL) {
                performSemanticAnalysis(classNode, symbolTable);  // We call perforganicanalysis for each class
                classNode = classNode->next;  // We advance to the next class node
            }

            // We analyze the functions (tour Null finished list)
            ASTNode* functionNode = program->functions;
            while (functionNode != NULL) {
                performSemanticAnalysis(functionNode, symbolTable);  // We call perforganalysis for each function
                functionNode = functionNode->next;  // We advance to the next function node
            }

            break;
        }
        case AST_FUNCTION: {
            ASTFunctionNode* function = (ASTFunctionNode*)root;

            // Call for parameters and body
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
                fprintf(stderr, "error:Variable '%s' not declared.\n", varNode->name);
                return -1;
            }
            break;
        }
        case AST_DECLARATION: {
	    ASTDeclarationNode* decl = (ASTDeclarationNode*)root;
	    // Verify if the type is a definite class
	    if (!isValidType(decl->type) && !isDefinedClass(decl->type, symbolTable)) {
	        fprintf(stderr, "Error: Unknown type '%s' For the variable '%s'.\n", decl->type, decl->name);
	        return -1;
	    }
	    // Verify if the variable is already declared
	    if (check_variable_declaration(symbolTable, decl->name) == -1) {
	        return -1; // Error already reported
	    }

	    // Agregar la variable a la tabla de símbolos
	    //add_symbol(symbolTable, decl->name, decl->type, false, false, false, NULL, 0);

	    // If there is initialization, perform the semantic analysis of the expression
	    if (decl->init) {
	        const char* initType = getNodeType(decl->init, symbolTable);
	        if (!initType) {
	            fprintf(stderr, "Error: type of initialization not valid for '%s'.\n", decl->name);
	            return -1;
	        }

	        // Verify if the type of initialization is compatible with the declared type
	        if (!check_types_compatibility(decl->type, initType, "declaration")) {
	            fprintf(stderr, "Error: Incompatible types when initializing '%s'. It was expected '%s', But it was obtained '%s'.\n",
	                    decl->name, decl->type, initType);
	            return -1;
	        }
	    }
	    break;
        }
	case AST_BINARY_OP: {
	    ASTBinaryOpNode* binOp = (ASTBinaryOpNode*)root;

	    // Analyze the right side of the binary operation
	    performSemanticAnalysis(binOp->right, symbolTable);

	    // Check if it is an assignment (op_assign)
	    if (binOp->op == OP_ASSIGN) {
	        const char* leftType = NULL;

	        // Verify if the left side is an access to member
	        if (binOp->left->type == AST_MEMBER_ACCESS) {
	            ASTMemberAccessNode* memberAccess = (ASTMemberAccessNode*)binOp->left;

	            // Verify the object (example: 'r' in 'r.id')
	            if (memberAccess->expression->type != AST_VARIABLE) {
	                fprintf(stderr, "Error: only members of objects can be assigned.\n");
	                return -1;
	            }

	            ASTVariableNode* objectNode = (ASTVariableNode*)memberAccess->expression;
	            int objectIndex = find_symbol(symbolTable, objectNode->name);

	            if (objectIndex == -1) {
	                fprintf(stderr, "Error: Object '%s' not declared.\n", objectNode->name);
	                return -1;
	            }

	            const char* objectType = symbolTable->symbols[objectIndex].type;

	            // Get the type of the member using `getMemberType`
	            const char* memberName = memberAccess->memberName;
	            leftType = getMemberType(objectType, memberName, symbolTable);
	            if (!leftType) {
	                fprintf(stderr, "Error: '%s' does not have a member called '%s'.\n", objectType, memberName);
	                return -1;
	            }
	        } else {
	            // If it is not an access to a member, analyze as a general type
	            leftType = getNodeType(binOp->left, symbolTable);
	            if (!leftType) {
	                fprintf(stderr, "Error: The type of the left side of the allocation could not be determined.\n");
	                return -1;
	            }
	        }

	        // Obtain the type of the right side of the allocation
	        const char* rightType = getNodeType(binOp->right, symbolTable);
	        if (!rightType) {
	            fprintf(stderr, "Error: The type of the right side of the allocation could not be determined.\n");
	            return -1;
	        }

	        printf("Left-hand side type: %s\n", leftType);
	        printf("Right-hand side type: %s\n", rightType);

	        // Verify compatibility between basic types and classes
	        if (!check_types_compatibility(leftType, rightType, "assignment") &&
	            !areCompatibleClasses(leftType, rightType, symbolTable)) {
	            fprintf(stderr, "Error: Incompatible types when assigning '%s' a '%s'.\n", rightType, leftType);
	            return -1;
	        }
	    } else {
	        // Management of other binary operations (+, -, *, /, etc.)
	        const char* leftType = getNodeType(binOp->left, symbolTable);
	        const char* rightType = getNodeType(binOp->right, symbolTable);

	        if (!leftType || !rightType) {
	            fprintf(stderr, "Error: the types in the binary operation could not be determined.\n");
	            return -1;
	        }

	        printf("Binary operation types: %s %s\n", leftType, rightType);

	        // Verify compatibility in binary operations
	        if (!check_types_compatibility(leftType, rightType, "binary operation")) {
	            fprintf(stderr, "Error: incompatible types in binary operation between '%s' y '%s'.\n", leftType, rightType);
	            return -1;
	        }
	    }

	    break;
	}

	case AST_BLOCK: {
	    printf("Analyzing a block of code...\n");
	    ASTBlockNode* blockNode = (ASTBlockNode*)root;

	    // Tour all statements or sentences within the block
	    ASTNode* statement = blockNode->statements;
	    while (statement) {
	        performSemanticAnalysis(statement, symbolTable);
	        statement = statement->next;  // Go to the next node in the block
	    }
	    break;
	}
	case AST_CLASS: {
	    ASTClassNode* classNode = (ASTClassNode*)root;
	    printf("Analyzing class: %s\n", classNode->name);

	    if (find_symbol(symbolTable, "Object") == -1) {
	        add_symbol(symbolTable, "Object", "class", false, true, false, NULL, 0, NULL, NULL, 0, NULL, 0);
	        printf("Class 'Object' added to the symbols table.\n");
	    }

	    // Verify if the class is in the symbols table
	    int classIndex = find_symbol(symbolTable, classNode->name);
	    if (classIndex == -1) {
	        fprintf(stderr, "Error: class '%s' It is not defined in the symbols table.\n", classNode->name);
	        return -1;
	    }

	    // Verify the base class (if it exists)
	    if (classNode->parent) {
	        int parentIndex = find_symbol(symbolTable, classNode->parent);
	        if (parentIndex == -1) {
	            fprintf(stderr, "Error: Base class '%s' of class '%s' It is not defined.\n",
	                    classNode->parent, classNode->name);
	            return -1;
	        }
	    }

	    // Analyze attributes and methods
	    printf("Analyzing class attributes and methods '%s'\n", classNode->name);
	    ASTNode* member = classNode->members;
	    while (member) {
	        performSemanticAnalysis(member, symbolTable);  // Analyze each member
	        member = member->next;  // Advance to the next member
	    }

	    break;
	}
	case AST_ASSIGNMENT: {
	    ASTBinaryOpNode* assignment = (ASTBinaryOpNode*)root;

	    // Right side of the allocation
	    performSemanticAnalysis(assignment->right, symbolTable);

	    // Left side of the allocation (the variable)
	    ASTVariableNode* varNode = (ASTVariableNode*)assignment->left;
	    int index = find_symbol(symbolTable, varNode->name);
	    if (index == -1) {
	        fprintf(stderr, "Error: Variable '%s' not declared before assigning.\n", varNode->name);
	        return -1;
	    }

	    // Verify the guy
	    const char* varType = symbolTable->symbols[index].type;
	    const char* exprType = getNodeType(assignment->right, symbolTable);
	    if (!check_types_compatibility(varType, exprType, "assignment")) {
	        fprintf(stderr, "Error: Incompatible types when assigning '%s' a '%s'.\n", exprType, varType);
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
	    printf("Analyzing function call...\n");

	    ASTFunctionCallNode* funcCall = (ASTFunctionCallNode*)root;

	    // Verify if the function is declared in the symbols table
	    int index = find_symbol(symbolTable, funcCall->functionName);
	    if (index == -1) {
	        fprintf(stderr, "Error: Function '%s' not declared.\n", funcCall->functionName);
	        return -1;
	    }

	    // Obtain information from the expected parameters
	    Symbol* functionSymbol = &symbolTable->symbols[index];
	    if (!functionSymbol->is_function) {
	        fprintf(stderr, "Error: '%s' It is not a function.\n", funcCall->functionName);
	        return -1;
	    }

	    // Verify that the number of arguments coincides
	    int argCount = 0;
	    ASTNode* arg = funcCall->arguments;
	    while (arg) {
	        argCount++;
	        // Analyze each recursively argument
	        if (performSemanticAnalysis(arg, symbolTable) == -1) {
	            return -1; // Error in one of the arguments
	        }
	        arg = arg->next;
	    }

	    if (argCount != functionSymbol->func.param_count) {
	        fprintf(stderr, "Error: The function '%s' I expected %d arguments, But he received %d.\n",
	                funcCall->functionName, functionSymbol->func.param_count, argCount);
	        return -1;
	    }

	    // Verify types of arguments
	    arg = funcCall->arguments;
	    for (int i = 0; i < functionSymbol->func.param_count; i++) {
	        const char* expectedType = functionSymbol->func.parameters[i];
	        const char* actualType = getNodeType(arg, symbolTable);
	        if (!check_parameter_type(expectedType, actualType)) {
	            fprintf(stderr, "Error: Argument %d In the call to '%s': It was expected '%s', But it was obtained '%s'.\n",
	                    i + 1, funcCall->functionName, expectedType, actualType);
	            return -1;
	        }
	        arg = arg->next;
	    }

	    break;
	}
	case AST_STRING_LITERAL: {
	    ASTLiteralNode* literal = (ASTLiteralNode*)root;
	    printf("Found chain literal: value=%s\n", literal->value);
	    return "string";
	}
	case AST_MEMBER_ACCESS: {
	    ASTMemberAccessNode* accessNode = (ASTMemberAccessNode*)root;
	    printf("Analyzing member access: %s\n", accessNode->memberName);

	    // Verify if the object is "This"
	    if (accessNode->expression->type == AST_THIS) {
	        // Determine the "This" class (may be in a global field or in the symbols table)
	        const char* currentClassName = getNodeType(accessNode->expression, symbolTable);
	        if (!currentClassName) {
	            fprintf(stderr, "Error: 'This' outside the context of a class.\n");
	            return -1;
	        }

	        // Verify if the member exists in the current class or in the base classes
	        if (!hasClassMember(currentClassName, accessNode->memberName, symbolTable)) {
	            fprintf(stderr, "Error: 'This' does not have a member called '%s'.\n", accessNode->memberName);
	            return -1;
	        }
	    } else {
	        // If it is not "This", perform the semantic analysis of the object
	        if (performSemanticAnalysis(accessNode->expression, symbolTable) == -1) {
	            return -1;
	        }

	        // Verify the type of the object
	        const char* objectType = getNodeType(accessNode->expression, symbolTable);
	        if (!objectType || find_symbol(symbolTable, objectType) == -1) {
	            fprintf(stderr, "Error: type of unknown or not defined object for '%s'.\n", accessNode->memberName);
	            return -1;
	        }

	        // Verify if the member exists in the object class
	        if (!hasClassMember(objectType, accessNode->memberName, symbolTable)) {
	            fprintf(stderr, "Error: '%s' does not have a member called '%s'.\n", objectType, accessNode->memberName);
	            return -1;
	        }
	    }

	    break;
	}
        default:
            printf("Unknown node type encountered: %d\n", root->type);
            break;
    }
    return 0; // Successful analysis
}


// Verify if a variable is already declared
int check_variable_declaration(SymbolTable* table, const char* name) {
    int aux=find_symbol(table, name);
    //printf("Indice '%i para la función '%s'. \n",aux,name);
    if (find_symbol(table, name) == -1) {
        fprintf(stderr, "Error: Variable '%s' already declared.\n", name);
        return -1;  // It is already declared
    }
    return 1;  // It is not declared
}

// Verify if a function is already declared
int check_function_redefinition(SymbolTable* table, const char* name) {
    int aux=find_symbol(table, name);
    //printf("Indice '%i para la función '%s'. \n",aux,name);
    if (find_symbol(table, name) == -1) {
        fprintf(stderr, "Error: Function '%s' already declared.\n", name);
        return -1;  // It already exists
    }
    return 1;  // It does not exist
}

// Verification of type compatibility (in operations)
int check_types_compatibility(const char* type1, const char* type2, const char* context) {
    printf("Checking compatibility: %s vs %s in %s\n", type1, type2, context);
    if (strcmp(type1, type2) == 0) {
        return 1;  // Compatible types
    }

    fprintf(stderr, "Error: Incompatible types in %s between '%s' y '%s'.\n", context, type1, type2);
    return 0;  // Incompatible types
}

// Verify that the parameter type coincides with the expected type
int check_parameter_type(const char* expected_type, const char* actual_type) {
    if (strcmp(expected_type, actual_type) != 0) {
        fprintf(stderr, "Error: It was expected '%s' But it was obtained '%s' as parameter type.\n", expected_type, actual_type);
        return 0;
    }
    return 1;  // Types coincide
}

const char* getNodeType(ASTNode* node, SymbolTable* symbolTable) {
    if (!node) return NULL;

    switch (node->type) {
        case AST_LITERAL: {
            ASTLiteralNode* literal = (ASTLiteralNode*)node;
	    printf("Literal found: type=%s, valor=%s\n", literal->literalType, literal->value);
            return literal->literalType; // Type of the literal (e.g., "int", "string")
        }
        case AST_VARIABLE: {
            ASTVariableNode* var = (ASTVariableNode*)node;
            int index = find_symbol(symbolTable, var->name);
            if (index != -1) {
                return symbolTable->symbols[index].type; // Variable type
            } else {
                fprintf(stderr, "Error: Variable '%s' not declared.\n", var->name);
                return NULL;
            }
	    printf("Variable found: Name=%s, type=%s\n", var->name, symbolTable->symbols[index].type);
            return symbolTable->symbols[index].type;
        }
	case AST_BINARY_OP: {
            ASTBinaryOpNode* binOp = (ASTBinaryOpNode*)node;
            // The types of operands of the binary operation are evaluated
            const char* leftType = getNodeType(binOp->left, symbolTable);
            const char* rightType = getNodeType(binOp->right, symbolTable);
            if (!leftType || !rightType) {
                return NULL; // If any of the types cannot be determined, it is not possible to continue
            }
            // Here you could add logic to handle specific operations
            return leftType; // We assume that the two operands have the same type to simplify
        }
        case AST_UNARY_OP: {
            ASTUnaryOpNode* unaryOp = (ASTUnaryOpNode*)node;
            // Evaluate the type of operand of the Operation Unaria
            const char* operandType = getNodeType(unaryOp->operand, symbolTable);
            return operandType;
        }
        case AST_FUNCTION_CALL: {
            ASTFunctionCallNode* funcCall = (ASTFunctionCallNode*)node;
            int index = find_symbol(symbolTable, funcCall->functionName);
            if (index != -1) {
                return symbolTable->symbols[index].type; // Returns the type of the function called
            } else {
                fprintf(stderr, "Error: Function '%s' not declared.\n", funcCall->functionName);
                return NULL;
            }
        }
        case AST_ASSIGNMENT: {
            ASTBinaryOpNode* assignment = (ASTBinaryOpNode*)node;
            // The type of allocation is the type of value on the right side
            return getNodeType(assignment->right, symbolTable);
        }
        case AST_BLOCK: {
            ASTBlockNode* block = (ASTBlockNode*)node;
            // Here you must handle the types of sentences within the block, if necessary
            return "block"; // The type is a block of code
        }
        case AST_IF: {
            ASTIfNode* ifNode = (ASTIfNode*)node;
            // The type would be determined by conditional expression
            return getNodeType(ifNode->condition, symbolTable);
        }
        case AST_WHILE: {
            ASTWhileNode* whileNode = (ASTWhileNode*)node;
            // The type would be determined by conditional expression
            return getNodeType(whileNode->condition, symbolTable);
        }
        case AST_RETURN: {
            ASTReturnNode* returnNode = (ASTReturnNode*)node;
            if (returnNode->expression) {
                return getNodeType(returnNode->expression, symbolTable);
            }
            return "void"; // If there is no expression, the guy is void
        }
        case AST_PRINT: {
            ASTPrintNode* printNode = (ASTPrintNode*)node;
            // Evaluate the types of arguments
            return getNodeType(printNode->arguments, symbolTable);
        }
        case AST_FUNCTION: {
            ASTFunctionNode* function = (ASTFunctionNode*)node;
            return function->returnType; // Type of function return
        }
        case AST_CLASS: {
            ASTClassNode* classNode = (ASTClassNode*)node;
            return "class"; // The guy is "class"
        }
        case AST_MEMBER_ACCESS: {
            ASTMemberAccessNode* memberAccess = (ASTMemberAccessNode*)node;
            return getNodeType(memberAccess->expression, symbolTable); // Type of property or method
        }
        case AST_METHOD_CALL: {
            ASTMethodCallNode* methodCall = (ASTMethodCallNode*)node;
            return getNodeType(methodCall->expression, symbolTable); // Type of the object that makes the call
        }
        case AST_IDENTIFIER_LIST: {
            ASTIdentifierListNode* idList = (ASTIdentifierListNode*)node;
            return "identifier_list"; // The type would be a list of identifiers
        }
        case AST_STRING_LITERAL: {
            ASTStringLiteralNode* stringLiteral = (ASTStringLiteralNode*)node;
            return "string"; // String type literal
        }
        case AST_NEW: {
            ASTNewNode* newNode = (ASTNewNode*)node;
            return newNode->className; // Type of object that is created
        }
        case AST_SUPER: {
            return "super"; // It refers to the base class in a hierarchy of classes
        }
        case AST_TYPE_CAST: {
            ASTTypeCastNode* typeCastNode = (ASTTypeCastNode*)node;
            return typeCastNode->typeName; // Type to which it becomes
        }
        case AST_THIS: {
            return "this"; // Refers to the current object in a class
        }
        default:
            return NULL; // The type cannot be determined
    }
}

int isValidType(const char* type) {
    const char* validTypes[] = {"int", "float", "char", "void", "string"};
    for (size_t i = 0; i < sizeof(validTypes) / sizeof(validTypes[0]); i++) {
        if (strcmp(type, validTypes[i]) == 0) {
            return 1; // Valid type
        }
    }
    return 0; // Tipo no válido
}

int isDefinedClass(const char* className, SymbolTable* symbolTable) {
    int index = find_symbol(symbolTable, className);
    if (index != -1 && symbolTable->symbols[index].is_class) {
        return 1; // It is a definite class
    }
    return 0; // It is not defined
}

int areCompatibleClasses(const char* parent, const char* child, SymbolTable* symbolTable) {
    if (strcmp(parent, child) == 0) return 1; // Same type

    int index = find_symbol(symbolTable, child);
    while (index != -1) {
        const char* parentClass = symbolTable->symbols[index].class.parentClass;
        if (parentClass && strcmp(parentClass, parent) == 0) {
            return 1; // Found a common ancestor
        }
        index = find_symbol(symbolTable, parentClass);
    }
    return 0; // They are not compatible
}

int hasClassMember(const char* className, const char* memberName, SymbolTable* symbolTable) {
    int index = find_symbol(symbolTable, className);
    if (index == -1 || !symbolTable->symbols[index].is_class) return 0;

    Symbol* classSymbol = &symbolTable->symbols[index];

    // Search for the attributes of the current class
    for (int i = 0; i < classSymbol->class.attr_count; i++) {
        char* attributeEntry = classSymbol->class.attributes[i];
        char* delimiter = strchr(attributeEntry, ':');
        if (!delimiter) continue;

        size_t nameLength = delimiter - attributeEntry;
        if (strncmp(attributeEntry, memberName, nameLength) == 0 && strlen(memberName) == nameLength) {
            return 1; // Attribute found
        }
    }

    // Search in the current class methods
    for (int i = 0; i < classSymbol->class.method_count; i++) {
        if (strcmp(classSymbol->class.methods[i], memberName) == 0) {
            return 1; // Method Found
        }
    }

    // If you are not in the current class, search the base class
    if (classSymbol->class.parentClass) {
        return hasClassMember(classSymbol->class.parentClass, memberName, symbolTable);
    }

    return 0; // Member not found
}