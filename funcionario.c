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
        // Buscar nome do departamento diretamente em departamentos.dat
        const char *nomeDepartamento = "Desconhecido";
        Departamento d;
        FILE *arqDep = fopen("departamentos.dat", "rb");
        if (arqDep) {
            while (fread(&d, sizeof(Departamento), 1, arqDep) == 1) {
                if (d.codigo == f.codigoDepartamento) {
                    nomeDepartamento = d.nome;
                    break;
                }
            }
            fclose(arqDep);
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

// Busca binária em funcionarios.dat (requer arquivo ORDENADO por f.codigo)
int buscaBinariaFuncionario(int codigo) {
    FILE *arquivo = fopen("funcionarios.dat", "rb"); // abre o arquivo em leitura binária
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de funcionarios.\n");
        return -1; // erro ao abrir
    }

    clock_t inicio, fim;           // marcação de tempo
    double tempoExecucao;          // tempo total da busca

    inicio = clock();              // início da medição

    Funcionario f;                 // buffer para ler um registro do arquivo
    int inicioBusca = 0;           // limite inferior da busca (índice lógico)
    int fimBusca = totalFuncionarios - 1; // limite superior
    int meio;                      // índice do meio
    int encontrado = 0;            // flag de sucesso

    // Loop da busca binária no arquivo
    while (inicioBusca <= fimBusca) {
        meio = (inicioBusca + fimBusca) / 2;                    // calcula posição do meio
        fseek(arquivo, meio * sizeof(Funcionario), SEEK_SET);   // posiciona no registro "meio"
        fread(&f, sizeof(Funcionario), 1, arquivo);             // lê o registro do meio

        if (f.codigo == codigo) {        // compara o campo-chave
            encontrado = 1;              // achou
            break;                       // sai do loop
        } else if (f.codigo < codigo) {  // se o código no arquivo é menor
            inicioBusca = meio + 1;      // busca continua na metade superior
        } else {                          // se o código no arquivo é maior
            fimBusca = meio - 1;         // busca continua na metade inferior
        }
    }

    fim = clock(); // fim da medição
    tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC; // calcula tempo em segundos

    // registra no log a operação (tipo, chave, sucesso, tempo)
    gravarLogPesquisa("Pesquisa Binaria (Funcionario)", codigo, encontrado, tempoExecucao);

    fclose(arquivo); // fecha o arquivo

    if (encontrado) {
        // imprime dados do funcionário encontrado
        printf("Funcionario encontrado:\n");
        printf("Codigo: %d\n", f.codigo);
        printf("Nome: %s\n", f.nome);
        printf("Cargo: %s\n", f.cargo);
        printf("Salario: %.2f\n", f.salario);
    } else {
        printf("Funcionario não encontrado.\n");
    }

    // retorna o índice (meio) se encontrou, senão -1
    return encontrado ? meio : -1;
}

// Busca sequencial em funcionarios.dat (varre registro a registro até encontrar)
int buscaSequencialFuncionario(int codigo) {
    FILE *arquivo = fopen("funcionarios.dat", "rb"); // abre o arquivo em leitura binária
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de funcionarios.\n");
        return -1; // erro ao abrir
    }

    Funcionario f;           // buffer para leitura de um registro
    int encontrado = 0;      // flag de sucesso
    clock_t inicio, fim;     // para medir o tempo
    double tempoExecucao;    // tempo total da busca

    inicio = clock();        // início da medição

    // Varredura sequencial do arquivo
    while (fread(&f, sizeof(Funcionario), 1, arquivo) == 1) { // lê 1 registro por vez
        if (f.codigo == codigo) {   // compara o campo-chave
            encontrado = 1;         // achou
            break;                   // interrompe a varredura
        }
    }

    fim = clock(); // fim da medição
    tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC; // calcula tempo

    // registra no log a operação (tipo, chave, sucesso, tempo)
    gravarLogPesquisa("Pesquisa Sequencial (Funcionario)", codigo, encontrado, tempoExecucao);

    fclose(arquivo); // fecha o arquivo

    if (encontrado) {
        // imprime dados do funcionário encontrado
        printf("Funcionario encontrado:\n");
        printf("Codigo: %d\n", f.codigo);
        printf("Nome: %s\n", f.nome);
        printf("Cargo: %s\n", f.cargo);
        printf("Salario: %.2f\n", f.salario);
    } else {
        printf("Funcionario não encontrado.\n");
    }

    // retorna 1 se encontrou, -1 se não
    return encontrado ? 1 : -1;
}
