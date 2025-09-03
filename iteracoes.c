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

void menuHash(void) {
    int op;
    do {
        printf("\nMENU HASH - Pacientes\n");
        printf("1. Inicializar Hash Encadeado (indice)\n");
        printf("2. Inserir Paciente (grava no .dat e indexa no hash)\n");
        printf("3. Buscar Paciente (via hash)\n");
        printf("4. Remover Paciente (tira do hash e marca inativo no .dat)\n");
        printf("5. Listar Pacientes (Hash Encadeado/Arquivo)\n");
        printf("0. Voltar\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &op) != 1) { op = -1; }

        switch (op) {
            case 1: { // inicializar/zerar apenas o índice
                char confirma;
                printf("ATENCAO: Isso ZERA APENAS o arquivo de indice (pacientes_hash.dat). Continuar? (s/n): ");
                scanf(" %c", &confirma);
                if (confirma == 's' || confirma == 'S') {
                    FILE *arqHash = fopen("pacientes_hash.dat", "wb+");
                    if (arqHash) {
                        inicializarTabelaHashEncArquivo(arqHash);
                        fclose(arqHash);
                        printf("Indice hash zerado com sucesso.\n");
                    } else {
                        printf("Erro ao criar/zerar pacientes_hash.dat.\n");
                    }
                } else {
                    printf("Operacao cancelada.\n");
                }
                break;
            }

            case 2: { // inserir manualmente
                Paciente p;
                printf("Nome: ");                      scanf(" %[^\n]", p.nome);
                printf("Codigo (ID): ");                scanf("%d", &p.codigo);
                printf("CPF: ");                        scanf("%s",  p.cpf);
                printf("Data Nasc (dd/mm/aaaa): ");     scanf("%s",  p.dataNascimento);
                printf("Telefone: ");                   scanf("%s",  p.telefone);
                printf("Endereco: ");                   scanf(" %[^\n]", p.endereco);
                printf("Codigo do Departamento: ");     scanf("%d", &p.codigoDepartamento);
                printf("Codigo do Funcionario Resp.: ");scanf("%d", &p.codigoFuncionarioResponsavel);
                p.ativo = 1;

                FILE *arqDat  = fopen(ARQUIVO_PACIENTES, "ab");
                if (!arqDat) { printf("Erro ao abrir %s.\n", ARQUIVO_PACIENTES); break; }
                long posDat = ftell(arqDat);
                fwrite(&p, sizeof(Paciente), 1, arqDat);
                fclose(arqDat);

                FILE *arqHash = fopen("pacientes_hash.dat", "rb+");
                if (!arqHash) { printf("Indice inexistente. Use a opcao 1 para criar.\n"); break; }
                inserirPacienteHashEncArquivo(arqHash, p);
                fclose(arqHash);

                printf("Inserido: %s | Codigo: %d | pos .dat: %ld | gaveta: %d\n",
                       p.nome, p.codigo, posDat, hash(p.codigo));
                break;
            }

            case 3: { // buscar
                int codigo;
                printf("Codigo do paciente: ");
                scanf("%d", &codigo);

                FILE *arqHash = fopen("pacientes_hash.dat", "rb");
                if (!arqHash) { printf("Indice inexistente. Use a opcao 1.\n"); break; }

                Paciente p;
                if (buscarPacienteHashEncArquivo(arqHash, codigo, &p)) {
                    printf("Encontrado (gaveta %d): %s | Codigo: %d\n",
                           hash(codigo), p.nome, p.codigo);
                } else {
                    printf("Nao encontrado no indice.\n");
                }
                fclose(arqHash);
                break;
            }

            case 4: { // remover
                int codigo;
                printf("Codigo do paciente a remover: ");
                scanf("%d", &codigo);

                FILE *arqHash = fopen("pacientes_hash.dat", "rb+");
                if (!arqHash) { printf("Indice inexistente. Use a opcao 1.\n"); break; }

                if (removerPacienteHashEncArquivo(arqHash, codigo)) {
                    removerPacienteArquivo(codigo); // marca inativo no .dat
                }
                fclose(arqHash);
                break;
            }

            case 5: { // listar por gaveta (debug do índice)
                FILE *arqHash = fopen("pacientes_hash.dat", "rb");
                if (!arqHash) { printf("Indice inexistente. Use a opcao 1.\n"); break; }

                printf("\n--- Listagem de pacientes no INDICE hash (arquivo) ---\n");
                for (int i = 0; i < TAM_TABELA_HASH_ENC; i++) {
                    long head;
                    fseek(arqHash, i * sizeof(long), SEEK_SET);
                    if (fread(&head, sizeof(long), 1, arqHash) != 1) break;

                    long atual = head;
                    NoPaciente no;
                    if (atual != OFFSET_INVALIDO) {
                        printf("Gaveta %d:\n", i);
                        while (atual != OFFSET_INVALIDO) {
                            fseek(arqHash, atual, SEEK_SET);
                            if (fread(&no, sizeof(NoPaciente), 1, arqHash) != 1) break;
                            printf("  Nome: %s | Codigo: %d\n", no.paciente.nome, no.paciente.codigo);
                            atual = no.prox;
                        }
                    }
                }
                fclose(arqHash);
                break;
            }

            case 0:
                printf("Voltando ao menu principal...\n");
                break;

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
       recarregarHashAPartirDoDat();
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
    if (scanf("%d", &codPaciente) != 1) { puts("Entrada invalida."); return; }

    printf("Novo codigo do Departamento: ");
    if (scanf("%d", &codDepartamento) != 1) { puts("Entrada invalida."); return; }

    Departamento dep;
    if (buscarIndiceDepartamento(codDepartamento, &dep) != 1) {
        printf("Departamento nao encontrado.\n");
        return;
    }

    // Buscar novo responsavel no departamento
    FILE *arquivoFunc = fopen("funcionarios.dat", "rb");
    if (!arquivoFunc) { printf("Erro ao abrir funcionarios.dat\n"); return; }

    Funcionario f;
    int novoResponsavel = -1;
    while (fread(&f, sizeof(Funcionario), 1, arquivoFunc) == 1) {
        if (f.codigoDepartamento == codDepartamento) { novoResponsavel = f.codigo; break; }
    }
    fclose(arquivoFunc);

    if (novoResponsavel == -1) {
        printf("Departamento nao possui funcionario. Transferencia cancelada.\n");
        return;
    }

    // Atualiza paciente via arquivo temporario
    FILE *in = fopen("pacientes.dat", "rb");
    FILE *tmp = fopen("temp.dat", "wb");
    if (!in || !tmp) {
        printf("Erro ao abrir arquivos de pacientes.\n");
        if (in) fclose(in);
        if (tmp) fclose(tmp);
        return;
    }

    Paciente p;
    int transferido = 0;
    while (fread(&p, sizeof(Paciente), 1, in) == 1) {
        if (p.codigo == codPaciente) {
            p.codigoDepartamento = codDepartamento;
            p.codigoFuncionarioResponsavel = novoResponsavel;
            transferido = 1;
        }
        if (fwrite(&p, sizeof(Paciente), 1, tmp) != 1) { puts("Erro de escrita."); /* tratar */ }
    }
    fclose(in); fclose(tmp);

    if (transferido) {
        if (remove("pacientes.dat") != 0 || rename("temp.dat", "pacientes.dat") != 0) {
            puts("Falha ao substituir pacientes.dat"); return;
        }
        carregarPacientesDoArquivo();  // atualiza totalPacientes
        printf("Paciente transferido para o departamento %s (Resp. codigo %d).\n",
               dep.nome, novoResponsavel);
    } else {
        remove("temp.dat");
        printf("Paciente nao encontrado.\n");
    }
}

void listarPacientesPorDepartamento() {
    carregarDepartamentosDoArquivo();

    int codDepartamento;
    printf("\nDigite o codigo do Departamento: ");
    if (scanf("%d", &codDepartamento) != 1) { puts("Entrada invalida."); return; }

    Departamento dep;
    if (buscarIndiceDepartamento(codDepartamento, &dep) != 1) {
        printf("Departamento nao encontrado.\n");
        return;
    }

    printf("\nPacientes no Departamento %s:\n", dep.nome);

    FILE *arquivo = fopen("pacientes.dat", "rb");
    if (!arquivo) { printf("Erro ao abrir pacientes.dat\n"); return; }

    Paciente p; int encontrou = 0;
    while (fread(&p, sizeof(Paciente), 1, arquivo) == 1) {
        if (p.codigoDepartamento == codDepartamento) {
            printf("Codigo: %d | Nome: %s\n", p.codigo, p.nome);
            encontrou = 1;
        }
    }
    fclose(arquivo);
    if (!encontrou) printf("Nenhum paciente alocado neste departamento.\n");
}

void listarFuncionariosPorDepartamento() {
    carregarDepartamentosDoArquivo();

    int codDepartamento;
    printf("\nDigite o codigo do Departamento: ");
    if (scanf("%d", &codDepartamento) != 1) { puts("Entrada invalida."); return; }

    Departamento dep;
    if (buscarIndiceDepartamento(codDepartamento, &dep) != 1) {
        printf("Departamento nao encontrado.\n");
        return;
    }

    printf("\nFuncionarios no Departamento %s:\n", dep.nome);

    FILE *arq = fopen("funcionarios.dat", "rb");
    if (!arq) { printf("Erro ao abrir funcionarios.dat\n"); return; }

    Funcionario f; int encontrou = 0;
    while (fread(&f, sizeof(Funcionario), 1, arq) == 1) {
        if (f.codigoDepartamento == codDepartamento) {
            printf("Codigo: %d | Nome: %s | Cargo: %s\n", f.codigo, f.nome, f.cargo);
            encontrou = 1;
        }
    }
    fclose(arq);
    if (!encontrou) printf("Nenhum funcionario alocado neste departamento.\n");
}

void relatorioOcupacaoDepartamentos() {
    printf("\n=== Relatorio de Ocupacao por Departamento ===\n");

    FILE *arqDep = fopen("departamentos.dat", "rb");
    if (!arqDep) { printf("Erro ao abrir departamentos.dat\n"); return; }

    Departamento d;
    while (fread(&d, sizeof(Departamento), 1, arqDep) == 1) {
        int qtdPac = 0, qtdFunc = 0;

        FILE *ap = fopen("pacientes.dat", "rb");
        if (ap) {
            Paciente p;
            while (fread(&p, sizeof(Paciente), 1, ap) == 1)
                if (p.codigoDepartamento == d.codigo) qtdPac++;
            fclose(ap);
        } else {
            printf("Erro ao abrir pacientes.dat\n");
        }

        FILE *af = fopen("funcionarios.dat", "rb");
        if (af) {
            Funcionario f;
            while (fread(&f, sizeof(Funcionario), 1, af) == 1)
                if (f.codigoDepartamento == d.codigo) qtdFunc++;
            fclose(af);
        } else {
            printf("Erro ao abrir funcionarios.dat\n");
        }

        printf("Departamento: %s (Codigo: %d)\n", d.nome, d.codigo);
        printf("  Pacientes: %d\n", qtdPac);
        printf("  Funcionarios: %d\n\n", qtdFunc);
    }
    fclose(arqDep);
}
