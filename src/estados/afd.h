#pragma once

#include "afn.h"

// Estado nun AFD
typedef struct {
    u32 hash;
    bool final;
} EstadoAFD;

// Transición nun AFD
// Dende e ata son os índices do vector de estados (non os hashes)
typedef struct {
    u32 dende;
    u32 ata;
    Trans c;
} TransAFD;

typedef Vec(EstadoAFD) VecEstadoAFD;
typedef Vec(TransAFD) VecTransAFD;

// Representamos un AFD algo diferente dun AFN
typedef struct {
    VecEstadoAFD estados;
    VecTransAFD trans;
} AFD;

// Engade un estado ó AFD
//      @param a: AFD no que engadir
//      @param e: Estado a engadir
void afd_add_estado(AFD* a, EstadoAFD e);

// Engade unha transición ó AFD
//      @param a: AFD no que engadir
//      @param dende: Estado dende o que transicionar
//      @param ata: Estado ata o que transicionar
//      @param c: Caracter a transicionar
void afd_add_trans(AFD* a, const EstadoAFD* dende, const EstadoAFD* ata,
                   Trans c);

// Convirte un AFN nun AFD
//      @param a: AFN a convertir
AFD afn_to_afd(const AFN* a);

// Representa o autómata en formato graphviz
//      @param a Autómata a representar
//      @param f Ficheiro no que gardar a representación
void afd_graph(const AFD* a, FILE* f);
