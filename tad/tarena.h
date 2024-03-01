// Implementación dun alocador baseado en arenas de memoria
// Isto é útil para controlar exactamente como se asigna a memoria no programa
// Vai reservando páxinas de memoria contigua conforme sexan necesarias
// Poden marcarse bloques coma eliminados onde se colocarán "tumbas" que poderán
// ser reutilizadas
// Referencias:
//      - https://en.wikipedia.org/wiki/Region-based_memory_management
//      - https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator
//      - https://nullprogram.com/blog/2023/09/27/

#pragma once

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include "../definicions.h"

// TODO: Mover estos dous parametros a init para facela configurable
//          Permitir subarenas?
//          Poden funcionar coma un push inverso (en plan vanse asignando dende
//          o final) Deben de quitarse en orde de stack (a última primeiro)
//          Permite crecer na arena principal, permite crecer na subarena
//          (dentro do límite), permite engadir novas arenas

// Tamaño e número máximo de páxinas
#define CHUNK ((u64)1 << 20)      // 1mb
#define MAX_BLOCKS ((u64)1 << 10) // 16gb*
// NOTA: Pode configurarse moito máis alto, xa que só reservamos direccións de
// memoria virtuais, das que temos máis de 100tb a nosa disposición
// Por suposto, isto estará limitado cando se vaian creando páxinas pola
// memoria real do dispositivo

// Punteiro tumba (para rexións eliminadas)
// É básicamente unha lista vinculada, e non é o máis eficiente de percorrer
// Sen embargo, para o noso caso non é moi significativo, xa que case non vamos
// a eliminar obxectos. A táboa de símbolos non borra nada e vamos a ter poucos
// vectores dinámicos, polo que tampouco ten sentido facer algo máis sofisticado
typedef struct Tumba Tumba;
struct Tumba {
    Tumba* sig;
    u64 tam;
};

// Arena de memoria
typedef struct Arena Arena;
struct Arena {
    u8* inicio;
    u8* actual;
    u8* fin;
    Tumba* eliminados;
};

#define arena_len(A) (A.actual - A.inicio)
#define arena_cap(A) (A.fin - A.inicio)

// Arena global de memoria
// Ten que ser definida en *exactamente* un ficheiro .c
extern Arena arena;

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
    a->inicio = (u8*)mmap(NULL, CHUNK * MAX_BLOCKS, PROT_NONE,
                          MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (a->inicio == MAP_FAILED) {
        err("erro ao crear a arena de memoria: %d\n", errno);
    }
    log("arena creada en %p\n", a->inicio);

    // Reservamos a primeira páxina de memoria
    a->fin = a->inicio;
    a->actual = a->inicio;
    arena_grow(a);
}

// Crea unha nova rexión na arena de memoria
//      @param a: Arena de memoria
//      @param n: Cantidade de memoria a reservar
static inline u8* arena_push(Arena* a, u64 n) {
    u8* mem = NULL;

    // Busca nas tumbas se hai espazo dispoñible
    Tumba* t = a->eliminados;
    Tumba* prev = NULL;
    while (t != NULL) {
        // Se hai espazo na tumba
        if (t->tam >= n) {
            mem = (u8*)t;
            t->tam -= n;

            // Comprobamos se queda espazo suficiente para mover a tumba
            // Se non, simplemente aceptamos ese espazo como perdido
            Tumba* tn = t->sig;
            if (t->tam >= sizeof(Tumba)) {
                tn = (Tumba*)((u8*)t + n);
                tn->sig = t->sig;
                tn->tam = t->tam;
            }

            // Actualizamos a lista de tumbas
            if (prev != NULL) {
                prev->sig = tn;
            } else {
                a->eliminados = tn;
            }

            log("atopouse sitio nunha tumba: %p\n", mem);
            return mem;
        }
        prev = t;
        t = t->sig;
    }

    // Se non hai espazo suficiente, reservar máis páxinas
    while (n > a->fin - a->actual) {
        arena_grow(a);
    }

    // Crea o puntero á rexión de memoria apropiada e actualiza a arena
    mem = a->actual;
    a->actual += n;

    return mem;
}

// Crea unha nova rexión na arena de memoria e a inicializa a 0
//      @param a: Arena de memoria
//      @param n: Cantidade de memoria a reservar
static inline u8* arena_push_zero(Arena* a, u64 n) {
    u8* mem = arena_push(a, n);
    return (u8*)memset(mem, 0, n);
}

#define arena_push_arr(A, T, N) (T*)arena_push(A, sizeof(T) * ((u64)N))
#define arena_push_struct(A, T) arena_push_arr(A, T, 1)

// Elimina bytes do final do stack
//      @param a: Arena de memoria
//      @param n: Cantidade de memoria a eliminar
static inline void arena_pop(Arena* a, u64 n) {
    a->actual = n > a->actual - a->inicio ? a->inicio : a->actual - n;
}

// Elimina unha area de memoria do medio da arena e crea unha tumba no seu lugar
//      @param a: Arena de memoria
//      @param p: Punteiro da dirección na que empezar
//      @param n: Cantidade de memoria a eliminar
static inline void arena_del(Arena* a, u8* p, u64 n) {
    // Se o espazo libre é menor que o tamaño dunha tumba, aceptamos perdelo
    if (n < sizeof(Tumba)) {
        return;
    }

    // Comprobamos que o punteiro está dentro da rexión ocupada da arena
    assert(p >= a->inicio && p < a->fin);

    // Crea a tumba
    Tumba* t = (Tumba*)p;
    t->tam = n;
    t->sig = NULL;

    // Se todavía non hai tumbas creadas, engade a primeira
    if (a->eliminados == NULL) {
        a->eliminados = t;
        return;
    }

    // Percorre as tumbas existentes e engade a nova ó final
    // TODO: Comprobar que non hai outra tumba nese lugar, mezclar as
    // existentes e ver se coincide co final (mover punteiro actual)
    Tumba* it = a->eliminados;
    while (true) {
        if (it->sig == NULL) {
            it->sig = t;
            return;
        }
        it = it->sig;
    }
}

#define arena_del_arr(A, P, T, N) arena_del(A, P, sizeof(T) * ((u64)N))
#define arena_del_struct(A, P, T) arena_del_arr(A, P, T, 1)

// Limpa a arena (retorna a posición actual ó inicio)
//      @param a: Arena de memoria
static inline void arena_clear(Arena* a) { a->actual = a->inicio; }

// Destrúe unha arena de memoria
//      @param a: Arena de memoria
static inline void arena_free(Arena* a) {
    munmap(a->inicio, CHUNK * MAX_BLOCKS);
    log("arena liberada en %p\n", a->inicio);
}
