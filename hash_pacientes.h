#ifndef HASH_PACIENTES_H
#define HASH_PACIENTES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hospital.h"
#include "paciente.h"

#define TAM_TABELA_HASH_ENC 50  // Tamanho da tabela hash
#define ARQUIVO_PACIENTES "pacientes.dat"
#define OFFSET_INVALIDO -1L // Valor para indicar que não há próximo nó

// Estrutura do nó encadeado (com paciente e o próximo nó no arquivo)
typedef struct NoPaciente {
    Paciente paciente;  // Dados do paciente
    long prox;          // Offset para o próximo nó (ou -1 se não houver)
} NoPaciente;

// Estrutura da tabela hash com encadeamento exterior
typedef struct {
    long buckets[TAM_TABELA_HASH_ENC];  // Cada índice armazena o offset do primeiro nó da lista encadeada
} TabelaHashPacientes;

// Funções
int hash(int codigo);
void inicializarTabelaHashEncArquivo(FILE *arq);
void inserirPacienteHashEncArquivo(FILE *arq, Paciente paciente);
int buscarPacienteHashEncArquivo(FILE *arq, int codigo, Paciente *pacienteEncontrado);
int removerPacienteHashEncArquivo(FILE *arq, int codigo);
void carregarHashPacientes(FILE *arq, TabelaHashPacientes *tabela);
void liberarHash(TabelaHashPacientes *tabela);
void recarregarHashPacientes(TabelaHashPacientes *tabela);



#endif
