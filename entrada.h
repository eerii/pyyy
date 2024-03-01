#pragma once

#include <stdio.h>

#include "definicions.h"

typedef struct {
    FILE* file;
    bool comentario_linha;
    bool comentario_bloque;
} Arquivo;

Arquivo* abrir_arquivo(const char* nome);

i32 seguinte_caracter(Arquivo* a);

void devolver_caracter(Arquivo* a, i8 c);
