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
    char* name;  // Variable name
    const char* type;
    bool defined;
    bool is_function;   // If it is a function
    bool is_class;      // If it is a class
    bool is_object;     // If it is an object (instance of a class)
    union {
        // For functions, it contains the parameters
        struct {
            char** parameters;  // Parameter list
            int param_count;    // Parameter counter
        } func;

        // For classes, it contains the list of methods and attributes
        struct {
            char** methods;      // Class methods
            char** attributes;   // Class attributes
            int method_count;    // Method counter
            int attr_count;      // Attributes counter
	    char* parentClass    //Father class name
        } class;
    };
} Symbol;

// Structure for the symbols table
typedef struct {
    Symbol symbols[MAX_SYMBOLS];
    int symbol_count;
} SymbolTable;

// Declare the symbols table
//extern SymbolTable* symbol_table;

// Public functions
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