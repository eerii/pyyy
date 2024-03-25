#include "lexico.h"
#include "tipos/hash.h"
#include "tipos/str.h"

typedef HashTree(Str, u16) TaboaSimbolos;

// Taboa de símbolos global
extern TaboaSimbolos* ts;

// Inserta un par valor/chave na taboa de símbolos se non existe
// Tamén pode gardar o resultado nun lexema
//      @param key: Chave a insertar
//      @param val: Valor a insertar
//      @param l: Lexema a crear, opcional
static inline void ts_ins(Str key, u16 val, Lexema* l) {
    TaboaSimbolos* t = *hash_ins(ts, key, NULL);

    if (!t) {
        t = *hash_ins(ts, key, &arena);
        t->value = val;
    }

    if (l) {
        l->codigo = t->value;
        l->valor = &t->key;
    }
}

// Imprime a táboa de símbolos (so en debug)
static inline void ts_print() {
#ifdef DEBUG
    dbg("-------ts-------\n");
    hash_for_each(ts, h, { dbg("%s %d\n", h->key.data, h->value); });
    dbg("----------------\n\n");
#endif
}

// Inicializa a táboa de símbolos cargando todas as palabras reservadas
static inline void ts_init() {
    ts_ins(str("and"), AND, NULL);
    ts_ins(str("as"), AS, NULL);
    ts_ins(str("assert"), ASSERT, NULL);
    ts_ins(str("async"), ASYNC, NULL);
    ts_ins(str("await"), AWAIT, NULL);
    ts_ins(str("break"), BREAK, NULL);
    ts_ins(str("class"), CLASS, NULL);
    ts_ins(str("continue"), CONTINUE, NULL);
    ts_ins(str("def"), DEF, NULL);
    ts_ins(str("del"), DEL, NULL);
    ts_ins(str("elif"), ELIF, NULL);
    ts_ins(str("else"), ELSE, NULL);
    ts_ins(str("except"), EXCEPT, NULL);
    ts_ins(str("False"), FALSE, NULL);
    ts_ins(str("finally"), FINALLY, NULL);
    ts_ins(str("for"), FOR, NULL);
    ts_ins(str("from"), FROM, NULL);
    ts_ins(str("global"), GLOBAL, NULL);
    ts_ins(str("if"), IF, NULL);
    ts_ins(str("import"), IMPORT, NULL);
    ts_ins(str("in"), IN, NULL);
    ts_ins(str("is"), IS, NULL);
    ts_ins(str("lambda"), LAMBDA, NULL);
    ts_ins(str("None"), NONE, NULL);
    ts_ins(str("nonlocal"), NONLOCAL, NULL);
    ts_ins(str("not"), NOT, NULL);
    ts_ins(str("or"), OR, NULL);
    ts_ins(str("pass"), PASS, NULL);
    ts_ins(str("raise"), RAISE, NULL);
    ts_ins(str("return"), RETURN, NULL);
    ts_ins(str("True"), TRUE, NULL);
    ts_ins(str("try"), TRY, NULL);
    ts_ins(str("while"), WHILE, NULL);
    ts_ins(str("with"), WITH, NULL);
    ts_ins(str("yield"), YIELD, NULL);

    ts_ins(str("match"), MATCH, NULL);
    ts_ins(str("case"), CASE, NULL);
    ts_ins(str("type"), TYPE, NULL);
    ts_ins(str("_"), UNDERSCORE, NULL);

    ts_print();
}
