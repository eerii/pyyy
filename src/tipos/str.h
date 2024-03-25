// Implementación dun string variable facendo uso do vector dinámico
// É unha especificación que permite todos os metodos vec_
// Tamén engade as funcións específicas str_ para facilitar o traballo con
// strings e o caracter de terminación \0

#pragma once

#include "vec.h"

// Un string é simplemente un vector de caracteres
// Este typedef é importante xa que permite ter un tipo concreto que especificar
// nos parámetros das funcións
typedef Vec(char) Str;

// Engade un caracter ó final do string, preservando o caracter de terminación
//      @param s: String no que engadir
//      @param c: Caracter a engadir
static inline void str_push(Str* s, char c) {
    // Inserta antes do último caracter (\0)
    vec_ins((*s), s->len - 2, c);
}

// Función hash de 64 bits para strings
//       @param key: Chave a hashear
static inline u64 hash_str(Str key) {
    u64 h = 0x100;
    for (u64 i = 0; i < key.len; i++) {
        h ^= key.data[i];
        h *= 1111111111111111111u;
    }
    return h;
}

// Compara duas chaves
//       @param a: Chave 1
//       @param b: Chave 2
static inline bool equals_str(Str a, Str b) {
    return a.len == b.len && !memcmp(a.data, b.data, a.len);
}

// Xunta dous strings correctamente
//      @param S: String no que engadir
//      @param X: String a engadir
#define str_append(S, X) (vec_pop(S), vec_append(S, X))

// Crea un Str dende un literal en linea
// Non reserva memoria dinámica se non se cambia o tamaño
//      @param l: Literal string
#define str(l)                                                                 \
    (Str) { .len = (sizeof(l) / sizeof(l[0])), .data = (l) }
