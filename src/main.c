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

    Fragmento afn = afn_init(regex_afn("a"));
    printf("%c: %b\n", 'a', afn_step(&afn, 'a'));
    printf("%c: %b\n", 'a', afn_step(&afn, 'a'));
    printf("%c: %b\n", 'b', afn_step(&afn, 'b'));
    printf("%c: %b\n", 'a', afn_step(&afn, 'a'));

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
