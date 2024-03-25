#include "tipos/hash.h"
#include "tipos/str.h"

typedef HashTree(Str, u16) TablaSimbolos;

// Taboa de símbolos global
static TablaSimbolos* ts = NULL;

// Inserta un par valor/chave na taboa de símbolos
//      @param key: Chave a insertar
//      @param val: Valor a insertar
static inline void ts_ins(Str key, u16 val) {
    *hash_ins(ts, key, &arena) = val;
}

// Comproba se unha clave existe na taboa de símbolos
//      @param key: Chave a comprobar
//      @return: True se existe, false en caso contrário
static inline bool ts_contains(Str key) {
    return hash_ins(ts, key, NULL) != NULL;
}

// Inicializa a táboa de símbolos cargando todas as palabras reservadas
static inline void ts_init() {
    ts_ins(str("and"), AND);
    ts_ins(str("as"), AS);
    ts_ins(str("assert"), ASSERT);
    ts_ins(str("async"), ASYNC);
    ts_ins(str("await"), AWAIT);
    ts_ins(str("break"), BREAK);
    ts_ins(str("class"), CLASS);
    ts_ins(str("continue"), CONTINUE);
    ts_ins(str("def"), DEF);
    ts_ins(str("del"), DEL);
    ts_ins(str("elif"), ELIF);
    ts_ins(str("else"), ELSE);
    ts_ins(str("except"), EXCEPT);
    ts_ins(str("False"), FALSE);
    ts_ins(str("finally"), FINALLY);
    ts_ins(str("for"), FOR);
    ts_ins(str("from"), FROM);
    ts_ins(str("global"), GLOBAL);
    ts_ins(str("if"), IF);
    ts_ins(str("import"), IMPORT);
    ts_ins(str("in"), IN);
    ts_ins(str("is"), IS);
    ts_ins(str("lambda"), LAMBDA);
    ts_ins(str("None"), NONE);
    ts_ins(str("nonlocal"), NONLOCAL);
    ts_ins(str("not"), NOT);
    ts_ins(str("or"), OR);
    ts_ins(str("pass"), PASS);
    ts_ins(str("raise"), RAISE);
    ts_ins(str("return"), RETURN);
    ts_ins(str("True"), TRUE);
    ts_ins(str("try"), TRY);
    ts_ins(str("while"), WHILE);
    ts_ins(str("with"), WITH);
    ts_ins(str("yield"), YIELD);

    ts_ins(str("match"), MATCH);
    ts_ins(str("case"), CASE);
    ts_ins(str("type"), TYPE);
    ts_ins(str("_"), UNDERSCORE);
}
