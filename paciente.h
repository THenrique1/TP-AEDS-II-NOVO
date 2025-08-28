#ifndef PACIENTE_H
#define PACIENTE_H

#include "hospital.h"  // Para acessar a defini��o da struct Paciente

// Fun��es de manipula��o de dados de pacientes
void listarPacientes();
void ordenarPacientesPorCodigo();
int buscaSequencialPaciente(int codigo); // Adaptada para busca diretamente no arquivo
int buscaBinariaPaciente(int codigo);   // Adaptada para busca diretamente no arquivo

#endif // PACIENTE_H
