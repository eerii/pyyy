// Abstracción do sistema de entrada
// Utiliza centinela por debaixo

#pragma once

#include "tipos/centinela.h"

typedef Centinela SistemaEntrada;

// Centinela global
extern SistemaEntrada entrada;

// Inicializa o sistema de entrada co arquivo indicado
//      @param nome: Nome do arquivo a abrir
static inline void entrada_init(const char* nome) {
    if (!centinela_init(&entrada, nome)) {
        err("non se puido inicializar o sistema de entrada\n\n");
        exit(1);
    }
}

// Lemos un caracter do sistema de entrada
static inline u8 entrada_ler() { return centinela_ler(&entrada); }

// Retrocede o sistema de entrada ó espazo anterior
static inline void entrada_prev() { centinela_prev(&entrada); }

// Establece o inicio do sistema de entrada no punto actual
static inline void entrada_inicio() { centinela_inicio(&entrada); }

// Obtén a cadea de caracteres representada actualmente no sistema de entrada
//      @param s: Cadea de caracteres
static inline void entrada_str(Str* s) { centinela_str(&entrada, s); }
