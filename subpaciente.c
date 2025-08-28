#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <direct.h>
#include <sys/stat.h>
#include <time.h>
#include "tempo.h"
#include "subpaciente.h"
#include "paciente.h"

#define MAX_VENCEDORES 50
#define BLOCO_TAM 6
#define NOME_BLOCO_BASE "particoes/"

extern int totalPacientes;
extern void salvarTotalPacientes();


// Cada nó armazena o índice da fonte com o menor paciente entre seus dois filhos.
void construirArvoreVencedores(int *arvore, Fonte *fontes, int k) {
    // Preenche as folhas da árvore com os índices das fontes ativas
    for (int i = 0; i < k; i++)
        arvore[k + i] = fontes[i].ativo ? i : -1;
    for (int i = k - 1; i > 0; i--) {
        int esq = arvore[2 * i];
        int dir = arvore[2 * i + 1];

        if (esq == -1 && dir == -1)
            arvore[i] = -1;
        else if (esq == -1)
            arvore[i] = dir;
        else if (dir == -1)
            arvore[i] = esq;
        else
            // Ambos ativos: escolhe o de menor código
            arvore[i] = (fontes[esq].atual.codigo <= fontes[dir].atual.codigo) ? esq : dir;
    }
}

// Atualiza a árvore de vencedores após a leitura de um novo paciente
void atualizarArvore(int *arvore, Fonte *fontes, int k, int idxModificado) {
    // Posição da folha correspondente à fonte modificada
    int pos = k + idxModificado;
    arvore[pos] = fontes[idxModificado].ativo ? idxModificado : -1;
    // Sobe na árvore atualizando apenas os nós do caminho até a raiz
    while (pos > 1) {
        int pai = pos / 2;
        int esq = arvore[2 * pai];
        int dir = arvore[2 * pai + 1];

        if (esq == -1 && dir == -1)
            arvore[pai] = -1;
        else if (esq == -1)
            arvore[pai] = dir;
        else if (dir == -1)
            arvore[pai] = esq;
        else
            // Ambos ativos: escolhe o de menor código
            arvore[pai] = (fontes[esq].atual.codigo <= fontes[dir].atual.codigo) ? esq : dir;

        pos = pai;
    }
}

void intercalarBlocos(char **arquivos, int numBlocos, const char *saidaFinal) {
    Fonte fontes[MAX_VENCEDORES];              // Vetor que representa cada fonte
    int arvore[2 * MAX_VENCEDORES];            // Estrutura da árvore de vencedores.

    FILE *saida = fopen(saidaFinal, "wb");
    if (!saida) {
        printf("Erro ao criar %s\n", saidaFinal);
        return;
    }

    // Abre cada arquivo de entrada (bloco ordenado)
    for (int i = 0; i < numBlocos; i++) {
        fontes[i].arquivo = fopen(arquivos[i], "rb");
        fontes[i].ativo = 0;

        if (fontes[i].arquivo && fread(&fontes[i].atual, sizeof(Paciente), 1, fontes[i].arquivo) == 1)   // leitura do paciente e ativa a fonte
            fontes[i].ativo = 1;

        else if (fontes[i].arquivo) {
            fclose(fontes[i].arquivo);
            fontes[i].arquivo = NULL;
        }
    }

    construirArvoreVencedores(arvore, fontes, numBlocos);

    // Enquanto houver pelo menos uma fonte ativa, continua intercalando
    while (1) {
        int vencedorIdx = arvore[1];
        if (vencedorIdx == -1) break;

        // Escreve o menor paciente no arquivo de saída
        fwrite(&fontes[vencedorIdx].atual, sizeof(Paciente), 1, saida);
        if (fread(&fontes[vencedorIdx].atual, sizeof(Paciente), 1, fontes[vencedorIdx].arquivo) != 1) {
            fontes[vencedorIdx].ativo = 0;
            fclose(fontes[vencedorIdx].arquivo);
            fontes[vencedorIdx].arquivo = NULL;
        }

        // Atualiza apenas o caminho necessário na árvore com o novo valor lido
        atualizarArvore(arvore, fontes, numBlocos, vencedorIdx);
    }

    fclose(saida);
}

