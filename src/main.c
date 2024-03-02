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

    AFN aa = afn_atomic('a');
    AFN bb = afn_atomic('b');
    AFN cc = afn_and(&aa, &bb);
    AFN afn = afn_un_ou_mais(&cc);

    const char* s = "ccab";

    VecEstado vi, vs;
    vec_init(vi);
    vec_push(vi, afn.inicio);
    vec_init(vs);

    for (u32 i = 0; i < strlen(s); ++i) {
        for (u32 j = 0; j < vi.len; ++j) {
            afn_delta(vi.data[j], s[i], &vs);
        }

        printf("AFN: %c (%d)\n  actual:\n", s[i], s[i]);
        for (u32 i = 0; i < vi.len; i++) {
            Estado* e = vi.data[i];
            printf("    %p %d-%d\n", e, e->trans[0], e->trans[1]);
        }
        printf("  seguinte:\n");
        for (u32 i = 0; i < vs.len; i++) {
            Estado* e = vs.data[i];
            printf("    %p %d-%d\n", e, e->trans[0], e->trans[1]);
        }
        printf("\n\n");

        VecEstado tmp = vi;
        vi = vs;
        vs = tmp;
        vec_clear(vs); 
    }

    bool aceptado = false;
    vec_for_each(vi, e, aceptado |= e == afn.fin);
    printf("Aceptado: %s\n\n", aceptado ? "true" : "false");

    FILE* graph = fopen("docs/afn.dot", "w");
    afn_graph("ab", &afn, graph);
    fclose(graph);

    afn_free(&afn);

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
