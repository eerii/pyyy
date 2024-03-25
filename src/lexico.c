#include "lexico.h"
#include "estados/afd.h"
#include "estados/regex.h"
#include <sys/stat.h>

// Lista cos autómatas formales definidos e os seus índices
// A sintexe de expresións regulares que admitimos é moi moi básica
// Non hai precedencia de operadores así que é necesario utilizar parénteses
// Permitense alternancia (a|b), concatenacion (ab), e repeticións (a*, a+, a?)
// Tamén permítense as clases de caracteres letra (\w), dixito (\d), espazo (\s)
// e calqueira (\.), co requisito de escapar a barra
typedef enum {
    R_ESPAZO,
    R_STRING_DOUBLE,
    R_STRING_SINGLE,
    R_MULTISTRING,
    COUNT,
    PENDENTE
} Automata;
const char* regex[COUNT] = {
    "\\s",            // ESPAZO
    "\"\\'*\"",       // STRING DOUBLE
    "'\\\"*'",        // STRING SINGLE
    "\"\"\".*\"\"\"", // MULTISTRING
};

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
                if (res == R_ESPAZO) {
                    centinela_prev(c);
                } else {
                    dbg("automata completado: %d\n\n", res);
                }
                centinela_inicio(c);

                switch (res) {
                case R_STRING_DOUBLE || R_STRING_SINGLE || R_MULTISTRING:
                    return LITERAL;
                default:
                    break;
                };
            }
        }
    }

    return 0;
}
