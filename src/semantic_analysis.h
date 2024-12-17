#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include "symbol_table.h"
#include "ast.h"

const char* getNodeType(ASTNode* node, SymbolTable* symbolTable);
int performSemanticAnalysis(ASTNode* root, SymbolTable* symbolTable);
int check_variable_declaration(SymbolTable* table, const char* name);
int check_function_redefinition(SymbolTable* table, const char* name);
int check_types_compatibility(const char* type1, const char* type2, const char* operator);
int check_parameter_type(const char* expected_type, const char* actual_type);


#endif
