%code requires {
    // Importamos as librerías necesarias
    #include "definicions.h"
    #include "lexico.h"
}

%{
    #include "lexico.yy.h"
    // Función de erro sintáctico de bison
    void yyerror(const char *s);
%}

%union {
    char* cadena;
    f64 numero;
};

%start input

%token <numero> NUMERO
%token <cadena> ID
%token <cadena> ARQUIVO

%token CARGAR
%token AXUDA

%precedence '='
%left '+' '-'
%left '*' '/'
%left '%'
%right '^'
%precedence NNN

%type <numero> expr
%type <numero> asign
%type <numero> op
%type <numero> fn

%%

input:
    %empty {printf(">> ");}
    | input linha
;

linha:
    '\n' {printf("\n>> ");}
;

expr:
    NUMERO {$$ = $1;}
;

asign:
    %empty {printf(">> ");}
;

op:
    %empty {printf(">> ");}
;

fn:
    %empty {printf(">> ");}
;

%%

void yyerror(const char *s) {
    err("erro sintáctico %s\n", s);
}
