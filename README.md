# pyyy 🌿

un pequeno analizador léxico de python.
restrinxe a especificación ó atopado no arquivo `wilcoxon.py`.
utiliza [flex](https://github.com/westes/flex) con expresións regulares para definir as regras léxicas.
para a versión sen flex ver a [rama principal](https://github.com/eerii/pyyy)

## uso

```
make (debug) run (arquivo)
```

- `debug`: activa a macro `DEBUG` e imprime mais diagnósticos. esto fai que o programa estea menos optimizado. xera representacións gráficas dos AFD en formato graphviz (poden verse online en https://dreampuf.github.io/GraphvizOnline)
- `arquivo`: indica o arquivo a analizar. por defecto é `wilcoxon.py`

## estructura do código
     
- `definicions`: arquivo cabeceira importado por todos os demais. contén definicións de tipos, lexemas, funcións útiles
 - `lexico`: módulo do analizador léxico escrito en flex
 - `ts`: abstracción da taboa de símbolos, utiliza `hash`
 - `tipos`: tipos de datos abstractos
     - `arena`: implementación dun alocador de arena. xestiona toda a memoria do programa e permite unha limpeza fácil.
     - `vec`: lista dinámica xenérica e polimórfica
     - `str`: cadea de caracteres implementada enriba de `vec`
     - `hash`: árbore hash trie xenérica sen restricción de tamaño
     - `centinela`: lista dobre centinela para o sistema de entrada
