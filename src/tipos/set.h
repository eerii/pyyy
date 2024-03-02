// Implementación de hash set para unha lista de estados
// É unha copia de hash.h sen ter un valor asociado, só con chave

#pragma once

#include "arena.h"
#include "vec.h"

// Tipos do set
typedef struct Estado Estado;
typedef const Estado* SetKey;

// Set
typedef struct Set Set;
struct Set {
    Set* child[4];
    SetKey key;
};

static const u64 a1 = 0x65d200ce55b19ad8u;
static const u64 b1 = 0x4f2162926e40c299u;
static const u64 c1 = 0x162dd799029970f8u;
static const u64 a2 = 0x68b665e6872bd1f4u;
static const u64 b2 = 0xb6cfcf9d79b51db2u;
static const u64 c2 = 0x7a2b92ae912898c2u;

// Función hash para números de 64 bits
//       @param key: Chave a hashear
static inline u64 hash_64(u64 key) {
    u32 low = (u32)key;
    u32 high = (u32)(key >> 32);
    return ((a1 * low + b1 * high + c1) >> 32) |
           ((a2 * low + b2 * high + c2) & 0xFFFFFFFF00000000u);
}

// Modifica ou inserta unha chave no set
// Se non se pasa arena, é un lookup simple
//      @param m: Árbore hash
//      @param key: Chave a engadir ou modificar
//      @param a: Arena de memoria na que ubicar o set (pode ser NULL)
static inline bool set_ins(Set** m, SetKey key, Arena* a) {
    // Xeramos un hash a partir da chave indicada
    // Usamos dous bits da chave hash para elexir unha das catro ramas
    // En cada nivel, dous bits son shifteados (trie of hash bits)
    for (u64 h = hash_64((u64)key); *m; h <<= 2) {
        // Se atopa o elemento, devolve verdadeiro
        if (key == (*m)->key) {
            return true;
        }
        // Se non, continua buscando
        m = &(*m)->child[h >> 62];
    }

    // Se non se proporciona unha arena e non se atopa un elemento, falso
    if (!a) {
        return false;
    }

    // Crea un novo set usando a arena e asigna o valor da chave
    *m = (Set*)arena_push_zero(a, sizeof(Set));
    (*m)->key = key;
    return false;
}

// Percorre o set executando unha función para cada elemento
//      @param S: Set a percorrer
//      @param VAR: Variável auxiliar na que se garda o valor de cada elemento
//      @param DO: Código a executar
#define set_for_each(S, VAR, DO)                                               \
    do {                                                                       \
        if (!S) break;                                                         \
        Vec(Set*) stack;                                                       \
        vec_init(stack);                                                       \
        vec_push(stack, S);                                                    \
        while (stack.len > 0) {                                                \
            typeof(S) VAR = *vec_pop(stack);                                   \
            for (u8 i = 0; i < 4; i++) {                                       \
                if (VAR->child[i]) {                                           \
                    vec_push(stack, VAR->child[i]);                            \
                }                                                              \
            }                                                                  \
            DO;                                                                \
        }                                                                      \
        vec_free(stack);                                                       \
    } while (0)

// Compara dous sets para ver se son iguais
//      @param a: Primer set
//      @param b: Segundo set
static inline bool set_eq(const Set* a, const Set* b) {
    if (a == b) 
        return true;
    if (!a || !b) 
        return false;
    if (a->key != b->key) 
        return false;
    for (u8 i = 0; i < 4; i++) {
        if (!set_eq(a->child[i], b->child[i])) 
            return false;
    }
    return true;
}

// Elimina un set de memoria
//      @param s: Set a eliminar
static inline void set_free(Set* s) {
    set_for_each(s, x, arena_del(&arena, (u8*)x, sizeof(Set)));
}
