// Implementación dun alocador baseado en arenas de memoria
// Isto é útil para controlar exactamente como se asigna a memoria no programa
// Vai reservando páxinas de memoria contigua conforme sexan necesarias

#pragma once

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include "../definicions.h"

// Tamaño e número máximo de páxinas
#define CHUNK ((u64)1<<24) // 16mb
#define MAX_BLOCKS ((u64)1<<10) // 16gb*
// NOTA: Pode configurarse moito máis alto, xa que só reservamos direccións de
// memoria virtuais, das que temos máis de 100tb a nosa disposición
// Por suposto, isto estará limitado cando se vaian creando páxinas pola
// memoria real do dispositivo

// Arena de memoria
typedef struct Arena Arena;
struct Arena {
    void* inicio;
    void* actual;
    void* fin;
};

// Engade unha páxina á arena de memoria
//      @param a: Arena de memoria
static inline void arena_grow(Arena* a) {
    assert(a->fin < a->inicio + CHUNK * MAX_BLOCKS);

    if (mprotect(a->fin, CHUNK, PROT_READ | PROT_WRITE) != 0) {
        err("erro ó expandir a arena de memoria: %d\n", errno);
    }

    a->fin = a->fin + CHUNK;
    log("arena extendida ata %p\n", a->fin);
}

// Crea unha nova arena de memoria
//      @param a: Arena de memoria
static inline void arena_init(Arena* a) {
    assert(a != NULL);

    // Usamos mmap para reservar unha rexión de direccións de memoria
    // (pero todavía non a memoria física)
    a->inicio = mmap(NULL, CHUNK * MAX_BLOCKS, PROT_NONE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (a->inicio == MAP_FAILED) {
        err("erro ao crear a arena de memoria: %d\n", errno);
    } 
    log("arena creada en %p\n", a->inicio);

    // Reservamos a primeira páxina de memoria
    a->fin = a->inicio;
    a->actual = a->inicio;
    arena_grow(a);
}

// Destrúe unha arena de memoria
//      @param a: Arena de memoria
static inline void arena_free(Arena* a) {
    munmap(a->inicio, CHUNK * MAX_BLOCKS);
    log("arena liberada en %p\n", a->inicio);
}

// Crea unha nova rexión na arena de memoria
//      @param a: Arena de memoria
//      @param n: Cantidade de memoria a reservar
static inline void* arena_push(Arena* a, u64 n) {
    // Se non hai espazo suficiente, reservar máis páxinas
    while (n > a->fin - a->actual) {
        arena_grow(a);
    }

    // Crea o puntero á rexión de memoria apropiada e actualiza a arena
    void* mem = a->actual;
    a->actual += n;

    return mem;
}

// Crea unha nova rexión na arena de memoria e a inicializa a 0
//      @param a: Arena de memoria
//      @param n: Cantidade de memoria a reservar
static inline void* arena_push_zero(Arena* a, u64 n) {
    void* mem = arena_push(a, n);
    return memset(mem, 0, n);
}

#define arena_push_arr(A, T, N) (T*)arena_push(A, sizeof(T) * N)
#define arena_push_struct(A, T) arena_push_arr(A, T, 1)

// Elimina bytes do final do stack
//      @param a: Arena de memoria
//      @param n: Cantidade de memoria a eliminar
static inline void arena_pop(Arena* a, u64 n) {
    a->actual = n > a->actual - a->inicio ? a->inicio : a->actual - n;
}

// Limpa a arena (retorna a posición actual ó inicio)
//      @param a: Arena de memoria
static inline void arena_clear(Arena* a) {
    a->actual = a->inicio;
}
