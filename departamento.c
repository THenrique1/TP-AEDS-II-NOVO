#include "hospital.h"
#include "departamento.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tempo.h"


// Função de listagem de departamentos (lê diretamente do arquivo)
void listarDepartamentos() {
    FILE *arqDeptos = fopen("departamentos.dat", "rb");
    if (!arqDeptos) {
        printf("Erro ao abrir o arquivo de departamentos.\n");
        return;
    }

    // --- Carregar funcionários ---
    FILE *arqFuncs = fopen("funcionarios.dat", "rb");
    int totalFuncs = 0;
    Funcionario *funcionarios = NULL;
    if (arqFuncs) {
        fseek(arqFuncs, 0, SEEK_END);
        long tamFuncs = ftell(arqFuncs);
        rewind(arqFuncs);
        totalFuncs = tamFuncs / sizeof(Funcionario);
        funcionarios = malloc(tamFuncs);
        fread(funcionarios, sizeof(Funcionario), totalFuncs, arqFuncs);
        fclose(arqFuncs);
    }

    // --- Carregar pacientes ---
    FILE *arqPacs = fopen("pacientes.dat", "rb");
    int totalPacs = 0;
    Paciente *pacientes = NULL;
    if (arqPacs) {
        fseek(arqPacs, 0, SEEK_END);
        long tamPacs = ftell(arqPacs);
        rewind(arqPacs);
        totalPacs = tamPacs / sizeof(Paciente);
        pacientes = malloc(tamPacs);
        fread(pacientes, sizeof(Paciente), totalPacs, arqPacs);
        fclose(arqPacs);
    }

    Departamento d;
    int registrosLidos = 0;

    printf("\n======= Lista de Departamentos =======\n");

    while (fread(&d, sizeof(Departamento), 1, arqDeptos) == 1) {
        int qtdFuncionarios = 0;
        int qtdPacientes = 0;

        // Contar funcionários do departamento
        for (int i = 0; i < totalFuncs; i++) {
            if (funcionarios[i].codigoDepartamento == d.codigo) {
                qtdFuncionarios++;
            }
        }

        // Contar pacientes do departamento
        for (int i = 0; i < totalPacs; i++) {
            if (pacientes[i].codigoDepartamento == d.codigo) {
                qtdPacientes++;
            }
        }

        printf("Codigo: %d\n", d.codigo);
        printf("Nome: %s\n", d.nome);
        printf("Responsavel: %s\n", d.responsavel);
        printf("Andar: %d\n", d.andar);
        printf("Quantidade de Funcionarios: %d\n", qtdFuncionarios);
        printf("Quantidade de Pacientes: %d\n", qtdPacientes);
        printf("-------------------------------------\n");

        registrosLidos++;
    }

    if (registrosLidos == 0) {
        printf("Nenhum departamento encontrado.\n");
    }

    free(funcionarios);
    free(pacientes);
    fclose(arqDeptos);
}

// Função de busca sequencial para departamentos (lê diretamente do arquivo)
int buscaSequencialDepartamento(int codigo) {
    FILE *arquivo = fopen("departamentos.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de departamentos.\n");
        return -1;
    }

    Departamento d;
    int encontrado = 0;
    clock_t inicioBusca = clock();

    // Leitura sequencial
    while (fread(&d, sizeof(Departamento), 1, arquivo) == 1) {
        if (d.codigo == codigo) {
            encontrado = 1;
            break;
        }
    }

    clock_t fimBusca = clock();
    double tempoExecucao = ((double)(fimBusca - inicioBusca)) / CLOCKS_PER_SEC;

    fclose(arquivo);

    if (encontrado) {
        // Contar funcionários associados a este departamento
        int count = 0;
        FILE *arqFunc = fopen("funcionarios.dat", "rb");
        if (arqFunc != NULL) {
            Funcionario f;
            while (fread(&f, sizeof(Funcionario), 1, arqFunc) == 1) {
                if (f.codigoDepartamento == d.codigo) {
                    count++;
                }
            }
            fclose(arqFunc);
        }

        printf("Departamento encontrado:\n");
        printf("Codigo: %d\n", d.codigo);
        printf("Nome: %s\n", d.nome);
        printf("Responsavel: %s\n", d.responsavel);
        printf("Andar: %d\n", d.andar);
        printf("Quantidade de Funcionarios: %d\n", count);
    } else {
        printf("Departamento não encontrado.\n");
    }

    gravarLogPesquisa("Pesquisa Sequencial (Departamento)", codigo, encontrado, tempoExecucao);
    return encontrado ? 1 : -1;
}

// Função de busca binária para departamentos (lê diretamente do arquivo)
int buscaBinariaDepartamento(int codigo) {
    FILE *arquivo = fopen("departamentos.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de departamentos.\n");
        return -1;
    }

    // Determinar o número total de departamentos
    fseek(arquivo, 0, SEEK_END);
    long tamanhoArquivo = ftell(arquivo);
    int totalDepartamentos = tamanhoArquivo / sizeof(Departamento);

    // Retornar à posição inicial do arquivo
    fseek(arquivo, 0, SEEK_SET);

    Departamento d;
    int inicio = 0, fim = totalDepartamentos - 1, meio;
    int encontrado = 0;
    clock_t inicioBusca, fimBusca;
    double tempoExecucao;

    // Começa a medir o tempo de execução
    inicioBusca = clock();

    // Busca binária
    while (inicio <= fim) {
        meio = (inicio + fim) / 2;
        fseek(arquivo, meio * sizeof(Departamento), SEEK_SET);
        fread(&d, sizeof(Departamento), 1, arquivo);

        if (d.codigo == codigo) {
            encontrado = 1;
            break;
        } else if (d.codigo < codigo) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    // Calcula o tempo de execução
    fimBusca = clock();
    tempoExecucao = ((double)(fimBusca - inicioBusca)) / CLOCKS_PER_SEC;

    fclose(arquivo);

    // Exibe os dados do departamento encontrado
    if (encontrado) {
        // Conta real de funcionários associados
        int count = 0;
        FILE *arqFunc = fopen("funcionarios.dat", "rb");
        if (arqFunc != NULL) {
            Funcionario f;
            while (fread(&f, sizeof(Funcionario), 1, arqFunc) == 1) {
                if (f.codigoDepartamento == d.codigo) {
                    count++;
                }
            }
            fclose(arqFunc);
        }

        printf("Departamento encontrado:\n");
        printf("Codigo: %d\n", d.codigo);
        printf("Nome: %s\n", d.nome);
        printf("Responsavel: %s\n", d.responsavel);
        printf("Andar: %d\n", d.andar);
        printf("Quantidade de Funcionarios: %d\n", count);
    } else {
        printf("Departamento não encontrado.\n");
    }

    // Chama a função para gravar no log
    gravarLogPesquisa("Pesquisa Binaria (Departamento)", codigo, encontrado, tempoExecucao);

    return encontrado ? 1 : -1;  // Retorna 1 se encontrado, -1 se não encontrado
}

