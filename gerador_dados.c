#include "hospital.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#define MAX_CODIGOS 1000000

// Variáveis globais externas
extern int totalPacientes;
extern int totalDepartamentos;
extern int totalFuncionarios;

extern Departamento departamentos[MAX_DEPARTAMENTOS];

// Controle de códigos únicos
int codigosUsados[MAX_CODIGOS + 1] = {0};

// Listas de dados aleatórios
char *nomes[] = {
    "Ana Silva", "Bruno Costa", "Carla Dias", "Daniel Souza", "Eduarda Lima",
    "Felipe Nunes", "Giovana Rocha", "Henrique Alves", "Isabela Martins", "Lucas Pedro",
    "Karina Moreira", "Leonardo Tavares", "Mariana Duarte", "Nicolas Ribeiro", "Olivia Fernandes",
    "Paulo Azevedo", "Maria Almeida", "Rafael Pinto", "Sabrina Torres", "Tiago Henrique",
    "Ursula Moura", "Vinicius Facadio", "Wesley Dias", "Livia Gandra", "Yara Mendes", "Gabriel Amaral",
    "Renata Borges", "Joao Vitor Silva", "Camila Ferreira", "Hugo Andrade", "Vanessa Teixeira",
    "Caua Oliveira", "Juliana Meireles", "Murilo Barbosa", "Leticia Fonseca", "Ricardo Antunes",
    "Amanda Ribeiro", "Douglas Menezes", "Bruna Cardoso", "Erico Lacerda", "Tatiane Luz"
};

char *enderecos[] = {
    "Rua A, 123", "Rua B, 456", "Rua C, 789", "Av. Central, 100", "Travessa 10, n 55",
    "Rua das Flores, 321", "Av. Brasil, 200", "Rua do Sol, 900", "Alameda das Acacias, 45",
    "Estrada Velha, km 12", "Praca das Aguas, 777", "Rua Sao Joao, 303", "Av. Minas Gerais, 888",
    "Rua dos Pinheiros, 512", "Rua Boa Vista, 60", "Rua Esperanca, 78",
    "Av. dos Trabalhadores, 654", "Rua 7 de Setembro, 119"
};

char *cargos[] = {"Medico", "Enfermeiro", "Tecnico", "Administrador"};

char *especializacoes[] = {
    "Cardiologia", "UTI", "Raio-X", "Pediatria", "Clinica Geral",
    "Dermatologia", "Neurologia", "Ortopedia", "Psiquiatria", "Ginecologia",
    "Infectologia", "Endocrinologia", "Gastroenterologia", "Nefrologia"
};

char *responsaveis[] = {
    "Dr. Ana", "Dr. Joao", "Dra. Clara", "Dr. Pedro", "Dra. Helena",
    "Dr. Marcelo", "Dr. Tiago", "Dra. Fernanda", "Dr. Gustavo",
    "Dra. Patrícia", "Dr. Eduardo", "Dr. Vagner", "Dra. Beatriz", "Dr. Murilo", "Dra. Camila"
};

char *departamentoNomes[] = {
    "Cardiologia", "Ortopedia", "Pediatria", "Radiologia", "Clinica Geral",
    "Neurologia", "Oncologia", "Psiquiatria", "Dermatologia",
    "Ginecologia", "Urgência e Emergência", "Endocrinologia", "Reabilitação", "Imunologia"
};

void inicializarAleatorio() {
    srand(time(NULL));
}

int gerarCodigoUnico(const char *arquivoContador) {
    FILE *f = fopen(arquivoContador, "rb+");
    int ultimoCodigo = 0;

    if (f) {
        fread(&ultimoCodigo, sizeof(int), 1, f);
    } else {
        // Arquivo ainda não existe, iniciar do zero
        f = fopen(arquivoContador, "wb+");
        if (!f) {
            printf("Erro ao criar arquivo de código único: %s\n", arquivoContador);
            return -1;
        }
    }

    ultimoCodigo++; // Novo código

    rewind(f);
    fwrite(&ultimoCodigo, sizeof(int), 1, f);
    fclose(f);

    return ultimoCodigo;
}

void gerarCPF(char *cpf) {
    sprintf(cpf, "%03d%03d%03d%02d", rand() % 900 + 100, rand() % 900 + 100, rand() % 900 + 100, rand() % 90 + 10);
}

void gerarTelefone(char *tel) {
    sprintf(tel, "319%04d%04d", rand() % 10000, rand() % 10000);
}
void embaralharDepartamentos(Departamento *vet, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Departamento temp = vet[i];
        vet[i] = vet[j];
        vet[j] = temp;
    }
}

void gerarDepartamentosAleatorios(int quantidade) {
    Departamento *vet = malloc(sizeof(Departamento) * quantidade);
    totalDepartamentos = 0;

    for (int i = 0; i < quantidade; i++) {
        vet[i].codigo = gerarCodigoUnico("codigo_departamentos.dat");
        strcpy(vet[i].nome, departamentoNomes[i % 14]);
        strcpy(vet[i].responsavel, responsaveis[i % 15]);
        vet[i].andar = 1 + rand() % 5;
        vet[i].quantidadeFuncionarios = 0;
        totalDepartamentos++;
    }

    // Embaralha antes de salvar
    embaralharDepartamentos(vet, quantidade);

    FILE *arquivo = fopen("departamentos.dat", "wb");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de departamentos.\n");
        free(vet);
        return;
    }
    fwrite(vet, sizeof(Departamento), quantidade, arquivo);
    fclose(arquivo);

    free(vet);

    salvarTotalDepartamentos();
    carregarDepartamentosDoArquivo();

    printf("%d Departamentos aleatorios gerados com sucesso.\n", totalDepartamentos);
}


