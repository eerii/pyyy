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

// Operadores e delimitadores (usamos ASCII)
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

// ··········
// Utilidades
// ··········

#define C_RESET "\x1b[0m"
#define C_RED "\x1b[31m"
#define C_GREEN "\x1b[32m"
#define C_YELLOW "\x1b[33m"
#define C_BLUE "\x1b[34m"
#define C_BOLD "\x1b[1m"

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
#define log(FMT, ...) print(C_BLUE, FMT, __VA_ARGS__)
#define err(FMT, ...) print(C_RED, FMT, __VA_ARGS__)

#define e_compatible(x, T) _Generic((x), T: 1, default: 0)

// Se usamos clang (non soporta todo c23 todavía), alias auto a __auto_type
#ifdef __clang__
#define auto __auto_type
#endif
