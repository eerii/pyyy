// Implementación dun vector dinámico xenérico
// Utiliza macros para permitir calqueira tipo de dato
// Funcións e nomes baseados na implementación Vec de rust
// Pode utilizarse como array estático sen reservar memoria dinámica
//
// O uso de macros pode incrementar o tamaño do binario, pero para a nosa
// aplicación non debería de ser moi preocupante. Tampouco debería de ter maior
// impacto no tempo de execución
//
// Os do / while están para garantizar o correcto funcionamento das macros cando
// se utilizan seguidas de punto e coma, semellantes a unha función convencional
//
// Usa a arena de memoria global
//
// @author: @eerii, jose.pazos.perez@rai.usc.es

#pragma once

#include <stdlib.h>

#include "arena.h"

#define RESERVA_POR_DEFECTO_VEC 4
#define FACTOR_CRECEMENTO_VEC(V) (V.cap < 8192 ? V.cap * 2 : V.cap + 8192)

// Vector dinámico
// É recomendable facer un typedef to tipo que se vaia a utilizar para que o
// compilador entenda que múltiples instancias son do mesmo tipo e poder
// utilizalo como parámetro en funcións convencionais
#define Vec(T)                                                                 \
    struct {                                                                   \
        u32 len;                                                               \
        u32 cap;                                                               \
        typeof(T)* data;                                                       \
    }

// Conta os elementos dun array a engadir a un vector dinámico
//      @param V: Vector no que se vai engadir
//      @param X: Array a contar
#define vec_countof(V, X) (sizeof(X) / sizeof(typeof(V.data[0])))

// Inicializa dende un array preexistente
// Esta solución *non* reserva memoria dinámica (malloc)
// ata que o array se redimensiona
//      @param V: Vector a inicializar
//      @param X: Array de elementos co que inicializalo
//      @param N: Cantidade de elementos
#define vec_init_from_n(V, X, N)                                               \
    do {                                                                       \
        V.data = (typeof(V.data))&(X);                                         \
        V.len = N;                                                             \
        V.cap = 0;                                                             \
    } while (0)

// Inicializa dende un array preexistente
// Esta solución *non* reserva memoria dinámica (malloc)
// ata que o array se redimensiona
//      @param V: Vector a inicializar
//      @param X: Array de elementos co que inicializalo
#define vec_init_from(V, X) vec_init_from_n(V, vec_countof(V, X))

// Inicialización reservando memoria
//      @param V: Vector a inicializar
//      @param N: Cantidade de elementos que reservar inicialmente
#define vec_init_res(V, N)                                                     \
    do {                                                                       \
        typedef typeof(*V.data) T;                                             \
        V.data = arena_push_arr(&arena, T, N);                                 \
        V.len = 0;                                                             \
        V.cap = N;                                                             \
    } while (0)

// Inicialización por defecto (reserva `RESERVA_POR_DEFECTO_VEC` elementos)
//      @param V: Vector a inicializar
#define vec_init(V) vec_init_res(V, RESERVA_POR_DEFECTO_VEC)

// Redimensiona a capacidade do vector (pode movelo en memoria)
// Se é un array estático, crea un novo array dinámico do tamaño indicado
//      @param V: Vector a redimensionar
//      @param N: Novo tamaño do vector
#define vec_reserve(V, N)                                                      \
    do {                                                                       \
        if (V.cap == 0 || N > V.cap) {                                         \
            typedef typeof(V.data[0]) T;                                       \
            T* tmp = V.data;                                                   \
            V.data = arena_push_arr(&arena, T, N);                             \
            memcpy(V.data, tmp, V.len * sizeof(T));                            \
            if (N > V.cap) {                                                   \
                arena_del(&arena, (u8*)tmp, V.cap);                            \
            }                                                                  \
            V.cap = N;                                                         \
        }                                                                      \
    } while (0)

// Redimensiona o vector, enchéndoo de valores nos espazos vacíos
// Se N é mais pequeno que o tamaño do vector só se actualiza o punteiro,
// non se libera memoria
//      @param V: Vector a redimensionar
//      @param N: Novo tamaño do vector
//      @param X: Valor co que encher os espazos vacíos
#define vec_resize(V, N, X)                                                    \
    do {                                                                       \
        vec_reserve(V, N);                                                     \
        for (u32 i = V.len; i < N; ++i)                                        \
            V.data[i] = X;                                                     \
        V.len = N;                                                             \
    } while (0)

// Busca un elemento no vector, devolve o índice da primeira ocurrencia
// Se non o atopa, I será V.len
//      @param V: Vector no que buscar
//      @param X: Elemento a buscar
//      @param I: Posición do elemento
//      @param F(a, b): Función de comparación
#define vec_find(V, X, I, F)                                                   \
    do {                                                                       \
        for (I = 0; I < V.len; ++I) {                                          \
            if (F(V.data[I], X)) {                                             \
                break;                                                         \
            }                                                                  \
        }                                                                      \
    } while (0)

