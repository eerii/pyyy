// Implementación dun vector dinámico xenérico
// Utiliza macros para permitir calqueira tipo de dato
// Funcións e nomes baseados na implementación Vec de rust
// Pode utilizarse como array estático sen reservar memoria dinámica
//
// O uso de macros pode incrementar o tamaño do binario, pero para a nosa
// aplicación non debería de ser moi preocupante. Tampouco debería de ter maior
// impacto no tempo de execución
//
// Utiliza os compound statements ({}) de gcc, polo que é recomendable usalo
// para compilar. Isto permite facer macros que devolvan valores coma funcións
// https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
//
// Usa a arena de memoria global
//
// Ademáis, se se inicializa un vector a partir dun literal, este non reserva
// memoria dinámica ata que se aplica redimensionamento. Isto permite unha
// interfaz común para tratar con memoria dinámica e estática moi útil.

#pragma once

#include <stdlib.h>

#include "arena.h"

// Parámetros que controlan a política de reserva de memoria do vector
// Modificar estes parámetros pode cambiar o balance memoria/procesamento do
// programa, e pode ser moi útil para optimizalo en sistemas con poucos
// recursos
// A política por defecto é reservar memoria para 4 elementos e ir multiplicando
// por dous a capacidade ata chegar a un lítite superior, a partir do cal crece
// linealmente
// Este é un bo balance para non ter que facer moitas recolocacións
// De todas maneiras, é posible utilizar os métodos específicos para reservar x
// cantidade de memoria explicitamente para maior eficiencia
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
//      @return: Cantidade de elementos
#define vec_countof(V, X) (sizeof(X) / sizeof(typeof(V.data[0])))

// Inicializa dende un array preexistente
// Esta solución *non* reserva memoria dinámica (malloc)
// ata que o array se redimensiona
//      @param V: Vector a inicializar
//      @param X: Array de elementos co que inicializalo
//      @param N: Cantidade de elementos
#define vec_init_from_n(V, X, N)                                               \
    ({                                                                         \
        V.data = (typeof(V.data))&(X);                                         \
        V.len = N;                                                             \
        V.cap = 0;                                                             \
    })

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
    ({                                                                         \
        typedef typeof(*V.data) T;                                             \
        V.data = arena_push_arr(&arena, T, N);                                 \
        V.len = 0;                                                             \
        V.cap = N;                                                             \
    })

// Inicialización por defecto (reserva `RESERVA_POR_DEFECTO_VEC` elementos)
//      @param V: Vector a inicializar
#define vec_init(V) vec_init_res(V, RESERVA_POR_DEFECTO_VEC)

// Crea un novo vector dende un array preexistente
//      @param VT: Tipo do vector
//      @param X: Array de elementos co que inicializalo
//      @param N: Cantidade de elementos
//      @return: Vector
#define vec_new_from_n(VT, X, N)                                               \
    ({                                                                         \
        VT _v;                                                                 \
        vec_init_from_n(_v, X, N);                                             \
        _v;                                                                    \
    })

// Crea un novo vector dende un array preexistente
//      @param VT: Tipo do vector
//      @param X: Array de elementos co que inicializalo
//      @return: Vector
#define vec_new_from(VT, X) vec_new_from_n(VT, vec_countof(VT, X))

// Crea un novo vector reservando memoria
//      @param VT: Tipo do vector
//      @param N: Cantidade de elementos que reservar inicialmente
//      @return: Vector
#define vec_new_res(VT, N)                                                     \
    ({                                                                         \
        VT _v;                                                                 \
        vec_init_res(_v, N);                                                   \
        _v;                                                                    \
    })

// Crea un novo vector por defecto
//      @param VT: Tipo do vector
//      @return: Vector
#define vec_new(VT) vec_new_res(VT, RESERVA_POR_DEFECTO_VEC)

// Crea unha copia dun vector
//      @param V: Vector a copiar
//      @return: Vector
#define vec_copy(V) vec_new_from_n(typeof(V), V.data, V.len)

// Redimensiona a capacidade do vector (pode movelo en memoria)
// Se é un array estático, crea un novo array dinámico do tamaño indicado
//      @param V: Vector a redimensionar
//      @param N: Novo tamaño do vector
#define vec_reserve(V, N)                                                      \
    ({                                                                         \
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
    })

