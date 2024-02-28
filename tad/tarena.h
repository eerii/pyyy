// Implementación dun alocador baseado en arenas de memoria
// Isto é útil para controlar exactamente como se asigna a memoria no programa
// Define a función alloc, análoga a malloc

#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../definicions.h"

#define CHUNK (u32)1<<24 // 16mb

// Macro de axuda para crear novas alocacións nunha arena
#define new(a, t, n) (t*)alloc(a, sizeof(t), _Alignof(t), n, 1)

// Arena de memoria
typedef struct Arena Arena;
struct Arena {
    u8* inicio;
    u8* fin;
    Arena* sig;
};

// Crea unha nova arena de memoria
//      @param a: Arena de memoria
//      @param cap: Capacidade da arena
//      @param prev: Arena anterior (pode ser NULL)
static inline void arena_init(Arena* a, u32 cap, Arena* prev) {
    if (a == NULL) {
        assert(0);
    }

    a->inicio = (u8*)malloc(cap);
    a->fin = a->inicio + cap;
    a->sig = NULL;
    
    if (prev != NULL) {
        prev->sig = a;
    }
}

// Crea un novo obxecto na arena de memoria
//      @param a: Arena de memoria
//      @param tam_obj: Tamaño do obxecto a crear
//      @param align: Alinhamento do obxecto
//      @param count: Cantidade de obxectos
//      @param zero: Inicializar o obxecto a cero
static inline u8* alloc(Arena* a, u32 tam_obj, u32 align, u32 count, bool cero) {
    u32 disponible = a->inicio - a->fin;

    // Padding necesario para aliñar o final do obxecto
    u32 padding = -(uptr)a->inicio & (align - 1);

    // Se non hai espazo suficiente para os obxectos, buscar a seguinte arena
    if (count > (disponible - padding) / tam_obj) {
        if (a->sig == NULL) {
            a->sig = malloc(sizeof(Arena));
            arena_init(a->sig, CHUNK, a);
        }
        a = a->sig;
        return alloc(a, tam_obj, align, count, cero);
    }

    // Crea o puntero á rexión de memoria apropiada e actualiza a arena
    a->inicio += padding;
    u8* res = a->inicio;
    a->inicio += tam_obj * count;

    // Se se pide, fai memset a 0 para todo o obxecto
    return cero ? (u8*)memset(res, 0, tam_obj * count) : res;
}

// Crea unha nova subarena de memoria dentro de otra
//      @param base: Dirección de memoria onde crear a nova arena
//      @param cap: Capacidade da arena
/*static inline Arena arena_nest(Arena* base, u32 cap) {
    Arena a = {0};
    a.inicio = alloc(base, 1, 1, cap, 0);
    a.fin = a.inicio + cap;
    return a;
}*/
