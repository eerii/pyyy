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

// Escribe unha transición no arquivo graphviz
void _afn_graph_trans(const Estado* e, u8 to, FILE* f) {
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
    VecEstado s = vec_new(VecEstado);
    afn_visit(&s, e);
    vec_for_each(s, e, {
        for (u8 i = 0; i < _trans_count(e); i++) {
            _afn_graph_trans(e, i, f);
        }
    });
    vec_free(s);
}

// Calcula recursivamente a clausura dun estado
void _afn_clausura_rec(VecEstado* s, const Estado* e) {
    for (u8 i = 0; i < _trans_count(e); ++i) {
        if (e->trans[i] != TRANS_EPSILON) {
            continue;
        }
        if (vec_push_unique((*s), e->to[i], EQ)) {
            _afn_clausura_rec(s, e->to[i]);
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

// Obtén as transicións dun conxunto de estados con un caracter
VecEstado afn_delta(const VecEstado* s, Trans c) {
    VecEstado sig = vec_new(VecEstado);

    vec_for_each((*s), e, {
        for (u8 i = 0; i < _trans_count(e); i++) {
            if (e->trans[i] == c) {
                vec_push_unique(sig, e->to[i], EQ);
            }
        }
    });

    return sig;
}

// Visita tódolos estados e chea un vector con eles
void afn_visit(VecEstado* s, const Estado* e) {
    for (i8 i = 0; i < _trans_count(e); i++) {
        if (vec_push_unique((*s), e->to[i], EQ)) {
            afn_visit(s, e->to[i]);
        }
    }
}

// Calcula a clausura para un estado
VecEstado afn_clausura(const Estado* e) {
    VecEstado c = vec_new(VecEstado);
    vec_push(c, e);
    _afn_clausura_rec(&c, e);
    return c;
}

// Calcula a clausura para múltiples estados
VecEstado afn_clausura_set(const VecEstado* s) {
    VecEstado c = vec_new(VecEstado);
    vec_for_each((*s), e, {
        vec_push_unique(c, e, EQ);
        _afn_clausura_rec(&c, e);
    });
    return c;
}

// Comproba se a clausura contén o estado final
bool afn_final(const AFN* a, const VecEstado* c) {
    return vec_find((*c), a->fin, EQ) != c->len;
}

// Obtén os símbolos usados
Str afn_simbolos(const AFN* a) {
    Str simbolos = vec_new(Str);

    VecEstado s = vec_new(VecEstado);
    vec_push(s, a->inicio);
    afn_visit(&s, a->inicio);

    vec_for_each(s, e, {
        if (e->trans[0] > TRANS_EPSILON) {
            vec_push(simbolos, e->trans[0]);
        }
        if (e->trans[1] > TRANS_EPSILON) {
            vec_push(simbolos, e->trans[1]);
        }
    });

    vec_free(s);
    return simbolos;
}

// Libera a memoria dun afn
void afn_free(AFN* a) {
    VecEstado s = vec_new(VecEstado);
    vec_push(s, a->inicio);
    afn_visit(&s, a->inicio);
    vec_for_each(s, e, arena_del(&arena, (u8*)(e), sizeof(Estado)));
    vec_free(s);
}

// Representa o autómata en formato graphviz
void afn_graph(const AFN* a, FILE* f) {
    VecEstado s = vec_new(VecEstado);
    vec_push(s, a->inicio);

    fprintf(f,
            "digraph finite_state_machine {\n"
            "    rankdir=LR;\n    size=\"8,5\"\n"
            "    labelloc=\"b\";\n"
            "    node [shape = doublecircle label=\"\"]; addr_%p\n"
            "    node [shape = circle]\n",
            (void*)a->fin);

    afn_visit(&s, a->inicio);
    vec_for_each(s, e, {
        for (u32 j = 0; j < _trans_count(e); j++) {
            _afn_graph_trans(e, j, f);
        }
    });

    fprintf(f, "    node [shape = none label=\"\"]; start\n"); /* start mark */
    fprintf(f, "    start -> addr_%p [ label = \"start\" ]\n}\n",
            (void*)a->inicio);

    vec_free(s);
}
