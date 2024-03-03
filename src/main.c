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

// Revisar C23
// - constexpr
// - compound literals
// - auto
// - varadic functions (log etc)

#include "tipos/arena.h"

#include "estados/afd.h"
#include "estados/afn.h"
#include "lexico.h"

// Definición da arena global
Arena arena;

i32 main() {
    arena_init(&arena);

    AFN aa = afn_atomic('a');
    AFN bb = afn_atomic('b');
    AFN bbb = afn_and(&aa, &bb);
    AFN ee = afn_atomic('a');
    AFN dd = afn_atomic('a');
    AFN aaa = afn_cero_ou_mais(&dd);
    AFN ddd = afn_and(&ee, &aaa);
    AFN cc = afn_or(&bbb, &ddd);

    AFN afn = afn_un_ou_mais(&cc);
    AFD afd _U_ = afn_to_afd(&afn);

    /*FILE* graph = fopen("docs/afn.dot", "w");
    afn_graph("(ab|aa*)+", &afn, graph);
    fclose(graph);

    FILE* graph2 = fopen("docs/afd.dot", "w");
    afd_graph(&afd, graph2);
    fclose(graph2);*/

    const char* s = "ccab";

    VecEstado vi = vec_new_from_n(VecEstado, afn.inicio, 1);
    VecEstado vs = vec_new(VecEstado);

    for (u32 i = 0; i < strlen(s); ++i) {
        printf("\n%c (%d)\n  actual:\n", s[i], s[i]);
        vec_for_each(vi, e,
                     printf("    %p %d-%d\n", e, e->trans[0], e->trans[1]));
        printf("  seguinte:\n");
        vec_for_each(vs, e,
                     printf("    %p %d-%d\n", e, e->trans[0], e->trans[1]));
        printf("\n");

        VecEstado tmp = vi;
        vi = vs;
        vs = tmp;
        vec_clear(vs);
    }

    bool aceptado = false;
    vec_for_each(vi, e, aceptado |= e == afn.fin);
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
    Tumba* t = arena.eliminados;
    u32 num_tumbas = 0;
    u32 tam_tumbas = 0;
    log("tumbas:");
    while (t) {
        printf("%u:%u,", num_tumbas++, t->tam);
        tam_tumbas += t->tam;
        t = t->sig;
    }
    printf("\n");
    log("total: %u, tam: %u\n", num_tumbas, tam_tumbas);
    log("arena ocupada real: %u\n", (u32)arena_len(arena) - tam_tumbas);

    arena_free(&arena);
    return 0;
}
