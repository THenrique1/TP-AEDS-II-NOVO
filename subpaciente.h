#ifndef SUBPACIENTE_H
#define SUBPACIENTE_H

#include <stdio.h>
#include "paciente.h"  // Certifique-se de que esta estrutura está definida corretamente

typedef struct {
    FILE *arquivo;
    Paciente atual;
    int ativo;
} Fonte;

void bubbleSortPaciente(Paciente *v, int n);


// Prototipação das funções auxiliares de árvore de vencedores
void construirArvoreVencedores(int *arvore, Fonte *fontes, int k);
void atualizarArvore(int *arvore, Fonte *fontes, int k, int fonteModificada);

// Funções principais de ordenação
int gerarParticoesPorSelecaoNaturalPacientes(void);
void intercalarComArvoreDeVencedoresPacientes(int totalBlocos);

// Função para limpar arquivos temporários (como blocos e intercalados)


#endif
