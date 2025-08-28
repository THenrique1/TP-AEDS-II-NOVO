#ifndef PACIENTE_H
#define PACIENTE_H

#include "hospital.h"  // Para acessar a definição da struct Paciente

// Funções de manipulação de dados de pacientes
void listarPacientes();
void ordenarPacientesPorCodigo();
int buscaSequencialPaciente(int codigo); // Adaptada para busca diretamente no arquivo
int buscaBinariaPaciente(int codigo);   // Adaptada para busca diretamente no arquivo

#endif // PACIENTE_H
