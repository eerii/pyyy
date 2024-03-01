// Implementación dun string variable facendo uso do vector dinámico
// É unha especificación que permite todos os metodos vec_
// Tamén engade as funcións específicas str_ para facilitar o traballo con
// strings e o caracter de terminación \0
//
// @author: @eerii, jose.pazos.perez@rai.usc.es

#pragma once

#include "tvec.h"

// Un string é simplemente un vector de caracteres
// Este typedef é importante xa que permite ter un tipo concreto que especificar
// nos parámetros das funcións
typedef Vec(char) Str;

// Engade un caracter ó final do string, preservando o caracter de terminación
//      @param s: String no que engadir
//      @param c: Caracter a engadir
static inline void str_push(Str* s, char c) { vec_ins((*s), s->len - 2, c); }

// Xunta dous strings correctamente
//      @param s: String no que engadir
//      @param a: String a engadir
static inline void str_append(Str* s, const char* a) {
    vec_pop((*s));
    vec_append((*s), a);
}

// Crea un Str dende un literal en linea
//      @param l: Literal string
#define str(l)                                                                 \
    (Str) { .data = (l), .len = (sizeof(l) / sizeof(l[0])) }
