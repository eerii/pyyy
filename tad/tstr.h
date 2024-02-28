#pragma once

#include "tvec.h"

// Un string é simplemente un vector de caracteres
#define Str Vec(char)

// Engade un caracter ó final do string, preservando o caracter de terminación
//      @param S: String no que engadir
//      @param C: Caracter a engadir
#define str_push(S, C) vec_ins(S, S.len - 2, C)

// Xunta dous strings correctamente
//      @param S: String no que engadir
//      @param A: String a engadir
#define str_append(S, A)                                                       \
    do {                                                                       \
        vec_pop(S);                                                            \
        vec_append(S, A);                                                      \
    } while (0)
