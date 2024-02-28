// Implementación dunha tabla hash con árbores trie
// Utiliza unha arena de memoria e shifting da función hash para
// crear e buscar nunha árbore con n=4o
// Baseado en https://en.wikipedia.org/wiki/Hash_trie
// Referencias:
//      - https://nrk.neocities.org/articles/hash-trees-and-tries
//      - https://nullprogram.com/blog/2022/08/08

#pragma once

#include "../definicions.h"
#include "tarena.h"
#include "tstr.h"

// Tipos do hashmap
//      Nota: nesta caso non compensa facelo xenérico coma o vector xa que
//      só vamos a utilizar esta estructura para a táboa de símbolos
typedef Str HashKey;
typedef i32 HashValue;

typedef struct HashTree HashTree;
struct HashTree {
    HashTree* child[4];
    HashKey key;
    HashValue value;
};

// Modifica ou inserta unha chave no hashmap
// Se non se pasa arena, é un lookup simple e devolve NULL se non se atopa
//      @param m: Árbore hash
//      @param key: Chave a engadir ou modificar
//      @param a: Arena de memoria na que ubicar o hashmap (pode ser NULL)
HashValue* hash_ins(HashTree** m, HashKey key, Arena* a);
