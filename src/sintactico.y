%code requires {
    // Importamos as librerías necesarias
    #include "definicions.h"
    #include "lexico.h"
    #include "ts.h"
}

%{
    #include "lexico.yy.h"
    #include <math.h>

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
        if (!ficheiro_aberto) {
            prompt(true);
        }
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
    | CARGAR ARQUIVO {
        info("executando arquivo %s\n", $2);
        $$ = NAN;
        arquivo_init($2.data);
        vec_free($2);
    }
;

asign:
    ID '=' expr {
        ts_ins($1, $3);
        $$ = $3;
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
    %empty {prompt(false);}
;

%%

void yyerror(const char *s) {
    err("erro sintáctico %s\n", s);
}
