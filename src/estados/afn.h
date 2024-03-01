// Autómatas finitos non determinados
// Utilidades para convertir expresións regulares en AFN
// Usa o algoritmo de construcción de Thomson
//
// Referencias:
//      - https://en.wikipedia.org/wiki/Thompson%27s_construction

// Gramática
// letra = [a-zA-Z]
// digito = [0-9]
// identificador = (letra|_)(letra|digito|_)*

#include "../definicions.h"
#include "../tipos/arena.h"
#include "../tipos/vec.h"

typedef struct Estado Estado;
typedef struct Fragmento Fragmento;
typedef Vec(Estado*) VecEstados;

#define ESTADO_DIVIDE 256
#define ESTADO_ACEPTA 257

#define TAM_STACK 256

#define estado_at(V, I) ((Estado*)(V->data + I * sizeof(Estado*)))

// Un estado dun autómata finito non determinista
// Pode representar 3 fragmentos dependendo do valor de c
//      - c < 256: Unha saída
//      - c = 256: Dúas saídas, división
//      - c = 257: Ningunha saída, estado de aceptación
struct Estado {
    i32 c;
    i32 ultima_lista;
    Estado* sig;
    Estado* sig_alt;
};

// Fragmentos de AFN computados polo algoritmo
struct Fragmento {
    Estado* inicio;
    VecEstados sig;
};

// Variables globais
Estado estado_aceptacion = {ESTADO_ACEPTA};
u32 num_estados = 0;

// Crea un novo estado
//      @param c: Caracter do estado
//      @param sig: Seguinte estado
//      @param sig_alt: Seguinte estado alternativo
static inline Estado* estado(i32 c, Estado* sig, Estado* sig_alt) {
    Estado* s;
    s = arena_push_struct(&arena, Estado);
    s->c = c;
    s->ultima_lista = 0;
    s->sig = sig;
    s->sig_alt = sig_alt;

    num_estados++;
    return s;
}

// Crea un novo fragmento
//      @param inicio: Estado inicial
//      @param sig: Lista de estados seguintes
static inline Fragmento frag(VecEstados sig, Estado* inicio) {
    Fragmento f = {.sig = sig, .inicio = inicio};
    for (i32 i = 0; i < sig.len; i++) {
        log("sig[%d] = %d\n", i, estado_at((&sig), i)->c);
    }
    return f;
}

// Fai que todos os estados dunha lista apunten ó indicado
//      @param l: Lista de estados
//      @param s: Estado a apuntar
static inline void patch(VecEstados* v, Estado* s) {
    printf("patch %u\n", v->len);
    for (u32 i = 0; i < v->len; i++) {
        estado_at(v, i)->sig = s;
    }
}

// Convirte unha expresión regular nun AFN
//      @param regex: Expresión regular
static inline Estado* regex_afn(const char* regex) {
    Estado* s;
    Fragmento stack[TAM_STACK], *stackp;
    Fragmento a, b;
    VecEstados v;

#define push(S) *stackp++ = S
#define pop() *--stackp

    stackp = stack;
    for (const char* p = regex; *p; p++) {
        printf("procesando '%c'\n", *p);
        switch (*p) {
        case '.': // Concatenación
            b = pop();
            a = pop();
            patch(&a.sig, b.inicio);
            push(frag(b.sig, a.inicio));
            break;
        case '|': // Alternancia
            b = pop();
            a = pop();
            s = estado(ESTADO_DIVIDE, a.inicio, b.inicio);
            vec_init_from_n(v, a.sig.data, a.sig.len);
            vec_append_n(v, b.sig.data, b.sig.len);
            push(frag(v, s));
            break;
        case '?': // 0 ou 1
            a = pop();
            s = estado(ESTADO_DIVIDE, a.inicio, NULL);
            vec_init_from_n(v, a.sig.data, a.sig.len);
            vec_push(v, s->sig_alt);
            push(frag(v, s));
            break;
        case '*': // 0 ou mais
            a = pop();
            s = estado(ESTADO_DIVIDE, a.inicio, NULL);
            patch(&a.sig, s);
            vec_init_from_n(v, s->sig_alt, 1);
            push(frag(v, s));
            break;
        case '+': // 1 ou mais
            a = pop();
            s = estado(ESTADO_DIVIDE, a.inicio, NULL);
            patch(&a.sig, s);
            vec_init_from_n(v, s->sig_alt, 1);
            push(frag(v, a.inicio));
            break;
        default: // Caracter literal
            s = estado(*p, NULL, NULL);
            vec_init_from_n(v, s->sig, 1);
            push(frag(v, s));
            break;
        }
    }

    a = pop();
    patch(&a.sig, &estado_aceptacion);
    printf("a = %u, %p %p %p\n", a.inicio->c, estado_at((&v), 0),
           estado_at((&v), 0)->sig, &estado_aceptacion);
    printf("fin\n");
    return a.inicio;
}

// Prepara un AFN para comprobar caracteres
//      @param inicio: Estado inicial
static inline Fragmento afn_init(Estado* inicio) {
    Fragmento a = {.inicio = inicio};

    vec_free(a.sig);
    vec_init_from_n(a.sig, inicio, 1);

    return a;
}

// Executa unha transición de estado dun AFN
// Devolve true se algún dos estados é de aceptación
//      @param actual: Vector de estados actuais
//      @param sig: Vector de estados seguintes
//      @param c: Caracter
static inline bool afn_step(Fragmento* a, char c) {
    log("step %c\n", c);
    Estado* s;
    bool aceptacion = false;
    VecEstados v;
    vec_init_res(v, a->sig.len);

    for (u32 i = 0; i < a->sig.len; i++) {
        s = estado_at((&a->sig), i);
        log("%d, %d\n", c, s->c);
        if (s->c == c) {
            vec_push(v, s->sig);
            aceptacion |= s->sig == &estado_aceptacion;
        }
    };

    vec_clear(a->sig);
    vec_append_n(a->sig, v.data, v.len);
    return aceptacion;
}
