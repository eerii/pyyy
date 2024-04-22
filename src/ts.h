// Abstracción da tábla de símbolos
// Utiliza hash tree por debaixo

#pragma once

#include "tipos/hash.h"

typedef HashTree(Str, f64) TaboaSimbolos;

// Taboa de símbolos global
extern TaboaSimbolos* ts;

// Obtén un valor da táboa de símbolos se existe
// Se non devolve null
//      @param key: Chave a buscar
static inline f64* ts_get(Str key) {
    TaboaSimbolos* t = *hash_ins(ts, key, NULL);
    dbg("%s %p %p %d\n", key.data, t, t ? &(t->value) : NULL, t ? t->value : 0);
    return t ? &(t->value) : NULL;
}

// Inserta un par valor/chave na taboa de símbolos se non existe
//      @param key: Chave a insertar
//      @param val: Valor a insertar
static inline void ts_ins(Str key, f64 val) {
    if (ts_get(key)) {
        return;
    }
    TaboaSimbolos* t = *hash_ins(ts, key, &arena);
    t->value = val;
}

// Imprime a táboa de símbolos (so en debug)
static inline void ts_print() {
#ifdef DEBUG
    dbg("-------ts-------\n");
    hash_for_each(ts, h, { dbg("%s %d\n", h->key.data, h->value); });
    dbg("----------------\n\n");
#endif
}
