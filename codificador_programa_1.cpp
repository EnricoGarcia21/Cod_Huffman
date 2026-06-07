#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dados {
    int simbolo;
    char palavra[30];
    char cod_huff[100];
};
typedef struct dados Dados;

struct Tabela {
    Dados reg_dados;
    int cont_freq;
    struct Tabela *prox;
};
typedef struct Tabela Tabela;

struct NoArvore {
    int simbolo; 
    int cont_freq;
    struct NoArvore *esq, *dir;
};
typedef struct NoArvore NoArvore;

struct Floresta {
    NoArvore *raiz;
    struct Floresta *prox;
};
typedef struct Floresta Floresta;

struct bits {
    unsigned char b0:1;
    unsigned char b1:1;
    unsigned char b2:1;
    unsigned char b3:1;
    unsigned char b4:1;
    unsigned char b5:1;
    unsigned char b6:1;
    unsigned char b7:1;
};

union byte {
    struct bits bi;
    unsigned char num;
};

int contadorSimbolo = 0;


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
            frase[j] == '.' || frase[j] == '?' || frase[j] == ':' || frase[j] == ';') {
            int k;
            for (k = j; frase[k] != '\0'; k++)
                frase[k] = frase[k + 1];
            j--;
        }
    }
}

void extraiPalavra(char frase[], int inicio, int fim, char palavra[]) {
    int i, j = 0;
    for (i = inicio; i < fim; i++)
        palavra[j++] = frase[i];
    palavra[j] = '\0';
}

int comparaPalavra(char a[], char b[]) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

Tabela *inserePalavra(Tabela *lista, char palavra[]) {
    Tabela *aux = lista;
    while (aux != NULL) {
        if (comparaPalavra(aux->reg_dados.palavra, palavra)) {
            aux->cont_freq++;
            return lista;
        }
        aux = aux->prox;
    }

    Tabela *novo = (Tabela *)malloc(sizeof(Tabela));
    novo->reg_dados.simbolo = contadorSimbolo++;
    novo->cont_freq = 1;
    novo->prox = NULL;
    novo->reg_dados.cod_huff[0] = '\0';

    int i = 0;
    while (palavra[i] != '\0') {
        novo->reg_dados.palavra[i] = palavra[i];
        i++;
    }
    novo->reg_dados.palavra[i] = '\0';

    if (lista == NULL || novo->cont_freq < lista->cont_freq) {
        novo->prox = lista;
        return novo;
    }

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
            lista = inserePalavra(lista, "[ESP]"); 
            inicio = i + 1;
        }
        i++;
    }
 
    if (i > inicio) {
        extraiPalavra(frase, inicio, i, palavra);
        lista = inserePalavra(lista, palavra);
    }

    return lista;
}

void imprimeFrequencia(Tabela *lista) {
    printf("\n%-5s %-15s %s\n", "Simb", "Palavra", "Freq");
    printf("-------------------------------\n");
    while (lista != NULL) {
        printf("%-5d %-15s %d\n",
               lista->reg_dados.simbolo,
               lista->reg_dados.palavra,
               lista->cont_freq);
        lista = lista->prox;
    }
}

NoArvore *criaNO(int simbolo, int freq) {
    NoArvore *no = (NoArvore *)malloc(sizeof(NoArvore));
    no->simbolo = simbolo;
    no->cont_freq = freq;
    no->esq = NULL;
    no->dir = NULL;
    return no;
}

Floresta *insereListaNo(Floresta *lista, NoArvore *no) {
    Floresta *novo = (Floresta *)malloc(sizeof(Floresta));
    novo->raiz = no;
    novo->prox = NULL;

    if (lista == NULL || no->cont_freq < lista->raiz->cont_freq) {
        novo->prox = lista;
        return novo;
    }

    Floresta *ant = lista;
    while (ant->prox != NULL && ant->prox->raiz->cont_freq <= no->cont_freq) {
        ant = ant->prox;
    }
    novo->prox = ant->prox;
    ant->prox = novo;

    return lista;
}

