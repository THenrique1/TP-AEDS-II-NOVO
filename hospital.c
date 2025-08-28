#include "hospital.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int totalDepartamentos = 0;
int totalFuncionarios = 0;
int totalPacientes = 0;

Departamento departamentos[MAX_DEPARTAMENTOS];
// ----- Funções para salvar totais -----

void salvarTotalPacientes() {
    FILE *arquivo = fopen("totalPacientes.dat", "wb");
    if (!arquivo) {
        printf("Erro ao salvar total de pacientes.\n");
        return;
    }
    fwrite(&totalPacientes, sizeof(int), 1, arquivo);
    fclose(arquivo);
}

void salvarTotalFuncionarios() {
    FILE *arquivo = fopen("totalFuncionarios.dat", "wb");
    if (arquivo == NULL) {
        printf("Erro ao salvar o numero de funcionarios.\n");
        return;
    }
    fwrite(&totalFuncionarios, sizeof(int), 1, arquivo);
    fclose(arquivo);
}

void salvarTotalDepartamentos() {
    FILE *arquivo = fopen("totalDepartamentos.dat", "wb");
    if (arquivo == NULL) {
        printf("Erro ao salvar o número de departamentos.\n");
        return;
    }
    fwrite(&totalDepartamentos, sizeof(int), 1, arquivo);
    fclose(arquivo);
}

// ----- Funções para carregar totais -----

void carregarTotalPacientes() {
    FILE *arquivo = fopen("totalPacientes.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao carregar o numero de pacientes.\n");
        totalPacientes = 0;
        return;
    }
    fread(&totalPacientes, sizeof(int), 1, arquivo);
    fclose(arquivo);
}

void carregarTotalFuncionarios() {
    FILE *arquivo = fopen("totalFuncionarios.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao carregar o número de funcionarios.\n");
        totalFuncionarios = 0;
        return;
    }
    fread(&totalFuncionarios, sizeof(int), 1, arquivo);
    fclose(arquivo);
}

void carregarTotalDepartamentos() {
    FILE *arquivo = fopen("totalDepartamentos.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao carregar o número de departamentos.\n");
        return;
    }
    fread(&totalDepartamentos, sizeof(int), 1, arquivo);
    fclose(arquivo);
}


// ----- Funções para carregar arquivos principais -----
void carregarPacientesDoArquivo() {
    FILE *arquivo = fopen("pacientes.dat", "rb");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de pacientes.dat para leitura.\n");
        totalPacientes = 0;
        return;
    }

    fseek(arquivo, 0, SEEK_END);  // vai para o final do arquivo
    long tamanho = ftell(arquivo); // obtém posição final (em bytes)
    fclose(arquivo);

    totalPacientes = tamanho / sizeof(Paciente);
    printf("Carregamento de %d pacientes (dados permanecem no disco).\n", totalPacientes);
}

void carregarFuncionariosDoArquivo() {
    FILE *arquivo = fopen("funcionarios.dat", "rb");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de funcionarios.dat para leitura.\n");
        totalFuncionarios = 0;
        return;
    }

    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    fclose(arquivo);

    totalFuncionarios = tamanho / sizeof(Funcionario);
    printf("Carregamento de %d funcionarios (dados permanecem no disco).\n", totalFuncionarios);
}

void carregarDepartamentosDoArquivo() {
    FILE *arquivo = fopen("departamentos.dat", "rb");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de departamentos.dat para leitura.\n");
        totalDepartamentos = 0;
        return;
    }

    totalDepartamentos = 0;
    while (fread(&departamentos[totalDepartamentos], sizeof(Departamento), 1, arquivo) == 1) {
        totalDepartamentos++;
        if (totalDepartamentos >= MAX_DEPARTAMENTOS) break;
    }

    fclose(arquivo);
    printf("Departamentos carregados: %d\n", totalDepartamentos);
}


// ----- Funções de busca por código -----

int buscarIndicePaciente(int codigo) {
    FILE *arquivo = fopen("pacientes.dat", "rb");
    if (!arquivo) {
        printf("Erro ao abrir pacientes.dat\n");
        return -1;
    }

    Paciente p;
    while (fread(&p, sizeof(Paciente), 1, arquivo) == 1) {
        if (p.codigo == codigo) {
            fclose(arquivo);
            return 1;  // Encontrado
        }
    }

    fclose(arquivo);
    return -1; // Não encontrado
}

int buscarIndiceFuncionario(int codigo) {
    FILE *arquivo = fopen("funcionarios.dat", "rb");
    if (!arquivo) {
        printf("Erro ao abrir funcionarios.dat\n");
        return -1;
    }

    Funcionario f;
    while (fread(&f, sizeof(Funcionario), 1, arquivo) == 1) {
        if (f.codigo == codigo) {
            fclose(arquivo);
            return 1;  // Encontrado
        }
    }

    fclose(arquivo);
    return -1; // Não encontrado
}

int buscarIndiceDepartamento(int codigo) {
    for (int i = 0; i < totalDepartamentos; i++) {
        if (departamentos[i].codigo == codigo)
            return i;
    }
    return -1;
}
