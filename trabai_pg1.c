#include <stdio.h>
#include <stdlib.h>


struct dados{
    int simbolo;
    char palavra[30];
    char cod_huff[30];
};

struct Tabela{
    struct dados reg_dados;
    int cont_freq;
    struct Tabela *prox;
};
struct NoArvore{
    int simbulo;
    int cont_freq;
    struct NoArvore *esq, *dir;
};

struct floresta{
    struct NoArvore *raiz;
    struct Tabela *prox;
};





int main(){

    return 0;
}