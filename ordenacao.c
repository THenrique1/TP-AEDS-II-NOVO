#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <direct.h>
#include <errno.h>
#include "ordem.h"
#include "paciente.h"
#include "departamento.h"
#include "funcionario.h"
#include "gerador_dados.h"
#include "subpaciente.h"
#include "tempo.h"

#define BLOCO_TAM 20
#define MAX_INTERCALACAO 500
#define NOME_BLOCO_BASE "bloco_"

void registrarLog(const char *mensagem) {
    // Garante que a pasta 'log' exista
    _mkdir("log");  // No Windows. Ignorado se já existir

    FILE *arquivo = fopen("log/logs_classificacao.txt", "a");
    if (arquivo != NULL) {
        fprintf(arquivo, "%s\n", mensagem);
        fclose(arquivo);
    } else {
        printf("Erro ao abrir arquivo de log: %s\n", strerror(errno));
    }
}

void bubbleSortPaciente(Paciente *v, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (v[j].codigo > v[j + 1].codigo) {
                Paciente temp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = temp;
            }
        }
    }
}
void bubbleSortFuncionario(Funcionario *v, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (v[j].codigo > v[j + 1].codigo) {
                Funcionario temp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = temp;
            }
}

void bubbleSortDepartamento(Departamento *v, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (v[j].codigo > v[j + 1].codigo) {
                Departamento temp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = temp;
            }
}

// ====== Contagem de blocos ======
int contarBlocosArquivo(const char *nomeArquivo, size_t tamanhoRegistro) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) return 0;
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    fclose(arquivo);
    long totalRegistros = tamanho / tamanhoRegistro;
    return (totalRegistros + BLOCO_TAM - 1) / BLOCO_TAM;
}

// ====== Ordenação com seleção por seleção Natural (Pacientes) ======
void ordenarPacientesPorCodigo() {
    printf("Iniciando ordenação \n");
    int totalBlocos = gerarParticoesPorSelecaoNaturalPacientes();
    printf("Total de blocos gerados: %d\n", totalBlocos);
    intercalarComArvoreDeVencedoresPacientes(totalBlocos);
    printf(" Ordenação concluída.\n");
}


// ====== (Pacientes ordenação antiga) ======
void ordenarPacientesPorCodigoBubbleSort() {
    FILE *arq = fopen("pacientes.dat", "rb+");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de pacientes.\n");
        return;
    }

    clock_t inicio = clock();

    // Calcula o número total de pacientes no arquivo
    fseek(arq, 0, SEEK_END);
    long tamanhoArquivo = ftell(arq);
    int qtdPacientes = tamanhoArquivo / sizeof(Paciente);
    rewind(arq);

    printf("Iniciando ordenacao de %d pacientes usando memoria secundaria...\n", qtdPacientes);

    int trocas = 0;
    int comparacoes = 0;

    for (int i = 0; i < qtdPacientes - 1; i++) {
        int houveTroca = 0;

        for (int j = 0; j < qtdPacientes - 1 - i; j++) {
            Paciente p1, p2;

            // Lê paciente j
            fseek(arq, j * sizeof(Paciente), SEEK_SET);
            fread(&p1, sizeof(Paciente), 1, arq);

            // Lê paciente j+1
            fread(&p2, sizeof(Paciente), 1, arq);

            comparacoes++;

            if (p1.codigo > p2.codigo) {
                // Reposiciona e escreve os pacientes invertidos
                fseek(arq, j * sizeof(Paciente), SEEK_SET);
                fwrite(&p2, sizeof(Paciente), 1, arq);
                fwrite(&p1, sizeof(Paciente), 1, arq);

                trocas++;
                houveTroca = 1;
            }
        }

        if (!houveTroca)
            break;

        if ((i + 1) % 10 == 0)
            printf("Passada %d concluida...\n", i + 1);
    }

    clock_t fim = clock();
double tempoGasto = (double)(fim - inicio) / CLOCKS_PER_SEC;

printf("\nOrdenacao concluida!\n");
printf("Total de comparacoes: %d\n", comparacoes);
printf("Total de trocas: %d\n", trocas);
printf("Tempo de execucao: %.2f ms\n", tempoGasto * 1000);

// Log usando função padronizada
logTempoExecucaoOrdenacao("Bubble Sort", qtdPacientes, tempoGasto);

printf("\nBase de dados de pacientes ordenada com sucesso!\n");
fclose(arq);


}

