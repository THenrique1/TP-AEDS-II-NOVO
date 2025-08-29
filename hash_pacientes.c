#include "hash_pacientes.h"

// Função de hash por divisão
int hash(int codigo) {
    int h = codigo % TAM_TABELA_HASH_ENC;
    if (h < 0) {
        h = h + TAM_TABELA_HASH_ENC;
    }
    return h;
}

// Função para inicializar a tabela hash no arquivo
void inicializarTabelaHashEncArquivo(FILE *arq) {
    if (!arq) return; // Se o arquivo não foi aberto corretamente, retorna
    long offsets[TAM_TABELA_HASH_ENC];
    for (int i = 0; i < TAM_TABELA_HASH_ENC; i++) offsets[i] = OFFSET_INVALIDO; // Inicializa todos os ponteiros como inválidos
    fseek(arq, 0, SEEK_SET); // Vai para o início do arquivo
    fwrite(offsets, sizeof(long), TAM_TABELA_HASH_ENC, arq); // Escreve o vetor de ponteiros no arquivo
    fflush(arq); // Garante que os dados sejam gravados imediatamente
}

// Insere paciente no hash encadeado exterior (arquivo)
void inserirPacienteHashEncArquivo(FILE *arq, Paciente paciente) {
    if (!arq) return; // Se o arquivo não foi aberto corretamente, retorna
    int idx = hash(paciente.codigo); // Calcula o índice da gaveta usando hash por divisão
    long head;
    fseek(arq, idx * sizeof(long), SEEK_SET); // Vai até o ponteiro da lista encadeada da gaveta
    fread(&head, sizeof(long), 1, arq); // Lê o ponteiro do início da lista

    // Verifica duplicidade
    long atual = head;
    NoPaciente no;
    while (atual != OFFSET_INVALIDO) { // Percorre a lista encadeada
        fseek(arq, atual, SEEK_SET); // Vai até o nó atual
        fread(&no, sizeof(NoPaciente), 1, arq); // Lê o nó
        if (no.paciente.codigo == paciente.codigo) { // Se já existe paciente com o mesmo código
            printf("Paciente com código %d já existe na tabela hash encadeada (arquivo).\n", paciente.codigo);
            return; // Não insere duplicado
        }
        atual = no.prox; // Vai para o próximo nó
    }

    // Insere no início da lista
    fseek(arq, 0, SEEK_END); // Vai para o final do arquivo
    long novo_offset = ftell(arq); // Pega o offset onde será inserido
    NoPaciente novoNo;
    novoNo.paciente = paciente; // Copia os dados do paciente
    novoNo.prox = head; // O próximo do novo nó aponta para o antigo início da lista
    fwrite(&novoNo, sizeof(NoPaciente), 1, arq); // Escreve o novo nó no arquivo

    // Atualiza o ponteiro da gaveta
    fseek(arq, idx * sizeof(long), SEEK_SET);
    fwrite(&novo_offset, sizeof(long), 1, arq); // Atualiza o ponteiro para o novo início
    fflush(arq); // Garante que os dados sejam gravados
    printf("Paciente %s (cod=%d) inserido na tabela hash encadeada (arquivo, slot %d).\n", paciente.nome, paciente.codigo, idx); // Mensagem de sucesso
}

// Busca paciente no hash encadeado exterior (arquivo)
int buscarPacienteHashEncArquivo(FILE *arq, int codigo, Paciente *pacienteEncontrado) {
    if (!arq) return 0; // Se o arquivo não foi aberto corretamente, retorna 0
    int idx = hash(codigo); // Calcula o índice da gaveta
    long head;
    fseek(arq, idx * sizeof(long), SEEK_SET); // Vai até o ponteiro da lista encadeada da gaveta
    fread(&head, sizeof(long), 1, arq); // Lê o ponteiro do início da lista
    long atual = head; // Começa do início da lista
    NoPaciente no;
    while (atual != OFFSET_INVALIDO) { // Percorre a lista encadeada
        fseek(arq, atual, SEEK_SET); // Vai até o nó atual
        fread(&no, sizeof(NoPaciente), 1, arq); // Lê o nó
        if (no.paciente.codigo == codigo) { // Se encontrou o paciente
            *pacienteEncontrado = no.paciente; // Copia o paciente encontrado para o ponteiro de retorno
            return 1; // Sucesso
        }
        atual = no.prox; // Vai para o próximo nó
    }
    return 0; // Não encontrou
}

