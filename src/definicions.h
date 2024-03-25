#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// ······
// Léxico
// ······

#define NEWLINE 290
#define INDENT 291
#define DEDENT 292
#define ID 293
#define LITERAL 294

// Palabras clave (300+)
#define AND 300
#define AS 301
#define ASSERT 302
#define ASYNC 303
#define AWAIT 304
#define BREAK 305
#define CLASS 306
#define CONTINUE 307
#define DEF 308
#define DEL 309
#define ELIF 310
#define ELSE 311
#define EXCEPT 312
#define FALSE 313
#define FINALLY 314
#define FOR 315
#define FROM 316
#define GLOBAL 317
#define IF 318
#define IMPORT 319
#define IN 320
#define IS 321
#define LAMBDA 322
#define NONE 323
#define NONLOCAL 324
#define NOT 325
#define OR 326
#define PASS 327
#define RAISE 328
#define RETURN 329
#define TRUE 330
#define TRY 331
#define WHILE 332
#define WITH 333
#define YIELD 334

// Palabras clave lixeiras (400+)
#define MATCH 400
#define CASE 401
#define TYPE 402
#define UNDERSCORE 403

// Operadores e delimitadores (usamos ASCII menos para os dobres, 450+)
#define OP_EQ 450
#define OP_NEQ 451
#define OP_LEQ 452
#define OP_GEQ 453
#define OP_EQ_DEF 454
#define OP_SHIFT_DER 455
#define OP_SHIFT_ESQ 456
#define OP_STAR_STAR 457

// Espazos en blanco *non* son tokens

// ·····
// Tipos
// ·····

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define Trans i8

// ··········
// Caracteres
// ··········

#define TRANS_NONE -1
#define TRANS_EPSILON -2
#define TRANS_LETRA -3               // [a-zA-Z]
#define TRANS_DIXITO -4              // [0-9]
#define TRANS_DIXITO_NON_CERO -5     // [1-9]
#define TRANS_DIXITO_OCTAL -6        // [0-7]
#define TRANS_DIXITO_HEX -7          // [0-9a-fA-F]
#define TRANS_ESPAZO -8              // isspace
#define TRANS_ANY -9                 // .
#define TRANS_SHORTSTRING_DOUBLE -10 // [^\n']
#define TRANS_SHORTSTRING_SINGLE -11 // [^\n"]

static inline Trans trans_char(Trans ch) {
    switch (ch) {
    case TRANS_EPSILON:
        return 'e';
    case TRANS_LETRA:
        return 'w';
    case TRANS_DIXITO:
        return 'd';
    case TRANS_DIXITO_NON_CERO:
        return 'D';
    case TRANS_DIXITO_OCTAL:
        return 'o';
    case TRANS_DIXITO_HEX:
        return 'x';
    case TRANS_ESPAZO:
        return 's';
    case TRANS_ANY:
        return '.';
    case TRANS_SHORTSTRING_DOUBLE:
        return '"';
    case TRANS_SHORTSTRING_SINGLE:
        return '\'';
    default:
        return ' ';
    }
}

// Tamaño máximo do lexema (limitado polos buffers de entrada e saída)
#define MAX_BUF_LEN 32

// ··········
// Utilidades
// ··········

// Cores de consola
#define C_RESET "\x1b[0m"
#define C_RED "\x1b[31m"
#define C_GREEN "\x1b[32m"
#define C_YELLOW "\x1b[33m"
#define C_BLUE "\x1b[34m"
#define C_BOLD "\x1b[1m"
#define C_UNDERLINE "\x1b[4m"

// Impresión varádica e con diferentes tipos
#define PFILE(COLOR) COLOR C_BOLD "[" __FILE__ ":%d] " C_RESET
#define PRINT_I(COLOR, FMT) printf(PFILE(COLOR) "%s", __LINE__, FMT)
#define PRINT_II(COLOR, FMT, ...)                                              \
    printf(PFILE(COLOR) FMT, __LINE__, __VA_ARGS__)
#define print(COLOR, FMT, ...)                                                 \
    PRINT_I##__VA_OPT__(I)(COLOR, FMT __VA_OPT__(, ) __VA_ARGS__)

#ifdef DEBUG
#define dbg(FMT, ...) print(C_YELLOW, FMT, __VA_ARGS__)
#else
#define dbg(...)
#endif
#define info(FMT, ...) print(C_BLUE, FMT, __VA_ARGS__)
#define err(FMT, ...) print(C_RED, FMT, __VA_ARGS__)

// Macro que compara dous valores (pasados como referencia)
#define EQ(A, B) (*A == *B)

// Comproba se un valor x é compatible co tipo T
#define COMPATIBLE(X, T) _Generic((X), T: true, default: false)

// Macro condicional
// So executa a función se *non* ten argumentos
#define _IF_NOT_TEST(...) __VA_ARGS__
#define _IF_NOT_TEST0(...) __VA_ARGS__
#define _IF_NOT_TEST1(...)
#define IF_NOT(C, ...) _IF_NOT_TEST##C(__VA_ARGS__)

// Se usamos clang (non soporta todo c23 todavía), alias auto a __auto_type
#ifdef __clang__
#define auto __auto_type
#endif

// Variables que poden non utilizarse
#define _U_ __attribute__((unused))