// ====== Funcionários ======
void ordenarFuncionariosPorCodigo() {
    FILE *arq = fopen("funcionarios.dat", "rb+");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de funcionarios.\n");
        return;
    }

    clock_t inicio = clock();

    // Calcula o número total de funcionários no arquivo
    fseek(arq, 0, SEEK_END);
    long tamanhoArquivo = ftell(arq);
    int qtdFuncionarios = tamanhoArquivo / sizeof(Funcionario);
    rewind(arq);

    if (qtdFuncionarios <= 1) {
        printf("Nada a ordenar.\n");
        fclose(arq);
        return;
    }

    printf("Iniciando ordenacao de %d funcionarios usando memoria secundaria...\n", qtdFuncionarios);

    int trocas = 0;
    int comparacoes = 0;

    for (int i = 0; i < qtdFuncionarios - 1; i++) {
        int houveTroca = 0;

        for (int j = 0; j < qtdFuncionarios - 1 - i; j++) {
            Funcionario f1, f2;

            // Lê funcionário j
            fseek(arq, j * sizeof(Funcionario), SEEK_SET);
            fread(&f1, sizeof(Funcionario), 1, arq);

            // Lê funcionário j+1
            fread(&f2, sizeof(Funcionario), 1, arq);

            comparacoes++;

            if (f1.codigo > f2.codigo) {
                // Reposiciona e escreve os funcionários invertidos
                fseek(arq, j * sizeof(Funcionario), SEEK_SET);
                fwrite(&f2, sizeof(Funcionario), 1, arq);
                fwrite(&f1, sizeof(Funcionario), 1, arq);

                trocas++;
                houveTroca = 1;
            }
        }

        if (!houveTroca)
            break;

        if ((i + 1) % 10 == 0)
            printf("Passada %d concluida...\n", i + 1);
    }

    clock_t fim = clock();
    double tempoGasto = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("\nOrdenacao concluida!\n");
    printf("Total de comparacoes: %d\n", comparacoes);
    printf("Total de trocas: %d\n", trocas);
    printf("Tempo de execucao: %.2f ms\n", tempoGasto * 1000);

    // Log usando função padronizada
    logTempoExecucaoOrdenacao("Bubble Sort (Funcionarios)", qtdFuncionarios, tempoGasto);

    printf("\nBase de dados de funcionarios ordenada com sucesso!\n");
    fclose(arq);
}



// ====== Departamentos ======
void ordenarDepartamentosPorCodigo() {
    FILE *arq = fopen("departamentos.dat", "rb+");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de departamentos.\n");
        return;
    }

    clock_t inicio = clock();

    // Calcula o número total de departamentos no arquivo
    fseek(arq, 0, SEEK_END);
    long tamanhoArquivo = ftell(arq);
    int qtdDepartamentos = tamanhoArquivo / sizeof(Departamento);
    rewind(arq);

    if (qtdDepartamentos <= 1) {
        printf("Nada a ordenar.\n");
        fclose(arq);
        return;
    }

    printf("Iniciando ordenacao de %d departamentos usando memoria secundaria...\n", qtdDepartamentos);

    int trocas = 0;
    int comparacoes = 0;

    for (int i = 0; i < qtdDepartamentos - 1; i++) {
        int houveTroca = 0;

        for (int j = 0; j < qtdDepartamentos - 1 - i; j++) {
            Departamento d1, d2;

            // Lê departamento j
            fseek(arq, j * sizeof(Departamento), SEEK_SET);
            fread(&d1, sizeof(Departamento), 1, arq);

            // Lê departamento j+1
            fread(&d2, sizeof(Departamento), 1, arq);

            comparacoes++;

            if (d1.codigo > d2.codigo) {
                // Reposiciona e escreve os departamentos invertidos
                fseek(arq, j * sizeof(Departamento), SEEK_SET);
                fwrite(&d2, sizeof(Departamento), 1, arq);
                fwrite(&d1, sizeof(Departamento), 1, arq);

                trocas++;
                houveTroca = 1;
            }
        }

        if (!houveTroca)
            break;

        if ((i + 1) % 10 == 0)
            printf("Passada %d concluida...\n", i + 1);
    }

    clock_t fim = clock();
    double tempoGasto = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("\nOrdenacao concluida!\n");
    printf("Total de comparacoes: %d\n", comparacoes);
    printf("Total de trocas: %d\n", trocas);
    printf("Tempo de execucao: %.2f ms\n", tempoGasto * 1000);

    // Log usando função padronizada
    logTempoExecucaoOrdenacao("Bubble Sort (Departamentos)", qtdDepartamentos, tempoGasto);

    printf("\nBase de dados de departamentos ordenada com sucesso!\n");
    fclose(arq);
}
