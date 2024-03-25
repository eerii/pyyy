// Convertir regex a AFN
//
// A sintaxe non é estándar e está algo limitada. En particular, permítense:
//      - Alternancia a|b
//      - Concatenación ab
//      - Cero ou mais a*
//      - Un ou mais a+
//      - Un ou ningún a?
//
// Recoméndase utilizar abundantes parénteses () para definir claramente a
// prioridade de operacións
//
// Pódense escapar os caracteres da sintexe utilizando '\\'
// Ademáis, engade as seguintes clases de caracteres para facilitar escribir
// expresións e para mellorar e eficiencia e cantidade de memoria utilizada.
// Estas clases están baseadas na sintaxe de python:
//      - Letras, minusculas e maiusculas \\w -> [a-zA-Z]
//      - Dixitos \\d -> [0-9]
//      - Dixitos sen cero \\D -> [1-9]
//      - Dixito octal \\o -> [0-7]
//      - Dixito hexadecimal \\x -> [0-9a-fA-F]
//      - Espazos \\s -> \s
//      - Caracteres permitidos nun string con comiñas dobres \\" -> [^"\n]
//      - Caracteres permitidos nun string con comiás simples \' -> [^\n']
//      - Calqueira . -> .

#pragma once

#include "afn.h"

// Convirte unha expresión regular a un AFN
//      @param regex: Expresión regular a convertir
AFN regex_to_afn(const char* regex);
