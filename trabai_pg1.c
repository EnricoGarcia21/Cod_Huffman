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
 void toLower(char frase[20]){
        int i = 0;
        while(frase[i]!= '\0'){
            if(frase[i] >= 'A' && frase[i] <= 'Z'){
                frase[i] = frase[i] + 32;
            }
            i++;
        }
    }

//remover todas as vogais acentuadas, pontos e exclamação
    void RemovePontuacao(char frase[100]){
        int j = 0;
        for (j = 0; frase[j] != '\0'; j++){
            if(frase[j] == 'á' || frase[j] == 33 || frase[j] == 46 ||frase[j] == 63){
                int k;
                for (k = j; frase[k] != '\0'; k++){
                    frase[k] = frase[k + 1];
                }
                j--;
            }
        }

    }
    
    //remover pontuaca
            // char frase2 [100] = "OLá, TESTEENTICCO! BIlau.";
            // int j = 0;
            // for (j = 0; frase2[j] != '\0'; j++){
            //     if(frase2[j] == 'á' || frase2[j] == '!' || frase2[j] == '.'||frase2[j] == 'Á'){
            //         int k;
            //         for (k = j; frase2[k] != '\0'; k++){
            //             frase2[k] = frase2[k + 1];
            //         }
            //         j--;
            //     }
            // } 
            // printf("%s\n", frase2);

    int main(){

        char frase [100] = "OLá TEsTeEntCo!. BEELEU?";
        toLower(frase);
        RemovePontuacao(frase);
        printf("%s\n", frase);
        return 0;
    }



