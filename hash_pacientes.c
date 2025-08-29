#include "hash_pacientes.h"

// Fun��o de hash por divis�o
int hash(int codigo) {
    int h = codigo % TAM_TABELA_HASH_ENC;
    if (h < 0) {
        h = h + TAM_TABELA_HASH_ENC;
    }
    return h;
}

// Fun��o para inicializar a tabela hash no arquivo
void inicializarTabelaHashEncArquivo(FILE *arq) {
    if (!arq) return; // Se o arquivo n�o foi aberto corretamente, retorna
    long offsets[TAM_TABELA_HASH_ENC];
    for (int i = 0; i < TAM_TABELA_HASH_ENC; i++) offsets[i] = OFFSET_INVALIDO; // Inicializa todos os ponteiros como inv�lidos
    fseek(arq, 0, SEEK_SET); // Vai para o in�cio do arquivo
    fwrite(offsets, sizeof(long), TAM_TABELA_HASH_ENC, arq); // Escreve o vetor de ponteiros no arquivo
    fflush(arq); // Garante que os dados sejam gravados imediatamente
}

// Insere paciente no hash encadeado exterior (arquivo)
void inserirPacienteHashEncArquivo(FILE *arq, Paciente paciente) {
    if (!arq) return; // Se o arquivo n�o foi aberto corretamente, retorna
    int idx = hash(paciente.codigo); // Calcula o �ndice da gaveta usando hash por divis�o
    long head;
    fseek(arq, idx * sizeof(long), SEEK_SET); // Vai at� o ponteiro da lista encadeada da gaveta
    fread(&head, sizeof(long), 1, arq); // L� o ponteiro do in�cio da lista

    // Verifica duplicidade
    long atual = head;
    NoPaciente no;
    while (atual != OFFSET_INVALIDO) { // Percorre a lista encadeada
        fseek(arq, atual, SEEK_SET); // Vai at� o n� atual
        fread(&no, sizeof(NoPaciente), 1, arq); // L� o n�
        if (no.paciente.codigo == paciente.codigo) { // Se j� existe paciente com o mesmo c�digo
            printf("Paciente com c�digo %d j� existe na tabela hash encadeada (arquivo).\n", paciente.codigo);
            return; // N�o insere duplicado
        }
        atual = no.prox; // Vai para o pr�ximo n�
    }

    // Insere no in�cio da lista
    fseek(arq, 0, SEEK_END); // Vai para o final do arquivo
    long novo_offset = ftell(arq); // Pega o offset onde ser� inserido
    NoPaciente novoNo;
    novoNo.paciente = paciente; // Copia os dados do paciente
    novoNo.prox = head; // O pr�ximo do novo n� aponta para o antigo in�cio da lista
    fwrite(&novoNo, sizeof(NoPaciente), 1, arq); // Escreve o novo n� no arquivo

    // Atualiza o ponteiro da gaveta
    fseek(arq, idx * sizeof(long), SEEK_SET);
    fwrite(&novo_offset, sizeof(long), 1, arq); // Atualiza o ponteiro para o novo in�cio
    fflush(arq); // Garante que os dados sejam gravados
    printf("Paciente %s (cod=%d) inserido na tabela hash encadeada (arquivo, slot %d).\n", paciente.nome, paciente.codigo, idx); // Mensagem de sucesso
}

// Busca paciente no hash encadeado exterior (arquivo)
int buscarPacienteHashEncArquivo(FILE *arq, int codigo, Paciente *pacienteEncontrado) {
    if (!arq) return 0; // Se o arquivo n�o foi aberto corretamente, retorna 0
    int idx = hash(codigo); // Calcula o �ndice da gaveta
    long head;
    fseek(arq, idx * sizeof(long), SEEK_SET); // Vai at� o ponteiro da lista encadeada da gaveta
    fread(&head, sizeof(long), 1, arq); // L� o ponteiro do in�cio da lista
    long atual = head; // Come�a do in�cio da lista
    NoPaciente no;
    while (atual != OFFSET_INVALIDO) { // Percorre a lista encadeada
        fseek(arq, atual, SEEK_SET); // Vai at� o n� atual
        fread(&no, sizeof(NoPaciente), 1, arq); // L� o n�
        if (no.paciente.codigo == codigo) { // Se encontrou o paciente
            *pacienteEncontrado = no.paciente; // Copia o paciente encontrado para o ponteiro de retorno
            return 1; // Sucesso
        }
        atual = no.prox; // Vai para o pr�ximo n�
    }
    return 0; // N�o encontrou
}

