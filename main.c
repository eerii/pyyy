#include <stdio.h>
#include <string.h>

#include "entrada.h"
#include "lexico.h"
#include "tad/thash.h"
#include "tad/tvec.h"
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

// Implementar string que crece?

// En el main tenemos 3 fases
// - Inicialización (estructuras de datos como la tabla de simbolos (crear
// memoria, cargar palabras reservadas))
// - Ejecución (vamos pidiendo el siguiente componente léxico hasta el final)
// - Finalización (liberar toda la memoria)

i32 main() {
    Vec(int) v;
    Vec(int) y;
    Vec(float) z;

    int _init[] = {1, 2, 3, 4};
    vec_init_from(v, _init);
    vec_pop(v);
    vec_push(v, 5);
    vec_push(v, 6);
    printf("%d %d %d %d %d %d\n", v.data[0], v.data[1], v.data[2], v.data[3], v.data[4], v.data[5]);
    printf("%d %d\n", v.len, v.cap);

    vec_init(y);
    printf("%d %d %d %d\n", y.data[0], y.data[1], y.data[2], y.data[3]);
    printf("%d %d\n", y.len, y.cap);

    vec_resize(y, 2, 1);
    vec_push(y, 3);
    vec_del(y, 0);
    printf("%d %d %d %d\n", y.data[0], y.data[1], y.data[2], y.data[3]);
    printf("%d %d\n", y.len, y.cap);

    vec_resize(v, 2, 1);
    vec_push(v, 8);
    vec_push(v, 111);
    printf("%d %d %d %d\n", *vec_get(v, 0), *vec_get(v, 1), *vec_get(v, 2), *vec_get(v, 3));
    printf("%d %d\n", v.len, v.cap);

    vec_for_each(v, x, printf("hola %d\n", x));
    vec_del(v, 1);
    vec_ins(v, 2, 33);

    do {
        int x = vec_pop(v);
        printf("%d %d %d\n", x, v.len, v.cap);
    } while (v.len > 0);

    vec_free(v);
    vec_free(y);
    vec_init(z);
    vec_free(z);

    Str str;
    vec_init_from(str, "hey"); 
    str_push(str, ' ');
    str_push(str, 'h');
    str_push(str, 'i');
    str_append(str, " hello :D");
    printf("%s\n", str.data);

    return 0;

    Arquivo* a = abrir_arquivo("wilcoxon.py");
    if (a == NULL) {
        return 1;
    }

    i32 l;
    while ((l = seguinte_lexico(a)) != EOF) {
        // ...
    }

    return 0;
}
