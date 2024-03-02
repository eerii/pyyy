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
    e->fin = false;
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
void _afn_visit(const Estado* e, VecEstado* v) {
    u32 it;
    for (i8 i = 0; i < _trans_count(e); i++) {
        vec_find_eq((*v), e->to[i], it);
        if (it == v->len) {
            vec_push((*v), e->to[i]);
            _afn_visit(e->to[i], v);
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
void _afn_graph_alcanzables(const Estado* e, VecEstado* v, FILE* f) {
    u32 it;
    for (u8 i = 0; i < _trans_count(e); ++i) {
        _afn_graph_trans(e, i, f);
        vec_find_eq((*v), e->to[i], it);
        if (it == v->len) {
            vec_push((*v), e->to[i]);
            _afn_graph_alcanzables(e->to[i], v, f);
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

// Libera a memoria dun afn
void afn_free(AFN* a) {
    VecEstado v;
    vec_init_from_n(v, a->inicio, 1);
    _afn_visit(a->inicio, &v);
    vec_for_each(v, e, arena_del(&arena, (u8*)e, sizeof(Estado)));
    vec_free(v);
}

// Representa o autómata en formato graphviz
void afn_graph(const char* regex, const AFN* a, FILE* f) {
    VecEstado v;
    vec_init_from_n(v, a->inicio, 1);
    fprintf(f,
            "digraph finite_state_machine {\n"
            "    rankdir=LR;\n    size=\"8,5\"\n"
            "    labelloc=\"b\";\n    label=\"Regex : %s\";"
            "    node [shape = doublecircle label=\"\"]; addr_%p\n"
            "    node [shape = circle]\n",
            regex, (void*)a->fin);
    _afn_graph_alcanzables(a->inicio, &v, f);
    fprintf(f, "    node [shape = none label=\"\"]; start\n"); /* start mark */
    fprintf(f, "    start -> addr_%p [ label = \"start\" ]\n}\n",
            (void*)a->inicio);
    vec_free(v);
}
