#include "hospital.h"
#include "funcionario.h"
#include "paciente.h"
#include "departamento.h"
#include "gerador_dados.h"
#include "subpaciente.h"
#include "tempo.h"
#include "iteracoes.h"
#include "hash_pacientes.h"
extern HashPacientes tabelaHash;
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



void exibirCabecalho() {

    printf("\n===================================================\n");
    printf("               Sistema de Gestao Hospitalar        \n");
    printf("===================================================\n");
}

void exibirMenu() {
    printf("\n================== Menu Principal =================\n");
    printf("1. Gerar Departamnetos Aleatorios\n");
    printf("2. Gerar Funcionarios Aleatorios\n");
    printf("3. Gerar Pacientes Aleatorios\n");
    printf("4. Listar Pacientes\n");
    printf("5. Listar Funcionarios\n");
    printf("6. Listar Departamentos\n");
    printf("7. Ordenar Paciente com Bubble Sort \n");
    printf("8. Ordenar Funcionarios com Bubble Sort\n");
    printf("9. Ordenar Departamentos com Bubble Sort \n");
    printf("10. Buscar Paciente por Codigo (Sequencial)/ (Binaria)\n");
    printf("11. Buscar Funcionario por Codigo (Sequencial)/(Binaria)\n");
    printf("12. Buscar Departamento por Codigo (Sequencial)/(Binaria)\n");
    printf("13. Listar Pacientes por Departamento\n");
    printf("14. Transferir Paciente de Departamento\n");
    printf("15. Listar Funcionarios por Departamento\n");
    printf("16. Relatorio de Ocupacao dos Departamentos\n");
    printf("17. Ordenar pacientes usando Selecao Natural + Arvore de Vencedores\n");
    printf("18. Testar tempos de Execucao Bubble sort, Selecao Natural e Arvore de Vencedores \n");
    printf("19. Inserir Paciente (Hash)\n");
    printf("20. Buscar Paciente (Hash)\n");
    printf("21. Remover Paciente (Hash)\n");
    printf("22. imprimir tabela Hash");
    printf("====================================================\n");
    printf("Escolha uma opcao: ");
}


void processarMenuPrincipal() {
    int opcao;

carregarTotalDepartamentos();
carregarTotalFuncionarios();
carregarTotalPacientes();


    do {
        exibirCabecalho();
        exibirMenu();
        scanf(" %d", &opcao);
        processarOpcaoMenu(opcao);
    } while (opcao != 29);
}

