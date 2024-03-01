#include "entrada.h"
#include "tipos/arena.h"

Arquivo* abrir_arquivo(const char* nome) {
    FILE* file = fopen(nome, "r");
    if (file == NULL) {
        err("non se puido abrir o arquivo '%s'\n", nome);
        return NULL;
    }
    log("abriuse o arquivo %s\n", nome);

    Arquivo* arquivo = (Arquivo*)arena_push(&arena, sizeof(Arquivo));
    arquivo->file = file;

    return arquivo;
}

i32 seguinte_caracter(Arquivo* a) {
    i32 c = fgetc(a->file);
    if (c == EOF) {
        fclose(a->file);
        log("%s", "chegouse ó final do arquivo\n");
    }

    return c;
}

void devolver_caracter(Arquivo* a, i8 c) { ungetc(c, a->file); }
