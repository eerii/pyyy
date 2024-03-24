// Autómata finito determinado
// Acompaña ó AFN, máis fácil de executar

#pragma once

#include "../tipos/hash.h"
#include "afn.h"

// Mapa hash entre caracteres de transición e seguintes estados
// O valor do hashmap é o índice do vector de estados (non o hash do estado)
typedef HashTree(Trans, u32) HashTransEstado;

// Estado nun AFD
typedef struct {
    u32 hash;
    bool final;
    HashTransEstado* trans;
} EstadoAFD;

// Un AFD simplemente é unha lista de estados
typedef Vec(EstadoAFD) AFD;

// Engade un estado ó AFD
//      @param a: AFD no que engadir
//      @param e: Estado a engadir
void afd_add_estado(AFD* a, EstadoAFD e);

// Engade unha transición ó AFD
//      @param a: AFD no que engadir
//      @param dende: Indice do estado dende o que transicionar
//      @param ata: Indice do estado ata o que transicionar
//      @param c: Caracter a transicionar
void afd_add_trans(AFD* a, u32 dende, u32 ata, Trans c);

// Convirte un AFN nun AFD
//      @param a: AFN a convertir
AFD afn_to_afd(const AFN* a);

// Realiza unha transición do AFD con un caracter
//      @param a: AFD
//      @param actual: Estado actual
//      @param c: Caracter a comprobar
EstadoAFD* afd_delta(const AFD* a, EstadoAFD* actual, Trans c);

// Representa o autómata en formato graphviz
//      @param a Autómata a representar
//      @param f Ficheiro no que gardar a representación
void afd_graph(const AFD* a, FILE* f);
