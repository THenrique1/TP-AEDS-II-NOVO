#include "hash_pacientes.h"


// Função de hash por divisão
int hash(int codigo) {
    int h = codigo % TAM_TABELA_HASH_ENC; // tamanho = 51
    if (h < 0) {
        h = h + TAM_TABELA_HASH_ENC;
    }
    return h;
}



// Função para inicializar a tabela hash no arquivo
void inicializarTabelaHashEncArquivo(FILE *arq) {
    if (!arq) return; // Se o arquivo não foi aberto corretamente, retorna
    long offsets[TAM_TABELA_HASH_ENC];
    for (int i = 0; i < TAM_TABELA_HASH_ENC; i++) offsets[i] = OFFSET_INVALIDO; // Inicializa todos os ponteiros como inválidos !
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
    // printf("Paciente %s (cod=%d) inserido na tabela hash encadeada (arquivo, slot %d).\n", paciente.nome, paciente.codigo, idx); // Mensagem de sucesso
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
                // Remove do meio/fim: atualiza o ponteiro do nó anterior para saber que é o proximo ou vira -1 caso seja o ultimo
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



int recarregarHashAPartirDoDat(void) {
    // 1) Abre/cria e zera o arquivo de hash
    FILE *h = fopen("pacientes_hash.dat", "wb+");
    if (!h) {
        printf("Erro ao criar pacientes_hash.dat\n");
        return 0;
    }
    inicializarTabelaHashEncArquivo(h);

    // 2) Percorre o arquivo de dados e insere no índice
    FILE *d = fopen(ARQUIVO_PACIENTES, "rb");
    if (!d) {
        printf("Erro ao abrir %s\n", ARQUIVO_PACIENTES);
        fclose(h);
        return 0;
    }

    Paciente p;
    int inseridos = 0;
    while (fread(&p, sizeof(Paciente), 1, d) == 1) { // Lê um paciente por vez do arquivo (pacientes.dat) e continua o loop enquanto a leitura retornar 1 (ou seja, até o fim do arquivo)
        if (p.ativo != 1) continue;              // só indexa ativos
        inserirPacienteHashEncArquivo(h, p);     // grava nó (NoPaciente) no hash encadeado
        inseridos++;
    }

    fclose(d);
    fclose(h);
   // printf("Reindexacao concluida: %d paciente(s) ativo(s) indexado(s).\n", inseridos);
    return 1;
}

