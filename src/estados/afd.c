#include "afd.h"

// Estructura temporal para almacear as clausuras cando construímos un AFD
typedef struct {
    u64 hash;
    VecEstado v;
} Clausura;
typedef Vec(Clausura) VecClausura;

// Función hash para a clausura dun estado
//      @param c: Conxunto de estados
u64 _hash_clausura(const VecEstado* c) {
    u64 h = 0;
    vec_for_each((*c), e, {
        h ^= (u64)e;
        h *= 1111111111111111111u;
    });
    return h;
}

// Compara se dúas clausuras son iguais
//      @param a: Clausura 1
//      @param b: Clausura 2
bool _clausura_eq(const Clausura* a, const Clausura* b) {
    return a->hash == b->hash;
}

// Convirte un AFN nun AFD
AFD afn_to_afd(const AFN* a) {
    // Símbolos posibles
    Str simbolos = afn_simbolos(a);

    // Engade o estado inciial ó AFD
    Estado* inicio = arena_push_struct(&arena, Estado);
    *inicio = *a->inicio;
    AFD d = {inicio, NULL};

    VecClausura estados = vec_new(VecClausura);

    // Fai un bucle ata que non queden estados pendentes
    VecEstado ci = afn_clausura(a->inicio);
    vec_push(estados, ((Clausura){_hash_clausura(&ci), ci}));

    log("estado %lu: ", (u64)a->inicio & 0xfff);
    vec_for_each(ci, e, { printf("%lu,", (u64)e & 0xfff); });
    printf("\n");

    u32 i = 0;
    while (estados.len > i) {
        Clausura* c = &estados.data[i];

        // Para cada simbolo, calculamos as transicións na clausura
        vec_for_each(simbolos, s, {
            VecEstado sig = afn_delta(&c->v, s);
            VecEstado csig = afn_clausura_set(&sig);
            vec_free(sig);
            vec_push_unique(estados, ((Clausura){_hash_clausura(&csig), csig}),
                            _clausura_eq);

            log("(%c) estado %lu: ", s, c->hash & 0xfff);
            vec_for_each(csig, e, { printf("%lu,", (u64)e & 0xfff); });
            printf(" (%lu)\n", _hash_clausura(&csig) & 0xfff);

            // TODO: Engadir ó AFD
            // TODO: Crear as transicións
            // TODO: Estados finais
        });

        i++;
    }

    // TODO: Liberar vectores
    return d;
}
