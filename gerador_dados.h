#ifndef ORDENACAO_H
#define ORDENACAO_H
#include "hospital.h"

// Inicialização de aleatoriedade
void inicializarAleatorio();
void gerarPacientesAleatorios(int quantidade);
void gerarDepartamentosAleatorios(int quantidade);
void gerarFuncionariosAleatorios(int quantidade);

// parte da ordenação
void ordenarPacientesPorCodigo();
void ordenarPacientesPorCodigoBubbleSort();
void ordenarDepartamentosPorCodigo();
void ordenarFuncionariosPorCodigo();

int gerarCodigoUnico(const char *arquivoContador);

#endif
