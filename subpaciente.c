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

#define MAX_VENCEDORES 15
#define BLOCO_TAM 6
#define NOME_BLOCO_BASE "particoes/"

// Cada nó armazena o índice da fonte com o menor paciente entre seus dois filhos.
void construirArvoreVencedores(int *arvore, Fonte *fontes, int k) {
    // Preenche as folhas da árvore com os índices das fontes ativas
    for (int i = 0; i < k; i++)
        arvore[k + i] = fontes[i].ativo ? i : -1;

//Construir do fundo pra cima garante que, ao final, a raiz já tem o vencedor sem percorrer todas as fontes de novo
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


extern int totalPacientes;
extern void salvarTotalPacientes();



void intercalarBlocos(char **arquivos, int numBlocos, const char *saidaFinal) {
    Fonte fontes[MAX_VENCEDORES];   // Vetor de fontes: uma por partição
    int arvore[2 * MAX_VENCEDORES]; // Vetor que representa a arvore

    // Abre o arquivo final para escrita binária (zera/reescreve se já existe)
    FILE *saida = fopen(saidaFinal, "wb");
    if (!saida) {
        printf("Erro ao criar %s\n", saidaFinal);
        return; // Não prossegue sem arquivo de saída
    }

    // Para cada bloco ordenado (arquivo de partição)
    for (int i = 0; i < numBlocos; i++) {
        // Tenta abrir para leitura binária
        fontes[i].arquivo = fopen(arquivos[i], "rb");
        fontes[i].ativo = 0; // Assume inativo até conseguir ler o primeiro registro

        // Se abriu e conseguiu ler o primeiro Paciente, tornara a fonte está ativa e 'atual' fica carregado
        if (fontes[i].arquivo &&
            fread(&fontes[i].atual, sizeof(Paciente), 1, fontes[i].arquivo) == 1)
        {
            fontes[i].ativo = 1;
        }
        // Caso o arquivo exista mas esteja vazio/corrompido (não leu 1 registro), fecha e invalida
        else if (fontes[i].arquivo) {
            fclose(fontes[i].arquivo);
            fontes[i].arquivo = NULL;
        }
    }

    // Constrói a arovre com base no primeiro elemento de cada fonte ativa
    //  A árvore coloca na raiz (índice 1) o índice da fonte com menor 'atual'
    //  Fontes inativas entram como -1
    construirArvoreVencedores(arvore, fontes, numBlocos);

    // Laço principal: segue enquanto houver pelo menos uma fonte ativa (vencedor != -1)
    while (1) {
        int vencedorIdx = arvore[1];   // Índice da fonte que contém o menor registro no momento
        if (vencedorIdx == -1) break;  // Nenhuma fonte ativa -> terminou a intercalação

        // Escreve no arquivo final o menor registro disponível
        fwrite(&fontes[vencedorIdx].atual, sizeof(Paciente), 1, saida);

        // Avança a fonte vencedora: tenta ler o próximo registro do bloco dela
        if (fread(&fontes[vencedorIdx].atual, sizeof(Paciente), 1, fontes[vencedorIdx].arquivo) != 1) {
            // Chegou ao fim desse bloco (ou erro de leitura): marca como inativa e fecha
            fontes[vencedorIdx].ativo = 0;
            fclose(fontes[vencedorIdx].arquivo);
            fontes[vencedorIdx].arquivo = NULL;
        }

        // Atualiza a arvore somente ao longo do caminho afetado pelo vencedor
        // (recalcula comparações até a raiz de forma eficiente)
        atualizarArvore(arvore, fontes, numBlocos, vencedorIdx);
    }

    // Fecha o arquivo final
    fclose(saida);
}

void intercalarComArvoreDeVencedoresPacientes(int totalBlocos) {
    int rodada = 0;                         // Contador da rodada de intercalação
    int blocosRestantes = totalBlocos;      // Quantos arquivos ainda precisam ser intercalados
    char *arquivosGerados[1000];            // Vetor de nomes dos arquivos atuais (limite fixo)

    printf("\nIniciando intercalacao com Arvore de Vencedores...\n");
    printf("Total de particoes a intercalar: %d\n", totalBlocos);

    // Monta a lista inicial de arquivos de entrada: particoes
    for (int i = 0; i < totalBlocos; i++) {
        arquivosGerados[i] = malloc(128);   // Reserva espaço para o nome do arquivo
        sprintf(arquivosGerados[i], "particoes/particao_%03d.dat", i); // Escreve o nome
    }

    // Enquanto houver mais de um arquivo, seguimos intercalando em grupos
    while (blocosRestantes > 1) {
        // Número de grupos na rodada
        int numGrupos = (blocosRestantes + MAX_VENCEDORES - 1) / MAX_VENCEDORES;

        char *novosArquivos[1000];          // Guardará os nomes dos arquivos gerados nesta rodada
        int novosCont = 0;                   // Quantos novos arquivos foram criados na rodada

        // Processa cada grupo da rodada
        for (int g = 0; g < numGrupos; g++) {
            int ini = g * MAX_VENCEDORES;   // Índice inicial do grupo nos arquivos atuais
            int fim = ini + MAX_VENCEDORES; // Índice final (exclusivo) do grupo
            if (fim > blocosRestantes) fim = blocosRestantes;
            int qtde = fim - ini;           // Quantos arquivos há neste grupo

            // Nome do arquivo de saída deste grupo/rodada
            char nomeSaida[64];
            sprintf(nomeSaida, "intercalado_%03d_%03d.dat", rodada, g);

            // Prepara o vetor de ponteiros de nomes do grupo a intercalar
            char *grupo[MAX_VENCEDORES];
            for (int k = 0; k < qtde; k++)
                grupo[k] = arquivosGerados[ini + k];

            // Faz a intercalação do grupo usando a arvore
            intercalarBlocos(grupo, qtde, nomeSaida);

            // Após gerar o arquivo de saída do grupo, remove e libera os arquivos antigos do grupo
            for (int k = 0; k < qtde; k++) {
                remove(grupo[k]);   // Deleta o arquivo físico antigo
                free(grupo[k]);     // Libera a string do nome correspondente
            }

            // Registra o nome do novo arquivo gerado para a próxima rodada
            novosArquivos[novosCont] = malloc(64);
            strcpy(novosArquivos[novosCont++], nomeSaida);
        }

        // Atualiza a lista de "arquivosGerados" para apontar aos arquivos recém-criados
        for (int i = 0; i < novosCont; i++) {
            arquivosGerados[i] = novosArquivos[i];
        }

        // Agora a próxima rodada trabalhará só com os arquivos recém-criados
        blocosRestantes = novosCont;
        rodada++;
    }

    // Ao final, deve restar exatamente 1 arquivo intercalado
    // Move/renomeia esse arquivo final para "pacientes.dat"
    remove("pacientes.dat");                 // Garante que não existe um arquivo antigo com o mesmo nome
    rename(arquivosGerados[0], "pacientes.dat");
    free(arquivosGerados[0]);                // Libera o nome do arquivo final (não é mais necessário)

    // Faz uma passada para contar o total de pacientes no arquivo final
    FILE *f = fopen("pacientes.dat", "rb");
    totalPacientes = 0;
    Paciente p;
    while (fread(&p, sizeof(Paciente), 1, f) == 1) totalPacientes++;
    fclose(f);

    salvarTotalPacientes();

    printf("\nIntercalacao concluida!\n");
    printf("Total de pacientes no arquivo final: %d\n", totalPacientes);
}





// Função que gera partições utilizando o algoritmo de Seleção Natural
int gerarParticoesPorSelecaoNaturalPacientes() {
    // Cria a pasta "particoes" (compatível com Windows e Linux)
#ifdef _WIN32
    _mkdir("particoes");
#else
    mkdir("particoes", 0777);
#endif

    // Abre o arquivo de entrada com todos os pacientes
    FILE *arquivoEntrada = fopen("pacientes.dat", "rb");
    if (!arquivoEntrada) {
        printf("Erro ao abrir pacientes.dat\n");
        return 0;
    }

    // Cria um arquivo temporário chamado reservatorio.dat
    FILE *arquivoReservatorio = fopen("reservatorio.dat", "wb+");
    if (!arquivoReservatorio) {
        printf("Erro ao criar reservatorio.dat\n");
        fclose(arquivoEntrada);
        return 0;
    }

    clock_t inicio = clock(); // Marca o tempo de início

    // Área de seleção (memória limitada )
    Paciente areaSelecao[BLOCO_TAM];
    int posicaoValida[BLOCO_TAM]; // Marca quais posições da memória estão ocupadas (1 = sim, 0 = não)

    int totalParticoes = 0;            // Contador de partições geradas
    int totalPacientesEntrada = 0;     // Contador total de pacientes no arquivo de entrada

    // Descobre quantos pacientes existem no arquivo de entrada
    fseek(arquivoEntrada, 0, SEEK_END);
    long tamanhoArquivo = ftell(arquivoEntrada);
    totalPacientesEntrada = (int)(tamanhoArquivo / sizeof(Paciente));
    rewind(arquivoEntrada);

    // Mensagens informativas iniciais
    printf("\nIniciando geracao de particoes com SELECAO NATURAL...\n");
    printf("Total de pacientes: %d\n", totalPacientesEntrada);
    printf("Tamanho da memoria (area de selecao): %d pacientes\n", BLOCO_TAM);

    // Registra logs iniciais
    char logMsg[256];
    snprintf(logMsg, sizeof(logMsg), "=== INICIO SELECAO NATURAL ==="); registrarLog(logMsg);
    snprintf(logMsg, sizeof(logMsg), "Total de pacientes: %d", totalPacientesEntrada); registrarLog(logMsg);
    snprintf(logMsg, sizeof(logMsg), "Tamanho da memoria: %d pacientes", BLOCO_TAM);   registrarLog(logMsg);

    // Variáveis de controle
    int fimArquivoEntrada = 0;          // Marca quando o arquivo de entrada termina
    int lendoReservatorio = 0;          // Indica se está lendo do reservatório (1) ou da entrada (0)

    // Posições de leitura/escrita no arquivo reservatório
    long posicaoGravacaoReservatorio = 0; // fim do reservatório (próxima vaga de escrita).
    long posicaoLeituraReservatorio  = 0; // início do reservatório (próximo a ser lido).

    // Loop principal: continua até não restarem dados no arquivo nem no reservatório
    while (!fimArquivoEntrada || (posicaoLeituraReservatorio < posicaoGravacaoReservatorio)) {
        // Cria um novo arquivo de partição
        char nomeArquivoParticao[64];
        sprintf(nomeArquivoParticao, "particoes/particao_%03d.dat", totalParticoes);
        FILE *arquivoParticao = fopen(nomeArquivoParticao, "wb");
        if (!arquivoParticao) {
            printf("Erro ao criar %s\n", nomeArquivoParticao);
            break;
        }

        int pacientesGravados = 0; // Contador de registros na partição

        // Se há dados pendentes no reservatório, a próxima partição deve começar consumindo-o.
        if (posicaoLeituraReservatorio < posicaoGravacaoReservatorio)
            lendoReservatorio = 1;

        // Marca todas as posições da área de seleção como vazias
        for (int i = 0; i < BLOCO_TAM; i++) posicaoValida[i] = 0;

        // Preenche a área de seleção com registros da fonte atual (entrada(disco) ou reservatório).
        if (lendoReservatorio) {
            // Caso esteja lendo do reservatório (nova partição)
            // Coloca o cursor do arquivo reservatorio.dat exatamente na posição indicada por posicaoLeituraReservatorio
            for (int i = 0; i < BLOCO_TAM && posicaoLeituraReservatorio < posicaoGravacaoReservatorio; i++) {
                fseek(arquivoReservatorio, posicaoLeituraReservatorio, SEEK_SET);
                if (fread(&areaSelecao[i], sizeof(Paciente), 1, arquivoReservatorio) == 1) {
                    posicaoValida[i] = 1;
                    posicaoLeituraReservatorio += sizeof(Paciente);
                }
            }
        } else {
            // Caso esteja lendo do arquivo de entrada
            for (int i = 0; i < BLOCO_TAM; i++) {
                if (!fimArquivoEntrada && fread(&areaSelecao[i], sizeof(Paciente), 1, arquivoEntrada) == 1) {
                    posicaoValida[i] = 1;
                } else {
                    if (!fimArquivoEntrada) fimArquivoEntrada = 1;
                    posicaoValida[i] = 0;
                }
            }
        }

        // Se não há pacientes na área e nem no reservatório, encerra o laço.
        int existeAtivo = 0;
        for (int i = 0; i < BLOCO_TAM; i++) if (posicaoValida[i]) { existeAtivo = 1; break; }
        if (!existeAtivo) {
            fclose(arquivoParticao);
            if (fimArquivoEntrada && (posicaoLeituraReservatorio >= posicaoGravacaoReservatorio)) break;
            continue;
        }

        int ultimoCodigo = -1; // Último código gravado (para manter ordem crescente)

        // Laço interno que constrói uma partição
        while (1) {
            // Busca o menor paciente válido na memeoria
            int indiceMenor = -1;
            int menorCodigo = INT_MAX;
            for (int i = 0; i < BLOCO_TAM; i++) {
                if (posicaoValida[i] && areaSelecao[i].codigo < menorCodigo) {
                    menorCodigo = areaSelecao[i].codigo;
                    indiceMenor = i;
                }
            }
            if (indiceMenor == -1) break; // Nenhum ativo - fim da partição

            // Escreve o paciente selecionado na partição
            if (fwrite(&areaSelecao[indiceMenor], sizeof(Paciente), 1, arquivoParticao) != 1) {
                printf("Erro de escrita em %s\n", nomeArquivoParticao);
                break;
            }
            pacientesGravados++;
            ultimoCodigo = areaSelecao[indiceMenor].codigo;

            // sessão
            // Apos remover o menor, tenta prrencher o espaço
            if (!lendoReservatorio) {
                Paciente novo;
                if (!fimArquivoEntrada && fread(&novo, sizeof(Paciente), 1, arquivoEntrada) == 1) {
                    if (novo.codigo >= ultimoCodigo) {
                        // Ainda segue a ordem - substitui na área
                        areaSelecao[indiceMenor] = novo;
                        posicaoValida[indiceMenor] = 1;
                    } else {
                        // Fora de ordem - envia para reservatório
                        fseek(arquivoReservatorio, posicaoGravacaoReservatorio, SEEK_SET);
                        if (fwrite(&novo, sizeof(Paciente), 1, arquivoReservatorio) == 1) {
                            posicaoGravacaoReservatorio += sizeof(Paciente);
                        }
                        posicaoValida[indiceMenor] = 0;
                    }
                } else {
                    fimArquivoEntrada = 1;
                    posicaoValida[indiceMenor] = 0;
                }
            } else {
                // Caso esteja lendo do reservatório
                if (posicaoLeituraReservatorio < posicaoGravacaoReservatorio) {
                    fseek(arquivoReservatorio, posicaoLeituraReservatorio, SEEK_SET);
                    if (fread(&areaSelecao[indiceMenor], sizeof(Paciente), 1, arquivoReservatorio) == 1) {
                        posicaoLeituraReservatorio += sizeof(Paciente);
                        posicaoValida[indiceMenor] = 1;
                    } else {
                        posicaoValida[indiceMenor] = 0;
                    }
                } else {
                    posicaoValida[indiceMenor] = 0;
                }
            }
        }

        // Fecha partição criada
        fclose(arquivoParticao);
        printf("Particao %d criada com %d pacientes (%s)\n", totalParticoes, pacientesGravados, nomeArquivoParticao);
        snprintf(logMsg, sizeof(logMsg), "Particao %d: %d pacientes", totalParticoes, pacientesGravados); registrarLog(logMsg);
        totalParticoes++;

        // Se o reservatório foi consumido todo, limpa-o e reinicia
        if (posicaoLeituraReservatorio >= posicaoGravacaoReservatorio) {
            fclose(arquivoReservatorio);
            arquivoReservatorio = fopen("reservatorio.dat", "wb+");
            posicaoLeituraReservatorio = posicaoGravacaoReservatorio = 0;
            lendoReservatorio = 0;
        } else {
            lendoReservatorio = 1;
        }
    }

    // Fecha arquivos
    fclose(arquivoReservatorio);
    fclose(arquivoEntrada);

    // Calcula tempo total
    clock_t fim = clock();
    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    // Logs finais
    snprintf(logMsg, sizeof(logMsg), "Particoes criadas: %d", totalParticoes); registrarLog(logMsg);
    snprintf(logMsg, sizeof(logMsg), "Tempo total selecao natural: %.2f ms", tempo * 1000); registrarLog(logMsg);
    snprintf(logMsg, sizeof(logMsg), "Pacientes por particao (media): %.1f",
             totalParticoes ? (double)totalPacientesEntrada / totalParticoes : 0.0); registrarLog(logMsg);
    snprintf(logMsg, sizeof(logMsg), "=== FIM SELECAO NATURAL ==="); registrarLog(logMsg);

    return totalParticoes; // Retorna o número de partições criadas
}
