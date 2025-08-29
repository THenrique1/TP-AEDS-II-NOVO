#include "hash_pacientes.h"
#include "hospital.h"

// Função de hash
int funcao_hash(int cod) {
    return cod % TAMANHO_TABELA_HASH;
}

// Criar a tabela hash de pacientes
void criar_tabela_hash_pacientes(FILE *tabela_hash) {
    // Move o cursor para o início do arquivo
    rewind(tabela_hash);

    int pos_vazia = -1;
    printf("Criando a tabela com %d posicoes\n", TAMANHO_TABELA_HASH);

    // Preenche a tabela com -1 indicando posições vazias
    for (int i = 0; i < TAMANHO_TABELA_HASH; i++) {
        fwrite(&pos_vazia, sizeof(int), 1, tabela_hash);
    }

    // Garante que os dados sejam gravados no disco
    fflush(tabela_hash);  // Garante que os dados sejam efetivamente gravados no arquivo

    // Agora, mova o ponteiro de volta para o início do arquivo
    rewind(tabela_hash);

    printf("Tabela Hash 'pacientes.dat' criada e inicializada com sucesso\n");
}

// Função para inicializar a tabela hash
void inicializarHash(HashPacientes *tabela) {
    for (int i = 0; i < TAMANHO_TABELA_HASH; i++) {
        tabela->buckets[i] = NULL;
    }
}

// Função para inserir um paciente na lista encadeada na tabela hash
void inserir_na_lista_hash(FILE *tabela_hash, FILE *arq_pacientes, int pos_novo_paciente, int cod_novo_paciente) {
    int hash_addr = funcao_hash(cod_novo_paciente);
    fseek(tabela_hash, hash_addr * sizeof(int), SEEK_SET);

    int pos_cabeca_lista;
    fread(&pos_cabeca_lista, sizeof(int), 1, tabela_hash);

    if (pos_cabeca_lista == -1) {
        // Se não houver colisão, insere o novo paciente
        fseek(tabela_hash, hash_addr * sizeof(int), SEEK_SET);
        fwrite(&pos_novo_paciente, sizeof(int), 1, tabela_hash);
    } else {
        // Se houver colisão, insere o paciente no final da lista encadeada
        int pos_atual = pos_cabeca_lista;
        Paciente *paciente_percorrido;

        while (1) {
            fseek(arq_pacientes, pos_atual * tamanho_registro_paciente(), SEEK_SET);
            paciente_percorrido = le_paciente(arq_pacientes);

            // Verifica o último paciente da lista
            if (paciente_percorrido->prox == -1) {
                paciente_percorrido->prox = pos_novo_paciente;
                fseek(arq_pacientes, pos_atual * tamanho_registro_paciente(), SEEK_SET);
                salva_paciente(paciente_percorrido, arq_pacientes);
                free(paciente_percorrido);
                break;
            }

            pos_atual = paciente_percorrido->prox;
            free(paciente_percorrido);
        }
    }
}

// Função para inserir um paciente na tabela hash
void inserirPacienteHash(HashPacientes *tabela, Paciente p) {
    p.ativo = 1;  // Define o paciente como ativo
    Paciente *existente = buscarPacienteHash(tabela, p.codigo, NULL);

    if (existente != NULL) {
        printf("Erro: paciente com codigo %d ja existe (ativo).\n", p.codigo);
        free(existente);
        return;
    }

    FILE *arquivo = abrirArquivoPacientesParaAnexar();
    if (!arquivo) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    // Move para o final do arquivo para adicionar o paciente
    if (fseek(arquivo, 0, SEEK_END) != 0) {
        printf("Erro em fseek(END).\n");
        fclose(arquivo);
        return;
    }

    long pos = ftell(arquivo);
    if (pos == -1L) {
        printf("Erro em ftell.\n");
        fclose(arquivo);
        return;
    }

    // Escreve o novo paciente no arquivo
    size_t escritos = fwrite(&p, sizeof(Paciente), 1, arquivo);
    if (escritos != 1) {
        printf("Erro ao gravar o registro.\n");
        fclose(arquivo);
        return;
    }

    if (fflush(arquivo) != 0) {
        printf("Aviso: fflush falhou.\n");
    }
    fclose(arquivo);

    // Insere o paciente na tabela hash
    int indice = funcao_hash(p.codigo);
    No *novo = (No*) malloc(sizeof(No));
    if (!novo) {
        printf("Erro de memoria.\n");
        return;
    }
    novo->codigo = p.codigo;
    novo->offset = pos;
    novo->prox   = tabela->buckets[indice];
    tabela->buckets[indice] = novo;

    totalPacientes++;
    salvarTotalPacientes();
    printf("Paciente %s (cod=%d) inserido com sucesso!\n", p.nome, p.codigo);
}

