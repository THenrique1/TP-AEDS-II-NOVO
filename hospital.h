#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DEPARTAMENTOS 1000

// Definição das estruturas
typedef struct {
    int codigo;
    char nome[100];
    char responsavel[100];
    int andar;
    int quantidadeFuncionarios;
} Departamento;

typedef struct {
    int codigo;
    char nome[100];
    char cargo[50];
    char especializacao[50];
    float salario;
    int codigoDepartamento;
} Funcionario;

typedef struct {
    int codigo;
    char nome[100];
    char cpf[15];
    char dataNascimento[11];
    char telefone[15];
    char endereco[200];
    int codigoDepartamento;
    int codigoFuncionarioResponsavel;
    int ativo;

} Paciente;

// Funções de manipulação de dados e busca
void cadastrarDepartamento();
void listarDepartamentos();
void cadastrarFuncionario();
void listarFuncionarios();
void cadastrarPaciente();
void listarPacientes();

// Funções para salvar e carregar variáveis globais
void salvarTotalDepartamentos();
void carregarTotalDepartamentos();
void salvarTotalFuncionarios();
void carregarTotalFuncionarios();
void salvarTotalPacientes();
void carregarTotalPacientes();

// Funções de carregamento de dados dos arquivos
void carregarFuncionariosDoArquivo();
void carregarPacientesDoArquivo();
void carregarDepartamentosDoArquivo();

// Funções de busca
int buscaSequencialDepartamento(int codigo);
int buscaBinariaDepartamento(int codigo);
int buscaSequencialFuncionario(int codigo);
int buscaBinariaFuncionario(int codigo);

// Variáveis globais
extern int totalFuncionarios;
extern int totalDepartamentos;
extern int totalPacientes;

extern Departamento departamentos[MAX_DEPARTAMENTOS];

void removerPaciente(int codigo);
void removerDepartamento(int codigo);

int buscarIndiceFuncionario(int codigo);
int buscarIndicePaciente(int codigo);
int buscarIndiceDepartamento(int codigo);


//#define BLOCO_TAM 100
int contarBlocosArquivo(const char *nomeArquivo, size_t tamanhoRegistro);

void buscarNomeFuncionarioPorCodigo(int codigo, char *destino, size_t tamanho);
void buscarNomeDepartamentoPorCodigo(int codigo, char *destino, size_t tamanho);


#endif
