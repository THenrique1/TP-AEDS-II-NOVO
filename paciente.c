#include "paciente.h"
#include "hospital.h"
#include "tempo.h"
#include "hash_pacientes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Função para buscar paciente por código (sequencial)
int buscaSequencialPaciente(int codigo) {
    FILE *arquivo = fopen("pacientes.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de pacientes.\n");
        return -1;
    }

    Paciente p;
    int encontrado = 0;
    clock_t inicio, fim;
    double tempoExecucao;

    inicio = clock();

    // Busca sequencial no arquivo
    while (fread(&p, sizeof(Paciente), 1, arquivo) == 1) {
        if (p.codigo == codigo) {
            encontrado = 1;
            break;
        }
    }

    fim = clock();
    tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    fclose(arquivo);

    if (encontrado) {
        printf("Paciente encontrado:\n");
        printf("Codigo: %d\n", p.codigo);
        printf("Nome: %s\n", p.nome);
        printf("CPF: %s\n", p.cpf);
        printf("Endereço: %s\n", p.endereco);
    } else {
        printf("Paciente não encontrado.\n");
    }

    gravarLogPesquisa("Pesquisa Sequencial (Paciente)", codigo, encontrado, tempoExecucao);

    return encontrado ? 1 : -1;
}

// Função para busca binária de paciente por código
int buscaBinariaPaciente(int codigo) {
    FILE *arquivo = fopen("pacientes.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de pacientes.\n");
        return -1;
    }

    // total de pacientes
    fseek(arquivo, 0, SEEK_END);
    long tamanhoArquivo = ftell(arquivo);
    int totalPacientes = tamanhoArquivo / sizeof(Paciente);

    fseek(arquivo, 0, SEEK_SET);

    Paciente p;
    int inicio = 0, fim = totalPacientes - 1, meio;
    int encontrado = 0;
    clock_t inicioBusca, fimBusca;
    double tempoExecucao;

    inicioBusca = clock();

    // Busca binária
    while (inicio <= fim) {
        meio = (inicio + fim) / 2;
        fseek(arquivo, meio * sizeof(Paciente), SEEK_SET);
        fread(&p, sizeof(Paciente), 1, arquivo);

        if (p.codigo == codigo) {
            encontrado = 1;
            break;
        } else if (p.codigo < codigo) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    fimBusca = clock();
    tempoExecucao = ((double)(fimBusca - inicioBusca)) / CLOCKS_PER_SEC;

    fclose(arquivo);

    if (encontrado) {
        printf("Paciente encontrado:\n");
        printf("Codigo: %d\n", p.codigo);
        printf("Nome: %s\n", p.nome);
        printf("CPF: %s\n", p.cpf);
        printf("Endereco: %s\n", p.endereco);
    } else {
        printf("Paciente não encontrado.\n");
    }


    gravarLogPesquisa("Pesquisa Binária (Paciente)", codigo, encontrado, tempoExecucao);

    return encontrado ? 1 : -1;
}

void listarPacientes(void) {
    FILE *arquivo = fopen(ARQUIVO_PACIENTES, "rb"); // use a macro única
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de pacientes.\n");
        return;
    }

    Paciente p;
    int exibidos = 0;

    printf("\n--- Lista de Pacientes ---\n");

    while (fread(&p, sizeof(Paciente), 1, arquivo) == 1) {
        /* FILTRO ESSENCIAL: só mostra ativos */
        if (p.ativo != 1) {
            continue;
        }

        /* Buscar nome do departamento */
        const char *nomeDep = "Desconhecido";
        for (int i = 0; i < totalDepartamentos; i++) {
            if (departamentos[i].codigo == p.codigoDepartamento) {
                nomeDep = departamentos[i].nome;
                break;
            }
        }

        /* Buscar nome do responsável (funcionário) */
        char nomeResp[100] = "Desconhecido";
        buscarNomeFuncionarioPorCodigo(p.codigoFuncionarioResponsavel, nomeResp, sizeof(nomeResp));

        printf("Codigo: %d\n", p.codigo);
        printf("Nome: %s\n", p.nome);
        printf("CPF: %s\n", p.cpf);
        /* Se dataNascimento for string, mantenha assim; se for struct, ajuste a impressão */
        printf("Nascimento: %s\n", p.dataNascimento);
        printf("Telefone: %s\n", p.telefone);
        printf("Endereco: %s\n", p.endereco);
        printf("Departamento: %d - %s\n", p.codigoDepartamento, nomeDep);
        printf("Responsavel: %d - %s\n", p.codigoFuncionarioResponsavel, nomeResp);
        printf("----------------------------------------\n");

        exibidos++;
    }

    fclose(arquivo);

    if (exibidos == 0) {
        printf("Nenhum paciente encontrado.\n");
    }

    /* Mostra o que de fato foi listado, não o global */
    printf("Total listado: %d pacientes.\n", exibidos);
}


 void buscarNomeFuncionarioPorCodigo(int codigo, char *destino, size_t tamanho) {
    FILE *arq = fopen("funcionarios.dat", "rb");
    if (!arq) {
        strncpy(destino, "Desconhecido", tamanho);
        return;
    }

    Funcionario f;
    while (fread(&f, sizeof(Funcionario), 1, arq) == 1) {
        if (f.codigo == codigo) {
            strncpy(destino, f.nome, tamanho);
            fclose(arq);
            return;
        }
    }

    fclose(arq);
    strncpy(destino, "Desconhecido", tamanho);
}

void buscarNomeDepartamentoPorCodigo(int codigo, char *destino, size_t tamanho) {
    FILE *arq = fopen("departamentos.dat", "rb");
    if (!arq) {
        strncpy(destino, "Desconhecido", tamanho);
        return;
    }

    Departamento d;
    while (fread(&d, sizeof(Departamento), 1, arq) == 1) {
        if (d.codigo == codigo) {
            strncpy(destino, d.nome, tamanho);
            fclose(arq);
            return;
        }
    }

    fclose(arq);
    strncpy(destino, "Desconhecido", tamanho);
}