Floresta *criaFloresta(Tabela *lista) {
    Floresta *floresta = NULL;
    Tabela *aux = lista;
    while (aux != NULL) {
        NoArvore *folha = criaNO(aux->reg_dados.simbolo, aux->cont_freq);
        floresta = insereListaNo(floresta, folha);
        aux = aux->prox;
    }
    return floresta;
}

NoArvore *constroiHuffman(Floresta *floresta) {
    while (floresta != NULL && floresta->prox != NULL) {
        NoArvore *esq = floresta->raiz;
        floresta = floresta->prox;

        NoArvore *dir = floresta->raiz;
        floresta = floresta->prox;

        NoArvore *pai = criaNO(-1, esq->cont_freq + dir->cont_freq);
        pai->esq = esq;
        pai->dir = dir;

        floresta = insereListaNo(floresta, pai);
    }
    return floresta->raiz; 
}

Tabela *buscaSimbolo(Tabela *lista, int simbolo) {
    while (lista != NULL) {
        if (lista->reg_dados.simbolo == simbolo)
            return lista;
        lista = lista->prox;
    }
    return NULL;
}

void geraCodigosHuffman(NoArvore *raiz, Tabela *lista, char codigo[], int nivel) {
    if (raiz != NULL) {
        if (raiz->esq == NULL && raiz->dir == NULL) {
            codigo[nivel] = '\0';
            Tabela *entrada = buscaSimbolo(lista, raiz->simbolo);
            if (entrada != NULL) {
                int i = 0;
                while (codigo[i] != '\0') {
                    entrada->reg_dados.cod_huff[i] = codigo[i];
                    i++;
                }
                entrada->reg_dados.cod_huff[i] = '\0';
            }
        } else {
            codigo[nivel] = '0';
            geraCodigosHuffman(raiz->esq, lista, codigo, nivel + 1);

            codigo[nivel] = '1';
            geraCodigosHuffman(raiz->dir, lista, codigo, nivel + 1);
        }
    }
}

void imprimeArvore(NoArvore *raiz, int espaco) {
    if (raiz != NULL) {
        espaco += 8;
        imprimeArvore(raiz->dir, espaco);
        printf("\n");
        int i;
        for (i = 8; i < espaco; i++) printf(" ");

        if (raiz->simbolo == -1)
            printf("[%d]\n", raiz->cont_freq);     
        else
            printf("(%d|freq:%d)\n", raiz->simbolo, raiz->cont_freq); 

        imprimeArvore(raiz->esq, espaco);
    }
}

void imprimeTabelaCodigos(Tabela *lista) {
    printf("\n%-5s %-15s %-6s %s\n", "Simb", "Palavra", "Freq", "Cod.Huffman");
    printf("------------------------------------------\n");
    while (lista != NULL) {
        printf("%-5d %-15s %-6d %s\n",
               lista->reg_dados.simbolo,
               lista->reg_dados.palavra,
               lista->cont_freq,
               lista->reg_dados.cod_huff);
        lista = lista->prox;
    }
}

void gravaArquivoBinario(Tabela *lista) {
    FILE *arq = fopen("tabela_huffman.bin", "wb");
    if (arq == NULL) {
        printf("Erro ao criar arquivo binario\n");
    } else {
        Tabela *aux = lista;
        while (aux != NULL) {
            fwrite(&aux->reg_dados, sizeof(Dados), 1, arq);
            aux = aux->prox;
        }
        fclose(arq);
        printf("\nArquivo dicionario (tabela_huffman.bin) gravado com sucesso!\n");
    }
}

void concatenaCodigo(Tabela *lista, char palavra[], char bitString[]) {
    Tabela *aux = lista;
    while (aux != NULL) {
        if (comparaPalavra(aux->reg_dados.palavra, palavra)) {
            int fimString = 0;
            while (bitString[fimString] != '\0') {
                fimString++;
            }
            
            int j = 0;
            while (aux->reg_dados.cod_huff[j] != '\0') {
                bitString[fimString] = aux->reg_dados.cod_huff[j];
                fimString++;
                j++;
            }
            bitString[fimString] = '\0';
            
            return;
        }
        aux = aux->prox;
    }
    printf("Erro: Palavra '%s' nao mapeada no dicionario base!\n", palavra);
}

