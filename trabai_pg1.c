#include <stdio.h>
#include <stdlib.h>

typedef struct dados {
    int simbolo;
    char palavra[30];
    char cod_huff[30];
} Dados;

typedef struct Tabela {
    Dados reg_dados;
    int cont_freq;
    struct Tabela *prox;
} Tabela;

typedef struct NoArvore {
    int simbolo;
    int cont_freq;
    struct NoArvore *esq, *dir;
} NoArvore;

typedef struct Floresta {
    NoArvore *raiz;
    Tabela *prox;
} Floresta;

// protótipos
void toLower(char frase[]);
void RemovePontuacao(char frase[]);
void extraiPalavra(char frase[], int inicio, int fim, char palavra[]);
int comparaPalavra(char a[], char b[]);
Tabela *inserePalavra(Tabela *lista, char palavra[]);
Tabela *contaFrequencia(char frase[], Tabela *lista);
void imprimeFrequencia(Tabela *lista);
void le_arq();

void toLower(char frase[]) {
    int i = 0;
    while (frase[i] != '\0') {
        if (frase[i] >= 'A' && frase[i] <= 'Z')
            frase[i] = frase[i] + 32;
        i++;
    }
}

void RemovePontuacao(char frase[]) {
    int j = 0;
    for (j = 0; frase[j] != '\0'; j++) {
        if (frase[j] == '"' || frase[j] == '!' || frase[j] == ',' ||
            frase[j] == '.' || frase[j] == '"' || frase[j] == '?' ||
            frase[j] == ':' || frase[j] == ';') {
            int k;
            for (k = j; frase[k] != '\0'; k++)
                frase[k] = frase[k + 1];
            j--;
        }
    }
}

//strtok
void extraiPalavra(char frase[], int inicio, int fim, char palavra[]) {
    int i, j = 0;
    for (i = inicio; i < fim; i++)
        palavra[j++] = frase[i];
    palavra[j] = '\0';
}


// -> verifica se a palavra é igual se for ela conta
int comparaPalavra(char a[], char b[]) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

Tabela *inserePalavra(Tabela *lista, char palavra[]) {
    // procura se já existe
    Tabela *aux = lista;
    while (aux != NULL) {
        if (comparaPalavra(aux->reg_dados.palavra, palavra)) {
            aux->cont_freq++;
            return lista;
        }
        aux = aux->prox;
    }

    // cria novo nópredicativos
    Tabela *novo = (Tabela *)malloc(sizeof(Tabela));
    int i = 0;
    while (palavra[i] != '\0') {
        novo->reg_dados.palavra[i] = palavra[i];
        i++;
    }
    novo->reg_dados.palavra[i] = '\0';
    novo->cont_freq = 1;
    novo->prox = NULL;

    // lista vazia ou novo é menor que o primeiro
    if (lista == NULL || novo->cont_freq < lista->cont_freq) {
        novo->prox = lista;
        return novo;
    }

    // acha a posição certa
    Tabela *ant = lista;
    while (ant->prox != NULL && ant->prox->cont_freq <= novo->cont_freq) {
        ant = ant->prox;
    }
    novo->prox = ant->prox;
    ant->prox = novo;

    return lista;
}
Tabela *contaFrequencia(char frase[], Tabela *lista) {
    int i = 0, inicio = 0;
    char palavra[30];

    while (frase[i] != '\0') {
        if (frase[i] == ' ' || frase[i] == '\n') {
            if (i > inicio) {
                extraiPalavra(frase, inicio, i, palavra);
                lista = inserePalavra(lista, palavra);
            }
            inicio = i + 1;
        }
        i++;
    }
    // última palavra da linha
    if (i > inicio) {
        extraiPalavra(frase, inicio, i, palavra);
        lista = inserePalavra(lista, palavra);
    }

    return lista;
}

void imprimeFrequencia(Tabela *lista) {
    while (lista != NULL) {
        printf("%-15s -> %d\n", lista->reg_dados.palavra, lista->cont_freq);
        lista = lista->prox;
    }
}

void le_arq() {
    FILE *arq = fopen("frases.txt", "r");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo\n");
    }else{
        Tabela *lista = NULL;
        char frase[100];

        fgets(frase, 100, arq);
        while (!feof(arq)) {
            toLower(frase);
            RemovePontuacao(frase);
            lista = contaFrequencia(frase, lista);
            fgets(frase, 100, arq);
        }

        fclose(arq);

        printf("=== Frequencia das palavras ===\n");
        imprimeFrequencia(lista);
    }

  
}

int main() {
    le_arq();
    return 0;
}