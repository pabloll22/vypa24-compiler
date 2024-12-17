#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#define MAX_SYMBOLS 100
#define MAX_ATTRIBUTES 1000
#define MAX_METHODS 100

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
	    char* parentClass    //Nombre de la clase padre
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
int add_symbol(SymbolTable* table, const char* name, const char* type, bool is_function, bool is_class, bool is_object, char** params, int param_count,const char* parentClass, char** attributes, int attr_count, char** methods, int method_count);
int find_symbol(SymbolTable* table, const char* name);
void print_symbol_table(SymbolTable* table);
void free_symbol_table(SymbolTable* table);
char** extractAttributesFromClassBody(ASTNode* class_body);
char** extractMethodsFromClassBody(ASTNode* class_body);
int countAttributes(ASTNode* class_body);
int countMethods(ASTNode* class_body);
const char* getMemberType(const char* className, const char* memberName, SymbolTable* symbolTable);

#endif
