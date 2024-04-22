%{
    // Importamos as librerías necesarias
    #include "definicions.h"
    #include "sintactico.tab.h"
    int fileno(FILE *stream);
%}

%option noinput nounput noyywrap yylineno

/* Definición das clases de expresións regulares */

LINHA                   \n
ESPAZO                  [ ]
SEPARADOR               (\r|\t|;|{ESPAZO})+

LETRA                   [a-zA-Z]
DIXITO                  [0-9]
ALNUM                   {LETRA}|{DIXITO}

ID                      ({LETRA}|_|:)({ALNUM}|_)*

INTEGER                 {DIXITO}+
FLOAT                   {DIXITO}*\.{DIXITO}+
FLOAT_EXP               ({INTEGER}|{FLOAT})[eE][\+\-]?{INTEGER}
NUMERO                  {INTEGER}|{FLOAT}|{FLOAT_EXP}

OPERADOR                [\+\-\*\/%^=,.!<>()]

%%

"q()" {
    yylex_destroy();
    yyterminate();
}

{SEPARADOR}
{OPERADOR}|{LINHA}      return yytext[0];

{ID} {
    yylval.cadena = str_(yytext);
    return(ID);
}

{NUMERO} {
    yylval.numero = atof(yytext);
    return(NUMERO);
}

. err("erro léxico na liña %d", yylineno);

<<EOF>> {
    yyterminate();
}
