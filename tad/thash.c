#include "thash.h"

// Función hash de 64 bits
//       @param key: Chave a hashear
u64 hash(HashKey key) {
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
bool equals(HashKey a, HashKey b) {
    return a.len == b.len && !memcmp(a.data, b.data, a.len);
}

// Modifica ou inserta unha chave no hashmap
HashValue* hash_ins(HashTree** m, HashKey key, Arena* a) {
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
    *m = new (a, HashTree, 1);
    (*m)->key = key;
    return &(*m)->value;
}
