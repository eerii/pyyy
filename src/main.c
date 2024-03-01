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

// En el main tenemos 3 fases
// - Inicialización (estructuras de datos como la tabla de simbolos (crear
// memoria, cargar palabras reservadas))
// - Ejecución (vamos pidiendo el siguiente componente léxico hasta el final)
// - Finalización (liberar toda la memoria)

#include "lexico.h"
#include "tipos/arena.h"

#include "estados/afd.h"

// Definición da arena global
Arena arena;

i32 main() {
    AFD hola;
    hola.test = 0;

    return 0;

    // ---

    arena_init(&arena);

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
