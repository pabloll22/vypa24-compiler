#include "symbol_table.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void init_symbol_table(SymbolTable* table) {
    table->symbol_count = 0;
}

// Agregar un nuevo símbolo a la tabla
int add_symbol(SymbolTable* table, const char* name, const char* type,
               bool is_function, bool is_class, bool is_object, char** params, int param_count) {
    if (table->symbol_count >= MAX_SYMBOLS) {
        printf("Error: Tabla de símbolos llena\n");
        return -1;
    }

    if (find_symbol(table, name) != -1) {
        printf("Error: El símbolo '%s' ya está definido\n", name);
        return -2;
    }

    Symbol new_symbol;
    new_symbol.name = strdup(name);
    new_symbol.type = type;
    new_symbol.defined = false;  // Inicialmente no definido
    new_symbol.is_function = is_function;
    new_symbol.is_class = is_class;
    new_symbol.is_object = is_object;

    if (is_function) {
        new_symbol.func.parameters = params;
        new_symbol.func.param_count = param_count;
    } else if (is_class) {
        new_symbol.class.methods = NULL;
        new_symbol.class.attributes = NULL;
        new_symbol.class.method_count = 0;
        new_symbol.class.attr_count = 0;
    }

    table->symbols[table->symbol_count++] = new_symbol;
    return 0;
}

// Buscar un símbolo por su nombre
int find_symbol(SymbolTable* table, const char* name) {
    for (int i = 0; i < table->symbol_count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return i; // Índice del símbolo
        }
    }
    return -1; // No encontrado
}

// Imprimir la tabla de símbolos
void print_symbol_table(SymbolTable* table) {
    for (int i = 0; i < table->symbol_count; i++) {
        Symbol* sym = &table->symbols[i];
        printf("Name: %s, Type: %s, Defined: %s",
               sym->name, sym->type, sym->defined ? "Yes" : "No");

        if (sym->is_function) {
            printf(", Function with %d parameters", sym->func.param_count);
        } else if (sym->is_class) {
            printf(", Class with %d methods and %d attributes",
                   sym->class.method_count, sym->class.attr_count);
        } else if (sym->is_object) {
            printf(", Object");
        }
        printf("\n");
    }
}