#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#define MAX_SYMBOLS 100

typedef struct Symbol {
    char* name;  // Nombre de la variable
    const char* type;
    bool defined;
    bool is_function;   // Si es una función
    bool is_class;      // Si es una clase
    bool is_object;     // Si es un objeto (instancia de una clase)
    union {
        // Para funciones, contiene los parámetros
        struct {
            char** parameters;  // Lista de parámetros
            int param_count;    // Contador de parámetros
        } func;

        // Para clases, contiene la lista de métodos y atributos
        struct {
            char** methods;      // Métodos de la clase
            char** attributes;   // Atributos de la clase
            int method_count;    // Contador de métodos
            int attr_count;      // Contador de atributos
        } class;
    };
} Symbol;

// Estructura para la tabla de símbolos
typedef struct {
    Symbol symbols[MAX_SYMBOLS];
    int symbol_count;
} SymbolTable;

// Declarar la tabla de símbolos
//extern SymbolTable* symbol_table;

// Funciones públicas
void init_symbol_table(SymbolTable* table);
int add_symbol(SymbolTable* table, const char* name, const char* type, bool is_function, bool is_class, bool is_object, char** params, int param_count);
int find_symbol(SymbolTable* table, const char* name);
void print_symbol_table(SymbolTable* table);

#endif