// Remove paciente do hash encadeado exterior (arquivo)
int removerPacienteHashEncArquivo(FILE *arq, int codigo) {
    if (!arq) return 0; // Se o arquivo não foi aberto corretamente, retorna 0
    int idx = hash(codigo); // Calcula o índice da gaveta
    long head;
    fseek(arq, idx * sizeof(long), SEEK_SET); // Vai até o ponteiro da lista encadeada da gaveta
    fread(&head, sizeof(long), 1, arq); // Lê o ponteiro do início da lista
    long atual = head, anterior = OFFSET_INVALIDO; // Inicializa ponteiros para percorrer a lista
    NoPaciente no;
    while (atual != OFFSET_INVALIDO) { // Percorre a lista encadeada
        fseek(arq, atual, SEEK_SET); // Vai até o nó atual
        fread(&no, sizeof(NoPaciente), 1, arq); // Lê o nó
        if (no.paciente.codigo == codigo) { // Se encontrou o paciente
            // Remove o nó
            if (anterior == OFFSET_INVALIDO) {
                // Remove do início: atualiza o ponteiro da gaveta
                fseek(arq, idx * sizeof(long), SEEK_SET);
                fwrite(&no.prox, sizeof(long), 1, arq); // Atualiza o ponteiro para o próximo nó
            } else {
                // Remove do meio/fim: atualiza o ponteiro do nó anterior
                NoPaciente anteriorNo;
                fseek(arq, anterior, SEEK_SET);
                fread(&anteriorNo, sizeof(NoPaciente), 1, arq);
                anteriorNo.prox = no.prox;
                fseek(arq, anterior, SEEK_SET);
                fwrite(&anteriorNo, sizeof(NoPaciente), 1, arq);
            }
            fflush(arq); // Garante que os dados sejam gravados
            printf("Paciente de código %d removido da tabela hash encadeada (arquivo, slot %d).\n", codigo, idx); // Mensagem de sucesso
            return 1;
        }
        anterior = atual;
        atual = no.prox;
    }
    printf("Paciente de código %d não encontrado na tabela hash encadeada (arquivo).\n", codigo); // Mensagem de erro
    return 0;
}

// Carrega os pacientes na tabela hash do arquivo
void carregarHashPacientes(FILE *arq, TabelaHashPacientes *tabela) {
    if (!arq) return; // Se o arquivo não foi aberto corretamente, retorna
    for (int i = 0; i < TAM_TABELA_HASH_ENC; i++) tabela->buckets[i] = OFFSET_INVALIDO; // Inicializa os ponteiros
    Paciente p;
    long pos = 0;
    NoPaciente no;
    while (fread(&no, sizeof(NoPaciente), 1, arq)) { // Lê os dados do arquivo
        int idx = hash(no.paciente.codigo);
        tabela->buckets[idx] = pos;
        pos = ftell(arq);
    }
}

// Função para liberar a memória da tabela hash
void liberarHash(TabelaHashPacientes *tabela) {
    // Neste caso, não há memória alocada dinamicamente, então nada a liberar
}

void recarregarHashPacientes(TabelaHashPacientes *tabela) {
    if (!tabela) return;  // Verifica se a tabela não é nula

    // Abre o arquivo de pacientes para leitura
    FILE *arq = fopen(ARQUIVO_PACIENTES, "rb");
    if (!arq) {
        printf("Erro ao abrir o arquivo de pacientes.\n");
        return;  // Se não conseguiu abrir o arquivo, retorna
    }

    Paciente p;
    long pos = 0;
    NoPaciente no;

    // Percorre o arquivo de pacientes e atualiza os offsets na tabela hash
    while (fread(&no, sizeof(NoPaciente), 1, arq)) {
        int idx = hash(no.paciente.codigo);  // Calcula o índice da gaveta usando o código do paciente
        tabela->buckets[idx] = pos;  // Atualiza a tabela hash com o offset do paciente
        pos = ftell(arq);  // Avança para a próxima posição no arquivo
    }

    fclose(arq);  // Fecha o arquivo após terminar
}
