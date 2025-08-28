#include "hospital.h"
#include "funcionario.h"
#include "paciente.h"
#include "departamento.h"
#include "gerador_dados.h"
#include "ordem.h"
#include "tempo.h"
#include "hash_pacientes.h"
#include "iteracoes.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
extern HashPacientes tabelaHash;
HashPacientes tabelaHash;

int main() {
inicializarHash(&tabelaHash);
carregarHashPacientes(&tabelaHash);
processarMenuPrincipal();
liberarHash(&tabelaHash);
    return 0;
}
