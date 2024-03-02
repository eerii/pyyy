// Autómatas finitos non determinados
// Utilidades para convertir expresións regulares en AFN
// Usa o algoritmo de construcción de Thomson
//
// Referencias:
//      - https://en.wikipedia.org/wiki/Thompson%27s_construction
//      - https://swtch.com/~rsc/regexp/regexp1.html

// Gramática
// letra = [a-zA-Z]
// dixito = [0-9]
// identificador = (letra|_)(letra|digito|_)*

#pragma once

#include "../tipos/vec.h"

// Caracteres especiais para marcar transicións
#define TRANS_NONE 1
#define TRANS_EPSILON 2
#define TRANS_LETRA 3 // TODO: Engadir soporte para clases
#define TRANS_DIXITO 4
typedef char Trans;

// Representa un estado do autómata
// Pode ter 0, 1 ou dúas transicións (marcadas por TRANS_NONE)
typedef struct Estado Estado;
struct Estado {
    Estado* to[2];
    Trans trans[2];
};

// Autómata finito non determinado
// Ten un estado inicial e un estado final
typedef struct {
    Estado *inicio, *fin;
} AFN;

// Vector de estados
typedef Vec(Estado*) VecEstado;

// O autómata finito máis pequeno
// Recoñece un caracter c e transiciona ó estado final
//      @param c Caracter
AFN afn_atomic(char c);

// Concatenación (ab)
//      @param a Primeiro estado
//      @param b Segundo estado
AFN afn_and(const AFN* a, const AFN* b);

// Alternancia (a|b)
//      @param a Primeiro estado
//      @param b Segundo estado
AFN afn_or(const AFN* a, const AFN* b);

// Kleene, cero ou mais (a*)
//      @param a Estado
AFN afn_cero_ou_mais(const AFN* a);

// Positivo, un ou mais (a+)
//      @param a Estado
AFN afn_un_ou_mais(const AFN* a);

// Opcional, cero ou un (a?)
//      @param a Estado
AFN afn_cero_ou_un(const AFN* a);

// Libera a memoria dun afn
//      @param a Autómata a liberar
void afn_free(AFN* a);

// Representa o autómata en formato graphviz
//      @param regex Expresión regular
//      @param a Autómata a representar
//      @param f Ficheiro no que gardar a representación
void afn_graph(const char* regex, const AFN* a, FILE* f);
