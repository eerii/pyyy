#include "lexico.h"

bool comentario_linha = false;

u32 seguinte_lexico(Centinela* c) {
    // TODO: Funcións de expresións regulares
    //      - Identificador alfanumérico
    //      - Número
    //      - Operadores ...
    //      Mirar se podo usar regex en c

    while (true) {
        char ch = centinela_ler(c);

        // Comprobar fin de arquivo
        if (ch == EOF) {
            return ch;
        }

        // Comprobar comentario
        if (ch == '\n' || ch == '\r') {
            comentario_linha = false;
        }
        if (comentario_linha || ch == '#') {
            comentario_linha = true;
            centinela_inicio(c);
            continue;
        }

        // Proba
        if (ch == 'i') {
            return 100;
        }
    }

    return 0;
}
