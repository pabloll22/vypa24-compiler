#include <stdio.h>
#include "parser.h"

extern int yyparse();
extern int yydebug;

int main() {
    yydebug = 1; // Habilitar depuración
    printf("Parsing...\n");
    if (yyparse() == 0) {
        printf("Parsing completed successfully.\n");
    } else {
        printf("Parsing failed.\n");
    }
    return 0;
}
