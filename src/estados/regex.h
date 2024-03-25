// Convertir regex a AFN

#pragma once

#include "afn.h"

// Convirte unha expresión regular a un AFN
//      @param regex: Expresión regular a convertir
AFN regex_to_afn(const char* regex);
