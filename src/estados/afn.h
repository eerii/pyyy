// Autómatas finitos non determinados
// Utilidades para convertir expresións regulares en AFN
// Usa o algoritmo de construcción de Thomson
//
// Referencias:
//      - https://en.wikipedia.org/wiki/Thompson%27s_construction

// Gramática
// letra = [a-zA-Z]
// digito = [0-9]
// identificador = (letra|_)(letra|digito|_)*

// TODO: Multiples estados finais?

// TODO: Añadir descripcions

#pragma once

#include "../tipos/vec.h"

#define TRANS_NONE 1
#define TRANS_EPSILON 2
typedef char Trans;

typedef struct Estado Estado;
struct Estado {
    Estado* to[2];
    Trans trans[2];
};

typedef struct {
    Estado *inicio, *fin;
} AFN;

typedef Vec(Estado*) VecEstado;

AFN afn_atomic(char c);

AFN afn_and(const AFN* a, const AFN* b); // ab
AFN afn_or(const AFN* a, const AFN* b);  // a|b
AFN afn_cero_ou_mais(const AFN* a);      // a*
AFN afn_cero_ou_un(const AFN* a);        // a?
AFN afn_un_ou_mais(const AFN* a);        // a+
void afn_del(AFN* a);

void afn_delta(const Estado* e, const char c, VecEstado* v);

void afn_graphviz(const char* regex, const AFN* a);
