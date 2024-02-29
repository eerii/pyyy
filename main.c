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

#include "entrada.h"
#include "lexico.h"
#include "tad/tarena.h"
#include "tad/thash.h"
#include "tad/tstr.h"
#include "tad/tvec.h"

i32 main() {
    Arena a;
    arena_init(&a);

    HashTree* m = NULL;
    *hash_ins(&m, str("a"), &a) = 10;
    *hash_ins(&m, str("b"), &a) = 13;
    *hash_ins(&m, str("c"), &a) = 15;
    *hash_ins(&m, str("d"), &a) = 20;
    log("%d\n", *hash_ins(&m, str("c"), 0));

    void* p = hash_ins(&m, str("c"), &a);
    arena_del_struct(&a, p, HashTree); // Esto non é seguro para o hashmap
    p = hash_ins(&m, str("b"), &a);
    arena_del_struct(&a, p, HashTree);

    Tumba* t = a.eliminados;
    while(t != NULL) {
        log("tumba %p, %lu\n", t, t->tam);
        t = t->sig;
    }

    *hash_ins(&m, str("e"), &a) = 30;
    *hash_ins(&m, str("f"), &a) = 45;

    log("tam: %ld, fin: %p\n", a.actual - a.inicio, a.fin);

    arena_free(&a);

    Str s;
    vec_init_from(s, "hey");
    printf("%s\n", s.data);

    return 0;

    /*Arquivo* a = abrir_arquivo("wilcoxon.py");
    if (a == NULL) {
        return 1;
    }

    i32 l;
    while ((l = seguinte_lexico(a)) != EOF) {
        // ...
    }

    return 0;*/
}
