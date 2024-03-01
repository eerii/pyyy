#include "lexico.h"
#include "tipos/str.h"

u32 seguinte_lexico(Arquivo* a) {
    // TODO: Mover buf a str/vec

    // TODO: Funcións de expresións regulares
    //      - Identificador alfanumérico
    //      - Número
    //      - Operadores ...
    //      Mirar se podo usar regex en c

    i32 c;
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

        // Comprobar fin de liña
        if (c == '\n' || c == '\t') {
            vec_push(buf, '\0');
            log("%s", buf.data);
            vec_clear(buf);
        }
    }

    /*
        // Comprobar comentario de liña
        // TODO: Comentario de encoding
        if (c == '\n' || c == '\t') {
            a->comentario_linha = false;
        }
        if (a->comentario_linha || c == '#') {
            a->comentario_linha = true;
            continue;
        }

        // Comprobar comentario de bloque (TODO: esto non e un comentario)
        if (c == '"' && buf[i - 1] == '"' && buf[i - 2] == '"') {
            a->comentario_bloque = !a->comentario_bloque;
            c = ' ';
            buf[i - 2] = '\0';
            i -= 2;
            continue;
        }
        if (a->comentario_bloque) {
            if (c == '"') {
                buf[i++] = c;
            } else {
                u32 j = i;
                while (buf[j - 1] == '"') {
                    buf[j-- - 1] = '\0';
                }
                i = j;
            }
            continue;
        }
    }*/

    return 0;
}
