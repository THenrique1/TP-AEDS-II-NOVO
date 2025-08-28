#ifndef SUBPACIENTE_H
#define SUBPACIENTE_H

#include <stdio.h>
#include "paciente.h"  // Certifique-se de que esta estrutura est� definida corretamente

typedef struct {
    FILE *arquivo;
    Paciente atual;
    int ativo;
} Fonte;

void bubbleSortPaciente(Paciente *v, int n);


// Prototipa��o das fun��es auxiliares de �rvore de vencedores
void construirArvoreVencedores(int *arvore, Fonte *fontes, int k);
void atualizarArvore(int *arvore, Fonte *fontes, int k, int fonteModificada);

// Fun��es principais de ordena��o
int gerarParticoesPorSelecaoNaturalPacientes(void);
void intercalarComArvoreDeVencedoresPacientes(int totalBlocos);

// Fun��o para limpar arquivos tempor�rios (como blocos e intercalados)


#endif