void intercalarComArvoreDeVencedoresPacientes(int totalBlocos) {
    int rodada = 0;
    int blocosRestantes = totalBlocos;
    char *arquivosGerados[1000];

    printf("\nIniciando intercalacao com Arvore de Vencedores...\n");
    printf("Total de particoes a intercalar: %d\n", totalBlocos);

    // Lista inicial de arquivos
    for (int i = 0; i < totalBlocos; i++) {
        arquivosGerados[i] = malloc(128);
        sprintf(arquivosGerados[i], "particoes/particao_%03d.dat", i);
    }

    while (blocosRestantes > 1) {
        int numGrupos = (blocosRestantes + MAX_VENCEDORES - 1) / MAX_VENCEDORES;
        char *novosArquivos[1000];
        int novosCont = 0;

        for (int g = 0; g < numGrupos; g++) {
            int ini = g * MAX_VENCEDORES;
            int fim = ini + MAX_VENCEDORES;
            if (fim > blocosRestantes) fim = blocosRestantes;
            int qtde = fim - ini;

            char nomeSaida[64];
            sprintf(nomeSaida, "intercalado_%03d_%03d.dat", rodada, g);

            // Define o grupo de arquivos a serem intercalados
            char *grupo[MAX_VENCEDORES];
            for (int k = 0; k < qtde; k++)
                grupo[k] = arquivosGerados[ini + k];

            intercalarBlocos(grupo, qtde, nomeSaida);

            // Libera e remove arquivos antigos do grupo
            for (int k = 0; k < qtde; k++) {
                remove(grupo[k]);               // remove arquivo físico
                free(grupo[k]);                 // libera memória da string
            }

            novosArquivos[novosCont] = malloc(64);
            strcpy(novosArquivos[novosCont++], nomeSaida);
        }

        // Atualiza lista de arquivos gerados para a próxima rodada
        for (int i = 0; i < novosCont; i++) {
            arquivosGerados[i] = novosArquivos[i];
        }

        blocosRestantes = novosCont;
        rodada++;
    }

    // Move arquivo final para pacientes.dat
    remove("pacientes.dat");
    rename(arquivosGerados[0], "pacientes.dat");
    free(arquivosGerados[0]);

    // Conta total de pacientes no final
    FILE *f = fopen("pacientes.dat", "rb");
    totalPacientes = 0;
    Paciente p;
    while (fread(&p, sizeof(Paciente), 1, f) == 1) totalPacientes++;
    fclose(f);

    salvarTotalPacientes();

    printf("\nIntercalacao concluida!\n");
    printf("Total de pacientes no arquivo final: %d\n", totalPacientes);
}