void gerarFuncionariosAleatorios(int quantidade) {
    carregarDepartamentosDoArquivo();

    if (totalDepartamentos == 0) {
        printf("Erro: Nenhum departamento encontrado. Gere os departamentos primeiro.\n");
        return;
    }

    FILE *arquivo = fopen("funcionarios.dat", "wb");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de funcionarios.\n");
        return;
    }

    totalFuncionarios = 0;

    // Gerar códigos únicos embaralhados
    int *codigos = malloc(sizeof(int) * quantidade);
    for (int i = 0; i < quantidade; i++) {
        codigos[i] = i + 1;
    }
    for (int i = quantidade - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = codigos[i];
        codigos[i] = codigos[j];
        codigos[j] = temp;
    }

    for (int i = 0; i < quantidade; i++) {
        Funcionario f;
        f.codigo = codigos[i];
        strcpy(f.nome, nomes[rand() % 40]);
        strcpy(f.cargo, cargos[rand() % 4]);

        if (strcmp(f.cargo, "Medico") == 0) {
            strcpy(f.especializacao, especializacoes[rand() % 14]);
            f.salario = 9000 + rand() % 5000;
        } else if (strcmp(f.cargo, "Administrador") == 0) {
            strcpy(f.especializacao, "-");
            f.salario = 5000 + rand() % 3000;
        } else {
            strcpy(f.especializacao, "-");
            f.salario = 3000 + rand() % 2000;
        }

        // Departamento aleatório
        int indiceDep = rand() % totalDepartamentos;
        f.codigoDepartamento = departamentos[indiceDep].codigo;

        fwrite(&f, sizeof(Funcionario), 1, arquivo);
        totalFuncionarios++;
    }

    free(codigos);
    fclose(arquivo);
    salvarTotalFuncionarios();
    printf("%d Funcionarios aleatorios gerados com sucesso.\n", totalFuncionarios);
}


void gerarPacientesAleatorios(int quantidade) {
    carregarDepartamentosDoArquivo();
    carregarTotalFuncionarios();

    if (totalDepartamentos == 0 || totalFuncionarios == 0) {
        printf("Erro: É necessário ter ao menos 1 departamento e 1 funcionário.\n");
        return;
    }

    FILE *arquivoPacientes = fopen("pacientes.dat", "wb");
    FILE *arquivoFuncionarios = fopen("funcionarios.dat", "rb");
    if (!arquivoPacientes || !arquivoFuncionarios) {
        printf("Erro ao abrir arquivos de pacientes ou funcionários.\n");
        if (arquivoPacientes) fclose(arquivoPacientes);
        if (arquivoFuncionarios) fclose(arquivoFuncionarios);
        return;
    }

    // Gerar vetor com códigos únicos aleatórios de 1 até quantidade
    int *codigos = malloc(quantidade * sizeof(int));
    if (!codigos) {
        printf("Erro ao alocar memória para códigos.\n");
        fclose(arquivoPacientes);
        fclose(arquivoFuncionarios);
        return;
    }

    for (int i = 0; i < quantidade; i++)
        codigos[i] = i + 1;

    // Embaralhar o vetor de códigos
    for (int i = quantidade - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = codigos[i];
        codigos[i] = codigos[j];
        codigos[j] = temp;
    }

    // Identificar departamentos válidos
    int departamentosValidos[MAX_DEPARTAMENTOS];
    int totalValidos = 0;

    for (int d = 0; d < totalDepartamentos; d++) {
        int codDep = departamentos[d].codigo;
        rewind(arquivoFuncionarios);
        Funcionario f;
        while (fread(&f, sizeof(Funcionario), 1, arquivoFuncionarios) == 1) {
            if (f.codigoDepartamento == codDep) {
                departamentosValidos[totalValidos++] = codDep;
                break;
            }
        }
    }

    if (totalValidos == 0) {
        printf("Erro: Nenhum departamento com funcionários disponíveis.\n");
        fclose(arquivoPacientes);
        fclose(arquivoFuncionarios);
        free(codigos);
        return;
    }

    totalPacientes = 0;

    for (int i = 0; i < quantidade; i++) {
        Paciente p;
        p.codigo = codigos[i]; // código aleatório único
        strcpy(p.nome, nomes[rand() % 40]);
        gerarCPF(p.cpf);
        sprintf(p.dataNascimento, "%02d/%02d/%04d", rand() % 28 + 1, rand() % 12 + 1, 1970 + rand() % 40);
        gerarTelefone(p.telefone);
        strcpy(p.endereco, enderecos[rand() % 18]);

        // Departamento sorteado
        int indice = rand() % totalValidos;
        int codDepSorteado = departamentosValidos[indice];

        // Buscar funcionário no departamento sorteado
        rewind(arquivoFuncionarios);
        Funcionario f;
        int responsavel = -1;
        while (fread(&f, sizeof(Funcionario), 1, arquivoFuncionarios) == 1) {
            if (f.codigoDepartamento == codDepSorteado) {
                responsavel = f.codigo;
                break;
            }
        }

        p.codigoDepartamento = codDepSorteado;
        p.codigoFuncionarioResponsavel = responsavel;

        fwrite(&p, sizeof(Paciente), 1, arquivoPacientes);
        totalPacientes++;
    }

    fclose(arquivoPacientes);
    fclose(arquivoFuncionarios);
    free(codigos);
    salvarTotalPacientes();

    printf("%d Pacientes aleatorios gerados com sucesso.\n", totalPacientes);
}
