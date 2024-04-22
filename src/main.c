// https://docs.python.org/3/reference/lexical_analysis.html

// ---------------------------
// Analizador léxico de python
// ---------------------------

// Uso:
//      `make (debug) run (arquivo)`
//      - `debug`: Activa a macro DEBUG e imprime mais diagnósticos
//         Esto fai que o programa estea menos optimizado
//      - `arquivo`: Indica o arquivo a analizar. Por defecto é wilcoxon.py

// Estructura do código (dentro de `src`):
//      - `definicions`: Arquivo cabeceira importado por todos os demais
//                       Contén definicións de tipos, lexemas, funcións útiles
//      - `ts`: Abstracción da taboa de símbolos, utiliza `hash`
//      - `lexico`: Analizador léxico utilizando flex
//      - `tipos`: Tipos de datos abstractos
//          - `arena`: Implementación dun alocador de arena. Xestiona toda a
//                     memoria do programa e permite unha limpeza fácil.
//          - `vec`: Lista dinámica xenérica
//          - `str`: Cadea de caracteres implementada enriba de `vec`
//          - `hash`: Árbore hash trie xenérica
//          - `centinela`: Lista dobre centinela para o sistema de entrada

// No main temos 3 fases
//      - Inicialización (estructuras de datos como a taboa de simbolos,
//        reservar memoria, cargar palabras reservadas)
//      - Execución (imos pedindo o siguiente compoñente léxico ata o final)
//      - Finalización (liberar toda a memoria, feito grazas á arena)

#include "sintactico.tab.h"
#include "tipos/arena.h"
#include "ts.h"

// Definición das variables globais (declaradas en `definicions.h`)
Arena arena;
TaboaSimbolos* ts;

i32 main(int argc, char* argv[]) {
    // Inicializamos os subsistemas
    arena_init(&arena);
    if (argc > 1) {
        arquivo_init(argv[1]);
    }

    // Chama ó analizador
    yyparse();

    // Imprime a táboa de símbolos final
    ts_print();

    // Imprime detalles sobre o uso de memoria
    arena_print(&arena);

    // Todas as alocacións do programa están feitas na arena de memoria
    // Por iso limpar a memoria é moi sinxelo e pode facerse cun só comando
    arena_free(&arena);
    return 0;
}
