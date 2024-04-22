#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

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

#define f32 float
#define f64 double

#define Trans i8

// ··········
// Utilidades
// ··········

// Cores de consola
#define C_RESET "\x1b[0m"
#define C_RED "\x1b[31m"
#define C_GREEN "\x1b[32m"
#define C_YELLOW "\x1b[33m"
#define C_BLUE "\x1b[34m"
#define C_MAGENTA "\x1b[35m"
#define C_CYAN "\x1b[36m"
#define C_BLACK "\x1b[30m"
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
#define dbg(FMT, ...)                                                          \
    print(C_YELLOW, C_YELLOW "debug: " C_RESET FMT, __VA_ARGS__)
#else
#define dbg(...)
#endif
#define info(FMT, ...) print(C_BLUE, C_BLUE "info: " C_RESET FMT, __VA_ARGS__)
#define err(FMT, ...) print(C_RED, C_RED "error: " C_RESET FMT, __VA_ARGS__)
#define prompt(line) printf(C_BLUE "%s> " C_RESET, line ? "\n" : "")
#define pres(FMT, ...) printf(C_YELLOW FMT C_RESET, __VA_ARGS__)

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
