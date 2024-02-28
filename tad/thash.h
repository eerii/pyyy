#include "../definicions.h"

#define CAPACIDADE_INICIAL 10
#define FACTOR_CRECEMENTO 2

typedef char* Key;
typedef i32 Value;

typedef struct TNodo {
    char key[MAX_LEN];
    Value value;
    struct TNodo* sig;
} Nodo;

typedef struct {
    Nodo** nodos;
    u32 capacidade;
    u32 n;
} TablaHash;

TablaHash* crear_tabla_hash(u32 capacidade);

bool add(TablaHash** tabla, Key key, Value value);

bool get(TablaHash* tabla, Key key);

bool del(TablaHash* tabla, Key key);
