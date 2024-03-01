#include <ctype.h>

#include "lexico.h"
#include "tipos/str.h"

// Función de proba
// Detecta a palabra "for"
i32 estado = 0;
void detectar_for(char c) {
    switch (estado) {
    case 0:
        if (c == 'f') {
            estado = 1;
            return;
        }
        break;
    case 1:
        if (c == 'o') {
            estado = 2;
            return;
        }
        break;
    case 2:
        if (c == 'r') {
            estado = 3;
            return;
        }
        break;
    case 3:
        if (!isalnum(c)) {
            estado = 4;
            return;
        }
    }
    estado = 0;
}

u32 seguinte_lexico(Arquivo* a) {
    // TODO: Funcións de expresións regulares
    //      - Identificador alfanumérico
    //      - Número
    //      - Operadores ...
    //      Mirar se podo usar regex en c

    i32 c;
    u32 l = 0;
    static Str buf;
    if (buf.cap == 0) {
        vec_init_res(buf, 128);
    }

    while (true) {
        c = seguinte_caracter(a);
        vec_push(buf, c);

        // Comprobar fin de arquivo
        if (c == EOF) {
            return c;
        }

        // Comentarios de liña
        if (c == '\n' || c == '\t') {
            a->comentario_linha = false;
        }
        if (a->comentario_linha || c == '#') {
            a->comentario_linha = true;
            continue;
        }

        // Detectar nova liña
        if (c == '\n') {
            l++;
        }

        // Buscar a palabra 'for'
        detectar_for(c);
        if (estado == 4) {
            log("for detectado na liña %u\n", l);
            vec_clear(buf);
        }
    }

    return 0;
}
