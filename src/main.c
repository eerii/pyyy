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

// TODO: Imprimir la tabla de simbolos inicial/final

#include "lexico.h"
#include "tipos/arena.h"
#include "ts.h"

// Definición das variables globais
Arena arena;
TablaSimbolos* ts;

i32 main() {
    // Inicializamos os subsistemas correspondentes
    arena_init(&arena);
    automatas_init();
    ts_init();

    // Creamos o dobre centinela do sistema de entrada
    Centinela c;
    if (!centinela_init(&c, "wilcoxon.py")) {
        return 1;
    }

    // Obtemos os tokens léxicos
    Lexema l;
    i32 limite = 0;
    while ((l = seguinte_lexico(&c)).codigo != EOF) {
        printf("%d\n", l.codigo);

        limite++;
        if (limite == 10)
            break;
        // ...
    }

    // Imprimir detalles sobre a ocupación da arena de memoria
    dbg("tam arena: %lu/%lu\n", arena_len(arena), arena_cap(arena));
    Tumba* t = arena.eliminados;
    u32 num_tumbas = 0;
    u32 tam_tumbas = 0;
    dbg("tumbas:");
    while (t) {
        printf("%u:%u,", num_tumbas++, t->tam);
        tam_tumbas += t->tam;
        t = t->sig;
    }
    printf("\n");
    dbg("total: %u, tam: %u\n", num_tumbas, tam_tumbas);
    dbg("arena ocupada real: %u\n\n", (u32)arena_len(arena) - tam_tumbas);

    // Imprimir a táboa de símbolos
    // ts_print();

    // Todas as alocacións do programa están feitas na arena de memoria
    // Por iso limpar a memoria é moi sinxelo e pode facerse cun só comando
    arena_free(&arena);
    return 0;
}
