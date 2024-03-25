// Dobre centinela
// Co obxectivo de reducir o número de accesos a memoria, creamos este dobre
// buffer para o sistema de entrada

#pragma once

#include "str.h"
#include <stdlib.h>

// Contén dous arrays de tamaño TAM_MAX, un punteiro ao inicio e outro ao actual
// Con él podemos ler un lexema (de tamaño máximo TAM_MAX) e desprazarnos
// por el sen medo a perder caracteres e minimizando os accesos a memoria
// A estrutura son dous arrais contiguos en memoria:
// [- - - - \0 - - - - \0]
//  ^   a   ^  ^   b   ^
//  |TAM_MAX| 1|TAM_MAX| 1
typedef struct {
    u8 datos[TAM_MAX * 2 + 2];
    u8* inicio;
    u8* actual;
    FILE* arquivo;
    bool erro_tam_max;
    bool hacia_atras;
} Centinela;

// Inicializa un centinela
// Non é necesaria limpeza, xa que reside na pila
//      @param c: Centinela a inicializar
//      @param nome: Nome do arquivo a abrir
//      @return: O centinela creouse correctamente
static inline bool centinela_init(Centinela* c, const char* nome) {
    // Poñemos os punteiros no final do array
    // (para a carga correcta no primeiro bloque)
    c->inicio = c->datos + TAM_MAX * 2;
    c->actual = c->inicio;

    // Poñemos os centinelas (terminadores de string)
    c->datos[TAM_MAX] = 0;
    c->datos[TAM_MAX * 2 + 1] = 0;

    // Abrimos o arquivo
    c->arquivo = fopen(nome, "r");
    if (c->arquivo == NULL) {
        err("non se puido abrir o arquivo '%s'\n\n", nome);
        return false;
    }
    dbg("abriuse o arquivo %s\n\n", nome);

    return true;
}

#ifdef DEBUG_CENTINELA
// Imprime o estado do buffer dun centinela
// Utiliza subliñado para amosar inicio (azul) e actual(amarelo)
//      @param c: Centinela
static inline void centinela_dbg(Centinela* c, const char* s) {
    print(C_BLACK, "%s: [", s);
    for (i32 i = 0; i < TAM_MAX * 2 + 2; i++) {
        u8 cc = c->datos[i];
        if (cc == '\0') {
            printf(C_MAGENTA "\\");
            cc = '0';
        }
        if (cc == '\n') {
            printf("\\");
            cc = 'n';
        }
        if (c->actual - c->datos == i)
            printf(C_UNDERLINE C_YELLOW);
        if (c->inicio - c->datos == i)
            printf(C_UNDERLINE C_BLUE);
        printf("%c" C_RESET, cc);
    }
    printf("]\n");
}
#else
#define centinela_dbg(c, s)
#endif

// Carga un bloque do arquivo nun dos arrays do centinela
//      @param c: Centinela
//      @param i: Bloque a cargar (0 ou 1)
//      @param f: Arquivo
static inline void centinela_cargar(Centinela* c, u8 i) {
#ifdef DEBUG_TRACE
    dbg("cargouse o bloque %d do centinela\n\n", i);
#endif
    u32 res = fread(c->datos + TAM_MAX * i + i, 1, TAM_MAX, c->arquivo);
    if (res < TAM_MAX) {
        dbg("lendo o final do arquivo\n\n");
        c->datos[TAM_MAX * i + i + res] = '\0';
    }
}

// Avanza o centinela un caracter
//      @param c: Centinela
static inline void centinela_sig(Centinela* c) {
    c->actual++;
    if (c->actual == c->datos + TAM_MAX * 2 + 2) {
        c->actual = c->datos;
    }
    // Comprobar se se superou o tamaño máximo
    if (!c->erro_tam_max && abs(c->actual - c->inicio) == TAM_MAX) {
        err("o buffer do centinela superou o tamaño maximo\n\n");
        c->erro_tam_max = true;
    }
    centinela_dbg(c, "sig");
}

// Retrocede o centinela un caracter
//      @param c: Centinela
static inline void centinela_prev(Centinela* c) {
    if (c->actual == c->datos) {
        c->actual = c->datos + TAM_MAX * 2 + 1;
    }
    c->actual--;
    c->hacia_atras = true;
    centinela_dbg(c, "prev");
}

// Lé un carácter do centinela
//      @param c: Centinela a ler
//      @return: Carácter lido
static inline u8 centinela_ler(Centinela* c) {
    centinela_sig(c);
    u8 res = *c->actual;

    // Se estamos nun dos centinelas, cargamos o outro bloque
    // (excepto se acabamos de volver cara atrás)
    if (res == '\0') {
        u8 i = c->actual == c->datos + TAM_MAX;
        if (i || c->actual == c->datos + TAM_MAX * 2 + 1) {
            if (!c->hacia_atras) {
                centinela_cargar(c, i);
            }
            centinela_sig(c);
            res = *c->actual;
        } else {
            // Chegamos ó final do arquivo
            return EOF;
        }
    }

    c->hacia_atras = false;
    return res;
}

// Establece o inicio do centinela no punto actual
//      @param c: Centinela a modificar
static inline void centinela_inicio(Centinela* c) {
    c->inicio = c->actual;
    c->erro_tam_max = false;
}

// Obtén a cadena de caracteres representada actualmente no centinela
//      @param c: Centinela
//      @param s: Cadea de caracteres
static inline void centinela_str(Centinela* c, Str* s) {
    if (!c) {
        err("centinela nulo");
        return;
    }

    if (c->inicio == c->actual) {
        return;
    }

    i32 a = c->inicio - c->datos;
    i32 b = c->actual - c->datos;
    u8* m = c->datos + TAM_MAX + 1;

    // Se hai un erro, tratamos de leer o tamaño máximo dende o final
    if (c->erro_tam_max) {
        a = b > (TAM_MAX + 1) ? b - (TAM_MAX + 1) : b + (TAM_MAX + 1);
    }

    u8* i = c->datos + a;

    // Inicio está na segunda metade
    if (a > TAM_MAX) {
        // Actual está na segunda metade
        if (b > TAM_MAX) {
            assert(b > a);
            vec_init_res((*s), (b - a));
            vec_append_n((*s), i, (b - a));
        } else {
            i32 da = TAM_MAX * 2 + 1 - a;
            vec_init_res((*s), (da + b));
            vec_append_n((*s), i, da);
            vec_append_n((*s), c->datos, b);
        }
    }
    // Inicio está na primeira metade
    else {
        // Actual está na segunda metade
        if (b > TAM_MAX) {
            i32 da = TAM_MAX - a;
            i32 db = b - TAM_MAX;
            vec_init_res((*s), (da + db));
            vec_append_n((*s), i, da);
            vec_append_n((*s), m, db);
        }
        // Actual está na primeira metade
        else {
            assert(b > a);
            vec_init_res((*s), (b - a));
            vec_append_n((*s), i, (b - a));
        }
    }

    vec_push((*s), '\0');
}
