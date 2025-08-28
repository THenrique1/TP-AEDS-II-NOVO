#ifndef INTERACOES_H
#define INTERACOES_H

#include "paciente.h"
#include "funcionario.h"
#include "departamento.h"

void transferirPaciente();
void listarPacientesPorDepartamento();
void listarFuncionariosPorDepartamento();
void relatorioOcupacaoDepartamentos();

void salvarTotalPacientes();
void salvarTotalFuncionarios();
void salvarTotalDepartamentos();

void processarMenuPrincipal();
void processarOpcaoMenu(int opcao);



#endif
