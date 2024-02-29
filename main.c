#include "entrada.h"
#include "lexico.h"
#include "tad/tarena.h"
#include "tad/thash.h"
#include "tad/tstr.h"

// https://docs.python.org/3/reference/lexical_analysis.html

// Arquivo de definicións
//      - Todas as palabras reservadas
//      - Identificador
//      - Operadores de máis dun caracter?
// Táboa de símbolos
//      - Hashmap / arbol binario con clave nome, dato identificador
// Sistema de entrada
// Xestión de errores

// Revisar macros e C17 para aliviar o traballo
// Separar tokenizer? Tipo crear función que divida unha liña en palabras

//      - Liñas físicas vs liñas lóxicas (poden ser varias físicas)
//      - Poden unirse liñas explícitamente / ou implicitamente (dentro de
//      parénteses, brackets ou corchetes)
//      - As liñas lóxicas levan un token NEWLINE ó final
//      - As liñas en blanco son ignoradas

// En el main tenemos 3 fases
// - Inicialización (estructuras de datos como la tabla de simbolos (crear
// memoria, cargar palabras reservadas))
// - Ejecución (vamos pidiendo el siguiente componente léxico hasta el final)
// - Finalización (liberar toda la memoria)

// TODO: Crecer arena
// TODO: Eliminar do hashmap?

i32 main() {
    Arena a;
    arena_init(&a);

    HashTree* m = NULL;
    *hash_ins(&m, str("a"), &a) = 10;
    *hash_ins(&m, str("b"), &a) = 13;
    *hash_ins(&m, str("c"), &a) = 15;
    *hash_ins(&m, str("d"), &a) = 20;
    log("%d\n", *hash_ins(&m, str("c"), 0));

    log("tam: %ld, fin: %p\n", a.actual - a.inicio, a.fin);

    arena_free(&a);

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
