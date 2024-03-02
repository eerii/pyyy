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

#include "estados/afd.h"
#include "estados/afn.h"
#include "lexico.h"
#include "tipos/arena.h"

// Definición da arena global
Arena arena;

i32 main() {
    arena_init(&arena);

    AFN aa = afn_atomic('a');
    AFN bb = afn_atomic('b');
    AFN cc = afn_or(&aa, &bb);
    AFN afn = afn_un_ou_mais(&cc);

    afn_to_afd(&afn);
    printf("hey\n");

    /* FILE* graph = fopen("afn.dot", "w"); */
    /* afn_graph("graph", &afn, graph); */
    /* fclose(graph); */

    const char* s = "ccab";

    Set* vi = 0;
    Set* vs = 0;
    set_ins(&vi, afn.inicio, &arena);

    for (u32 i = 0; i < strlen(s); ++i) {
        printf("AFN: %c (%d)\n  actual:\n", s[i], s[i]);
        set_for_each(vi, e, printf("    %p %d-%d\n", e->key, e->key->trans[0], e->key->trans[1]));
        printf("  seguinte:\n");
        set_for_each(vs, e, printf("    %p %d-%d\n", e->key, e->key->trans[0], e->key->trans[1]));
        printf("\n\n");

        Set* tmp = vi;
        vi = vs;
        vs = tmp;
        set_free(vs); // TODO: Cambiar por clear
        vs = 0;
    }

    bool aceptado = false;
    set_for_each(vi, e, aceptado |= e->key == afn.fin);
    printf("Aceptado: %s\n\n", aceptado ? "true" : "false");

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
