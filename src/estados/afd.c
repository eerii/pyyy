#include "afd.h"

// Convirte un AFN nun AFD
AFD afn_to_afd(const AFN* a) {
    // Comeza polo estado inicial
    Estado* inicio = arena_push_struct(&arena, Estado);
    *inicio = *a->inicio;
    AFD d = {inicio, NULL};

    // Calcula a clausura inicial
    Set* cl = afn_clausura(inicio);
    set_for_each(cl, e, { log("%p\n", e->key); });

    // Símbolos posibles
    Str simbolos = afn_simbolos(a);

    vec_for_each(simbolos, s, {
        log("%c\n", s);
        // TODO: Calcular o movemento da clausura para o simbolo s
    });

    return d;
}
