#include "afd.h"
#include "regex.h"

// -----------------
// Funcións internas
// -----------------

// Estructura temporal para almacear as clausuras cando construímos un AFD
typedef struct {
    u32 hash;
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
//      @param a: Estado 1
//      @param b: Clausura 2
bool _clausura_eq(const EstadoAFD* a, const Clausura* b) {
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
}

// ---
// API
// ---

// Engade un estado a un AFD
void afd_add_estado(AFD* a, EstadoAFD e) { vec_push((*a), e); }

// Engade unha transición ó AFD
void afd_add_trans(AFD* a, u32 dende_i, u32 ata_i, Trans c) {
    if (dende_i >= a->len || ata_i >= a->len) {
        return;
    }

    EstadoAFD* dende = &a->data[dende_i];
    EstadoAFD* ata = &a->data[ata_i];

    if (dende->hash == 0 || ata->hash == 0) {
        return;
    }

    printf("hi %p %c\n", dende->trans, c);
    *hash_ins(dende->trans, c, &arena) = ata_i;
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
        u32 dende = vec_find(afd, estados.data[i], _clausura_eq);

        // Para cada simbolo, calculamos as transicións na clausura
        vec_for_each(simbolos, s, {
            VecEstado sig = afn_delta(&estados.data[i].v, s);
            VecEstado csig = afn_clausura_set(&sig);
            Clausura cs =
                ((Clausura){.hash = (u32)_hash_clausura(&csig), .v = csig});

            // Comprobamos que o estado definido pola clausura é novo
            u32 ata = vec_find(afd, cs, _clausura_eq);
            if (ata == afd.len && cs.hash) {
                _afd_push(&afd, &cs, afn_final(a, &cs.v));
                vec_push(estados, cs);
            } else {
                vec_free(csig);
            }

            // Engadimos a transición
            afd_add_trans(&afd, dende, ata, s);
            vec_free(sig);
        });

        i++;
    }

    vec_free(estados);
    vec_free(ci);

#ifdef DEBUG
    dbg("afd: estados - %d, trans - %d\n", afd.len, estados.len);
    dbg("estados:\n");
    vec_for_each(afd, e _U_, {
        dbg("%u%s\n", e.hash & 0xfff, e.final ? " (final)" : "");
        hash_for_each(
            e.trans, t,
            dbg("\t%c -> %u\n", t->key, afd.data[t->value].hash & 0xfff));
    });
    dbg("fin\n");
#endif
    return afd;
}

// Calcula unha transición de estado
EstadoAFD* afd_delta(const AFD* a, EstadoAFD* actual, Trans c) {
    info("a: %p, actual %p, ch: %c, valido: %s\n", a, actual, c,
         caracter_valido(c) ? "true" : "false");
    if (!actual) {
        return NULL;
    }
    if (!caracter_valido(c)) {
        return actual;
    }
    u32* sig = hash_ins(actual->trans, c, NULL);
    if (sig == NULL) {
        return NULL;
    }
    return &a->data[*sig];
}

// Representa o autómata en formato graphviz
void afd_graph(const AFD* a, FILE* f) {
    fprintf(f, "digraph finite_state_machine {\n"
               "    rankdir=LR;\n    size=\"8,5\"\n"
               "    labelloc=\"b\";\n    label=\"AFD\";");

    vec_for_each((*a), e, {
        fprintf(f, "    node [shape = %s label = \"\"]; %u\n",
                e.final ? "doublecircle" : "circle", e.hash);
    });

    vec_for_each((*a), e, {
        hash_for_each(e.trans, t, {
            fprintf(f, "    %u -> %u [ label = \"%c\" ];\n", e.hash,
                    a->data[t->value].hash, t->key);
        });
    });

    fprintf(f, "    node [shape = none label=\"\"]; start\n"); /* start mark */
    fprintf(f, "    start -> %u [ label = \"start\" ]\n}\n", a->data[0].hash);
}
