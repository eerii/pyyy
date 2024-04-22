// ----------------------
// Calculadora sintáctica
// ----------------------

// Uso:
//      `make (debug) run`
//      - `debug`: Activa a macro DEBUG e imprime mais diagnósticos
//         Esto fai que o programa estea menos optimizado

// Estructura do código (dentro de `src`):
//      - `definicions`: Arquivo cabeceira importado por todos os demais
//                       Contén definicións de tipos, lexemas, funcións útiles
//      - `ts`: Abstracción da taboa de símbolos, utiliza `hash`
//      - `funcions`: Definicións das funcións do programa
//      - `lexico`: Analizador léxico utilizando flex
//      - `sintactico`: Analizador sintáctico utilizando bison
//      - `tipos`: Tipos de datos abstractos
//          - `arena`: Implementación dun alocador de arena. Xestiona toda a
//                     memoria do programa e permite unha limpeza fácil.
//          - `vec`: Lista dinámica xenérica
//          - `str`: Cadea de caracteres implementada enriba de `vec`
//          - `hash`: Árbore hash trie xenérica

// No main temos 3 fases
//      - Inicialización (estructuras de datos como a taboa de simbolos,
//        reservar memoria, cargar constantes)
//      - Execución (imos pedindo o siguiente compoñente léxico ata o final)
//      - Finalización (liberar toda a memoria, feito grazas á arena)

#include "funcions.h"
#include "sintactico.tab.h"
#include "tipos/arena.h"
#include "ts.h"

// Definición das variables globais (declaradas en `definicions.h`)
Arena arena;
TaboaSimbolos* ts;
TaboaFuncions* tf;

i32 main(int argc, char* argv[]) {
    // Inicializamos os subsistemas
    arena_init(&arena);
    ts_init();
    tf_init();

    printf("·······················\n" C_BLUE C_BOLD
           "      calculadora\n" C_RESET "utiliza h() pra a axuda\n"
           "·······················\n\n");

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
