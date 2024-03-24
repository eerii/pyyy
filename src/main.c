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
// - varadic functions (info etc)

// Gestión de errores
// Mirar MUCHO la memoria

// Imprimir la tabla de simbolos inicial/final

#include "tipos/arena.h"

#include "lexico.h"

// Definición da arena global
Arena arena;

i32 main() {
    arena_init(&arena);
    automatas_init();

    Centinela c;
    if (!centinela_init(&c, "docs/wilcoxon.py")) {
        return 1;
    }

    i32 l;
    i32 limite = 0;
    while ((l = seguinte_lexico(&c)) != EOF) {
        limite++;
        if (limite == 3)
            break;
        // ...
    }
    info("fin de arquivo\n");

    info("tam arena: %lu/%lu\n", arena_len(arena), arena_cap(arena));
    Tumba* t = arena.eliminados;
    u32 num_tumbas = 0;
    u32 tam_tumbas = 0;
    info("tumbas:");
    while (t) {
        printf("%u:%u,", num_tumbas++, t->tam);
        tam_tumbas += t->tam;
        t = t->sig;
    }
    printf("\n");
    info("total: %u, tam: %u\n", num_tumbas, tam_tumbas);
    info("arena ocupada real: %u\n", (u32)arena_len(arena) - tam_tumbas);

    arena_free(&arena);
    return 0;
}
