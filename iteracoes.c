#include "hospital.h"
#include "funcionario.h"
#include "paciente.h"
#include "departamento.h"
#include "gerador_dados.h"
#include "subpaciente.h"
#include "tempo.h"
#include "iteracoes.h"
#include "hash_pacientes.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

TabelaHashPacientes tabelaHash;


void menuHash() {
    int op;
    do {
        printf("\nMENU HASH - Pacientes\n");
        printf("1. Inicializar Hash Encadeado (Arquivo)\n");
        printf("2. Inserir Paciente (Hash Encadeado/Arquivo)\n");
        printf("3. Buscar Paciente (Hash Encadeado/Arquivo)\n");
        printf("4. Remover Paciente (Hash Encadeado/Arquivo)\n");
        printf("5. Listar Pacientes (Hash Encadeado/Arquivo)\n");
        printf("0. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &op);
        switch (op) {
            case 1: {
                char confirma;
                printf("ATENCAO: Esta operacao vai apagar TODOS os pacientes do hash! Deseja continuar? (s/n): ");
                scanf(" %c", &confirma);
                if (confirma == 's' || confirma == 'S') {
                    FILE *arqHash = fopen("pacientes_hash.dat", "wb+");
                    if (arqHash != NULL) {
                        inicializarTabelaHashEncArquivo(arqHash); // Inicializa a tabela hash
                        fclose(arqHash);
                        printf("Arquivo de hash encadeado inicializado com sucesso!\n");
                    } else {
                        printf("Erro ao criar arquivo de hash encadeado.\n");
                    }
                } else {
                    printf("Operacao cancelada. O hash NAO foi apagado.\n");
                }
                break;
            }
            case 2: {
                Paciente paciente;
                printf("Nome: ");
                scanf(" %[^\n]", paciente.nome);  // Corrigido para permitir leitura de strings com espaços
                printf("Codigo (ID): ");
                scanf("%d", &paciente.codigo);
                printf("CPF: ");
                scanf("%s", paciente.cpf);
                printf("Data de Nascimento (dd/mm/aaaa): ");
                scanf("%s", paciente.dataNascimento);
                printf("Telefone: ");
                scanf("%s", paciente.telefone);
                printf("Endereco: ");
                scanf(" %[^\n]", paciente.endereco); // Corrigido para permitir leitura de endereços com espaços
                printf("Codigo do Departamento: ");
                scanf("%d", &paciente.codigoDepartamento);
                printf("Codigo do Funcionario Responsavel: ");
                scanf("%d", &paciente.codigoFuncionarioResponsavel);
                paciente.ativo = 1; // Assume que o paciente é ativo inicialmente

                // Grava no arquivo .dat para obter a posição
                FILE *arqDat = fopen("pacientes.dat", "ab");
                long posicaoArquivo = -1;
                if (arqDat != NULL) {
                    posicaoArquivo = ftell(arqDat); // Obtém o offset do arquivo
                    fwrite(&paciente, sizeof(Paciente), 1, arqDat);
                    fclose(arqDat);
                }

                // Depois insere no hash
                FILE *arqHash = fopen("pacientes_hash.dat", "rb+");
                if (arqHash != NULL) {
                    inserirPacienteHashEncArquivo(arqHash, paciente); // Insere no hash
                    int slot = hash(paciente.codigo); // Exibe onde o paciente foi inserido
                    printf("Gaveta %d: %s | Codigo: %d | Posicao .dat: %ld\n",
                           slot, paciente.nome, paciente.codigo, posicaoArquivo);
                    fclose(arqHash);
                } else {
                    printf("Arquivo de hash encadeado nao encontrado. Inicialize primeiro!\n");
                }
                break;
            }
            case 3: {
                int codigo;
                printf("Digite o codigo (ID) do paciente para buscar: ");
                scanf("%d", &codigo);
                FILE *arqHash = fopen("pacientes_hash.dat", "rb");
                if (arqHash != NULL) {
                    Paciente paciente;
                    if (buscarPacienteHashEncArquivo(arqHash, codigo, &paciente)) {
                        int slot = hash(codigo);
                        printf("Paciente encontrado na gaveta %d: %s | Codigo: %d\n",
                               slot, paciente.nome, paciente.codigo);
                    } else {
                        printf("Paciente nao encontrado na tabela hash encadeada (arquivo).\n");
                    }
                    fclose(arqHash);
                } else {
                    printf("Arquivo de hash encadeado nao encontrado. Inicialize primeiro!\n");
                }
                break;
            }
            case 4: {
                int codigo;
                printf("Digite o codigo (ID) do paciente para remover: ");
                scanf("%d", &codigo);
                FILE *arqHash = fopen("pacientes_hash.dat", "rb+");
                if (arqHash != NULL) {
                    removerPacienteHashEncArquivo(arqHash, codigo); // Remove o paciente
                    fclose(arqHash);
                    removerPacienteArquivo(codigo);
                } else {
                    printf("Arquivo de hash encadeado nao encontrado. Inicialize primeiro!\n");
                }
                break;
            }
            case 5: {
                FILE *arqHash = fopen("pacientes_hash.dat", "rb");
                if (arqHash != NULL) {
                    printf("\n--- Listagem de pacientes na tabela hash encadeada (arquivo) ---\n");
                    for (int i = 0; i < TAM_TABELA_HASH_ENC; i++) {
                        long head;
                        fseek(arqHash, i * sizeof(long), SEEK_SET); // Vai até o ponteiro do bucket
                        fread(&head, sizeof(long), 1, arqHash); // Lê o ponteiro para o início da lista
                        long atual = head;
                        NoPaciente no;
                        if (atual != OFFSET_INVALIDO) {
                            printf("Gaveta %d:\n", i);
                            while (atual != OFFSET_INVALIDO) {
                                fseek(arqHash, atual, SEEK_SET); // Vai até o nó atual
                                fread(&no, sizeof(NoPaciente), 1, arqHash); // Lê o nó
                                printf("  Nome: %s | Codigo: %d\n",
                                       no.paciente.nome, no.paciente.codigo);
                                atual = no.prox; // Vai para o próximo nó
                            }
                        }
                    }
                    fclose(arqHash);
                } else {
                    printf("Arquivo de hash encadeado nao encontrado. Inicialize primeiro!\n");
                }
                break;
            }
            case 0: {
                printf("Voltando ao menu principal...\n");
                break;
            }
            default:
                printf("Opcao invalida!\n");
        }
    } while (op != 0);
}

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
    printf("19. Menu (Hash)\n");
    printf("20. Sair e Encerrar o Programa\n");
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
        gerarFuncionariosAleatorios(quantidade); // j  salva e atualiza total
        break;
      }

      case 3: {
       int quantidade;
       printf("Digite a quantidade de pacientes aleatorios: ");
       scanf("%d", &quantidade);
       gerarPacientesAleatorios(quantidade); // j  salva e atualiza total
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

    printf("\n--- Lista de Funcion rios ---\n");

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
        printf("Nenhum funcion rio cadastrado para ordenar.\n");
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
        printf("Op  o invalida!\n");

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
        printf("Op  o inv lida!\n");

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
        printf("Op  o inv lida!\n");

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

    printf("\n Intercalando parti  es com Arvore de Vencedores...\n");
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

    // === Teste Sele  o Natural ===
    gerarPacientesAleatorios(qtd);
    clock_t inicioSelecao = clock();
    int numParticoes = gerarParticoesPorSelecaoNaturalPacientes();
    clock_t fimSelecao = clock();
    double tempoSelecao = (double)(fimSelecao - inicioSelecao) / CLOCKS_PER_SEC;

    // === Teste  rvore de Vencedores ===
    double tempoVencedores = 0.0;
    if (numParticoes > 0) {
        clock_t inicioVencedores = clock();
        intercalarComArvoreDeVencedoresPacientes(numParticoes);
        clock_t fimVencedores = clock();
        tempoVencedores = (double)(fimVencedores - inicioVencedores) / CLOCKS_PER_SEC;
    } else {
        printf("Nenhuma parti  o gerada. Intercala  o cancelada.\n");
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
    case 19: {
      menuHash();
      break;
    }

        case 20:
            printf("Saindo...\n");
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
        printf("Departamento n o encontrado.\n");
        return;
    }

    // Buscar novo respons vel no departamento
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
        printf("Departamento n o possui funcionario. Transferencia cancelada.\n");
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
        printf("Paciente transferido para o departamento %s (Resp. c digo %d).\n",
               departamentos[iDepartamento].nome, novoResponsavel);
    } else {
        remove("temp.dat");
        printf("Paciente n o encontrado.\n");
    }
}

