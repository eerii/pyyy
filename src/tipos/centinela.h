#pragma once

#include "../definicions.h"

// Tamaño máximo do lexema (limitado polos buffers de entrada e saída)
#define TAM_MAX 10

// Dobre centinela
// Co obxectivo de reducir o número de accesos a memoria, creamos este dobre
// buffer para o sistema de entrada
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
} Centinela;

// Inicializa un centinela
// Non é necesaria a limpeza, xa que reside na pila
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
        err("non se puido abrir o arquivo '%s'\n", nome);
        return false;
    }
    dbg("abriuse o arquivo %s\n", nome);

    return true;
}

#ifdef DEBUG
// Imprime o estado do buffer dun centinela
// Utiliza subliñado para amosar inicio (azul) e actual(verde)
//      @param c: Centinela
static inline void centinela_dbg(Centinela* c, const char* s) {
    dbg("%s: [", s);
    for (i32 i = 0; i < TAM_MAX * 2 + 2; i++) {
        u8 cc = c->datos[i];
        if (cc == '\0') {
            printf(C_RED "\\");
            cc = '0';
        }
        if (cc == '\n') {
            printf("\\");
            cc = 'n';
        }
        if (c->actual - c->datos == i)
            printf(C_UNDERLINE C_GREEN);
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
    dbg("cargouse o bloque %d do centinela\n", i);

    u32 res = fread(c->datos + TAM_MAX * i + i, 1, TAM_MAX, c->arquivo);
    if (res < TAM_MAX) {
        dbg("lendo o final do arquivo\n");
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
    centinela_dbg(c, "sig");
}

// Retrocede o centinela un caracter
//      @param c: Centinela
static inline void centinela_prev(Centinela* c) {
    if (c->actual == c->datos) {
        c->actual = c->datos + TAM_MAX * 2 + 1;
    }
    c->actual--;
    centinela_dbg(c, "prev");
}

// Lé un carácter do centinela
//      @param c: Centinela a ler
//      @return: Carácter lido
static inline u8 centinela_ler(Centinela* c) {
    centinela_sig(c);
    u8 res = *c->actual;

    // Se estamos nun dos centinelas, cargamos o outro bloque
    if (res == '\0') {
        u8 i = c->actual == c->datos + TAM_MAX;
        if (i || c->actual == c->datos + TAM_MAX * 2 + 1) {
            centinela_cargar(c, i);
            centinela_sig(c);
            res = *c->actual;
        } else {
            // Chegamos ó final do arquivo
            return EOF;
        }
    }

    return res;
}

// Establece o inicio do centinela no punto actual
//      @param c: Centinela a modificar
static inline void centinela_inicio(Centinela* c) { c->inicio = c->actual; }
