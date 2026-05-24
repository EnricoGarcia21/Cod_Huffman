#include <stdio.h>
#include <stdlib.h>

// ============================================================
// STRUCTS
// ============================================================

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
    unsigned char b7:1;
    unsigned char b6:1;
    unsigned char b5:1;
    unsigned char b4:1;
    unsigned char b3:1;
    unsigned char b2:1;
    unsigned char b1:1;
    unsigned char b0:1;
};

union byte {
    struct bits bi;
    unsigned char num;
};

int contadorSimbolo = 0;
void toLower(char frase[]);
void RemovePontuacao(char frase[]);
void extraiPalavra(char frase[], int inicio, int fim, char palavra[]);
int comparaPalavra(char a[], char b[]);
Tabela *inserePalavra(Tabela *lista, char palavra[]);
Tabela *contaFrequencia(char frase[], Tabela *lista);
void imprimeFrequencia(Tabela *lista);
NoArvore *criaNO(int simbolo, int freq);
Floresta *insereListaNo(Floresta *lista, NoArvore *no);
Floresta *criaFloresta(Tabela *lista);
NoArvore *constroiHuffman(Floresta *floresta);
Tabela *buscaSimbolo(Tabela *lista, int simbolo);
void geraCodigosHuffman(NoArvore *raiz, Tabela *lista, char codigo[], int nivel);
Tabela *le_arq(NoArvore **raiz);

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

// ============================================================
// LISTA DE PALAVRAS (TABELA)
// ============================================================

Tabela *inserePalavra(Tabela *lista, char palavra[]) {
    // procura se já existe — só incrementa frequência
    Tabela *aux = lista;
    while (aux != NULL) {
        if (comparaPalavra(aux->reg_dados.palavra, palavra)) {
            aux->cont_freq++;
            return lista;
        }
        aux = aux->prox;
    }

    // palavra nova: cria nó
    Tabela *novo = (Tabela *)malloc(sizeof(Tabela));
    novo->reg_dados.simbolo = contadorSimbolo++; // símbolo único global
    novo->cont_freq = 1;
    novo->prox = NULL;
    novo->reg_dados.cod_huff[0] = '\0';

    int i = 0;
    while (palavra[i] != '\0') {
        novo->reg_dados.palavra[i] = palavra[i];
        i++;
    }
    novo->reg_dados.palavra[i] = '\0';

    // insere mantendo ordem crescente de frequência
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
            lista = inserePalavra(lista, "[ESP]"); // conta o espaço
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

// ============================================================
// ÁRVORE DE HUFFMAN
// ============================================================

NoArvore *criaNO(int simbolo, int freq) {
    NoArvore *no = (NoArvore *)malloc(sizeof(NoArvore));
    no->simbolo = simbolo;
    no->cont_freq = freq;
    no->esq = NULL;
    no->dir = NULL;
    return no;
}

// Insere nó na floresta mantendo ordem crescente de frequência
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

// Converte a Tabela em floresta de folhas
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

// Algoritmo principal de Huffman
NoArvore *constroiHuffman(Floresta *floresta) {
    while (floresta != NULL && floresta->prox != NULL) {
        // retira os dois de menor frequência
        NoArvore *esq = floresta->raiz;
        floresta = floresta->prox;

        NoArvore *dir = floresta->raiz;
        floresta = floresta->prox;

        // nó pai: símbolo -1 = nó interno (não é palavra)
        NoArvore *pai = criaNO(-1, esq->cont_freq + dir->cont_freq);
        pai->esq = esq;
        pai->dir = dir;

        // reinsere o pai na floresta ordenada
        floresta = insereListaNo(floresta, pai);
    }

    return floresta->raiz; // única raiz restante
}

// ============================================================
// GERAÇÃO DOS CÓDIGOS DE HUFFMAN
// ============================================================

// Busca na tabela pelo símbolo para preencher cod_huff
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
            // folha: grava o código na tabela
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
            // nó interno: desce esquerda com "0" e direita com "1"
            codigo[nivel] = '0';
            geraCodigosHuffman(raiz->esq, lista, codigo, nivel + 1);

            codigo[nivel] = '1';
            geraCodigosHuffman(raiz->dir, lista, codigo, nivel + 1);
        }
    }
}
// ============================================================
// EXIBIÇÃO DA ÁRVORE (horizontal, rotacionada 90°)
// ============================================================
void imprimeArvore(NoArvore *raiz, int espaco) {
    if (raiz != NULL) {
        espaco += 8;

        imprimeArvore(raiz->dir, espaco);

        printf("\n");
        int i;
        for (i = 8; i < espaco; i++) printf(" ");

        if (raiz->simbolo == -1)
            printf("[%d]\n", raiz->cont_freq);      // nó interno: só frequência
        else
            printf("(%d|freq:%d)\n", raiz->simbolo, raiz->cont_freq); // folha: símbolo + frequência

        imprimeArvore(raiz->esq, espaco);
    }
}
// ============================================================
// IMPRESSÃO DA TABELA COM CÓDIGOS
// ============================================================

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

// ============================================================
// LEITURA DO ARQUIVO
// ============================================================

// Retorna a lista e preenche *raiz com a raiz da árvore
Tabela *le_arq(NoArvore **raiz) {
    FILE *arq = fopen("frases.txt", "r");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo\n");
       
    }else{
    	
    	Tabela *lista = NULL;
    	char frase[200];

	    fgets(frase, 200, arq);
	    while (!feof(arq)) {
	        toLower(frase);
	        RemovePontuacao(frase);
	        lista = contaFrequencia(frase, lista);
	        fgets(frase, 200, arq);
	    }
	    fclose(arq);
	
	    // monta a árvore de huffman
	    Floresta *floresta = criaFloresta(lista);
	    *raiz = constroiHuffman(floresta);
	
	    // gera os códigos e preenche a tabela
	    char codigo[100];
	    geraCodigosHuffman(*raiz, lista, codigo, 0);
	    
	    return lista;

    }

    
}

//void gravaArquivoBinario(Tabela *lista) {
//    FILE *arq = fopen("tabela_huffman.bin", "wb");
//    if (arq == NULL) {
//        printf("Erro ao criar arquivo binario\n");
//    } else {
//        Tabela *aux = lista;
//        while (aux != NULL) {
//            fwrite(&aux->reg_dados, sizeof(Dados), 1, arq);
//            aux = aux->prox;
//        }
//        fclose(arq);
//        printf("Arquivo binario gravado com sucesso!\n");
//    }
//}
//void leArquivoBinario() {
//    FILE *arq = fopen("tabela_huffman.bin", "rb");
//    if (arq == NULL) {
//        printf("Erro ao abrir arquivo binario\n");
//    } else {
//        Dados reg;
//        printf("\n=== Leitura do arquivo binario ===\n");
//        while (fread(&reg, sizeof(Dados), 1, arq) == 1) {
//            printf("Simb: %-5d Palavra: %-15s Cod: %s\n",
//                   reg.simbolo, reg.palavra, reg.cod_huff);
//        }
//        fclose(arq);
//    }
//}

int main() {
    NoArvore *raiz = NULL;
    Tabela *lista = le_arq(&raiz);

    printf("=== Frequencia das palavras ===\n");
    imprimeFrequencia(lista);

    printf("\n=== Arvore de Huffman ===\n");
    imprimeArvore(raiz,1);

    printf("\n=== Tabela com codigos de Huffman ===\n");
    imprimeTabelaCodigos(lista);

    //gravaArquivoBinario(lista);
    
    return 0;
}
