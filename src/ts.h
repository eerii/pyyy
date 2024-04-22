// Abstracción da tábla de símbolos
// Utiliza hash tree por debaixo

#pragma once

#include "tipos/hash.h"
#include <ctype.h>

typedef HashTree(Str, f64) TaboaSimbolos;

// Taboa de símbolos global
extern TaboaSimbolos* ts;

// Devolve se unha variable é constante (todo maiusculas)
static inline bool es_constante(Str key) {
    for (int i = 0; i < key.len; i++) {
        if (islower(key.data[i]))
            return false;
    }
    return true;
}

// Obtén un valor da táboa de símbolos se existe
// Se non devolve null
//      @param key: Chave a buscar
static inline f64* ts_get(Str key) {
    TaboaSimbolos* t = *hash_ins(ts, key, NULL);
    return t ? &(t->value) : NULL;
}

// Inserta un par valor/chave na taboa de símbolos se non existe
// Se existe o actualiza
//      @param key: Chave a insertar
//      @param val: Valor a insertar
static inline void ts_ins(Str key, f64 val) {
    TaboaSimbolos* t = *hash_ins(ts, key, &arena);
    t->value = val;
}

// Imprime a táboa de símbolos (so en debug)
static inline void ts_print() {
    pres("\n%s\n", "-------ts-------");
    hash_for_each(ts, h, { pres("%s %lf\n", h->key.data, h->value); });
    pres("%s\n\n", "----------------");
}

static inline void ts_init() {
    ts_ins(str("PI"), 3.14159265358979323846);
    ts_ins(str("TAU"), 6.28318530717958647692);
    ts_ins(str("SQRT2"), 1.41421356237309504880);
    ts_ins(str("PHI"), 1.61803398874989484820);
    ts_ins(str("E"), 2.71828182845904523536);
}
