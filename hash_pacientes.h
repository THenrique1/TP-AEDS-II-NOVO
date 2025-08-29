#ifndef HASH_PACIENTES_H
#define HASH_PACIENTES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hospital.h"
#include "paciente.h"

#define TAM_TABELA_HASH_ENC 50  // Tamanho da tabela hash
#define ARQUIVO_PACIENTES "pacientes.dat"
#define OFFSET_INVALIDO -1L // Valor para indicar que n�o h� pr�ximo n�

// Estrutura do n� encadeado (com paciente e o pr�ximo n� no arquivo)
typedef struct NoPaciente {
    Paciente paciente;  // Dados do paciente
    long prox;          // Offset para o pr�ximo n� (ou -1 se n�o houver)
} NoPaciente;

// Estrutura da tabela hash com encadeamento exterior
typedef struct {
    long buckets[TAM_TABELA_HASH_ENC];  // Cada �ndice armazena o offset do primeiro n� da lista encadeada
} TabelaHashPacientes;

// Fun��es
int hash(int codigo);
void inicializarTabelaHashEncArquivo(FILE *arq);
void inserirPacienteHashEncArquivo(FILE *arq, Paciente paciente);
int buscarPacienteHashEncArquivo(FILE *arq, int codigo, Paciente *pacienteEncontrado);
int removerPacienteHashEncArquivo(FILE *arq, int codigo);
void carregarHashPacientes(FILE *arq, TabelaHashPacientes *tabela);
void liberarHash(TabelaHashPacientes *tabela);
void recarregarHashPacientes(TabelaHashPacientes *tabela);



#endif
