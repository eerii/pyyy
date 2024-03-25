// Analizador léxico

#include "lexico.h"
#include "estados/afd.h"
#include "estados/regex.h"
#include "ts.h"
#include <sys/stat.h>

// No futuro:
//      - Liñas físicas vs liñas lóxicas (poden ser varias, emiten NEWLINE)
//      - Incluír INDENT / DEDENT usando scopes con subareas

// Lista cos autómatas formales definidos e os seus índices
// A sintexe de expresións regulares que admitimos é moi moi básica
// Non hai precedencia de operadores así que é necesario utilizar parénteses
// Permitense alternancia (a|b), concatenacion (ab), e repeticións (a*, a+, a?)
// Tamén permítense as clases de caracteres (ver definicions.h)
typedef enum {
    R_ESPAZO,
    R_STRING_DOUBLE,
    R_STRING_SINGLE,
    R_STRING_MULTI,
    R_IDENTIFICADORES,
    R_INTEGER,
    R_INTEGER_BASE,
    R_FLOAT_FRAC,
    R_FLOAT_DOT,
    R_FLOAT_EXP,
    R_DELIMITADORES,
    R_OPERADORES,
    COUNT,
    PENDENTE
} Automata;

const char* regex[COUNT] = {
    "\\s",                                              // ESPAZO
    "\"\\\"*\"",                                        // STRING DOUBLE
    "'\\'*'",                                           // STRING SINGLE
    "\"\"\".*\"\"\"",                                   // STRING_MULTI
    "(\\w|_)(\\w|\\d|_)*",                              // IDENTIFICADRES
    "(\\D\\d*)|0+",                                     // INTEGER
    "0(((x|X)\\x+)|((o|O)\\o+)|((b|B)(0|1)+))",         // INTEGER_BASE
    "(\\d*\\.\\d+)",                                    // FLOAT_FRAC
    "((\\d+)\\.)",                                      // FLOAT_DOT
    "(\\d+)(e|E)(\\+|-)?\\d+",                          // FLOAT_EXP
    "(\\(|\\)|[|]|{|}|,|:|\\.|;|@|(\\+?=))",            // DELIMITADORES
    "(\\+|-|(\\*\\*?)|/|%|((!|<|>|=)=)|>|<|&|\\||^|~)", // OPERADORES
};

#ifdef DEBUG
const char* regex_nomes[COUNT] = {
    "espazo",        "string (double)", "string (single)", "string (multi)",
    "identificador", "integer",         "integer (base)",  "float (frac)",
    "float (dot)",   "float (exp)",     "delimitador",     "operador",
};
#endif

// Delimitadores que hai que devolver ó sistema de entrada
const char delim[] = {'(', ')', '[', ']', '{', '}', ',', '"',  '\'',
                      ':', '.', ';', '@', '=', ' ', '!', '\n', '\t',
                      '+', '-', '*', '<', '>', '/', '%', '&'};

// Lista onde se gardarán os autómatas construídos
typedef struct {
    AFD afd;
    EstadoAFD* actual;
} ExecAFD;

ExecAFD automatas[COUNT];
i32 ultimo_aceptado;

// Comproba se estamos nun comentario de liña (ignora o resto da liña fisica)
bool comentario_linha = false;

// -----------------
// Funcións internas
// -----------------

// Fai unha iteración dun caracter a través de tódolos autómatas
//      @param ch: Caracter a iterar
//      @return: Se hai aceptación, índice do primeiro autómata que acepta
//               Se non, devolve -1
i32 _percorrer_automatas(Trans ch) {
    i32 non_aceptados = 0;
    for (Automata i = 0; i < COUNT; ++i) {
        ExecAFD* e = &automatas[i];
        if (!e->actual) {
            non_aceptados++;
            continue;
        }

        e->actual = afd_delta(&e->afd, e->actual, ch);

        if (!e->actual) {
            non_aceptados++;
        } else if (e->actual->final) {
            ultimo_aceptado = i;
        }
    }
    if (non_aceptados == COUNT) {
        return ultimo_aceptado;
    }
    return PENDENTE;
}

