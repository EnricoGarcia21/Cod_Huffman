#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dados {
    int simbolo;
    char palavra[30];
    char cod_huff[100];
};
typedef struct dados Dados;

struct NoArvore {
    int simbolo;
    struct NoArvore *esq, *dir;
};
typedef struct NoArvore NoArvore;

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

NoArvore *criaNoInterno() {
    NoArvore *no = (NoArvore *)malloc(sizeof(NoArvore));
    no->simbolo = -1;
    no->esq = NULL;
    no->dir = NULL;
    return no;
}

NoArvore *reconstruirArvoreDeCodigos(Dados tabela[], int tam) {
    NoArvore *raiz = criaNoInterno();
    int i, j;
    
    for (i = 0; i < tam; i++) {
        NoArvore *atual = raiz;
        char *codigo = tabela[i].cod_huff;
        
        for (j = 0; codigo[j] != '\0'; j++) {
            if (codigo[j] == '0') {
                if (atual->esq == NULL) atual->esq = criaNoInterno();
                atual = atual->esq;
            } else if (codigo[j] == '1') {
                if (atual->dir == NULL) atual->dir = criaNoInterno();
                atual = atual->dir;
            }
        }
        atual->simbolo = tabela[i].simbolo;
    }
    return raiz;
}

char *buscarPalavra(Dados tabela[], int tam, int simbolo) {
    int i;
    for (i = 0; i < tam; i++) {
        if (tabela[i].simbolo == simbolo) {
            return tabela[i].palavra;
        }
    }
    return NULL; 
}

int main() {
    FILE *arqTab = fopen("tabela_huffman.bin", "rb");
    if (arqTab == NULL) {
        printf("Erro: Arquivo 'tabela_huffman.bin' nao encontrado.\n");
        return 1;
    }

    Dados dicionario[200];
    int totalRegistros = 0;
    while (fread(&dicionario[totalRegistros], sizeof(Dados), 1, arqTab) == 1) {
        totalRegistros++;
    }
    fclose(arqTab);

    NoArvore *raizReconstruida = reconstruirArvoreDeCodigos(dicionario, totalRegistros);

    FILE *arqCod = fopen("codigo.bin", "rb");
    if (arqCod == NULL) {
        printf("Erro: Arquivo 'codigo.bin' nao encontrado.\n");
        return 1;
    }

    int totalBits;
    fread(&totalBits, sizeof(int), 1, arqCod);

    printf("=============================================\n");
    printf("         EXIBICAO DO PROGRAMA 2\n");
    printf("=============================================\n\n\"");

    NoArvore *atual = raizReconstruida;
    int bitsProcessados = 0;
    union byte b;

    while (bitsProcessados < totalBits) {
        fread(&b.num, sizeof(unsigned char), 1, arqCod);
        
        int bitPos;
        for (bitPos = 0; bitPos < 8 && bitsProcessados < totalBits; bitPos++, bitsProcessados++) {
            int bitVal = 0;
            
            if (bitPos == 0) bitVal = b.bi.b0;
            else if (bitPos == 1) bitVal = b.bi.b1;
            else if (bitPos == 2) bitVal = b.bi.b2;
            else if (bitPos == 3) bitVal = b.bi.b3;
            else if (bitPos == 4) bitVal = b.bi.b4;
            else if (bitPos == 5) bitVal = b.bi.b5;
            else if (bitPos == 6) bitVal = b.bi.b6;
            else if (bitPos == 7) bitVal = b.bi.b7;

            if (bitVal == 0) atual = atual->esq;
            else atual = atual->dir;

            if (atual->esq == NULL && atual->dir == NULL) {
                // Removido o 'const'
                char *resultado = buscarPalavra(dicionario, totalRegistros, atual->simbolo);
                
                if (resultado != NULL) {
                    if (strcmp(resultado, "[ESP]") == 0) {
                        printf(" ");
                    } else {
                        printf("%s", resultado);
                    }
                }
                
                atual = raizReconstruida;
            }
        }
    }
    printf("\"\n\nDecodificacao binaria concluida com sucesso!\n");
    fclose(arqCod);

    return 0;
}