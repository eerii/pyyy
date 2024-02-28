#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thash.h"

u32 hash(Key key, u32 capacidade);

bool necesita_redimensionar(TablaHash* tabla);

TablaHash* redimensionar(TablaHash* tabla);

Nodo* add_nodo(Key key, Value value) {
    if (key == NULL) {
        err("%s\n", "a chave é nula");
        return NULL;
    }

    if (strlen(key) > MAX_LEN) {
        err("%s\n", "a chave excede a lonxitude máxima");
        return NULL;
    }

    Nodo* nodo = malloc(sizeof(Nodo));
    if (nodo == NULL) {
        err("%s\n", "non se puido alocar memoria");
        return NULL;
    }
    nodo->sig = NULL;

    strcpy(nodo->key, key);
    nodo->value = value;

    return nodo;
}

bool del_nodo(Nodo** nodo, Key key);

void limpar(Nodo** cabeza) {
    Nodo* n = *cabeza;
    Nodo* nn;

    while (n != NULL) {
        nn = n;
        n = nn->sig;
        free(n);
        log("%s\n", "eliminando");
    }

    *cabeza = NULL;
}

Value valor(Nodo* nodo, Key key);
