// Implementación dunha tabla hash con árbores trie
// Utiliza unha arena de memoria e shifting da función hash para
// crear e buscar nunha árbore con n=4
// Só implementa insertar, non fai falta borrar nada da tabla de símbolos
//
// Referencias:
//      - https://en.wikipedia.org/wiki/Hash_trie
//      - https://nrk.neocities.org/articles/hash-trees-and-tries
//      - https://nullprogram.com/blog/2022/08/08

#pragma once

#include "arena.h"

// Hashmap dinámico
// É recomendable facer un typedef co tipo que se vaia a utilizar para que o
// compilador entenda que múltiples instancias son do mesmo tipo e poder
// utilizalo como parámetro en funcións convencionais
// TODO: Probar a facer punteiros u32
#define _HashTree(K, V, N)                                                     \
    struct N {                                                                 \
        struct N* child[4];                                                    \
        K key;                                                                 \
        V value;                                                               \
    }

#define _HashTree_0(K, V) _HashTree(K, V, _Hash##K##V)

#define HashTree(K, V, ...)                                                    \
    __VA_OPT__(_HashTree(K, V, _Hash##__VA_ARGS))                              \
    IF_NOT(__VA_OPT__(1), _HashTree_0(K, V))

// Función hash para tipos de 64 bits
//      @param key: Chave a hashear
static inline u64 hash_64(u64 key) {
    key ^= key >> 33;
    key *= 0xff51afd7ed558ccdL;
    key ^= key >> 33;
    key *= 0xc4ceb9fe1a85ec53L;
    key ^= key >> 33;
    return key;
}

// Función hash para tipos de 32 bits (devolve 32 bits)
//      @param key: Chave a hashear
static inline u32 _hash_32(u32 key) {
    key = ~key + (key << 15);
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057;
    key = key ^ (key >> 16);
    return key;
}

// Función hash para tipos de 32 bits (devolve 64 bits)
//      @param key: Chave a hashear
static inline u64 hash_32(u32 key) {
    u32 h = _hash_32(key);
    return h + _hash_32(key ^ h);
}

// Función hash para caracteres
//      @param key: Chave a hashear
static inline u64 hash_8(u8 key) { return (u64)key; }

// Función hash xenérica
// Une as funcións hash de múltiples tipos nunha unica función
//      @param K: Chave a hashear
#define hash(K)                                                                \
    _Generic((K),                                                              \
        Str: hash_str,                                                         \
        i8: hash_8,                                                            \
        i32: hash_32,                                                          \
        i64: hash_64,                                                          \
        u8: hash_8,                                                            \
        u32: hash_32,                                                          \
        u64: hash_64)(K)

// Funcións de comparación para tipos básicos
#define _equals(T)                                                             \
    static inline bool equals_##T(T a, T b) { return a == b; }

_equals(i8);
_equals(i32);
_equals(i64);
_equals(u8);
_equals(u32);
_equals(u64);

// Función equals xenérica
// Une as funcións equals de múltiples tipos nunha unica función
//      @param A: Chave 1
//      @param B: Chave 2
#define equals(A, B)                                                           \
    _Generic((A),                                                              \
        Str: equals_str,                                                       \
        i8: equals_i8,                                                         \
        i32: equals_i32,                                                       \
        i64: equals_i64,                                                       \
        u8: equals_u8,                                                         \
        u32: equals_u32,                                                       \
        u64: equals_u64)(A, B)

// Modifica ou inserta unha chave no hashmap
// Se non se pasa arena, é un lookup simple e devolve NULL se non se atopa
//
// Xeramos un hash a partir da chave indicada
// Usamos dous bits da chave hash para elexir unha das catro ramas
// En cada nivel, dous bits son shifteados (trie of hash bits)
// Se atopa o elemento, devolve o valor, se non, sigue buscando
//
// Se non se proporciona unha arena e non se atopa un elemento, devolve NULL
// Se si hai, crea un novo hashmap usando a arena e asigna o valor da chave
//      @param H: Árbore hash
//      @param K: Chave a engadir ou modificar
//      @param A: Arena de memoria na que ubicar o hashmap (pode ser NULL)
#define hash_ins(H, K, A)                                                      \
    ({                                                                         \
        typeof(H)* hm = &H;                                                    \
        for (u64 h = hash(K); *hm; h <<= 2) {                                  \
            if (equals(K, (*hm)->key)) {                                       \
                break;                                                         \
            }                                                                  \
            hm = &(*hm)->child[h >> 62];                                       \
        }                                                                      \
        if (A != NULL && *hm == NULL) {                                        \
            *hm = arena_push_struct_zero(A, typeof(*H));                       \
            (*hm)->key = K;                                                    \
        }                                                                      \
        hm;                                                                    \
    })

// Percorre o hashmap executando unha función para cada elemento
//      @param H: Hashmap a percorrer
//      @param VAR: Variável auxiliar na que se garda o valor de cada elemento
//      @param DO: Código a executar
#define hash_for_each(H, VAR, DO)                                              \
    ({                                                                         \
        if (H) {                                                               \
            Vec(typeof(H)) stack;                                              \
            vec_init_res(stack, 4);                                            \
            vec_push(stack, H);                                                \
            while (stack.len > 0) {                                            \
                auto VAR = *vec_pop(stack);                                    \
                for (u8 i = 0; i < 4; i++) {                                   \
                    if (VAR->child[i]) {                                       \
                        vec_push(stack, VAR->child[i]);                        \
                    }                                                          \
                }                                                              \
                DO;                                                            \
            }                                                                  \
            vec_free(stack);                                                   \
        }                                                                      \
    })
