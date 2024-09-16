            /* Disciplina: Computacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Lab2:  */
/* Codigo: Faz o produto interno de dois vetores de forma sequêncial */
/* Aluno: Júlio Ricardo Burlamaqui dos Santos */
/* DRE: 121125214 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000 //valor maximo de um elemento do vetor

double produtoInterno(float* vetA, float* vetB, int dim)
{
    double produtoInterno;

    for(long int i = 0; i < dim; i++)
        produtoInterno += vetA[i] * vetB[i];

    return produtoInterno;
}

int main(int argc, char *argv[])
{
    long int dim; //dimensao das matrizes
    double prod_int;  // variavel que recebera o produto interno
    float *vetA, *vetB; // vetores que serão multiplicados
    int fator = 1; //fator multiplicador para gerar números negativos
    FILE * descritorArquivo; //descritor do arquivo de saida
    size_t ret; //retorno da funcao de escrita no arquivo de saida

    //recebe os argumentos de entrada
    if(argc < 3) {
        fprintf(stderr, "Digite: %s <dimensao> <arquivo saida>\n", argv[0]);
        return 1;
    }
    dim = atoi(argv[1]);

    //aloca memoria para o vetor A
    vetA = (float*) malloc(sizeof(float) * dim);
    if(!vetA) {
        fprintf(stderr, "Erro de alocao da memoria do vetor A\n");
        return 1;
    }

    //cria vetor A
    srand(time(NULL));
    for(long int i=0; i<dim; i++) {
        vetA[i] = (rand() % MAX)/3.0 * fator;
        fator*=-1;
    }
    
    //aloca memoria para o vetor B
    vetB = (float*) malloc(sizeof(float) * dim);
    if(!vetB) {
        fprintf(stderr, "Erro de alocao da memoria do vetor B\n");
        return 1;
    }

    //cria vetor B
    for(long int i=0; i<dim; i++) {
        vetB[i] = (rand() % MAX)/3.0 * fator;
        fator*=-1;
    }

    //escreve o vetor no arquivo
    //abre o arquivo para escrita binaria
    descritorArquivo = fopen(argv[2], "wb");
    if(!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }

    //escreve a dimensao
    ret = fwrite(&dim, sizeof(long int), 1, descritorArquivo);

    //escreve os elementos do vetor
    ret = fwrite(vetA, sizeof(float), dim, descritorArquivo);
    ret = fwrite(vetB, sizeof(float), dim, descritorArquivo);
    if(ret < dim)
    {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 4;
    }

    printf("Vetor A\n");   
    for(long int i = 0; i < dim; i++)
    {
        printf("%lf ", vetA[i]);   
    }

    printf("\n\nVetor B\n"); 
    for(long int i = 0; i < dim; i++)
    {
        printf("%lf ", vetB[i]);   
    }

    prod_int = produtoInterno(vetA, vetB, dim);     
    
    //escreve o produto interno
    ret = fwrite(&prod_int, sizeof(double), 1, descritorArquivo);
    printf("Produto interno: %lf\n", prod_int);   

    //finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(vetA);
    free(vetB);

    return 0;
}           