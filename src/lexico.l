%{
    // Importamos as librerías necesarias
    #include "definicions.h"
    #include "lexico.h"
    #include "sintactico.tab.h"
    int fileno(FILE *stream);
%}

%option noinput nounput noyywrap yylineno

/* Definición das clases de expresións regulares */

LINHA                   \n
ESPAZO                  [ ]
SEPARADOR               (\r|\t|{ESPAZO})+

LETRA                   [a-zA-Z]
DIXITO                  [0-9]
ALNUM                   {LETRA}|{DIXITO}

ID                      ({LETRA}|_)({ALNUM}|_)*

INTEGER                 {DIXITO}+
FLOAT                   {DIXITO}*\.{DIXITO}+
FLOAT_EXP               ({INTEGER}|{FLOAT})[eE][\+\-]?{INTEGER}
NUMERO                  {INTEGER}|{FLOAT}|{FLOAT_EXP}

OPERADOR                [\+\-\*\/%^=,.!<>();]

ARQUIVO                 {ID}+(\.{ALNUM}+)?
%x                      LER

%%

":q" {
    arquivo_close();
    yylex_destroy();
    yyterminate();
}

":l" {
    BEGIN(LER);
    return(CARGAR);
}


<LER>{ARQUIVO} {
    /*yylval.cadena = (char *)malloc(strlen(yytext + 1));
    strcpy(yylval.cadena, yytext);
    yylval.cadena[strlen(yytext)] = '\0';
    BEGIN(INITIAL);
    return(ARCHIVO);*/
}

{SEPARADOR}
{OPERADOR}|{LINHA}      return yytext[0];

{ID} {
    /*yylval.cadena = (char*)malloc(strlen(yytext + 1) * sizeof(char));
    strcpy(yylval.cadena, yytext);
    yylval.cadena[strlen(yytext)] = '\0';
    return(ID);*/
}

{NUMERO} {
    /*yylval.numero = atof(yytext);
    return(NUMERO);*/
}

. err("erro léxico na liña %d", yylineno);

<<EOF>> {
    yyterminate();
}

%%

bool ficheiro_aberto = false;

// Abre o arquivo indicado
void arquivo_init(const char* nome) {
    ficheiro_aberto = (yyin = fopen(nome, "r")) != NULL;
    if(!ficheiro_aberto) {
        err("non se puido abrir o ficheiro\n\n");
        yyin = stdin;
    }
}

// Cerra o arquivo do sistema de entrada
void arquivo_close() {
    if (ficheiro_aberto) {
        fclose(yyin);
        yyrestart(stdin);
        ficheiro_aberto = false;
    }
}
