#include "lexico.h"
#include "estados/afd.h"
#include "estados/regex.h"
#include "ts.h"
#include <sys/stat.h>

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
    R_DELIMITADORES_IGUAL,
    R_OPERADORES,
    COUNT,
    PENDENTE
} Automata;
const char* regex[COUNT] = {
    "\\s",                                      // ESPAZO
    "\"\\'*\"",                                 // STRING DOUBLE
    "'\\\"*'",                                  // STRING SINGLE
    "\"\"\".*\"\"\"",                           // STRING_MULTI
    "(\\w|_)(\\w|\\d|_)*",                      // IDENTIFICADRES
    "(\\D\\d*)|0+",                             // INTEGER
    "0(((x|X)\\x+)|((o|O)\\o+)|((b|B)(0|1)+))", // INTEGER_BASE
    "(\\d*\\.\\d+)",                            // FLOAT_FRAC
    "((\\d+)\\.)",                              // FLOAT_DOT
    "(\\d+)(e|E)(\\+|-)?\\d*",                  // FLOAT_EXP
    "(\\(|\\)|[|]|{|}|,|:|\\.|;|@)",            // DELIMITADORES
    "\\+?=",                                    // DELIMITADORES_IGUAL
    "(\\+|-|(\\*\\*?)|/|%|@|(<<?)|(>>?)|&|\\||^|~|((:|=|!|>|<)=))", // OPERADORES

};

const char delim[] = {'(', ')', '[', ']', '{', '}',  ',',
                      ':', '.', ';', '@', ' ', '\n', '\t'};

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
u32 seguinte_lexico(Centinela* c) {
    // TODO: Funcións de expresións regulares
    //      - Identificador alfanumérico
    //      - Número
    //      - Operadores ...
    //      Mirar se podo usar regex en c

    while (true) {
        // return EOF;

        char ch = centinela_ler(c);

        // Comprobar fin de arquivo
        if (ch == EOF) {
            return ch;
        }

        // Comprobar comentario
        if (ch == '\n' || ch == '\r') {
            comentario_linha = false;
        }
        if (comentario_linha || ch == '#') {
            comentario_linha = true;
            centinela_inicio(c);
            continue;
        }

        i32 res = _percorrer_automatas(ch);
        if (res != PENDENTE) {
            _reset_automatas();
            if (res == INT8_MIN) {
                err("erro de sintaxe: non se recoñeceu ningun autómata\n");
            } else {
                if (res == R_ESPAZO) { // TODO: || _es_delim(ch)) {
                    centinela_prev(c);
                }
                dbg("automata completado: %d\n\n", res);
                centinela_inicio(c);

                switch (res) {
                case R_IDENTIFICADORES:
                    Str id = centinela_str(c);
                    if (ts_contains(id)) {
                        info("%s\n", id.data);
                    } else {
                        // ts_ins(id, ID);
                    }
                    return ID;
                case R_STRING_DOUBLE:
                case R_STRING_SINGLE:
                case R_STRING_MULTI:
                    return LITERAL;
                case R_INTEGER:
                case R_INTEGER_BASE:
                case R_FLOAT_DOT:
                case R_FLOAT_FRAC:
                case R_FLOAT_EXP:
                    return LITERAL;
                case R_OPERADORES:
                // TODO: Comprobar dobles
                default:
                    break;
                };
            }
        }
    }

    return 0;
}
