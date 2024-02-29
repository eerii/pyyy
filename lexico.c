#include <ctype.h>
#include <string.h>

#include "lexico.h"

bool e_espazo(char c) { return c == ' ' || c == '\n' || c == '\t'; }
bool e_separador(char c) { return !isalnum(c) && c != '"' && c != '_'; }

void eliminar_espazos_finais(char* buf) {
    for (i32 i = strlen(buf) - 1; i >= 0; i--) {
        if (!e_espazo(buf[i])) {
            return;
        }
        buf[i] = '\0';
    }
}

void procesar_token(char* buf, int* i) {
    buf[*i] = '\0';
    eliminar_espazos_finais(buf);
    *i = 0;
    if (buf[0] != '\0') {
        printf(C_YELLOW "%s\n" C_RESET, buf);
    }
}

u32 seguinte_lexico(Arquivo* a) {
    // TODO: Mover buf a str/vec

    // TODO: Funcións de expresións regulares
    //      - Identificador alfanumérico
    //      - Número
    //      - Operadores ...
    //      Mirar se podo usar regex en c

    /*u32 c;
    u32 i = 0;
    char buf[MAX_LEN];

    while (true) {
        c = seguinte_caracter(a);

        // Comprobar fin de arquivo
        if (c == EOF) {
            return c;
        }

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

        // Engadir ó buffer
        buf[i++] = c;

        // Separar tokens
        if (e_separador(c)) {
            procesar_token(buf, &i);
        }
    }*/

    return 0;
}