// Restaura o estado dos autómatas
void _reset_automatas() {
    for (Automata i = 0; i < COUNT; ++i) {
        automatas[i].actual = &automatas[i].afd.data[0];
    }
    ultimo_aceptado = INT8_MIN;
}

// Comproba se o caracter é un delimitador
bool _es_delim(char ch) {
    for (u32 i = 0; i < sizeof(delim); ++i) {
        if (ch == delim[i]) {
            return true;
        }
    }
    return false;
}

// Comproba se o resultado emite un lexema
bool _emite_lexema(i32 res) {
    return res != R_ESPAZO && res != R_DELIMITADORES;
}

// ---
// API
// ---

// Inicialización dos autómatas a partir de regex ó comezo do programa
void automatas_init() {
    for (Automata i = 0; i < COUNT; ++i) {
        AFN afn = regex_to_afn(regex[i]);
        automatas[i].afd = afn_to_afd(&afn);

#ifdef DEBUG
        char out[16];
        sprintf(out, "afd/%d.dot", i);
        mkdir("afd", 0755);
        FILE* f = fopen(out, "w");
        afd_graph(&automatas[i].afd, f);
        fclose(f);
#endif
        afn_free(&afn);
    }
    _reset_automatas();
}

// Análise léxico
// Devolve o seguinte lexema detectado
Lexema seguinte_lexico() {
    while (true) {
        char ch = entrada_ler();

        // Comprobar fin de arquivo
        if (ch == EOF) {
            return (Lexema){.codigo = EOF};
        }

        // Comprobar comentario
        if (ch == '\n' || ch == '\r') {
            comentario_linha = false;
        }
        if (comentario_linha || ch == '#') {
            comentario_linha = true;
            entrada_inicio();
            continue;
        }

        // Facemos a transición en todos os autómatas
        i32 res = _percorrer_automatas(ch);

        // Se non quedan autómatas por rematar
        if (res != PENDENTE) {
            _reset_automatas();
            if (res == INT8_MIN) {
                err("lexico: non se recoñeceu ningun autómata\n");
            } else {
                dbg("aceptado: %s\n", regex_nomes[res]);

                // Obtenmos a chave dende o sistema de entrada
                Str key;
                entrada_str(&key);
                Lexema l = {.codigo = 0, .valor = NULL};

                // Se este autómata emite lexemas, calculamos o código e
                // comprobamos se xa existe na taboa de símbolos Se non existe,
                // o engadimos. En ambos casos cheamos o lexema ca información
                // necesaria
                if (_emite_lexema(res)) {
                    u16 tipo = 0;
                    switch (res) {
                    case R_IDENTIFICADORES:
                        tipo = ID;
                        break;
                    case R_STRING_DOUBLE:
                    case R_STRING_SINGLE:
                    case R_STRING_MULTI:
                    case R_INTEGER:
                    case R_INTEGER_BASE:
                    case R_FLOAT_DOT:
                    case R_FLOAT_FRAC:
                    case R_FLOAT_EXP:
                        tipo = LITERAL;
                        break;
                    case R_OPERADORES:
                        if (ch == '=') {
                            switch (key.data[0]) {
                            case '=':
                                tipo = OP_EQ;
                                break;
                            case '!':
                                tipo = OP_NEQ;
                                break;
                            case '<':
                                tipo = OP_LEQ;
                                break;
                            case '>':
                                tipo = OP_GEQ;
                                break;
                            default:
                                err("lexico: operador .= incorrecto");
                            }
                        } else if (ch == '*') {
                            if (key.len == 1) {
                                tipo = '*';
                            } else if (key.data[0] == '*') {
                                tipo = OP_STAR_STAR;
                            } else {
                                err("lexico: operador ** incorrecto");
                            }
                        } else {
                            tipo = ch;
                        }
                    };

                    ts_ins(key, tipo, &l);
                }

                // Colocamos o punteiro de inicio onde está o actual
                entrada_inicio();

                // Devolvemos o caracter ó sistema de entrada se é necesario
                if (res == R_ESPAZO || _es_delim(ch)) {
                    entrada_prev();
                }

                // Se emite un lexema, devolvelo
                if (_emite_lexema(res)) {
                    return l;
                }
            }
        }
    }
}
