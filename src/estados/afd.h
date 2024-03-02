#pragma once

#include "afn.h"

// Un AFD é un tipo especial de AFN
typedef AFN AFD;

// Convirte un AFN nun AFD
//      @param a: AFN a convertir
AFD afn_to_afd(const AFN* a);
