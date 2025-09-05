#include "hospital.h"
#include "departamento.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tempo.h"

// Lista departamentos lendo/contando direto dos arquivos .dat (memória secundária)
// Lista apenas os departamentos, sem contar funcionários e pacientes
void listarDepartamentos() {
    FILE *arqDeptos = fopen("departamentos.dat", "rb");
    if (!arqDeptos) {
        printf("Erro ao abrir o arquivo de departamentos.\n");
        return;
    }

    Departamento d;
    int registrosLidos = 0;

    printf("\n======= Lista de Departamentos =======\n");

    // Lê cada departamento diretamente do arquivo
    while (fread(&d, sizeof(Departamento), 1, arqDeptos) == 1) {
        printf("Codigo: %d\n", d.codigo);
        printf("Nome: %s\n", d.nome);
        printf("Responsavel: %s\n", d.responsavel);
        printf("Andar: %d\n", d.andar);
        printf("-------------------------------------\n");
        registrosLidos++;
    }

    if (registrosLidos == 0) {
        printf("Nenhum departamento encontrado.\n");
    } else {
        printf("Total listado: %d departamento(s).\n", registrosLidos);
    }

    fclose(arqDeptos);
}


// Busca sequencial em departamentos.dat (passa registro a registro até encontrar ou acabar)
int buscaSequencialDepartamento(int codigo) {
    FILE *arquivo = fopen("departamentos.dat", "rb"); // abre arquivo em leitura binária
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de departamentos.\n");
        return -1; // erro ao abrir
    }

    Departamento d;
    int encontrado = 0;
    clock_t inicioBusca = clock(); // marca inicio do tempo de execução

    // Leitura sequencial: percorre todo o arquivo
    while (fread(&d, sizeof(Departamento), 1, arquivo) == 1) {
        if (d.codigo == codigo) { // se encontrou o código buscado
            encontrado = 1;
            break;
        }
    }

    clock_t fimBusca = clock(); // marca fim do tempo
    double tempoExecucao = ((double)(fimBusca - inicioBusca)) / CLOCKS_PER_SEC;

    fclose(arquivo);

    if (encontrado) {
        // Conta funcionários associados ao departamento encontrado
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

        // Exibe informações do departamento
        printf("Departamento encontrado:\n");
        printf("Codigo: %d\n", d.codigo);
        printf("Nome: %s\n", d.nome);
        printf("Responsavel: %s\n", d.responsavel);
        printf("Andar: %d\n", d.andar);
        printf("Quantidade de Funcionarios: %d\n", count);
    } else {
        printf("Departamento não encontrado.\n");
    }

    // Registra no log os dados da busca
    gravarLogPesquisa("Pesquisa Sequencial (Departamento)", codigo, encontrado, tempoExecucao);

    return encontrado ? 1 : -1; // retorna 1 se achou, -1 se não achou
}


// Busca binária em departamentos.dat (requer que o arquivo esteja ordenado por código)
int buscaBinariaDepartamento(int codigo) {
    FILE *arquivo = fopen("departamentos.dat", "rb"); // abre o arquivo
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de departamentos.\n");
        return -1;
    }

    // Calcula o número total de departamentos no arquivo
    fseek(arquivo, 0, SEEK_END);
    long tamanhoArquivo = ftell(arquivo);                   // tamanho em bytes
    int totalDepartamentos = tamanhoArquivo / sizeof(Departamento); // total de registros
    fseek(arquivo, 0, SEEK_SET);                            // volta para o início

    Departamento d;
    int inicio = 0, fim = totalDepartamentos - 1, meio;
    int encontrado = 0;
    clock_t inicioBusca, fimBusca;
    double tempoExecucao;

    inicioBusca = clock(); // começa medição de tempo

    // Loop da busca binária
    while (inicio <= fim) {
        meio = (inicio + fim) / 2; // calcula posição do meio
        fseek(arquivo, meio * sizeof(Departamento), SEEK_SET); // posiciona no registro "meio"
        fread(&d, sizeof(Departamento), 1, arquivo);           // lê esse registro

        if (d.codigo == codigo) { // se encontrou
            encontrado = 1;
            break;
        } else if (d.codigo < codigo) {
            inicio = meio + 1; // descarta a metade inferior
        } else {
            fim = meio - 1;    // descarta a metade superior
        }
    }

    fimBusca = clock(); // fim do tempo
    tempoExecucao = ((double)(fimBusca - inicioBusca)) / CLOCKS_PER_SEC;

    fclose(arquivo);

    if (encontrado) {
        // Conta funcionários associados a este departamento para interação
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

        // Exibe os dados do departamento encontrado
        printf("Departamento encontrado:\n");
        printf("Codigo: %d\n", d.codigo);
        printf("Nome: %s\n", d.nome);
        printf("Responsavel: %s\n", d.responsavel);
        printf("Andar: %d\n", d.andar);
        printf("Quantidade de Funcionarios: %d\n", count);
    } else {
        printf("Departamento não encontrado.\n");
    }

    // Grava no log a execução da busca
    gravarLogPesquisa("Pesquisa Binaria (Departamento)", codigo, encontrado, tempoExecucao);

    return encontrado ? 1 : -1; // retorna 1 se achou, -1 se não achou
}

