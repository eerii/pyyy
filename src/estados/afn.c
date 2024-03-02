#include "afn.h"
#include "../tipos/arena.h"

// -----------------
// Funcións internas
// -----------------

// Crea un novo estado vacío
Estado* _estado() {
    Estado* e = arena_push_struct(&arena, Estado);
    Trans null_t = TRANS_NONE;
    e->to[0] = e->to[1] = NULL;
    e->trans[0] = e->trans[1] = null_t;
    return e;
}

// Devolve o número de transicións dun estado (0-2)
u8 _trans_count(const Estado* e) {
    if (e->trans[1] != TRANS_NONE) {
        return 2;
    }
    if (e->trans[0] != TRANS_NONE) {
        return 1;
    }
    return 0;
}

// Engade unha transición de un estado a outro cun caracter
bool _afn_add(Estado* dende, char c, Estado* ata) {
    int i = _trans_count(dende);
    if (i >= 2) {
        return false;
    }
    dende->trans[i] = c;
    dende->to[i] = ata;
    return true;
}

// Visita tódolos estados e crea un vector con eles
void _afn_visit(const Estado* e, Set** s) {
    for (i8 i = 0; i < _trans_count(e); i++) {
        if (!set_ins(s, e->to[i], &arena)) {
            _afn_visit(e->to[i], s);
        }
    }
}

// Escribe unha transición no arquivo graphviz
void _afn_graph_trans(const Estado* e, i8 to, FILE* f) {
    switch (e->trans[to]) {
    case TRANS_NONE:
        break;
    case TRANS_EPSILON:
        fprintf(f, "    addr_%p -> addr_%p [ label = \"\u03B5\" ];\n", (void*)e,
                (void*)e->to[to]);
        break;
    default:
        fprintf(f, "    addr_%p -> addr_%p [ label = \"%c\" ];\n", (void*)e,
                (void*)e->to[to], e->trans[to]);
        break;
    }
}

// Escribe as transicións dos estados alcanzables
void _afn_graph_alcanzables(const Estado* e, FILE* f) {
    Set* s = 0;
    u32 i = 0;
    _afn_visit(e, &s);
    set_for_each(s, e, _afn_graph_trans(e->key, i++, f));
    set_free(s);
}

// Calcula recursivamente a clausura dun estado
void _afn_clausura_rec(const Estado* e, Set* s) {
    for (u8 i = 0; i < _trans_count(e); ++i) {
        if (e->trans[i] != TRANS_EPSILON) {
            continue;
        }
        if (!set_ins(&s, e->to[i], &arena)) {
            _afn_clausura_rec(e->to[i], s);
        }
    }
}

// ---
// API
// ---

// Crea un autómata con dous estados que acepta o caracter c
AFN afn_atomic(char c) {
    AFN a = {_estado(), _estado()};
    _afn_add(a.inicio, c, a.fin);
    return a;
}

// Concatenación (ab)
// > A -> (B)
AFN afn_and(const AFN* a, const AFN* b) {
    AFN c = {a->inicio, b->fin};
    _afn_add(a->fin, TRANS_EPSILON, b->inicio);
    return c;
}

// Alternancia (a|b)
//      -> A -
// > C -|    |-> ()
//      -> B -
AFN afn_or(const AFN* a, const AFN* b) {
    AFN c = {_estado(), _estado()};
    _afn_add(c.inicio, TRANS_EPSILON, a->inicio);
    _afn_add(c.inicio, TRANS_EPSILON, b->inicio);
    _afn_add(a->fin, TRANS_EPSILON, c.fin);
    _afn_add(b->fin, TRANS_EPSILON, c.fin);
    return c;
}

// Kleene (a*)
//   |-----|
//   |     |
//   v  -> A
// > C -|
//      -> ()
AFN afn_cero_ou_mais(const AFN* a) {
    AFN c = {_estado(), _estado()};
    _afn_add(c.inicio, TRANS_EPSILON, a->inicio);
    _afn_add(c.inicio, TRANS_EPSILON, c.fin);
    _afn_add(a->fin, TRANS_EPSILON, c.inicio);
    return c;
}

// Positivo (a+)
//   |----|
//   v    |
// > C -> A -> ()
AFN afn_un_ou_mais(const AFN* a) {
    AFN c = {_estado(), _estado()};
    _afn_add(c.inicio, TRANS_EPSILON, a->inicio);
    _afn_add(a->fin, TRANS_EPSILON, c.inicio);
    _afn_add(a->fin, TRANS_EPSILON, c.fin);
    return c;
}

/// Opcional (a?)
//      -> A -
// > C -|    |-> ()
//      ------
AFN afn_cero_ou_un(const AFN* a) {
    AFN c = {_estado(), a->fin};
    _afn_add(c.inicio, TRANS_EPSILON, a->inicio);
    _afn_add(c.inicio, TRANS_EPSILON, a->fin);
    return c;
}

// Calcula a clausura para un estado
Set* afn_clausura(const Estado* e) {
    Set* c = 0;
    set_ins(&c, e, &arena);
    _afn_clausura_rec(e, c);
    return c;
}

// Calcula a clausura para múltiples estados
Set* afn_clausura_set(Set* s) {
    Set* c = 0;
    set_for_each(s, e, {
        set_ins(&c, e->key, &arena);
        _afn_clausura_rec(e->key, c);
    });
    return c;
}

// Obtén os símbolos usados
Str afn_simbolos(const AFN* a) {
    Str simbolos;
    vec_init(simbolos);

    Set* s = 0;
    set_ins(&s, a->inicio, &arena);
    _afn_visit(a->inicio, &s);

    set_for_each(s, e, {
        if (e->key->trans[0] > TRANS_EPSILON) {
            vec_push(simbolos, e->key->trans[0]);
        }
        if (e->key->trans[1] > TRANS_EPSILON) {
            vec_push(simbolos, e->key->trans[1]);
        }
    });

    set_free(s);
    return simbolos;
}

// Libera a memoria dun afn
void afn_free(AFN* a) {
    Set* s = 0;
    set_ins(&s, a->inicio, &arena);
    _afn_visit(a->inicio, &s);
    set_for_each(s, e, arena_del(&arena, (u8*)(e->key), sizeof(Estado)));
    set_free(s);
}

// Representa o autómata en formato graphviz
void afn_graph(const char* regex, const AFN* a, FILE* f) {
    Set* s = 0;
    set_ins(&s, a->inicio, &arena);

    fprintf(f,
            "digraph finite_state_machine {\n"
            "    rankdir=LR;\n    size=\"8,5\"\n"
            "    labelloc=\"b\";\n    label=\"Regex : %s\";"
            "    node [shape = doublecircle label=\"\"]; addr_%p\n"
            "    node [shape = circle]\n",
            regex, (void*)a->fin);

    _afn_visit(a->inicio, &s);
    set_for_each(s, e, {
        for (u32 j = 0; j < _trans_count(e->key); j++) {
            _afn_graph_trans(e->key, j, f);
        }
    });

    fprintf(f, "    node [shape = none label=\"\"]; start\n"); /* start mark */
    fprintf(f, "    start -> addr_%p [ label = \"start\" ]\n}\n",
            (void*)a->inicio);

    set_free(s);
}