// Função que retorna o tamanho do registro do paciente
int tamanho_registro_paciente() {
    return sizeof(Paciente);  // Retorna o tamanho da estrutura Paciente
}

// Função para ler um paciente do arquivo
Paciente* le_paciente(FILE *in) {
    Paciente *p = (Paciente*) malloc(sizeof(Paciente));
    if (!p) {
        printf("Erro de memória ao ler paciente.\n");
        return NULL;
    }
    size_t lidos = fread(p, sizeof(Paciente), 1, in);
    if (lidos != 1) {
        free(p);
        return NULL;
    }
    return p;
}

// Função para salvar um paciente no arquivo
void salva_paciente(Paciente *p, FILE *out) {
    size_t escritos = fwrite(p, sizeof(Paciente), 1, out);
    if (escritos != 1) {
        printf("Erro ao salvar paciente.\n");
    }
}

// Função para abrir o arquivo de pacientes para anexar dados
FILE* abrirArquivoPacientesParaAnexar() {
    FILE *f = fopen(ARQUIVO_PACIENTES, "rb+");
    if (!f) f = fopen(ARQUIVO_PACIENTES, "wb+");
    return f;
}

// Função para carregar os pacientes na tabela hash
void carregar_hash_pacientes(HashPacientes *tabela, FILE *arq_pacientes) {
    FILE *arquivo = fopen(ARQUIVO_PACIENTES, "rb");
    if (!arquivo) return;

    Paciente p;
    long pos = 0;

    while (1) {
        size_t lidos = fread(&p, sizeof(Paciente), 1, arquivo);
        if (lidos != 1) break;

        if (p.ativo) {
            int indice = funcao_hash(p.codigo);
            No *novo = (No*) malloc(sizeof(No));
            if (!novo) {
                printf("Erro de memoria ao carregar hash.\n");
                break;
            }
            novo->codigo = p.codigo;
            novo->offset = pos;
            novo->prox   = tabela->buckets[indice];
            tabela->buckets[indice] = novo;
        }

        pos = ftell(arquivo);
        if (pos == -1L) {
            printf("Erro em ftell durante carregamento.\n");
            break;
        }
    }

    fclose(arquivo);
}

// Função para liberar a memória da tabela hash
void liberar_hash(HashPacientes *tabela) {
    for (int i = 0; i < TAMANHO_TABELA_HASH; i++) {
        No *atual = tabela->buckets[i];
        while (atual != NULL) {
            No *tmp = atual;
            atual = atual->prox;
            free(tmp);
        }
        tabela->buckets[i] = NULL;
    }
}

// Função para recarregar a tabela hash de pacientes
void recarregar_hash_pacientes(HashPacientes *tabela, FILE *arq_pacientes) {
    liberar_hash(tabela);
    inicializarHash(tabela);
    carregar_hash_pacientes(tabela, arq_pacientes);
}

// Função para imprimir o estado atual da tabela hash
void imprimir_tabela_hash_pacientes(HashPacientes *tabela_hash, FILE *arq_pacientes) {
    printf("\n--- IMPRIMINDO TABELA HASH DE PACIENTES ---\n");

    for (int i = 0; i < TAMANHO_TABELA_HASH; i++) {
        No *atual = tabela_hash->buckets[i];
        if (atual == NULL) {
            printf("Bucket %d: Vazio\n", i);
        } else {
            printf("Bucket %d: ", i);
            while (atual != NULL) {
                Paciente *p = NULL;
                fseek(arq_pacientes, atual->offset, SEEK_SET);
                fread(&p, sizeof(Paciente), 1, arq_pacientes);

                if (p != NULL) {
                    printf("(Codigo %d) %s\n", p->codigo, p->nome);
                }

                atual = atual->prox;
            }
            printf("\n");
        }
    }
    printf("---------------------------------------------\n");
}
