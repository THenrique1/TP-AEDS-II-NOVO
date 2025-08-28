#include "hospital.h"
#include "funcionario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tempo.h"

// Listagem de funcionários
void listarFuncionarios() {
    FILE *arquivo = fopen("funcionarios.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir funcionarios.dat para leitura.\n");
        return;
    }

    Funcionario f;
    int exibidos = 0;

    printf("\n--- Lista de Funcionários ---\n");

    while (fread(&f, sizeof(Funcionario), 1, arquivo) == 1) {
        // Buscar nome do departamento
        const char *nomeDepartamento = "Desconhecido";
        for (int i = 0; i < totalDepartamentos; i++) {
            if (departamentos[i].codigo == f.codigoDepartamento) {
                nomeDepartamento = departamentos[i].nome;
                break;
            }
        }

        printf("Codigo: %d\n", f.codigo);
        printf("Nome: %s\n", f.nome);
        printf("Cargo: %s\n", f.cargo);
        printf("Especializacao: %s\n", f.especializacao);
        printf("Salario: %.2f\n", f.salario);
        printf("Departamento: %d - %s\n", f.codigoDepartamento, nomeDepartamento);
        printf("----------------------------------\n");

        exibidos++;
    }

    fclose(arquivo);

    if (exibidos == 0)
        printf("Nenhum funcionário encontrado.\n");

    printf("Total listado: %d funcionario(s).\n", totalFuncionarios);
}

// Função de busca binária
int buscaBinariaFuncionario(int codigo) {
    FILE *arquivo = fopen("funcionarios.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de funcionarios.\n");
        return -1;
    }

    clock_t inicio, fim;
    double tempoExecucao;

    inicio = clock();

    Funcionario f;
    int inicioBusca = 0, fimBusca = totalFuncionarios - 1, meio;
    int encontrado = 0;

    // A busca binária no arquivo
    while (inicioBusca <= fimBusca) {
        meio = (inicioBusca + fimBusca) / 2;

        fseek(arquivo, meio * sizeof(Funcionario), SEEK_SET);

        fread(&f, sizeof(Funcionario), 1, arquivo);

        if (f.codigo == codigo) {
            encontrado = 1;
            break;
        } else if (f.codigo < codigo) {
            inicioBusca = meio + 1;
        } else {
            fimBusca = meio - 1;
        }
    }

    fim = clock();
    tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    gravarLogPesquisa("Pesquisa Binaria (Funcionario)", codigo, encontrado, tempoExecucao);

    fclose(arquivo);

    if (encontrado) {
        printf("Funcionario encontrado:\n");
        printf("Codigo: %d\n", f.codigo);
        printf("Nome: %s\n", f.nome);
        printf("Cargo: %s\n", f.cargo);
        printf("Salario: %.2f\n", f.salario);
    } else {
        printf("Funcionario não encontrado.\n");
    }

    return encontrado ? meio : -1;
}


// Função de busca sequencial funcionários
int buscaSequencialFuncionario(int codigo) {
    FILE *arquivo = fopen("funcionarios.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de funcionarios.\n");
        return -1;
    }

    Funcionario f;
    int encontrado = 0;
    clock_t inicio, fim;
    double tempoExecucao;

    inicio = clock();

    // Busca sequencial no arquivo
    while (fread(&f, sizeof(Funcionario), 1, arquivo) == 1) {
        if (f.codigo == codigo) {
            encontrado = 1;
            break;
        }
    }

    fim = clock();

    tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    gravarLogPesquisa("Pesquisa Sequencial (Funcionario)", codigo, encontrado, tempoExecucao);

    fclose(arquivo);

    if (encontrado) {
        printf("Funcionario encontrado:\n");
        printf("Codigo: %d\n", f.codigo);
        printf("Nome: %s\n", f.nome);
        printf("Cargo: %s\n", f.cargo);
        printf("Salario: %.2f\n", f.salario);
    } else {
        printf("Funcionario não encontrado.\n");
    }

    return encontrado ? 1 : -1;
}