void processarOpcaoMenu(int opcao) {
    switch (opcao) {
        case 1: {
            int quantidade;
            printf("Digite a quantidade de departamentos aleatorios: ");
            scanf("%d", &quantidade);
            gerarDepartamentosAleatorios(quantidade);
            totalDepartamentos += quantidade;
            salvarTotalDepartamentos();
            break;
        }
       case 2: {
        int quantidade;
        printf("Digite a quantidade de funcionarios aleatorios: ");
        scanf("%d", &quantidade);
        gerarFuncionariosAleatorios(quantidade); // já salva e atualiza total
        break;
      }

      case 3: {
       int quantidade;
       printf("Digite a quantidade de pacientes aleatorios: ");
       scanf("%d", &quantidade);
       gerarPacientesAleatorios(quantidade); // já salva e atualiza total
        recarregarHashPacientes(&tabelaHash);
       break;
      }

       case 4: {
    FILE *arquivo = fopen("pacientes.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de pacientes.\n");
        break;
    }

    Paciente p;
    int exibidos = 0;

    printf("\n--- Lista de Pacientes ---\n");

    while (fread(&p, sizeof(Paciente), 1, arquivo) == 1) {
        char nomeDep[100], nomeResp[100];

        buscarNomeDepartamentoPorCodigo(p.codigoDepartamento, nomeDep, sizeof(nomeDep));
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
        printf("Nenhum paciente encontrado.\n");

    printf("Total listado: %d pacientes.\n", totalPacientes);
    break;
}

       case 5: {
    FILE *arquivo = fopen("funcionarios.dat", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir funcionarios.dat para leitura.\n");
        break;
    }

    Funcionario f;
    int exibidos = 0;

    printf("\n--- Lista de Funcionários ---\n");

    while (fread(&f, sizeof(Funcionario), 1, arquivo) == 1) {
        char nomeDep[100];
        buscarNomeDepartamentoPorCodigo(f.codigoDepartamento, nomeDep, sizeof(nomeDep));

        printf("Codigo: %d\n", f.codigo);
        printf("Nome: %s\n", f.nome);
        printf("Cargo: %s\n", f.cargo);
        printf("Especializacao: %s\n", f.especializacao);
        printf("Salario: %.2f\n", f.salario);
        printf("Departamento: %d - %s\n", f.codigoDepartamento, nomeDep);
        printf("--------------------------------------------\n");

        exibidos++;
    }

    fclose(arquivo);

    if (exibidos == 0)
        printf("Nenhum funcionario encontrado.\n");

    printf("Total listado: %d funcionario(s).\n", totalFuncionarios);
    break;
}
        case 6:
            listarDepartamentos();
            break;

    case 7:
         if (totalPacientes == 0) {
                printf("Nenhum paciente cadastrado para ordenar.\n");
                break;
            }
            printf("Bubble Sort \n");
            ordenarPacientesPorCodigoBubbleSort();
            recarregarHashPacientes(&tabelaHash);
        break;


case 8:
    if (totalFuncionarios == 0) {
        printf("Nenhum funcionário cadastrado para ordenar.\n");
        break;
    }
    ordenarFuncionariosPorCodigo();
    break;


 case 9: {
    if (totalDepartamentos == 0) {
        printf("Nenhum departamento cadastrado para ordenar.\n");
        break;
    }
    int blocos = contarBlocosArquivo("departamentos.dat", sizeof(Departamento));
    printf("Iniciando ordenacao externa de departamentos (%d blocos)...\n", blocos);
    ordenarDepartamentosPorCodigo();
    salvarTotalDepartamentos();
    break;
}
case 11: {
    int codigo, tipoBusca;
    printf("=== Busca de Funcionario ===\n");
    printf("Digite o codigo do funcionario: ");
    scanf("%d", &codigo);
    printf("Escolha o tipo de busca:\n");
    printf("1 - Binaria\n");
    printf("2 - Sequencial\n");
    printf("Opcao: ");
    scanf("%d", &tipoBusca);

    if (tipoBusca == 1)
        buscaBinariaFuncionario(codigo);
    else if (tipoBusca == 2)
        buscaSequencialFuncionario(codigo);
    else
        printf("Opção invalida!\n");

    break;
}

case 10: {
    int codigo, tipoBusca;
    printf("=== Busca de Paciente ===\n");
    printf("Digite o codigo do paciente: ");
    scanf("%d", &codigo);
    printf("Escolha o tipo de busca:\n");
    printf("1 - Binaria\n");
    printf("2 - Sequencial\n");
    printf("Opcao: ");
    scanf("%d", &tipoBusca);

    if (tipoBusca == 1)
        buscaBinariaPaciente(codigo);
    else if (tipoBusca == 2)
        buscaSequencialPaciente(codigo);
    else
        printf("Opção inválida!\n");

    break;
}

case 12: {
    int codigo, tipoBusca;
    printf("=== Busca de Departamento ===\n");
    printf("Digite o codigo do departamento: ");
    scanf("%d", &codigo);
    printf("Escolha o tipo de busca:\n");
    printf("1 - Binaria\n");
    printf("2 - Sequencial\n");
    printf("Opcao: ");
    scanf("%d", &tipoBusca);

    if (tipoBusca == 1)
        buscaBinariaDepartamento(codigo);
    else if (tipoBusca == 2)
        buscaSequencialDepartamento(codigo);
    else
        printf("Opção inválida!\n");

    break;
}


        case 13:
             listarPacientesPorDepartamento();
            break;
        case 14:
            transferirPaciente();
            break;
        case 15:
            listarFuncionariosPorDepartamento();
            break;
        case 16:
            relatorioOcupacaoDepartamentos();
            break;

case 17: {
    int qtd;
    printf("Digite a quantidade de pacientes para ordenar: ");
    scanf("%d", &qtd);

    gerarPacientesAleatorios(qtd);

    printf("\n Gerando particoes com Selecao Natural...\n");
    int totalParticoes = gerarParticoesPorSelecaoNaturalPacientes();

    printf("\n Intercalando partições com Arvore de Vencedores...\n");
    intercalarComArvoreDeVencedoresPacientes(totalParticoes);

    // === Listar os pacientes ordenados ===
    FILE *arq = fopen("pacientes.dat", "rb");
    if (!arq) {
        printf("Erro ao abrir pacientes.dat para leitura.\n");
        break;
    }

    Paciente p;
    while (fread(&p, sizeof(Paciente), 1, arq) == 1) {
        char nomeDep[100] = "Desconhecido";
        char nomeResp[100] = "Desconhecido";

        Departamento d;
        FILE *depFile = fopen("departamentos.dat", "rb");
        if (depFile) {
            while (fread(&d, sizeof(Departamento), 1, depFile) == 1) {
                if (d.codigo == p.codigoDepartamento) {
                    strcpy(nomeDep, d.nome);
                    break;
                }
            }
            fclose(depFile);
        }

        Funcionario f;
        FILE *funcFile = fopen("funcionarios.dat", "rb");
        if (funcFile) {
            while (fread(&f, sizeof(Funcionario), 1, funcFile) == 1) {
                if (f.codigo == p.codigoFuncionarioResponsavel) {
                    strcpy(nomeResp, f.nome);
                    break;
                }
            }
            fclose(funcFile);
        }

        printf("Codigo: %d\n", p.codigo);
        printf("Nome: %s\n", p.nome);
        printf("CPF: %s\n", p.cpf);
        printf("Nascimento: %s\n", p.dataNascimento);
        printf("Telefone: %s\n", p.telefone);
        printf("Endereco: %s\n", p.endereco);
        printf("Departamento: %d - %s\n", p.codigoDepartamento, nomeDep);
        printf("Responsavel: %d - %s\n", p.codigoFuncionarioResponsavel, nomeResp);
        printf("----------------------------------------\n");
    }
    printf("Total de pacientes ordenados: %d\n", totalPacientes);

    fclose(arq);
    break;
}

case 18: {
    int qtd;
    printf("Digite a quantidade de pacientes para teste: ");
    scanf("%d", &qtd);

    char logMsg[256];

    // === Teste Bubble Sort ===
    gerarPacientesAleatorios(qtd);
    FILE *arqBubble = fopen("pacientes.dat", "rb+");
    if (!arqBubble) {
        printf("Erro ao abrir pacientes.dat para Bubble Sort.\n");
        break;
    }

    clock_t inicioBubble = clock();
    ordenarPacientesPorCodigoBubbleSort(arqBubble);
    clock_t fimBubble = clock();
    double tempoBubble = (double)(fimBubble - inicioBubble) / CLOCKS_PER_SEC;
    fclose(arqBubble);

    // === Teste Seleção Natural ===
    gerarPacientesAleatorios(qtd);
    clock_t inicioSelecao = clock();
    int numParticoes = gerarParticoesPorSelecaoNaturalPacientes();
    clock_t fimSelecao = clock();
    double tempoSelecao = (double)(fimSelecao - inicioSelecao) / CLOCKS_PER_SEC;

    // === Teste Árvore de Vencedores ===
    double tempoVencedores = 0.0;
    if (numParticoes > 0) {
        clock_t inicioVencedores = clock();
        intercalarComArvoreDeVencedoresPacientes(numParticoes);
        clock_t fimVencedores = clock();
        tempoVencedores = (double)(fimVencedores - inicioVencedores) / CLOCKS_PER_SEC;
    } else {
        printf("Nenhuma partição gerada. Intercalação cancelada.\n");
    }

    // === Log consolidado (arquivo e terminal) ===
    logTempoExecucaoOrdenacao("Bubble Sort", qtd, tempoBubble);
    logTempoExecucaoOrdenacao("Selecao Natural", qtd, tempoSelecao);
    logTempoExecucaoOrdenacao("Arvore de Vencedores", qtd, tempoVencedores);

    snprintf(logMsg, sizeof(logMsg),
             "[RESUMO] TESTE %d PACIENTES -> Bubble: %.2fs | Selecao: %.2fs | Vencedores: %.2fs",
             qtd, tempoBubble, tempoSelecao, tempoVencedores);
    registrarLog(logMsg);
    printf("================ RELACAO DE TEMPO ENTRE OS METODOS ====================");
    printf("\n Bubble Sort: %.2f s | Selecao natural: %.2f s | Vencedores: %.2f s\n",
           tempoBubble, tempoSelecao, tempoVencedores);
    printf("=======================================================================");
    break;
}
   case 19: {  // Inserir Paciente (Hash)
            Paciente p;
            printf("Codigo: ");
            scanf("%d", &p.codigo);

            printf("Nome: ");
            scanf(" %[^\n]", p.nome);

            printf("CPF: ");
            scanf(" %[^\n]", p.cpf);

            printf("Data de Nascimento (dd/mm/aaaa): ");
            scanf(" %[^\n]", p.dataNascimento);

            printf("Telefone: ");
            scanf(" %[^\n]", p.telefone);

            printf("Endereco: ");
            scanf(" %[^\n]", p.endereco);

            printf("Codigo do Departamento: ");
            scanf("%d", &p.codigoDepartamento);

            printf("Codigo do Funcionario Responsavel: ");
            scanf("%d", &p.codigoFuncionarioResponsavel);

            p.ativo = 1;  // Marca como válido
            inserirPacienteHash(&tabelaHash, p);  // Chama a função de inserção no hash
            printf("Paciente inserido com sucesso!\n");
            break;
        }

        case 20: {  // Buscar Paciente (Hash)
            int codigo;
            printf("Codigo do paciente a buscar: ");
            scanf("%d", &codigo);

            Paciente *p = buscarPacienteHash(&tabelaHash, codigo);  // Chama a função de busca no hash
            if (p) {
                printf("\n--- Paciente encontrado ---\n");
                printf("Codigo: %d\n", p->codigo);
                printf("Nome: %s\n", p->nome);
                printf("CPF: %s\n", p->cpf);
                printf("Data de Nascimento: %s\n", p->dataNascimento);
                printf("Telefone: %s\n", p->telefone);
                printf("Endereco: %s\n", p->endereco);
                printf("Departamento: %d\n", p->codigoDepartamento);
                printf("Funcionario Responsavel: %d\n", p->codigoFuncionarioResponsavel);
                free(p);
            } else {
                printf("Paciente não encontrado.\n");
            }
            break;
        }

        case 21: {  // Remover Paciente (Hash)
            int codigo;
            printf("Codigo do paciente a remover: ");
            scanf("%d", &codigo);
            removerPacienteHash(&tabelaHash, codigo);  // Chama a função de remoção no hash
            printf("Paciente removido com sucesso!\n");
            break;
        }

        case 22: {  // Imprimir Tabela Hash Completa
            imprimir_tabela_hash_pacientes(&tabelaHash, arq_pacientes);  // Chama a função de impressão da tabela hash
            break;
        }
}

void transferirPaciente() {
     carregarDepartamentosDoArquivo();
    int codPaciente, codDepartamento;

    printf("\nCodigo do Paciente: ");
    scanf("%d", &codPaciente);

    printf("Novo codigo do Departamento: ");
    scanf("%d", &codDepartamento);

    int iDepartamento = buscarIndiceDepartamento(codDepartamento);
    if (iDepartamento == -1) {
        printf("Departamento não encontrado.\n");
        return;
    }

    // Buscar novo responsável no departamento
    FILE *arquivoFunc = fopen("funcionarios.dat", "rb");
    if (!arquivoFunc) {
        printf("Erro ao abrir funcionarios.dat\n");
        return;
    }

    Funcionario f;
    int novoResponsavel = -1;
    while (fread(&f, sizeof(Funcionario), 1, arquivoFunc) == 1) {
        if (f.codigoDepartamento == codDepartamento) {
            novoResponsavel = f.codigo;
            break;
        }
    }
    fclose(arquivoFunc);

    if (novoResponsavel == -1) {
        printf("Departamento não possui funcionario. Transferencia cancelada.\n");
        return;
    }

    // Atualizar o paciente no arquivo
    FILE *arquivoPacientes = fopen("pacientes.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    if (!arquivoPacientes || !temp) {
        printf("Erro ao abrir arquivos de pacientes.\n");
        if (arquivoPacientes) fclose(arquivoPacientes);
        if (temp) fclose(temp);
        return;
    }

    Paciente p;
    int transferido = 0;

    while (fread(&p, sizeof(Paciente), 1, arquivoPacientes) == 1) {
        if (p.codigo == codPaciente) {
            p.codigoDepartamento = codDepartamento;
            p.codigoFuncionarioResponsavel = novoResponsavel;
            transferido = 1;
        }
        fwrite(&p, sizeof(Paciente), 1, temp);
    }

    fclose(arquivoPacientes);
    fclose(temp);

    if (transferido) {
        remove("pacientes.dat");
        rename("temp.dat", "pacientes.dat");
        carregarPacientesDoArquivo();  // Atualiza totalPacientes
        printf("Paciente transferido para o departamento %s (Resp. código %d).\n",
               departamentos[iDepartamento].nome, novoResponsavel);
    } else {
        remove("temp.dat");
        printf("Paciente não encontrado.\n");
    }
}

// Função: Listar pacientes por departamento
void listarPacientesPorDepartamento() {
     carregarDepartamentosDoArquivo();
    int codDepartamento;
    printf("\nDigite o codigo do Departamento: ");
    scanf("%d", &codDepartamento);

    int iDepartamento = buscarIndiceDepartamento(codDepartamento);
    if (iDepartamento == -1) {
        printf("Departamento não encontrado.\n");
        return;
    }

    printf("\nPacientes no Departamento %s:\n", departamentos[iDepartamento].nome);

    FILE *arquivo = fopen("pacientes.dat", "rb");
    if (!arquivo) {
        printf("Erro ao abrir pacientes.dat\n");
        return;
    }

    Paciente p;
    int encontrou = 0;

    while (fread(&p, sizeof(Paciente), 1, arquivo) == 1) {
        if (p.codigoDepartamento == codDepartamento) {
            printf("Código: %d | Nome: %s\n", p.codigo, p.nome);
            encontrou = 1;
        }
    }

    fclose(arquivo);

    if (!encontrou) {
        printf("Nenhum paciente alocado neste departamento.\n");
    }
}

// Função: Listar funcionários por departamento
void listarFuncionariosPorDepartamento() {
     carregarDepartamentosDoArquivo();
    int codDepartamento;
    printf("\nDigite o codigo do Departamento: ");
    scanf("%d", &codDepartamento);

    int iDepartamento = buscarIndiceDepartamento(codDepartamento);
    if (iDepartamento == -1) {
        printf("Departamento não encontrado.\n");
        return;
    }

    printf("\nFuncionários no Departamento %s:\n", departamentos[iDepartamento].nome);

    FILE *arquivo = fopen("funcionarios.dat", "rb");
    if (!arquivo) {
        printf("Erro ao abrir funcionarios.dat\n");
        return;
    }

    Funcionario f;
    int encontrou = 0;

    while (fread(&f, sizeof(Funcionario), 1, arquivo) == 1) {
        if (f.codigoDepartamento == codDepartamento) {
            printf("Código: %d | Nome: %s | Cargo: %s\n",
                   f.codigo, f.nome, f.cargo);
            encontrou = 1;
        }
    }

    fclose(arquivo);

    if (!encontrou) {
        printf("Nenhum funcionário alocado neste departamento.\n");
    }
}


// Função: Relatório geral de ocupação
void relatorioOcupacaoDepartamentos() {
    printf("\n=== Relatorio de Ocupacao por Departamento ===\n");
   carregarDepartamentosDoArquivo();
    for (int i = 0; i < totalDepartamentos; i++) {
        int qtdPacientes = 0, qtdFuncionarios = 0;

        // Contar pacientes
        FILE *arquivoPacientes = fopen("pacientes.dat", "rb");
        if (arquivoPacientes) {
            Paciente p;
            while (fread(&p, sizeof(Paciente), 1, arquivoPacientes) == 1) {
                if (p.codigoDepartamento == departamentos[i].codigo) {
                    qtdPacientes++;
                }
            }
            fclose(arquivoPacientes);
        } else {
            printf("Erro ao abrir pacientes.dat\n");
        }

        // Contar funcionários
        FILE *arquivoFuncionarios = fopen("funcionarios.dat", "rb");
        if (arquivoFuncionarios) {
            Funcionario f;
            while (fread(&f, sizeof(Funcionario), 1, arquivoFuncionarios) == 1) {
                if (f.codigoDepartamento == departamentos[i].codigo) {
                    qtdFuncionarios++;
                }
            }
            fclose(arquivoFuncionarios);
        } else {
            printf("Erro ao abrir funcionarios.dat\n");
        }

        // Imprimir o relatório
        printf("Departamento: %s (Codigo: %d)\n", departamentos[i].nome, departamentos[i].codigo);
        printf("  Pacientes: %d\n", qtdPacientes);
        printf("  Funcionarios: %d\n\n", qtdFuncionarios);
    }
}

