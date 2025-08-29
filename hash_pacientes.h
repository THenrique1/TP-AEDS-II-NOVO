#ifndef HASH_PACIENTES_H
#define HASH_PACIENTES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "paciente.h"  // Inclua a estrutura Paciente

#define TAMANHO_TABELA_HASH 101  // Tamanho da tabela hash
#define ARQUIVO_PACIENTES "pacientes.dat"  // Arquivo de dados dos pacientes

// Estrutura do nó para a tabela hash (lista encadeada exterior)
typedef struct No {
    int codigo;
    long offset;  // Posição do paciente no arquivo
    struct No *prox;  // Ponteiro para o próximo nó (colisão)
} No;

// Estrutura da tabela hash
typedef struct {
    No *buckets[TAMANHO_TABELA_HASH];  // Array de buckets (listas encadeadas)
} HashPacientes;

// Funções de manipulação de dados e tabela hash
int funcao_hash(int codigo);  // Função de hash
void criar_tabela_hash_pacientes(FILE *tabela_hash);  // Cria a tabela hash
void inicializarHash(HashPacientes *tabela);  // Inicializa a tabela hash
void inserir_na_lista_hash(FILE *tabela_hash, FILE *arq_pacientes, int pos_novo_paciente, int cod_novo_paciente);  // Insere pacientes na lista encadeada da tabela hash
void inserirPacienteHash(HashPacientes *tabela, Paciente p);  // Insere um paciente na tabela hash
Paciente* buscarPacienteHash(HashPacientes *tabela, int codigo, FILE *arq_pacientes);  // Busca um paciente na tabela hash
int removerPacienteHash(HashPacientes *tabela, int codigo, FILE *arq_pacientes);  // Remove um paciente da tabela hash
void carregar_hash_pacientes(HashPacientes *tabela, FILE *arq_pacientes);  // Carrega dados de pacientes na tabela hash
void liberar_hash(HashPacientes *tabela);  // Libera a memória da tabela hash
void recarregar_hash_pacientes(HashPacientes *tabela, FILE *arq_pacientes);  // Recarrega a tabela hash
void imprimir_tabela_hash_pacientes(FILE *tabela_hash, FILE *arq_pacientes);  // Imprime a tabela hash e as listas encadeadas

// Funções auxiliares para leitura e escrita de pacientes
int tamanho_registro_paciente();  // Retorna o tamanho do registro de um paciente
Paciente* le_paciente(FILE *in);  // Lê um paciente do arquivo
void salva_paciente(Paciente *p, FILE *out);  // Salva um paciente no arquivo
FILE* abrirArquivoPacientesParaAnexar(void);  // Abre o arquivo de pacientes para anexar novos dados

#endif  // HASH_PACIENTES_H
