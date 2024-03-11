#include "afd.h"

// -----------------
// Funcións internas
// -----------------

// Estructura temporal para almacear as clausuras cando construímos un AFD
typedef struct {
    u32 hash;
    VecEstado v;
    EstadoAFD* e;
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

// Compara se dous estados afd son iguais
//      @param a: Estado 1
//      @param b: Estado 2
bool _estado_afd_eq(const EstadoAFD* a, const EstadoAFD* b) {
    return a->hash == b->hash;
}

// Engade unha clausura a un AFD coma un novo estado
void _afd_push(AFD* a, Clausura* c, bool f) {
    if (c->hash == 0) {
        return;
    }

    afd_add_estado(a, (EstadoAFD){.hash = c->hash, .final = f});
    c->e = &a->data[a->len - 1];
}

// ---
// API
// ---

// Engade un estado a un AFD
void afd_add_estado(AFD* a, EstadoAFD e) { vec_push((*a), e); }

// Engade unha transición ó AFD
void afd_add_trans(AFD* a, EstadoAFD* dende, const EstadoAFD* ata, Trans c) {
    if (!dende || dende->hash == 0 || !ata || ata->hash == 0) {
        return;
    }

    // Buscar os índices de dende e ata
    u32 ia = vec_find((*a), *ata, _estado_afd_eq);

    if (ia == a->len) {
        err("intentouse engadir unha transición a un estado inexistente\n");
        return;
    }

    *hash_ins(dende->trans, c, &arena) = ia;
}

// Convirte un AFN nun AFD
AFD afn_to_afd(const AFN* a) {
    // Símbolos posibles
    Str simbolos = afn_simbolos(a);

    // Lista cos novos estados do AFD
    VecClausura estados = vec_new(VecClausura);

    // Fai un bucle ata que non queden estados pendentes
    VecEstado ci = afn_clausura(a->inicio);
    vec_push(estados, ((Clausura){.hash = (u32)_hash_clausura(&ci), .v = ci}));

    // Engade o estado inciial ó AFD
    AFD afd = vec_new(AFD);
    _afd_push(&afd, &estados.data[0], false);

    // Iteramos ata que non se produzan novos estados
    u32 i = 0;
    while (estados.len > i) {
        Clausura* c = &estados.data[i];

        // Para cada simbolo, calculamos as transicións na clausura
        vec_for_each(simbolos, s, {
            VecEstado sig = afn_delta(&c->v, s);
            VecEstado csig = afn_clausura_set(&sig);
            vec_free(sig);

            Clausura cs =
                ((Clausura){.hash = (u32)_hash_clausura(&csig), .v = csig});

            // Comprobamos que o estado definido pola clausura é novo
            u32 it = vec_find(estados, cs, _clausura_eq);
            if (it == estados.len) {
                _afd_push(&afd, &cs, afn_final(a, &cs.v));
                vec_push(estados, cs);
            }

            // Engadimos a transición
            const EstadoAFD* ata =
                it == estados.len ? &afd.data[afd.len - 1] : estados.data[it].e;
            afd_add_trans(&afd, c->e, ata, s);
        });

        i++;
    }

    // TODO: Liberar vectores
    dbg("afd: estados - %d, trans - TODO\n", afd.len);
    dbg("estados:\n");
    vec_for_each(afd, e _U_,
                 dbg("%u%s\n", e.hash & 0xfff, e.final ? " (final)" : ""));
    dbg("transicions:\n");
    return afd;
}

// Representa o autómata en formato graphviz
void afd_graph(const AFD* a, FILE* f) {
    fprintf(f, "digraph finite_state_machine {\n"
               "    rankdir=LR;\n    size=\"8,5\"\n"
               "    labelloc=\"b\";\n    label=\"AFD\";");

    vec_for_each((*a), e, {
        fprintf(f, "    node [shape = %s label = \"\"]; %u\n",
                e.final ? "doublecircle" : "circle", e.hash);
        hash_for_each(e.trans, t, {
            fprintf(f, "    %u -> %u [ label = \"%c\" ];\n", e.hash,
                    a->data[t->value].hash, t->key);
        });
    });

    fprintf(f, "    node [shape = none label=\"\"]; start\n"); /* start mark */
    fprintf(f, "    start -> %u [ label = \"start\" ]\n}\n", a->data[0].hash);
}
