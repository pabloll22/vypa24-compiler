  GNU nano 4.9                                                 src/lexer.l
%{
#include "ast.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

%}

%option debug

%%


"class"         return CLASS;
"super"         return SUPER;
"this"          return THIS;
"new"           return NEW;
"int"           { yylval.sval = strdup(yytext); return INT; }
"string"        { yylval.sval = strdup(yytext); return STRING; }
"if"            return IF;
"else"          return ELSE;
"while"         return WHILE;
"return"        return RETURN;
"void"          { yylval.sval = strdup(yytext); return VOID; }
"print"         return PRINT;
"readInt"       return READ_INT;
":"             return ':';
"."             return '.';
"<"             return '<';
"<="            return LE;
">"             return '>';
">="            return GE;
"=="            return EQ;
"!="            return NE;
"+"             return '+';
"-"             return '-';
"*"             return '*';
"/"             return '/';
"("             return '(';
")"             return ')';
"{"             return '{';
"}"             return '}';
";"             return ';';
","             return ',';
"="             return '=';

[0-9]+           { yylval.ival = atoi(yytext); return INTEGER_LITERAL; }
\"([^\"\\]|\\["nt\\])*\"  { yylval.sval = strdup(yytext); return STRING_LITERAL; }
[a-zA-Z_][a-zA-Z0-9_]* {yylval.sval = strdup(yytext);return IDENTIFIER;}
"/*"([^*]|\*+[^*/])*"\*/" { /* Ignorar los comentarios en bloque */ }
[ \t\n]          ; /* Ignorar espacios en blanco */
"//".*           ; /* Ignorar comentarios de una línea */
.                { fprintf(stderr, "Error léxico: '%s'\n", yytext); return yytext[0]; }

%%

