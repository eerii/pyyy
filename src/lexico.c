#include <ctype.h>

#include "lexico.h"
#include "tipos/str.h"

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
    }

    return 0;
}