// Remove paciente do hash encadeado exterior (arquivo)
int removerPacienteHashEncArquivo(FILE *arq, int codigo) {
    if (!arq) return 0; // Se o arquivo n�o foi aberto corretamente, retorna 0
    int idx = hash(codigo); // Calcula o �ndice da gaveta
    long head;
    fseek(arq, idx * sizeof(long), SEEK_SET); // Vai at� o ponteiro da lista encadeada da gaveta
    fread(&head, sizeof(long), 1, arq); // L� o ponteiro do in�cio da lista
    long atual = head, anterior = OFFSET_INVALIDO; // Inicializa ponteiros para percorrer a lista
    NoPaciente no;
    while (atual != OFFSET_INVALIDO) { // Percorre a lista encadeada
        fseek(arq, atual, SEEK_SET); // Vai at� o n� atual
        fread(&no, sizeof(NoPaciente), 1, arq); // L� o n�
        if (no.paciente.codigo == codigo) { // Se encontrou o paciente
            // Remove o n�
            if (anterior == OFFSET_INVALIDO) {
                // Remove do in�cio: atualiza o ponteiro da gaveta
                fseek(arq, idx * sizeof(long), SEEK_SET);
                fwrite(&no.prox, sizeof(long), 1, arq); // Atualiza o ponteiro para o pr�ximo n�
            } else {
                // Remove do meio/fim: atualiza o ponteiro do n� anterior
                NoPaciente anteriorNo;
                fseek(arq, anterior, SEEK_SET);
                fread(&anteriorNo, sizeof(NoPaciente), 1, arq);
                anteriorNo.prox = no.prox;
                fseek(arq, anterior, SEEK_SET);
                fwrite(&anteriorNo, sizeof(NoPaciente), 1, arq);
            }
            fflush(arq); // Garante que os dados sejam gravados
            printf("Paciente de c�digo %d removido da tabela hash encadeada (arquivo, slot %d).\n", codigo, idx); // Mensagem de sucesso
            return 1;
        }
        anterior = atual;
        atual = no.prox;
    }
    printf("Paciente de c�digo %d n�o encontrado na tabela hash encadeada (arquivo).\n", codigo); // Mensagem de erro
    return 0;
}

// Carrega os pacientes na tabela hash do arquivo
void carregarHashPacientes(FILE *arq, TabelaHashPacientes *tabela) {
    if (!arq) return; // Se o arquivo n�o foi aberto corretamente, retorna
    for (int i = 0; i < TAM_TABELA_HASH_ENC; i++) tabela->buckets[i] = OFFSET_INVALIDO; // Inicializa os ponteiros
    Paciente p;
    long pos = 0;
    NoPaciente no;
    while (fread(&no, sizeof(NoPaciente), 1, arq)) { // L� os dados do arquivo
        int idx = hash(no.paciente.codigo);
        tabela->buckets[idx] = pos;
        pos = ftell(arq);
    }
}

// Fun��o para liberar a mem�ria da tabela hash
void liberarHash(TabelaHashPacientes *tabela) {
    // Neste caso, n�o h� mem�ria alocada dinamicamente, ent�o nada a liberar
}

void recarregarHashPacientes(TabelaHashPacientes *tabela) {
    if (!tabela) return;  // Verifica se a tabela n�o � nula

    // Abre o arquivo de pacientes para leitura
    FILE *arq = fopen(ARQUIVO_PACIENTES, "rb");
    if (!arq) {
        printf("Erro ao abrir o arquivo de pacientes.\n");
        return;  // Se n�o conseguiu abrir o arquivo, retorna
    }

    Paciente p;
    long pos = 0;
    NoPaciente no;

    // Percorre o arquivo de pacientes e atualiza os offsets na tabela hash
    while (fread(&no, sizeof(NoPaciente), 1, arq)) {
        int idx = hash(no.paciente.codigo);  // Calcula o �ndice da gaveta usando o c�digo do paciente
        tabela->buckets[idx] = pos;  // Atualiza a tabela hash com o offset do paciente
        pos = ftell(arq);  // Avan�a para a pr�xima posi��o no arquivo
    }

    fclose(arq);  // Fecha o arquivo ap�s terminar
}
