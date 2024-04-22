#pragma once

// Abre o arquivo indicado para ler os seus comandos
//      @param nome: Nome do arquivo a abrir
void arquivo_init(const char* nome);

// Pecha o arquivo
void arquivo_close();

// Contrala se hai un arquivo aberto
extern bool ficheiro_aberto;
