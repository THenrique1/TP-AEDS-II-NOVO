#include "hospital.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "paciente.h"
#include "funcionario.h"
#include "departamento.h"
#include "hash_pacientes.h"
#include "gerador_dados.h"


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
    "Ginecologia", "Urgência e Emergencia", "Endocrinologia", "Reabilitacao", "Imunologia"
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


// Gera departamentos aleatórios e grava diretamente em departamentos.dat
void gerarDepartamentosAleatorios(int quantidade) {
    if (quantidade <= 0) {
        printf("Quantidade invalida.\n");
        return;
    }

    FILE *arquivo = fopen("departamentos.dat", "wb");  // cria/sobrescreve o arquivo
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de departamentos.\n");
        return;
    }

    // gera códigos únicos embaralhados
    int *codigos = (int *)malloc((size_t)quantidade * sizeof(int));
    if (!codigos) {
        printf("Erro ao alocar memoria para codigos.\n");
        fclose(arquivo);
        return;
    }
    for (int i = 0; i < quantidade; i++) codigos[i] = i + 1;
    // Embaralha
    for (int i = quantidade - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = codigos[i]; codigos[i] = codigos[j]; codigos[j] = tmp;
    }

    // === gera e grava cada departamento no arquivo ===
    totalDepartamentos = 0;
    for (int i = 0; i < quantidade; i++) {
        Departamento d;

        d.codigo = codigos[i];  // código único desordenado
        strcpy(d.nome,        departamentoNomes[rand() % 14]); // nome aleatório
        strcpy(d.responsavel, responsaveis[rand() % 15]);      // responsável aleatório
        d.andar = 1 + rand() % 5;                              // andar entre 1 e 5

        if (fwrite(&d, sizeof(Departamento), 1, arquivo) != 1) {
            printf("Erro de escrita em departamentos.dat na posicao %d.\n", i);
            break;
        }
        totalDepartamentos++;
    }

    free(codigos);
    fclose(arquivo);

    salvarTotalDepartamentos(); // salva o total de departamentos em arquivo auxiliar
    printf("%d Departamentos aleatorios gerados com sucesso.\n", totalDepartamentos);
}

// Gera funcionários aleatórios e grava diretamente em funcionarios.dat
void gerarFuncionariosAleatorios(int quantidade) {
    carregarDepartamentosDoArquivo();

    FILE *arquivo = fopen("funcionarios.dat", "wb"); // cria/sobrescreve o arquivo
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de funcionarios.\n");
        return;
    }

    // Abre departamentos.dat para sortear departamentos
    FILE *arqDep = fopen("departamentos.dat", "rb");
    if (!arqDep) {
        printf("Erro ao abrir departamentos.dat\n");
        fclose(arquivo);
        return;
    }

    totalFuncionarios = 0;

    // gera códigos únicos embaralhados
    int *codigos = malloc(sizeof(int) * quantidade);
    if (!codigos) {
        printf("Erro ao alocar memoria para codigos.\n");
        fclose(arqDep);
        fclose(arquivo);
        return;
    }

    for (int i = 0; i < quantidade; i++) codigos[i] = i + 1;
    for (int i = quantidade - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = codigos[i];
        codigos[i] = codigos[j];
        codigos[j] = temp;
    }

    // gera e grava cada funcionário
    for (int i = 0; i < quantidade; i++) {
        Funcionario f;
        f.codigo = codigos[i];                      // código único desordenado
        strcpy(f.nome, nomes[rand() % 40]);         // nome aleatório
        strcpy(f.cargo, cargos[rand() % 4]);        // cargo aleatório

        // especialização e salário conforme cargo
        if (strcmp(f.cargo, "Medico") == 0) {
            strcpy(f.especializacao, especializacoes[rand() % 14]);
            f.salario = 9000 + rand() % 5000;
        } else if (strcmp(f.cargo, "Administrador") == 0) {
            strcpy(f.especializacao, "-");
            f.salario = 5000 + rand() % 3000;
        } else { // Enfermeiro ou Técnico
            strcpy(f.especializacao, "-");
            f.salario = 3000 + rand() % 2000;
        }

        // sorteia um departamento existente
        int indiceDep = rand() % totalDepartamentos;
        Departamento d;

        // lê o departamento sorteado diretamente do arquivo
        if (fseek(arqDep, (long)indiceDep * (long)sizeof(Departamento), SEEK_SET) == 0 &&
            fread(&d, sizeof(Departamento), 1, arqDep) == 1) {
            f.codigoDepartamento = d.codigo;
        } else {
            // fallback: usa o primeiro registro
            rewind(arqDep);
            if (fread(&d, sizeof(Departamento), 1, arqDep) == 1)
                f.codigoDepartamento = d.codigo;
            else
                f.codigoDepartamento = 0; // segurança
        }

        // grava o funcionário em funcionarios.dat
        if (fwrite(&f, sizeof(Funcionario), 1, arquivo) == 1) {
            totalFuncionarios++;
        }
    }

    free(codigos);
    fclose(arqDep);
    fclose(arquivo);

    salvarTotalFuncionarios(); // salva o total em arquivo auxiliar
    printf("%d Funcionarios aleatorios gerados com sucesso.\n", totalFuncionarios);
}