// Busca un elemento no vector, devolve o índice da primeira ocurrencia
// Se non o atopa, I será V.len
//      @param V: Vector no que buscar
//      @param X: Elemento a buscar
//      @param I: Posición do elemento
#define vec_find_eq(V, X, I)                                                   \
    do {                                                                       \
        for (I = 0; I < V.len; ++I) {                                          \
            if (V.data[I] == X) {                                              \
                break;                                                         \
            }                                                                  \
        }                                                                      \
    } while (0)

// Engade un elemento ó final do vector
// Se non cabe, redimensiona
//      @param V: Vector ó que engadir
//      @param X: Elemento a engadir
#define vec_push(V, X)                                                         \
    do {                                                                       \
        if (V.len >= V.cap)                                                    \
            vec_reserve(V, V.cap > 0 ? FACTOR_CRECEMENTO_VEC(V)                \
                                     : V.len + RESERVA_POR_DEFECTO_VEC);       \
        V.data[V.len++] = X;                                                   \
    } while (0)

// Engade un elemento ó final do vector se non existe
//      @param V: Vector no que engadir
//      @param X: Elemento a engadir
#define vec_push_unique(V, X)                                                  \
    do {                                                                       \
        u32 it;                                                                \
        vec_find_eq(V, X, it);                                                 \
        if (it == V.len)                                                       \
            vec_push(V, X);                                                    \
    } while (0)

// Engade outro array ó final do vector
//      @param V: Vector no que engadir
//      @param X: Array a engadir
//      @param N: Cantidad de elementos
#define vec_append_n(V, X, N)                                                  \
    do {                                                                       \
        vec_reserve(V, V.len + N);                                             \
        memcpy(&V.data[V.len], X, N * sizeof(V.data[0]));                      \
        V.len += N;                                                            \
    } while (0)

// Engade outro array ó final do vector
//      @param V: Vector no que engadir
//      @param X: Array a engadir
#define vec_append(V, X) vec_append_n(V, X, vec_countof(V, X))

// Obtén o último elemento do vector e o elimna
// Se non ten elementos, devolve NULL
//      @param V: Vector a comprobar
#define vec_pop(V) ((V.len > 0) ? &V.data[--V.len] : NULL)

// Obtén o elemento na posición i
// Se i >= len, devolve NULL
//      @param V: Vector do que obter
//      @param I: Posición do elemento
#define vec_get(V, I) ((I < V.len) ? &V.data[I] : NULL)

// Forza a convertir un vector en dinámico
//      @param V: Vector a convertir
#define vec_make_dyn(V)                                                        \
    do {                                                                       \
        if (V.cap == 0 && V.len > 0)                                           \
            vec_reserve(V, V.len);                                             \
    } while (0)

// Inserta un elemento na posición i
// Se i está fora de rango, a función non fai nada
//      @param V: Vector no que insertar
//      @param I: Posición do elemento
//      @param X: Elemento a insertar
#define vec_ins(V, I, X)                                                       \
    do {                                                                       \
        vec_make_dyn(V);                                                       \
        if (I == V.len) {                                                      \
            vec_push(V, X);                                                    \
        } else if (I < V.len) {                                                \
            ++V.len;                                                           \
            memmove(&V.data[I + 1], &V.data[I],                                \
                    (V.len - (I - 1)) * sizeof(*V.data));                      \
            V.data[I] = X;                                                     \
        }                                                                      \
    } while (0)

// Elimina o elemento na posición i
//      @param V: Vector no que eliminar o elemento
//      @param I: Posición do elemento
#define vec_del(V, I)                                                          \
    do {                                                                       \
        vec_make_dyn(V);                                                       \
        if (I < V.len) {                                                       \
            --V.len;                                                           \
            memmove(&V.data[I], &V.data[I + 1],                                \
                    (V.len - I) * sizeof(*V.data));                            \
        }                                                                      \
    } while (0)

// Executa o DO para cada elemento do vector
//      @param V: Vector no que executar o for
//      @param VAR: Variável auxiliar na que se garda o valor de cada elemento
//      @param DO: Código a executar
#define vec_for_each(V, VAR, DO)                                               \
    do {                                                                       \
        for (u32 i = 0; i < V.len; ++i) {                                      \
            typeof(*V.data) VAR = V.data[i];                                   \
            DO;                                                                \
        }                                                                      \
    } while (0)

// Devolve se o vector está vacío
// Non ten elementos, pero sí pode ter memoria reseervada
//      @param V: Vector a comprobar
#define vec_is_empty(V) (V.len == 0)

// Devolve se o vector é valido (ten capacidade ou é estático)
//      @param V: Vector a comprobar
#define vec_valid(V) (V.cap > 0 || v.len > 0)

// Limpa o vector (non libera memoria)
//      @param V: Vector a limpar
#define vec_clear(V) (V.len = 0)

// Destrúe o vector e libera memoria
//      @param V: Vector a destruir
#define vec_free(V)                                                            \
    do {                                                                       \
        if (V.cap != 0)                                                        \
            arena_del(&arena, (u8*)V.data, V.cap * sizeof(*V.data));           \
        V.len = 0;                                                             \
        V.cap = 0;                                                             \
    } while (0)