int gerarParticoesPorSelecaoNaturalPacientes() {
#ifdef _WIN32
    _mkdir("particoes");
#else
    mkdir("particoes", 0777);
#endif

    FILE *entrada = fopen("pacientes.dat", "rb");
    if (!entrada) {
        printf("Erro ao abrir pacientes.dat\n");
        return 0;
    }

    FILE *reservatorio = fopen("reservatorio.dat", "wb+");
    if (!reservatorio) {
        printf("Erro ao criar reservatorio.dat\n");
        fclose(entrada);
        return 0;
    }

    clock_t inicio = clock();

    Paciente areaSelecao[BLOCO_TAM];
    int ativos[BLOCO_TAM]; // 1 = válido, 0 = removido

    int totalParticoes = 0;
    int totalPacientes = 0;

    // Conta total de pacientes
    fseek(entrada, 0, SEEK_END);
    long tamanhoArquivo = ftell(entrada);
    totalPacientes = tamanhoArquivo / sizeof(Paciente);
    rewind(entrada);

    printf("\nIniciando geracao de particoes com SELECAO NATURAL...\n");
    printf("Total de pacientes: %d\n", totalPacientes);
    printf("Tamanho da memoria (area de selecao): %d pacientes\n", BLOCO_TAM);

    char logMsg[256];
    snprintf(logMsg, sizeof(logMsg), "=== INICIO SELECAO NATURAL ===");
    registrarLog(logMsg);
    snprintf(logMsg, sizeof(logMsg), "Total de pacientes: %d", totalPacientes);
    registrarLog(logMsg);
    snprintf(logMsg, sizeof(logMsg), "Tamanho da memoria: %d pacientes", BLOCO_TAM);
    registrarLog(logMsg);

    int fimArquivo = 0;
    int tamanhoReservatorio = 0;
    int usandoReservatorio = 0;

    while (!fimArquivo || usandoReservatorio) {
        // Abrir partição de saída
        char nome[64];
        sprintf(nome, "particoes/particao_%03d.dat", totalParticoes);
        FILE *particao = fopen(nome, "wb");
        if (!particao) {
            printf("Erro ao criar %s\n", nome);
            break;
        }

        int pacientesGravados = 0; //  declarada aqui

        // Carrega a área de seleção
        int tamanhoArea = 0;
        if (!usandoReservatorio) {
            for (int i = 0; i < BLOCO_TAM; i++) {
                if (fread(&areaSelecao[i], sizeof(Paciente), 1, entrada) == 1) {
                    ativos[i] = 1;
                    tamanhoArea++;
                } else {
                    ativos[i] = 0;
                    fimArquivo = 1;
                }
            }
        } else {
            rewind(reservatorio);
            for (int i = 0; i < BLOCO_TAM; i++) {
                if (i < tamanhoReservatorio && fread(&areaSelecao[i], sizeof(Paciente), 1, reservatorio) == 1) {
                    ativos[i] = 1;
                } else {
                    ativos[i] = 0;
                }
            }
            usandoReservatorio = 0;
        }

        rewind(reservatorio);
        tamanhoReservatorio = 0;
        int ultimoCodigo = -1;

        while (1) {
            int menorIdx = -1;
            int menorCodigo = INT_MAX;

            for (int i = 0; i < BLOCO_TAM; i++) {
                if (ativos[i] && areaSelecao[i].codigo < menorCodigo) {
                    menorCodigo = areaSelecao[i].codigo;
                    menorIdx = i;
                }
            }

            if (menorIdx == -1) break; // área de seleção vazia

            // Grava o menor na partição
            fwrite(&areaSelecao[menorIdx], sizeof(Paciente), 1, particao);
            pacientesGravados++; //  contador atualizado
            ultimoCodigo = areaSelecao[menorIdx].codigo;

            // Tenta repor com novo paciente
            Paciente novo;
            if (!fimArquivo && fread(&novo, sizeof(Paciente), 1, entrada) == 1) {
                if (novo.codigo >= ultimoCodigo) {
                    areaSelecao[menorIdx] = novo; // substitui direto
                } else {
                    // fora de ordem → vai para o reservatório
                    fwrite(&novo, sizeof(Paciente), 1, reservatorio);
                    ativos[menorIdx] = 0;
                    tamanhoReservatorio++;
                }
            } else {
                fimArquivo = 1;
                ativos[menorIdx] = 0;
            }
        }

        fclose(particao);
        printf("Particao %d criada com %d pacientes (%s)\n", totalParticoes, pacientesGravados, nome);
        snprintf(logMsg, sizeof(logMsg), "Particao %d: %d pacientes", totalParticoes, pacientesGravados);
        registrarLog(logMsg);

        totalParticoes++;

        if (tamanhoReservatorio > 0) {
            usandoReservatorio = 1;
        }
    }

    fclose(reservatorio);
    fclose(entrada);

    clock_t fim = clock();
    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    snprintf(logMsg, sizeof(logMsg), "Particoes criadas: %d", totalParticoes);
    registrarLog(logMsg);
    snprintf(logMsg, sizeof(logMsg), "Tempo total selecao natural: %.2f ms", tempo * 1000);
    registrarLog(logMsg);
    snprintf(logMsg, sizeof(logMsg), "Pacientes por particao (media): %.1f", (double)totalPacientes / totalParticoes);
    registrarLog(logMsg);
    snprintf(logMsg, sizeof(logMsg), "=== FIM SELECAO NATURAL ===");
    registrarLog(logMsg);

    return totalParticoes;
}
