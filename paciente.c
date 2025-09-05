#include "paciente.h"
#include "hospital.h"
#include "tempo.h"
#include "hash_pacientes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Busca sequencial em pacientes.dat (lê todos os registros até encontrar ou acabar)
int buscaSequencialPaciente(int codigo) {
    FILE *arquivo = fopen("pacientes.dat", "rb"); // abre o arquivo em modo leitura binária
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de pacientes.\n");
        return -1; // erro ao abrir
    }

    Paciente p;               // buffer para leitura de um registro
    int encontrado = 0;       // flag de sucesso
    clock_t inicio, fim;      // marcação de tempo
    double tempoExecucao;     // tempo da busca

    inicio = clock();         // início da medição

    // Busca sequencial: varre registro a registro até achar
    while (fread(&p, sizeof(Paciente), 1, arquivo) == 1) {
        if (p.codigo == codigo) { // se o código for igual ao buscado
            encontrado = 1;       // marca que encontrou
            break;                // interrompe a leitura
        }
    }

    fim = clock(); // fim da medição
    tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC; // calcula tempo em segundos

    fclose(arquivo); // fecha o arquivo

    if (encontrado) {
        // exibe os dados do paciente encontrado
        printf("Paciente encontrado:\n");
        printf("Codigo: %d\n", p.codigo);
        printf("Nome: %s\n", p.nome);
        printf("CPF: %s\n", p.cpf);
        printf("Endereço: %s\n", p.endereco);
    } else {
        printf("Paciente não encontrado.\n");
    }

    // registra no log: tipo de pesquisa, chave, sucesso e tempo
    gravarLogPesquisa("Pesquisa Sequencial (Paciente)", codigo, encontrado, tempoExecucao);

    // retorna 1 se achou, -1 se não
    return encontrado ? 1 : -1;
}
// Busca binária em pacientes.dat (requer arquivo ORDENADO por p.codigo)
int buscaBinariaPaciente(int codigo) {
    FILE *arquivo = fopen("pacientes.dat", "rb"); // abre arquivo em leitura binária
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de pacientes.\n");
        return -1; // erro ao abrir
    }

    // Descobre o total de registros no arquivo
    fseek(arquivo, 0, SEEK_END);                // vai para o fim
    long tamanhoArquivo = ftell(arquivo);       // pega tamanho em bytes
    int totalPacientes = tamanhoArquivo / sizeof(Paciente); // converte em nº de registros
    fseek(arquivo, 0, SEEK_SET);                // volta para o início

    Paciente p;                 // buffer para leitura
    int inicio = 0;             // limite inferior
    int fim = totalPacientes - 1; // limite superior
    int meio;                   // índice do meio
    int encontrado = 0;         // flag de sucesso
    clock_t inicioBusca, fimBusca;
    double tempoExecucao;

    inicioBusca = clock();      // início da medição

    // Algoritmo da busca binária
    while (inicio <= fim) {
        meio = (inicio + fim) / 2; // calcula posição do meio
        fseek(arquivo, meio * sizeof(Paciente), SEEK_SET); // posiciona no registro "meio"
        fread(&p, sizeof(Paciente), 1, arquivo);           // lê esse registro

        if (p.codigo == codigo) {   // se encontrou
            encontrado = 1;
            break;
        } else if (p.codigo < codigo) {
            inicio = meio + 1;      // descarta metade inferior
        } else {
            fim = meio - 1;         // descarta metade superior
        }
    }

    fimBusca = clock(); // fim da medição
    tempoExecucao = ((double)(fimBusca - inicioBusca)) / CLOCKS_PER_SEC;

    fclose(arquivo); // fecha o arquivo

    if (encontrado) {
        // exibe os dados do paciente encontrado
        printf("Paciente encontrado:\n");
        printf("Codigo: %d\n", p.codigo);
        printf("Nome: %s\n", p.nome);
        printf("CPF: %s\n", p.cpf);
        printf("Endereco: %s\n", p.endereco);
    } else {
        printf("Paciente não encontrado.\n");
    }

    // registra no log: tipo de pesquisa, chave, sucesso e tempo
    gravarLogPesquisa("Pesquisa Binária (Paciente)", codigo, encontrado, tempoExecucao);

    // retorna 1 se achou, -1 se não
    return encontrado ? 1 : -1;
}


void listarPacientes(void) {
    // Garante os departamentos carregados para resolver nomes
    carregarDepartamentosDoArquivo();

    FILE *arquivo = fopen(ARQUIVO_PACIENTES, "rb");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de pacientes (%s).\n", ARQUIVO_PACIENTES);
        return;
    }

    Paciente p;
    int exibidos = 0;

    printf("\n--- Lista de Pacientes ---\n");

    while (fread(&p, sizeof(Paciente), 1, arquivo) == 1) {
        // FILTRO: só mostra registros ativos
        if (p.ativo != 1) continue;

        // Nome do departamento
        char nomeDep[100] = "Desconhecido";
        buscarNomeDepartamentoPorCodigo(p.codigoDepartamento, nomeDep, sizeof(nomeDep));

        // Nome do responsável (funcionário)
        char nomeResp[100] = "Desconhecido";
        buscarNomeFuncionarioPorCodigo(p.codigoFuncionarioResponsavel, nomeResp, sizeof(nomeResp));

        printf("Codigo: %d\n", p.codigo);
        printf("Nome: %s\n", p.nome);
        printf("CPF: %s\n", p.cpf);
        printf("Nascimento: %s\n", p.dataNascimento);
        printf("Telefone: %s\n", p.telefone);
        printf("Endereco: %s\n", p.endereco);
        printf("Departamento: %d - %s\n", p.codigoDepartamento, nomeDep);
        printf("Responsavel: %d - %s\n", p.codigoFuncionarioResponsavel, nomeResp);
        printf("--------------------------------------------\n");

        exibidos++;
    }

    fclose(arquivo);

    if (exibidos == 0)
        printf("Nenhum paciente ativo encontrado.\n");

    printf("Total listado: %d paciente(s).\n", exibidos);
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

int removerPacienteArquivo(int codigo) {
    FILE *f = fopen("pacientes.dat", "rb+");
    if (!f) {
        printf("Erro ao abrir pacientes.dat para remoção.\n");
        return 0;
    }

    Paciente p;
    while (fread(&p, sizeof(Paciente), 1, f) == 1) {
        if (p.codigo == codigo && p.ativo == 1) {
            p.ativo = 0;  // marca como inativo
            fseek(f, -(long)sizeof(Paciente), SEEK_CUR);
            fwrite(&p, sizeof(Paciente), 1, f);
            fclose(f);
            printf("Paciente %d marcado como inativo no arquivo.\n", codigo);
            return 1;
        }
    }

    fclose(f);
    printf("Paciente %d não encontrado ou já removido.\n", codigo);
    return 0;
}
