#pragma once

#include "tipos/str.h"

// Valor que devolve o analizador léxico
// O código é o tipo de lexema que é (ver definicions.h)
// O valor é un punteiro ou ben á táboa de símbolos ou ben a táboa de literais
// que amosa o contenido do lexema
typedef struct {
    u32 codigo;
    Str* valor;
} Lexema;

// Inicializa o sistema de entrada co arquivo indicado
//      @param nome: Nome do arquivo a abrir
void entrada_init(const char* nome);

// Cerra o arquivo do sistema de entrada
void entrada_close();

// Devolve o seguinte lexema no analizador léxico
Lexema seguinte_lexico();
