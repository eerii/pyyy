#include "afn.h"
#include "../tipos/arena.h"

Estado* estado() {
    Estado* e = arena_push_struct(&arena, Estado);
    Trans null_t = TRANS_NONE;
    e->to[0] = e->to[1] = NULL;
    e->trans[0] = e->trans[1] = null_t;
    return e;
}

u8 trans_count(const Estado* e) {
    if (e->trans[1] != TRANS_NONE) {
        return 2;
    }
    if (e->trans[0] != TRANS_NONE) {
        return 1;
    }
    return 0;
}

bool afn_add(Estado* dende, char c, Estado* ata) {
    int i = trans_count(dende);
    if (i >= 2) {
        return false;
    }
    dende->trans[i] = c;
    dende->to[i] = ata;
    return true;
}

bool afn_add_epsilon(Estado* dende, Estado* ata) {
    return afn_add(dende, TRANS_EPSILON, ata);
}

AFN afn_atomic(char c) {
    AFN a = {estado(), estado()};
    afn_add(a.inicio, c, a.fin);
    return a;
}

// ab
AFN afn_and(const AFN* a, const AFN* b) {
    AFN c = {a->inicio, b->fin};
    afn_add_epsilon(a->fin, b->inicio);
    return c;
}

// a|b
AFN afn_or(const AFN* a, const AFN* b) {
    AFN c = {estado(), estado()};
    afn_add_epsilon(c.inicio, a->inicio);
    afn_add_epsilon(c.inicio, b->inicio);
    afn_add_epsilon(a->fin, c.fin);
    afn_add_epsilon(b->fin, c.fin);
    return c;
}

void afn_delta(const Estado* e, const char c, VecEstado* v) {
    for (u8 i = 0; i < trans_count(e); ++i) {
        printf("%d %d\n", e->trans[i], c);
        if (e->trans[i] == TRANS_EPSILON || e->trans[i] == c) {
            vec_push((*v), e->to[i]);
        }
    }
}