// Redimensiona o vector, enchéndoo de valores nos espazos vacíos
// Se N é mais pequeno que o tamaño do vector só se actualiza o punteiro,
// non se libera memoria
//      @param V: Vector a redimensionar
//      @param N: Novo tamaño do vector
//      @param X: Valor co que encher os espazos vacíos
#define vec_resize(V, N, X)                                                    \
    ({                                                                         \
        vec_reserve(V, N);                                                     \
        for (u32 i = V.len; i < N; ++i)                                        \
            V.data[i] = X;                                                     \
        V.len = N;                                                             \
    })

// Busca un elemento no vector, devolve o índice da primeira ocurrencia
// Se non o atopa, I será V.len
//      @param V: Vector no que buscar
//      @param X: Elemento a buscar
//      @param F(a, b): Función de comparación
//      @return: Posición do elemento
#define vec_find(V, X, F)                                                      \
    ({                                                                         \
        u32 I;                                                                 \
        for (I = 0; I < V.len; ++I) {                                          \
            if (F(&V.data[I], &X)) {                                           \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        I;                                                                     \
    })

// Engade un elemento ó final do vector
// Se non cabe, redimensiona
//      @param V: Vector ó que engadir
//      @param X: Elemento a engadir
#define vec_push(V, X)                                                         \
    ({                                                                         \
        if (V.len >= V.cap)                                                    \
            vec_reserve(V, V.cap > 0 ? FACTOR_CRECEMENTO_VEC(V)                \
                                     : V.len + RESERVA_POR_DEFECTO_VEC);       \
        V.data[V.len++] = X;                                                   \
    })

// Engade un elemento ó final do vector se non existe
//      @param V: Vector no que engadir
//      @param X: Elemento a engadir
//      @param F(a, b): Función de comparación
//      @return: True se é un elemento único e se engade
#define vec_push_unique(V, X, F)                                               \
    ({                                                                         \
        u32 it = vec_find(V, X, F);                                            \
        bool _unique = it == V.len;                                            \
        if (_unique)                                                           \
            vec_push(V, X);                                                    \
        _unique;                                                               \
    })

// Engade outro array ó final do vector
//      @param V: Vector no que engadir
//      @param X: Array a engadir
//      @param N: Cantidad de elementos
#define vec_append_n(V, X, N)                                                  \
    ({                                                                         \
        vec_reserve(V, V.len + N);                                             \
        memcpy(&V.data[V.len], X, N * sizeof(V.data[0]));                      \
        V.len += N;                                                            \
    })

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
    ({                                                                         \
        if (V.cap == 0 && V.len > 0)                                           \
            vec_reserve(V, V.len);                                             \
    })

// Inserta un elemento na posición i
// Se i está fora de rango, a función non fai nada
//      @param V: Vector no que insertar
//      @param I: Posición do elemento
//      @param X: Elemento a insertar
#define vec_ins(V, I, X)                                                       \
    ({                                                                         \
        vec_make_dyn(V);                                                       \
        if (I == V.len) {                                                      \
            vec_push(V, X);                                                    \
        } else if (I < V.len) {                                                \
            ++V.len;                                                           \
            memmove(&V.data[I + 1], &V.data[I],                                \
                    (V.len - (I - 1)) * sizeof(V.data[0]));                    \
            V.data[I] = X;                                                     \
        }                                                                      \
    })

// Elimina o elemento na posición i
//      @param V: Vector no que eliminar o elemento
//      @param I: Posición do elemento
#define vec_del(V, I)                                                          \
    ({                                                                         \
        vec_make_dyn(V);                                                       \
        if (I < V.len) {                                                       \
            --V.len;                                                           \
            memmove(&V.data[I], &V.data[I + 1],                                \
                    (V.len - I) * sizeof(V.data[0]));                          \
        }                                                                      \
    })

// Executa o DO para cada elemento do vector
//      @param V: Vector no que executar o for
//      @param VAR: Variável auxiliar na que se garda o valor de cada elemento
//      @param DO: Código a executar
#define vec_for_each(V, VAR, DO)                                               \
    ({                                                                         \
        for (u32 _i = 0; _i < V.len; ++_i) {                                   \
            auto VAR = V.data[_i];                                             \
            DO;                                                                \
        }                                                                      \
    })

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
    ({                                                                         \
        if (V.cap != 0)                                                        \
            arena_del(&arena, (u8*)V.data, V.cap * sizeof(V.data[0]));         \
        V.len = 0;                                                             \
        V.cap = 0;                                                             \
    })
