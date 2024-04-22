%code requires {
    // Importamos as librerías necesarias
    #include "definicions.h"
    #include "ts.h"
    #include "funcions.h"
}

%{
    #include "lexico.yy.h"

    // Función de erro sintáctico de bison
    void yyerror(const char *s);
%}

%union {
    Str cadena;
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
%precedence SIGNO

%type <numero> expr
%type <numero> asign
%type <numero> op
%type <numero> fn

%%

input:
    %empty {
        prompt(false);
    }
    | input linha
;

linha:
    '\n' {
        prompt(true);
    }
    | expr '\n' {
        if (!isnan($1)) {
            pres("%lf\n", $1);
        }
        prompt(true);
    }
    | error {
        prompt(false);
    }
;

expr:
    NUMERO {$$ = $1;}
    | ID {
        f64* v = ts_get($1);
        if (v) {
            $$ = *v;
        } else {
            yyerror("variable sin inicializar");
            $$ = NAN;
        }
        vec_free($1);
    }
    | asign
    | op
    | fn
    | '(' expr ')' {
        $$ = $2;
    }
;

asign:
    ID '=' expr {
        if (es_constante($1) && ts_get($1)) {
            yyerror("non se pode modificar unha constante");
            $$ = NAN;
        } else {
            ts_ins($1, $3);
            $$ = $3;
        }
    }
;

op:
    expr '+' expr {
        $$ = $1 + $3;
    }
    | expr '-' expr {
        $$ = $1 - $3;
    }
    | expr '*' expr {
        $$ = $1 * $3;
    }
    | expr '/' expr {
        if ($3 != 0) {
            $$ = $1 / $3;
        } else {
            yyerror("division por 0");
            $$ = NAN;
        }
    }
    | expr '%' expr {
        $$ = (i32)$1 % (i32)$3;
    }
    | '-' expr %prec SIGNO {
        $$ = -$2;
    }
    | '+' expr %prec SIGNO {
        $$ = $2;
    }
;

fn:
    ID '(' ')' {
        f64 (*f)() = fn_get($1, 0);
        if (f != NULL) {
            $$ = (*f)();
        } else {
            yyerror("funcion non recoñecida");
            $$ = NAN;
        }
        vec_free($1);
    }
    | ID '(' expr ')' { 
        f64 (*f)(f64) = fn_get($1, 1);
        if (f != NULL) {
            $$ = (*f)($3);
        } else {
            yyerror("funcion non recoñecida");
            $$ = NAN;
        }
        vec_free($1);
    }
    | ID '(' expr expr ')' {
        f64 (*f)(f64, f64) = fn_get($1, 2);
        if (f != NULL) {
            $$ = (*f)($3, $4);
        } else {
            yyerror("funcion non recoñecida");
            $$ = NAN;
        }
        vec_free($1);
    }
;

%%

void yyerror(const char *s) {
    err("erro sintáctico %s\n", s);
}
