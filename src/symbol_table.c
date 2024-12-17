#include "ast.h"
#include "symbol_table.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void init_symbol_table(SymbolTable* table) {
    table->symbol_count = 0;
}

// Add a new symbol to the table
int add_symbol(SymbolTable* table, const char* name, const char* type, bool is_function, bool is_class,
		bool is_object, char** params, int param_count,const char* parentClass,
		char** attributes, int attr_count, char** methods, int method_count) {
    if (table->symbol_count >= MAX_SYMBOLS) {
        printf("Error: Full symbols table\n");
        return -1;
    }

    if (find_symbol(table, name) != -1) {
        printf("Error: The symbol '%s' It is already defined\n", name);
        return -2;
    }

    Symbol new_symbol;
    new_symbol.name = strdup(name);
    new_symbol.type = strdup(type);
    new_symbol.defined = false;  // Initially not defined
    new_symbol.is_function = is_function;
    new_symbol.is_class = is_class;
    new_symbol.is_object = is_object;

    if (is_function) {
        new_symbol.func.parameters = params;
        new_symbol.func.param_count = param_count;
    } else if (is_class) {
	new_symbol.class.methods = methods;
        new_symbol.class.method_count = method_count;

        new_symbol.class.attributes = attributes;
        new_symbol.class.attr_count = attr_count;
	new_symbol.class.parentClass = parentClass ? strdup(parentClass) : NULL;
    }

    table->symbols[table->symbol_count++] = new_symbol;
    return 0;
}

// Find a symbol by name
int find_symbol(SymbolTable* table, const char* name) {
    for (int i = 0; i < table->symbol_count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return i; // Symbol index
        }
    }
    return -1; // Not found
}

// Print the symbols table
void print_symbol_table(SymbolTable* table) {
    printf("------ Symbol Table ------\n");
    for (int i = 0; i < table->symbol_count; i++) {
        Symbol* sym = &table->symbols[i];

        // General Information
        printf("Name: %s\n", sym->name);
        printf("  Type: %s\n", sym->type);
        printf("  Defined: %s\n", sym->defined ? "Yes" : "No");

        // If it is a function
        if (sym->is_function) {
            printf("  Symbol Type: Function\n");
            printf("  Parameters (%d):\n", sym->func.param_count);
            for (int j = 0; j < sym->func.param_count; j++) {
                printf("    - %s\n", sym->func.parameters[j]);
            }
        }

        // If it is a class
        else if (sym->is_class) {
            printf("  Symbol Type: Class\n");
            printf("  Parent Class: %s\n", sym->class.parentClass ? sym->class.parentClass : "None");
            printf("  Attributes (%d):\n", sym->class.attr_count);
            for (int j = 0; j < sym->class.attr_count; j++) {
                printf("    - %s\n", sym->class.attributes[j]);
            }
            printf("  Methods (%d):\n", sym->class.method_count);
            for (int j = 0; j < sym->class.method_count; j++) {
                printf("    - %s\n", sym->class.methods[j]);
            }
        }

        // If it is an object
        else if (sym->is_object) {
            printf("  Symbol Type: Object\n");
        }

        // Symbols separator
        printf("--------------------------\n");
    }
    printf("------ End of Table ------\n");
}

void free_symbol_table(SymbolTable* table) {
    for (int i = 0; i < table->symbol_count; i++) {
        free(table->symbols[i].name);  // Release symbol name
        // If you use dynamic memory for other fields, also free them here
    }
    table->symbol_count = 0;  // Reset the accountant
}

char** extractAttributesFromClassBody(ASTNode* class_body) {
    char** attributes = malloc(MAX_ATTRIBUTES * sizeof(char*));
    int count = 0;

    ASTNode* current = class_body;
    while (current) {
        if (current->type == AST_DECLARATION) {
            ASTDeclarationNode* decl = (ASTDeclarationNode*)current;

            // Create a chain with the format "Name: Type"
            char* attributeEntry = malloc(strlen(decl->name) + strlen(decl->type) + 2); // ':' y '\0'
            sprintf(attributeEntry, "%s:%s", decl->name, decl->type);

            attributes[count++] = attributeEntry; // Add to the attributes list
        }
        current = current->next;
    }
    return attributes;
}


char** extractMethodsFromClassBody(ASTNode* class_body) {
    char** methods = malloc(MAX_METHODS * sizeof(char*));
    int count = 0;

    ASTNode* current = class_body;
    while (current) {
        if (current->type == AST_FUNCTION) {
            ASTFunctionNode* func = (ASTFunctionNode*)current;
            methods[count++] = strdup(func->name);
        }
        current = current->next;
    }
    return methods;
}

int countAttributes(ASTNode* class_body) {
    int count = 0;
    ASTNode* current = class_body;
    while (current) {
        if (current->type == AST_DECLARATION) {
            count++;
        }
        current = current->next;
    }
    return count;
}

int countMethods(ASTNode* class_body) {
    int count = 0;
    ASTNode* current = class_body;
    while (current) {
        if (current->type == AST_FUNCTION) {
            count++;
        }
        current = current->next;
    }
    return count;
}

const char* getMemberType(const char* className, const char* memberName, SymbolTable* symbolTable) {
    int index = find_symbol(symbolTable, className);
    if (index == -1 || !symbolTable->symbols[index].is_class) return NULL;

    Symbol* classSymbol = &symbolTable->symbols[index];

    // Search for the attributes of the current class
    for (int i = 0; i < classSymbol->class.attr_count; i++) {
        char* attributeEntry = classSymbol->class.attributes[i];
        char* delimiter = strchr(attributeEntry, ':');
        if (!delimiter) continue;

        size_t nameLength = delimiter - attributeEntry;
        if (strncmp(attributeEntry, memberName, nameLength) == 0 && strlen(memberName) == nameLength) {
            return delimiter + 1; // Returns the guy after the ':'
        }
    }

    // Search in the current class methods
    for (int i = 0; i < classSymbol->class.method_count; i++) {
        if (strcmp(classSymbol->class.methods[i], memberName) == 0) {
            return "function"; // Predetermined type for methods
        }
    }

    // If you are not in the current class, search the base class
    if (classSymbol->class.parentClass) {
        return getMemberType(classSymbol->class.parentClass, memberName, symbolTable);
    }

    return NULL; // Member not found
}
