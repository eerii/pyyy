#pragma once

#include "tipos/centinela.h"

// Valor que devolve o analizador léxico
// O código é o tipo de lexema que é (ver definicions.h)
// O valor é un punteiro ou ben á táboa de símbolos ou ben a táboa de literais
// que amosa o contenido do lexema
typedef struct {
    u32 codigo;
    Str* valor;
} Lexema;

// Inicia os autómatas a partir de expresións regulares no analizador léxico
void automatas_init();

// Devolve o seguinte lexema no analizador léxico
//      @param a: Sistema de entrada
//      @return: Seguinte lexema
Lexema seguinte_lexico(Centinela* a);