void gerarPacientesAleatorios(int quantidade) {
    carregarDepartamentosDoArquivo();
    carregarTotalFuncionarios();

    if (quantidade <= 0) {
        printf("Quantidade invalida.\n");
        return;
    }

    //SOBRESCREVE a base (não acumula)

    FILE *arquivoPacientes   = fopen(ARQUIVO_PACIENTES, "wb");
    FILE *arquivoFuncionarios = fopen("funcionarios.dat", "rb");
    if (!arquivoPacientes || !arquivoFuncionarios) {
        printf("Erro ao abrir arquivos de pacientes ou funcionários.\n");
        if (arquivoPacientes)   fclose(arquivoPacientes);
        if (arquivoFuncionarios) fclose(arquivoFuncionarios);
        return;
    }

    //gera códigos únicos 1..quantidade embaralhados
    int *codigos = (int *)malloc((size_t)quantidade * sizeof(int));
    if (!codigos) {
        printf("Erro ao alocar memória para códigos.\n");
        fclose(arquivoPacientes);
        fclose(arquivoFuncionarios);
        return;
    }
    for (int i = 0; i < quantidade; i++) codigos[i] = i + 1;
    for (int i = quantidade - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = codigos[i]; codigos[i] = codigos[j]; codigos[j] = tmp;
    }

    //total de funcionarios direto do arquivo (confirmação)
    fseek(arquivoFuncionarios, 0, SEEK_END);
    long bytesFunc = ftell(arquivoFuncionarios);
    int totalFuncsArquivo = (int)(bytesFunc / (long)sizeof(Funcionario));
    if (totalFuncsArquivo <= 0) {
        printf("Erro: funcionarios.dat vazio.\n");
        free(codigos);
        fclose(arquivoPacientes);
        fclose(arquivoFuncionarios);
        return;
    }

    int gerados = 0;
    for (int i = 0; i < quantidade; i++) {
        // Escolhe UM funcionário aleatório do arquivo e usa seu departamento/código
        int idxFunc = rand() % totalFuncsArquivo;

        Funcionario f;
        if (fseek(arquivoFuncionarios, (long)idxFunc * (long)sizeof(Funcionario), SEEK_SET) != 0 ||
            fread(&f, sizeof(Funcionario), 1, arquivoFuncionarios) != 1) {
            rewind(arquivoFuncionarios);
            if (fread(&f, sizeof(Funcionario), 1, arquivoFuncionarios) != 1) {
                printf("Falha ao ler funcionario. \n");
                break;
            }
        }

        Paciente p;
        p.codigo = codigos[i];

        strcpy(p.nome, nomes[rand() % 40]);
        gerarCPF(p.cpf);
        sprintf(p.dataNascimento, "%02d/%02d/%04d",
                rand() % 28 + 1, rand() % 12 + 1, 1970 + rand() % 40);
        gerarTelefone(p.telefone);
        strcpy(p.endereco, enderecos[rand() % 18]);


        p.codigoDepartamento = f.codigoDepartamento;
        p.codigoFuncionarioResponsavel = f.codigo;
        p.ativo = 1;

        if (fwrite(&p, sizeof(Paciente), 1, arquivoPacientes) == 1) {
            gerados++;
        } else {
            printf("Falha ao escrever paciente codigo %d.\n", p.codigo);
        }
    }

    free(codigos);
    fclose(arquivoPacientes);
    fclose(arquivoFuncionarios);

    totalPacientes = gerados;
    salvarTotalPacientes();
    printf("%d Pacientes aleatorios gerados com sucesso.\n", gerados);

    // Código comentado do hash (pode ser reativado se necessário)
    // FILE *h = fopen("pacientes_hash.dat", "wb+");
    // if (h) {
    //     inicializarTabelaHashEncArquivo(h);  // zera as gavetas (-1)
    //     fclose(h);
    // }
    // recarregarHashAPartirDoDat();
}
