# pyyy 🌿

un pequeno analizador léxico de python.
restrinxe a especificación ó atopado no arquivo `wilcoxon.py`.
utiliza autómatas formais baseados en expresións regulares para definir as regras léxicas.

## uso

```
make (debug|centinela) run (arquivo)
```

- `debug`: activa a macro `DEBUG` e imprime mais diagnósticos. esto fai que o programa estea menos optimizado. xera representacións gráficas dos AFD en formato graphviz (poden verse online en https://dreampuf.github.io/GraphvizOnline)
- `centinela`: activa `DEBUG`, e a maiores `DEBUG_CENTINELA`. para cada acción sobre o centinela amosa os punteiros incio e actual
- `arquivo`: indica o arquivo a analizar. por defecto é `wilcoxon.py`

## estructura do código
     
- `definicions`: arquivo cabeceira importado por todos os demais. contén definicións de tipos, lexemas, funcións útiles
 - `lexico`: módulo do analizador léxico
 - `entrada`: abstracción do sistema de entrada, utiliza `centinela`
 - `ts`: abstracción da taboa de símbolos, utiliza `hash`
 - `tipos`: tipos de datos abstractos
     - `arena`: implementación dun alocador de arena. xestiona toda a memoria do programa e permite unha limpeza fácil.
     - `vec`: lista dinámica xenérica e polimórfica
     - `str`: cadea de caracteres implementada enriba de `vec`
     - `hash`: árbore hash trie xenérica sen restricción de tamaño
     - `centinela`: lista dobre centinela para o sistema de entrada
 - `estados`: xeración de autómatas en base a expresións regulares
     - `afd`: automata finito determinado compilado que pode executarse
     - `afn`: automata finito non determinado (paso intermedio)
     - `regex`: transforma unha expresión regular nun autómata
## todo

- [x] estructuras de datos
    - [x] array dinámico
    - [x] string
    - [x] arena de memoria
    - [x] hash map
- [x] autómatas formais
    - [x] afn epsilon
    - [x] afn a afd
        - [x] calculo clausuras
        - [x] transicións
        - [x] crear afd
        - [ ] minimizar
    - [x] executar afd
    - [x] regex a afn
        - [x] clases de caracteres (numeros, letras...)
- [ ] símbolos especiais
    - [ ] NEWLINE - liñas lóxicas
    - [ ] INDENT / DEDENT - indentación
        - [ ] scopes e scratch arenas
    - [x] comentarios
        - [ ] comentario de encoding
    - [x] liñas en branco
- [x] sistema de entrada
    - [x] buffer dobre centinela
    - [x] cargar outro buffer
    - [x] devolver caracter
    - [x] comprobación de limites
    - [x] erro simbolo demasiado largo
- [x] análise léxico
    - [x] palabras clave
    - [x] identificadores
    - [x] operadores
    - [x] literals
        - [x] números
        - [x] strings
    - [x] caracteres especiais
- [x] sistema de erros
- [x] macros varádicas
