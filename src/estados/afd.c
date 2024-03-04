#include "afd.h"

// -----------------
// Funcións internas
// -----------------

// Estructura temporal para almacear as clausuras cando construímos un AFD
typedef struct {
    u32 hash;
    VecEstado v;
    const EstadoAFD* e;
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

// Compara se dúas transicións afd son iguais
//      @param a: Transición 1
//      @param b: Transición 2
bool _trans_afd_eq(const TransAFD* a, const TransAFD* b) {
    return a->dende == b->dende && a->ata == b->ata && a->c == b->c;
}

// Engade unha clausura a un AFD coma un novo estado
void _afd_push(AFD* a, Clausura* c, bool f) {
    if (c->hash == 0) {
        return;
    }

    afd_add_estado(a, (EstadoAFD){.hash = c->hash, .final = f});
    c->e = &a->estados.data[a->estados.len - 1];
}

// ---
// API
// ---

// Engade un estado a un AFD
void afd_add_estado(AFD* a, EstadoAFD e) { vec_push(a->estados, e); }

// Engade unha transición ó AFD
void afd_add_trans(AFD* a, const EstadoAFD* dende, const EstadoAFD* ata,
                   Trans c) {
    if (!dende || dende->hash == 0 || !ata || ata->hash == 0) {
        return;
    }

    // Buscar os índices de dende e ata
    u32 id = vec_find(a->estados, *dende, _estado_afd_eq);
    u32 ia = vec_find(a->estados, *ata, _estado_afd_eq);

    if (id == a->estados.len || ia == a->estados.len) {
        err("intentouse engadir unha transición de estados inexistentes\n");
        return;
    }

    vec_push_unique(a->trans, ((TransAFD){.dende = id, .ata = ia, .c = c}),
                    _trans_afd_eq);
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
    AFD afd = ((AFD){.estados = vec_new(VecEstadoAFD),
                     .trans = vec_new(VecTransAFD)});
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
            const EstadoAFD* ata = it == estados.len
                                       ? &afd.estados.data[afd.estados.len - 1]
                                       : estados.data[it].e;
            afd_add_trans(&afd, c->e, ata, s);
        });

        i++;
    }

    // TODO: Liberar vectores
    dbg("afd: estados - %d, trans - %d\n", afd.estados.len, afd.trans.len);
    dbg("estados:\n");
    vec_for_each(afd.estados, e _U_,
                 dbg("%u%s\n", e.hash & 0xfff, e.final ? " (final)" : ""));
    dbg("transicions:\n");
    vec_for_each(afd.trans, t _U_, dbg("%u -%c-> %u\n", t.dende, t.c, t.ata));
    return afd;
}

// Representa o autómata en formato graphviz
void afd_graph(const AFD* a, FILE* f) {
    fprintf(f, "digraph finite_state_machine {\n"
               "    rankdir=LR;\n    size=\"8,5\"\n"
               "    labelloc=\"b\";\n    label=\"AFD\";");

    vec_for_each(a->estados, e, {
        fprintf(f, "    node [shape = %s label = \"\"]; %u\n",
                e.final ? "doublecircle" : "circle", e.hash);
    });

    vec_for_each(a->trans, t, {
        fprintf(f, "    %u -> %u [ label = \"%c\" ];\n",
                a->estados.data[t.dende].hash, a->estados.data[t.ata].hash,
                t.c);
    });

    fprintf(f, "    node [shape = none label=\"\"]; start\n"); /* start mark */
    fprintf(f, "    start -> %u [ label = \"start\" ]\n}\n",
            a->estados.data[0].hash);
}