void codificarEGravarFrase(char frase[], Tabela *lista) {
    char bitString[10000] = "";
    int i = 0, inicio = 0;
    char palavra[30];

    while (frase[i] != '\0') {
        if (frase[i] == ' ') {
            if (i > inicio) {
                extraiPalavra(frase, inicio, i, palavra);
                concatenaCodigo(lista, palavra, bitString);
            }
            concatenaCodigo(lista, "[ESP]", bitString);
            inicio = i + 1;
        }
        i++;
    }
    
    if (i > inicio) {
        extraiPalavra(frase, inicio, i, palavra);
        concatenaCodigo(lista, palavra, bitString);
    }

    FILE *arq = fopen("codigo.bin", "wb");
    if (!arq) return;

    int totalBits = 0;
    while (bitString[totalBits] != '\0') totalBits++;
    
    fwrite(&totalBits, sizeof(int), 1, arq);

    union byte b;
    b.num = 0;
    int bitCount = 0;

    for (i = 0; i < totalBits; i++) {
        int bitVal;
        if (bitString[i] == '1') {
            bitVal = 1;
        } else {
            bitVal = 0;
        }

        if (bitCount == 0) b.bi.b0 = bitVal;
        else if (bitCount == 1) b.bi.b1 = bitVal;
        else if (bitCount == 2) b.bi.b2 = bitVal;
        else if (bitCount == 3) b.bi.b3 = bitVal;
        else if (bitCount == 4) b.bi.b4 = bitVal;
        else if (bitCount == 5) b.bi.b5 = bitVal;
        else if (bitCount == 6) b.bi.b6 = bitVal;
        else if (bitCount == 7) b.bi.b7 = bitVal;

        bitCount++;
        
        if (bitCount == 8) {
            fwrite(&b.num, sizeof(unsigned char), 1, arq);
            b.num = 0;
            bitCount = 0;
        }
    }
    
    if (bitCount > 0) {
        fwrite(&b.num, sizeof(unsigned char), 1, arq);
    }
    
    fclose(arq);
    printf("Arquivo codificado (codigo.bin) gravado com sucesso! Total de bits: %d\n", totalBits);
}

int main() {
    char textoBase[20000] = 
        "Amar e sonhar sonhar e viver viver e curtir curtir e amar Cada um tera "
        "uma escolha cada um fara a escolha cada um escolhe a sua escolha Levaremos "
        "um tempo para crescer levaremos um tempo para amadurecer levaremos um tempo "
        "para entender levaremos um tempo para envelhecer levaremos um tempo para morrer "
        "viver e amar viver e crescer crescer e aprender cada escolha um tempo";

    char fraseParaCodificar[20000] = "levaremos um cada tera fara tempo para sonhar e viver";

    toLower(textoBase);
    RemovePontuacao(textoBase);

    Tabela *lista = NULL;
    lista = contaFrequencia(textoBase, lista);

    printf("=== Frequencia das palavras ===\n");
    imprimeFrequencia(lista);

    NoArvore *raiz = NULL;
    Floresta *floresta = criaFloresta(lista);
    raiz = constroiHuffman(floresta);

    char codigo[100];
    geraCodigosHuffman(raiz, lista, codigo, 0);

    printf("\n=== Arvore de Huffman ===\n");
    imprimeArvore(raiz, 1);

    printf("\n=== Tabela com codigos de Huffman ===\n");
    imprimeTabelaCodigos(lista);

    gravaArquivoBinario(lista);

    toLower(fraseParaCodificar);
    RemovePontuacao(fraseParaCodificar);
    codificarEGravarFrase(fraseParaCodificar, lista);

    return 0;
}