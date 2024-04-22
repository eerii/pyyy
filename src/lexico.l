%{
    // Importamos as librerías necesarias
    #include "lexico.h"
    #include "ts.h"
    int fileno(FILE *stream);
%}

%array
%option yylineno

/* Definición das clases de expresións regulares */
/* Baseadas na documentación de https://docs.python.org/3/reference/lexical_analysis.html */

LINHA                   \n
ESPAZO                  [ \t]

LETRA                   [a-zA-Z]
DIXITO                  [0-9]
DIXITO_OCT              [0-7]
DIXITO_HEX              [0-9a-fA-F]
DIXITO_NON_CERO         [1-9]
ALNUM                   {LETRA}|{DIXITO}

ENCODING                #.*coding[=:]{ESPAZO}*({ALNUM}|-)+.*
COMENTARIO              #.*

STR_CHAR_SHORT_SINGLE   [^\\\n']
STR_CHAR_SHORT_DOUBLE   [^\\\n"]
STR_CHAR_LONG_SINGLE    [^\\"]
STR_CHAR_LONG_DOUBLE    [^\\"]

STR_SHORT               ('{STR_CHAR_SHORT_SINGLE}*')|(["]{STR_CHAR_SHORT_DOUBLE}*["])
STR_LONG                ([']{3}{STR_CHAR_LONG_SINGLE}*['])|(["]{3}{STR_CHAR_LONG_DOUBLE}*["]{3})
STRING_LITERAL          {STR_SHORT}|{STR_LONG}

INTEGER_DEC             (0+(_?0)*)|({DIXITO_NON_CERO}(_?{DIXITO})*)
INTEGER_BIN             0[bB](_?[01])+
INTEGER_OCT             0[oO](_?{DIXITO_OCT})+
INTEGER_HEX             0[xX](_?{DIXITO_HEX})+
INTEGER_LITERAL         {INTEGER_DEC}|{INTEGER_BIN}|{INTEGER_OCT}|{INTEGER_HEX}

FLOAT_DIXITOS           {DIXITO}(_?{DIXITO})*
FLOAT_PUNTO             ({FLOAT_DIXITOS}?\.{FLOAT_DIXITOS})|({FLOAT_DIXITOS}\.)
FLOAT_EXP               ({FLOAT_DIXITOS}|{FLOAT_PUNTO})[eE][\+\-]?{FLOAT_DIXITOS}
FLOAT_LITERAL           {FLOAT_PUNTO}|{FLOAT_EXP}

ID                      ({LETRA}|_)({ALNUM}|_)*

OPERADOR                [\+\-\*\/%@&\|\^~<>]
DELIMITADOR             [\(\)\[\]\{\},:\.;=]
OP_SOME_EQ              [\+\-\*\/=!<>]=
OP_STAR_STAR            \*\*

%%

{LINHA}
{ESPAZO}
{ENCODING}                                          { info("encoding: %s\n\n", yytext); }
{COMENTARIO}

{STRING_LITERAL}|{INTEGER_LITERAL}|{FLOAT_LITERAL}  return LITERAL;
{ID}                                                return ID;
{OPERADOR}|{DELIMITADOR}                            return yytext[0];
{OP_SOME_EQ}                                        return OP_SOME_EQ;
{OP_STAR_STAR}                                      return OP_STAR_STAR;

<<EOF>>                                             { yyterminate(); }

%%

// Inicializa o sistema de entrada co arquivo indicado
void entrada_init(const char* nome){
    if((yyin = fopen(nome, "r")) == NULL) {
        err("non se puido inicializar o sistema de entrada\n\n");
        exit(1);
    }
}

// Cerra o arquivo do sistema de entrada
void entrada_close(){
    fclose(yyin);
    yylex_destroy();
}

// Arrega os operadores dobres
void arregla_op_dobre(u32* codigo, char primeiro) {
    switch (primeiro) {
        case '=':
            *codigo = OP_EQ;
            break;
        case '!':
            *codigo = OP_NEQ;
            break;
        case '<':
            *codigo = OP_LEQ;
            break;
        case '>':
            *codigo = OP_GEQ;
            break;
        case '+':
            *codigo = OP_PLUS_EQ;
            break;
        case '-':
            *codigo = OP_MINUS_EQ;
            break;
        case '*':
            *codigo = OP_STAR_EQ;
            break;
        case '/':
            *codigo = OP_DIV_EQ;
            break;
    }
}

// Devolve o seguinte lexema no analizador léxico
Lexema seguinte_lexico(){
    Lexema l = {.codigo = 0, .valor = NULL};
    while ((l.codigo = yylex()) == -1) {
        err("error léxico na liña %d: %s\n", yylineno, yytext);
    };

    Str key = vec_new_from_n(Str, yytext, yyleng);
    vec_push(key, '\0');
    vec_make_dyn(key);

    if (l.codigo == OP_SOME_EQ) {
        arregla_op_dobre(&l.codigo, key.data[0]);
    }

    ts_ins(key, l.codigo, &l);

    return l;
}
