#ifndef TEMPO_H
#define TEMPO_H

void obterTimestamp(char *destino, int tamanho);
void gravarLogPesquisa(const char *tipoPesquisa, int codigo, int encontrado, double tempoExecucao);
void logTempoExecucaoOrdenacao(const char *metodo, int tamanhoBase, double tempoExecucao);
void registrarLog(const char *mensagem);

#endif
