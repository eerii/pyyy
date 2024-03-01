// Implementación dunha tabla hash con árbores trie
// Utiliza unha arena de memoria e shifting da función hash para
// crear e buscar nunha árbore con n=4
// Só implementa insertar, non fai falta borrar nada da tabla de símbolos
//
// Referencias:
//      - https://en.wikipedia.org/wiki/Hash_trie
//      - https://nrk.neocities.org/articles/hash-trees-and-tries
//      - https://nullprogram.com/blog/2022/08/08
//
// @author: @eerii, jose.pazos.perez@rai.usc.es

#pragma once

#include "arena.h"
#include "str.h"

// Tipos do hashmap
//      Nota: nesta caso non compensa facelo xenérico coma o vector xa que
//      só vamos a utilizar esta estructura para a táboa de símbolos
typedef Str HashKey;
typedef u32 HashValue;

typedef struct HashTree HashTree;
struct HashTree {
    HashTree* child[4];
    HashKey key;
    HashValue value;
};

// Función hash de 64 bits
//       @param key: Chave a hashear
static inline u64 hash(HashKey key) {
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
static inline bool equals(HashKey a, HashKey b) {
    return a.len == b.len && !memcmp(a.data, b.data, a.len);
}

// Modifica ou inserta unha chave no hashmap
// Se non se pasa arena, é un lookup simple e devolve NULL se non se atopa
//      @param m: Árbore hash
//      @param key: Chave a engadir ou modificar
//      @param a: Arena de memoria na que ubicar o hashmap (pode ser NULL)
static inline HashValue* hash_ins(HashTree** m, HashKey key, Arena* a) {
    // Xeramos un hash a partir da chave indicada
    // Usamos dous bits da chave hash para elexir unha das catro ramas
    // En cada nivel, dous bits son shifteados (trie of hash bits)
    for (u64 h = hash(key); *m; h <<= 2) {
        // Se atopa o elemento, devolve o valor
        if (equals(key, (*m)->key)) {
            return &(*m)->value;
        }
        // Se non, continua buscando
        m = &(*m)->child[h >> 62];
    }

    // Se non se proporciona unha arena e non se atopa un elemento, devolve NULL
    if (!a) {
        return NULL;
    }

    // Crea un novo hashmap usando a arena e asigna o valor da chave
    *m = arena_push_struct(a, HashTree);
    (*m)->key = key;
    return &(*m)->value;
}