// Fun  o: Listar pacientes por departamento
void listarPacientesPorDepartamento() {
     carregarDepartamentosDoArquivo();
    int codDepartamento;
    printf("\nDigite o codigo do Departamento: ");
    scanf("%d", &codDepartamento);

    int iDepartamento = buscarIndiceDepartamento(codDepartamento);
    if (iDepartamento == -1) {
        printf("Departamento n o encontrado.\n");
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
            printf("C digo: %d | Nome: %s\n", p.codigo, p.nome);
            encontrou = 1;
        }
    }

    fclose(arquivo);

    if (!encontrou) {
        printf("Nenhum paciente alocado neste departamento.\n");
    }
}

// Fun  o: Listar funcion rios por departamento
void listarFuncionariosPorDepartamento() {
     carregarDepartamentosDoArquivo();
    int codDepartamento;
    printf("\nDigite o codigo do Departamento: ");
    scanf("%d", &codDepartamento);

    int iDepartamento = buscarIndiceDepartamento(codDepartamento);
    if (iDepartamento == -1) {
        printf("Departamento n o encontrado.\n");
        return;
    }

    printf("\nFuncion rios no Departamento %s:\n", departamentos[iDepartamento].nome);

    FILE *arquivo = fopen("funcionarios.dat", "rb");
    if (!arquivo) {
        printf("Erro ao abrir funcionarios.dat\n");
        return;
    }

    Funcionario f;
    int encontrou = 0;

    while (fread(&f, sizeof(Funcionario), 1, arquivo) == 1) {
        if (f.codigoDepartamento == codDepartamento) {
            printf("C digo: %d | Nome: %s | Cargo: %s\n",
                   f.codigo, f.nome, f.cargo);
            encontrou = 1;
        }
    }

    fclose(arquivo);

    if (!encontrou) {
        printf("Nenhum funcion rio alocado neste departamento.\n");
    }
}


// Fun  o: Relat rio geral de ocupa  o
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

        // Contar funcion rios
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

        // Imprimir o relat rio
        printf("Departamento: %s (Codigo: %d)\n", departamentos[i].nome, departamentos[i].codigo);
        printf("  Pacientes: %d\n", qtdPacientes);
        printf("  Funcionarios: %d\n\n", qtdFuncionarios);
    }
}

