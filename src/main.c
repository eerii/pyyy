// https://docs.python.org/3/reference/lexical_analysis.html

// Arquivo de definicións
// Táboa de símbolos
// Sistema de entrada
// Xestión de errores

// Liñas físicas vs liñas lóxicas (poden ser varias físicas, emiten NEWLINE)
// Poden unirse liñas explícitamente / ou implicitamente (dentro de parénteses)
// As liñas en blanco son ignoradas
// Incluír INDENT / DEDENT
//      Aquí necesito facer scopes? Igual si que hai que eliminar da TS
//      Podo usar subarenas?

// No main temos 3 fases
// - Inicialización (estructuras de datos como a taboa de simbolos (crear
// memoria, cargar palabras reservadas))
// - Execución (imos pedindo o siguiente compoñente léxico ata o final)
// - Finalización (liberar toda a memoria, feito grazas á arena)

#include "lexico.h"
#include "tipos/arena.h"
#include "estados/afn.h"

// Definición da arena global
Arena arena;

i32 main() {
    arena_init(&arena);

    VecEstado v;
    vec_init(v);

    AFN aa = afn_atomic('a');
    AFN bb = afn_atomic('b');
    AFN afn = afn_and(&aa, &bb);
    Estado* e = afn.inicio;

    const char* s = "ccaab";
    for (u32 i = 0; i < strlen(s); ++i) {
        afn_delta(e, s[i], &v);
        for (u32 j = 0; j < v.len; ++j) {
            printf("%p,", v.data[j]);
        }
        printf("\n");
        if (v.len > 0) {
            e = v.data[0]; // TODO: Las dos ramas
            vec_clear(v);
        }
        printf("AFN: %p %d-%d\n", e, e->trans[0], e->trans[1]); 
    }
    printf("Aceptado: %s\n\n", e == afn.fin ? "true" : "false");

    Arquivo* a = abrir_arquivo("docs/wilcoxon.py");
    if (a == NULL) {
        return 1;
    }

    i32 l;
    while ((l = seguinte_lexico(a)) != EOF) {
        // ...
    }

    log("tam arena: %lu/%lu\n", arena_len(arena), arena_cap(arena));
    arena_free(&arena);
    return 0;
}
