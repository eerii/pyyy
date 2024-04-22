#pragma once

#include "tipos/hash.h"
#include "ts.h"
#include <math.h>

typedef struct {
    void* fn;
    i8 args;
} Fn;

typedef HashTree(Str, Fn) TaboaFuncions;

// Taboa de funcions global
extern TaboaFuncions* tf;

// ········
// Funcións
// ········

static inline f64 fn_axuda() {
    pres("%s\n", "axuda:\n"
                 "\tq() - sair\n"
                 "\th() - axuda (esta pantalla)\n"
                 "\tt() - amosar taboa de simbolos\n"
                 "\tsin(x) - función trigonométrica\n"
                 "\tcos(x) - función trigonométrica\n"
                 "\ttan(x) - función trigonométrica\n"
                 "\tsqrt(x) - raíz cadrada\n"
                 "\tabs(x) - valor absoluto\n"
                 "\tpow(x, e) - potencia e de x\n");
    return NAN;
}

static inline f64 fn_taboa() {
    ts_print();
    return NAN;
}

static inline f64 fn_sin(f64 a) { return sin(a); }
static inline f64 fn_cos(f64 a) { return cos(a); }
static inline f64 fn_tan(f64 a) { return tan(a); }

static inline f64 fn_sqrt(f64 a) { return sqrt(a); }
static inline f64 fn_abs(f64 a) { return fabs(a); }
static inline f64 fn_pow(f64 a, f64 b) { return pow(a, b); }

// ······················
// Axudantes para a táboa
// ······················

static inline void tf_ins(Str key, Fn val) {
    TaboaFuncions* t = *hash_ins(tf, key, &arena);
    t->value = val;
}

static inline void* fn_get(Str key, i8 args) {
    TaboaFuncions* t = *hash_ins(tf, key, NULL);
    if (t == NULL) {
        return NULL;
    }
    Fn* f = &t->value;
    if (f->args != args) {
        return NULL;
    }
    return f->fn;
}

static inline void tf_init() {
    tf_ins(str("h"), (Fn){.fn = (void*)&fn_axuda, .args = 0});
    tf_ins(str("t"), (Fn){.fn = (void*)&fn_taboa, .args = 0});
    tf_ins(str("sin"), (Fn){.fn = (void*)&fn_sin, .args = 1});
    tf_ins(str("cos"), (Fn){.fn = (void*)&fn_cos, .args = 1});
    tf_ins(str("tan"), (Fn){.fn = (void*)&fn_tan, .args = 1});
    tf_ins(str("sqrt"), (Fn){.fn = (void*)&fn_sqrt, .args = 1});
    tf_ins(str("abs"), (Fn){.fn = (void*)&fn_abs, .args = 1});
    tf_ins(str("pow"), (Fn){.fn = (void*)&fn_pow, .args = 2});
}
