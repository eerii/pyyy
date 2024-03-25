#include "regex.h"

// -----------------
// Funcións internas
// -----------------

AFN _afn_expr_rep(char** c);
AFN _afn_expr_char(char** c);

// Convirte unha expresión básica nun AFN
// É a chamada de función principal para facer o algoritmo recursivo
// Xestiona os operadores alternancia e concatenación
//      @param c: Punteiro ó punto da expresión regular no que estamos
AFN _afn_expr(char** c) {
    AFN a, b, res;
    char ch;
    for (a = _afn_expr_rep(c);; a = res) {
        ch = **c;

        // Fin da expresión
        if (ch == '\0' || ch == ')') {
            return a;
        }

        // Alternancia a|b
        if (ch == '|') {
            *c += 1; // Seguinte simbolo
            b = _afn_expr_rep(c);
            res = afn_or(&a, &b);
        }
        // Concatenación ab
        else {
            b = _afn_expr_rep(c);
            res = afn_and(&a, &b);
        }
    }
    return res; // Non debería chegar aquí
}

// Seguinte paso no algoritmo recursivo
// Xestiona os operadores de repetición (*, +, ?)
//      @param c: Punteiro ó punto da expresión regular no que estamos
AFN _afn_expr_rep(char** c) {
    AFN a = _afn_expr_char(c), res;
    char ch = **c;

    switch (ch) {
    case '*': // Kleene a*
        res = afn_cero_ou_mais(&a);
        break;
    case '+': // Positivo a+
        res = afn_un_ou_mais(&a);
        break;
    case '?': // Opcional a?
        res = afn_cero_ou_un(&a);
        break;
    default:
        return a; // Non ten operador de repetición
    }

    *c += 1; // Seguinte símbolo
    return res;
}

// Paso final do algoritmo recursivo
// Faise cargo dos caracteres e dos parénteses
// Tamén reemplaza os caracteres escapados con \ polas suas clases
//      @param c: Punteiro ó punto da expresión regular no que estamos
AFN _afn_expr_char(char** c) {
    AFN res;
    char ch = **c;
    bool escaped = false;

    if (ch == '.') { // Calqueira
        ch = TRANS_ANY;
    }

    if (ch == '\\') { // Caracter escapado
        *c += 1;      // Seguinte símbolo
        switch (**c) {
        case 'w':
            ch = TRANS_LETRA;
            break;
        case 'd':
            ch = TRANS_DIXITO;
            break;
        case 'D':
            ch = TRANS_DIXITO_NON_CERO;
            break;
        case 'o':
            ch = TRANS_DIXITO_OCTAL;
            break;
        case 'x':
            ch = TRANS_DIXITO_HEX;
            break;
        case 's':
            ch = TRANS_ESPAZO;
            break;
        case '"':
            ch = TRANS_SHORTSTRING_DOUBLE;
            break;
        case '\'':
            ch = TRANS_SHORTSTRING_SINGLE;
            break;
        default:
            ch = **c;
            escaped = true;
        }
    }

    if (ch == '(' && !escaped) { // Parénteses
        *c += 1;                 // Seguinte símbolo
        res = _afn_expr(c);
        if (**c != ')' && !escaped) {
            err("os parénteses non están balanceados\n");
        }
    } else {
        res = afn_atomic(ch);
    }

    *c += 1; // Seguinte símbolo
    return res;
}

// ---
// API
// ---

// Convirte unha expresión regular a un AFN
AFN regex_to_afn(const char* regex) {
    dbg("regex: %s\n", regex);
    return _afn_expr((char**)(&regex));
}
