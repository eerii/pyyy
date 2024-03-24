#include "lexico.h"
#include "estados/afd.h"
#include "estados/regex.h"
#include <sys/stat.h>

// Lista cos autómatas formales definidos e os seus índices
typedef enum { AB, AA, COUNT, PENDIENTE, NINGUNO } Automata;
const char* regex[] = {
    "ab",
    "im((port)|(part))(ant)*",
};

// Lista onde se gardarán os autómatas construídos
typedef struct {
    AFD afd;
    EstadoAFD* actual;
} ExecAFD;

ExecAFD automatas[COUNT];

// Comproba se estamos nun comentario de liña (ignora o resto da liña fisica)
bool comentario_linha = false;

// -----------------
// Funcións internas
// -----------------

// Fai unha iteración dun caracter a través de tódolos autómatas
//      @param ch: Caracter a iterar
//      @return: Se hai aceptación, índice do primeiro autómata que acepta
//               Se non, devolve -1
u32 _percorrer_automatas(char ch) {
    u32 no_aceptados = 0;
    for (Automata i = 0; i < COUNT; ++i) {
        ExecAFD* e = &automatas[i];
        if (!e->actual) {
            no_aceptados++;
            continue;
        }

        e->actual = afd_delta(&e->afd, e->actual, ch);

        if (!e->actual) {
            no_aceptados++;
        } else if (e->actual->final) {
            return i;
        }
    }
    return no_aceptados == COUNT ? NINGUNO : PENDIENTE;
}

// Restaura o estado dos autómatas
void _reset_automatas() {
    for (Automata i = 0; i < COUNT; ++i) {
        automatas[i].actual = &automatas[i].afd.data[0];
    }
}

// ---
// API
// ---

// Inicialización dos autómatas a partir de regex ó comezo do programa
void automatas_init() {
    for (Automata i = 0; i < COUNT; ++i) {
        AFN afn = regex_to_afn(regex[i]);
        ExecAFD e = {.afd = afn_to_afd(&afn), .actual = &e.afd.data[0]};
        assert(&e.afd.data[0] == e.actual);
        automatas[i] = e;

#ifdef DEBUG
        char out[16];
        sprintf(out, "afd/%d.dot", i);
        mkdir("afd", 0755);
        FILE* f = fopen(out, "w");
        afd_graph(&e.afd, f);
        fclose(f);
#endif
        afn_free(&afn);
    }
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
        return EOF;

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

        u32 res = _percorrer_automatas(ch);
        if (res != PENDIENTE) {
            _reset_automatas();
            if (res != NINGUNO) {
                info("automata completado: %d\n", res);
                centinela_inicio(c);
                return res;
            }
        }
    }

    return 0;
}
