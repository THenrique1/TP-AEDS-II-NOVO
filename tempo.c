#include <stdio.h>
#include <time.h>
#include <string.h>

// Salva timestamp formatado
void obterTimestamp(char *destino, int tamanho) {
    time_t agora = time(NULL);
    struct tm *tempoLocal = localtime(&agora);
    strftime(destino, tamanho, "%Y-%m-%d %H:%M:%S", tempoLocal);
}

// Fun��o para registrar uma opera��o de pesquisa
void gravarLogPesquisa(const char *tipoPesquisa, int codigo, int encontrado, double tempoExecucao) {
    FILE *logFile = fopen("pesquisa.log", "w");
    if (!logFile) {
        printf("Erro ao abrir o arquivo de log de pesquisa.\n");
        return;
    }

    char dataHora[20];
    obterTimestamp(dataHora, sizeof(dataHora));

    fprintf(logFile, "[%s] Tipo de pesquisa: %s | C�digo: %d | Resultado: %s | Tempo: %.6f segundos\n",
            dataHora, tipoPesquisa, codigo, (encontrado ? "Encontrado" : "N�o encontrado"), tempoExecucao);

    fclose(logFile);
}

// Fun��o para registrar testes de ordena��o
void logTempoExecucaoOrdenacao(const char *metodo, int tamanhoBase, double tempoExecucao) {
    FILE *logFile = fopen("teste_ordenacao.log", "w");
    if (!logFile) {
        printf("Erro ao abrir o arquivo de log de ordena��o.\n");
        return;
    }

    char dataHora[20];
    obterTimestamp(dataHora, sizeof(dataHora));

    fprintf(logFile, "[%s] Metodo: %s | Tamanho: %d | Tempo: %.6f segundos\n",
            dataHora, metodo, tamanhoBase, tempoExecucao);

    fclose(logFile);
}
