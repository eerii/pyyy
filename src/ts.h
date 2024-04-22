// Abstracción da tábla de símbolos
// Utiliza hash tree por debaixo

#pragma once

#include "tipos/hash.h"
#include "tipos/str.h"

typedef HashTree(Str, f64) TaboaSimbolos;

// Taboa de símbolos global
extern TaboaSimbolos* ts;

// Inserta un par valor/chave na taboa de símbolos se non existe
//      @param key: Chave a insertar
//      @param val: Valor a insertar
static inline void ts_ins(Str key, f64 val) {
    TaboaSimbolos* t = *hash_ins(ts, key, NULL);

    if (!t) {
        t = *hash_ins(ts, key, &arena);
        t->value = val;
    }
}

// Imprime a táboa de símbolos (so en debug)
static inline void ts_print() {
#ifdef DEBUG
    dbg("-------ts-------\n");
    hash_for_each(ts, h, {
        if (h->value == ID || (h->value >= 300 && h->value < 450))
            dbg("%s %d\n", h->key.data, h->value);
    });
    dbg("----------------\n\n");
#endif
}
