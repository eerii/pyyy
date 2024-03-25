// https://docs.python.org/3/reference/lexical_analysis.html

// ---------------------------
// Analizador léxico de python
// ---------------------------

// Uso:
//      `make (debug|centinela) run (arquivo)`
//      - `debug`: Activa a macro DEBUG e imprime mais diagnósticos
//         Esto fai que o programa estea menos optimizado
//         Ademáis, xera representacións gráficas dos AFD en formato graphviz
//         (poden verse online en https://dreampuf.github.io/GraphvizOnline)
//      - `centinela`: Activa DEBUG, e a maiores DEBUG_CENTINELA
//         Para cada acción sobre o centinela amosa os punteiros incio e actual
//      - `arquivo`: Indica o arquivo a analizar. Por defecto é wilcoxon.py

// Estructura do código (dentro de `src`):
//      - `definicions`: Arquivo cabeceira importado por todos os demais
//                       Contén definicións de tipos, lexemas, funcións útiles
//      - `lexico`: Módulo do analizador léxico
//      - `entrada`: Abstracción do sistema de entrada, utiliza `centinela`
//      - `ts`: Abstracción da taboa de símbolos, utiliza `hash`
//      - `tipos`: Tipos de datos abstractos
//          - `arena`: Implementación dun alocador de arena. Xestiona toda a
//                     memoria do programa e permite unha limpeza fácil.
//          - `vec`: Lista dinámica xenérica
//          - `str`: Cadea de caracteres implementada enriba de `vec`
//          - `hash`: Árbore hash trie xenérica
//          - `centinela`: Lista dobre centinela para o sistema de entrada
//      - `estados`: Xeración de autómatas en base a expresións regulares
//          - `afd`: Automata finito determinado compilado que pode executarse
//          - `afn`: Automata finito non determinado (paso intermedio)
//          - `regex`: Transforma unha expresión regular nun autómata

// No main temos 3 fases
//      - Inicialización (estructuras de datos como a taboa de simbolos,
//        reservar memoria, cargar palabras reservadas)
//      - Execución (imos pedindo o siguiente compoñente léxico ata o final)
//      - Finalización (liberar toda a memoria, feito grazas á arena)

#include "lexico.h"
#include "tipos/arena.h"
#include "ts.h"

// Definición das variables globais (declaradas en `definicions.h`)
Arena arena;
TaboaSimbolos* ts;
SistemaEntrada entrada;

i32 main(int argc, char* argv[]) {
    // Inicializamos os subsistemas
    arena_init(&arena);
    automatas_init();
    ts_init();
    entrada_init(argc > 1 ? argv[0] : "wilcoxon.py");

    // Obtemos os tokens léxicos e os representamos
    info("análise léxico:\n\n");
    Lexema l;
    while ((l = seguinte_lexico()).codigo != EOF) {
        printf("%s%s (%d)\n" C_RESET, cor_lexema(l.codigo), l.valor->data,
               l.codigo);
    }
    printf("\n");

    // Imprimir a táboa de símbolos
    ts_print();

    // Imprimir detalles sobre o uso de memoria
    arena_print(&arena);

    // Todas as alocacións do programa están feitas na arena de memoria
    // Por iso limpar a memoria é moi sinxelo e pode facerse cun só comando
    arena_free(&arena);
    return 0;
}
