/* Disciplina: Computacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Lab3:  */
/* Codigo: Faz o produto de matrizes quadrados de forma concorrente */
/* Aluno: Júlio Ricardo Burlamaqui dos Santos */
/* DRE: 121125214 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include "timer.h"

typedef struct 
{
    int dim;
    float *valores;
} Matriz;

//variaveis globais
Matriz *mat; // matriz resultado
Matriz *matA; // matriz A do produto
Matriz *matB; // matriz B do produto
int nthreads; // número de threads

void alocaMatriz(Matriz *matriz)
{//aloca memoria para os valores da struct
    matriz->valores = (float*) malloc(sizeof(float) * matriz->dim * matriz->dim);
    if(!matriz->valores) 
    {
        fprintf(stderr, "Erro de alocao da memoria da matriz\n");
        exit(-1);
    }
}

void leMatriz(const char* filename, Matriz* matriz)
{//carrega matriz do binário
    int n, m; // dimensão das matrizes
    size_t ret; //retorno da funcao de leitura no arquivo de entrada
    FILE *arq = fopen(filename, "rb");

    if(arq==NULL)
    {
        printf("--ERRO: fopen()\n"); 
        exit(-1);
    }

    ret = fread(&n, sizeof(int), 1, arq);
    if(!ret) 
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz \n");
        exit(-1);
    }
    ret = fread(&m, sizeof(int), 1, arq);
    if(!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz \n");
        exit(-1);
    }
    // verifica se matriz é quadrada
    if(n != m)
    {
        printf("A matriz A deve ser quadrada!");
        exit(-1);
    }
    matriz->dim = n;

    alocaMatriz(matriz);

    //carrega a matriz do binario
    ret = fread(matriz->valores, sizeof(float), matriz->dim * matriz->dim, arq);
    if(ret < matriz->dim) 
    {
        fprintf(stderr, "Erro de leitura dos elementos do vetor A\n");        
        exit(-1);
    }

    fclose(arq);
}

void imprimeMatriz(Matriz *matriz)
{// Imprime matrizes
    for(int i = 0; i < matriz->dim; i++)
    {
        for(int j = 0; j < matriz->dim; j++)
            printf("%.2f ", matriz->valores[i*matriz->dim+j]); 
        printf("\n");
    }
}

void *multiplicaMatrizes(void *tid)
{
    int id = (int) tid; //identificador da thread
    int ini, fim, bloco; //auxiliares para divisao do vetor em blocos

    bloco = matA->dim/nthreads; //tamanho do bloco de dados de cada thread
    ini = id*bloco; //posicao inicial do vetor
    fim = (id == (nthreads - 1)) ? matA->dim : ini + bloco; // posicao final do vetor
    //a ultima thread trata os elementos restantes no caso de divisao nao exata

    int i, j, k;

    for(i = ini; i < fim; i++)
    {
        for(j = 0; j < matA->dim; j++)
        {
            mat->valores[i*matA->dim+j] = 0;
            for(k = 0; k < matA->dim; k++)
                mat->valores[id*matA->dim+j] += matA->valores[i*matA->dim+k] * matB->valores[k*matA->dim+j];
        }
    }

    pthread_exit(NULL); 
}

//funcao principal do programa
int main(int argc, char *argv[]) {
    FILE *descritorArquivo; //descritor do arquivo de saida
    size_t ret; //retorno da funcao de leitura no arquivo de saida
    pthread_t *tid_sistema; //vetor de identificadores das threads no sistema
    double inicio, fim, tempo_init, tempo_mult, tempo_fim;

    GET_TIME(inicio);

    //valida e recebe os valores de entrada
    if(argc < 5) { printf("Use: %s <arquivo de entrada da primeira matriz> <arquivo de entrada da segunda matriz> <arquivo de saída> <numero de threads> \n", argv[0]); exit(-1); }

    matA = (Matriz*) malloc(sizeof(Matriz));
    leMatriz(argv[1], matA);
    printf("\nMatriz A\n");
    imprimeMatriz(matA);

    matB = (Matriz*) malloc(sizeof(Matriz));
    leMatriz(argv[2], matB);
    printf("\nMatriz B:\n");
    imprimeMatriz(matB);
    
    if(matA->dim != matB->dim)
    {
        printf("\nAs matrizes devem ter mesma dimensão\n");
        return 1;
    }

    mat = (Matriz*) malloc(sizeof(Matriz));
    mat->dim = matA->dim;
    alocaMatriz(mat);

    GET_TIME(fim);

    tempo_init = fim - inicio;

    GET_TIME(inicio);

    //le o numero de threads da entrada do usuario 
    nthreads = atoi(argv[4]);
    //limita o numero de threads ao tamanho do vetor
    if(nthreads>matA->dim || nthreads < 2) {
        fprintf(stderr, "Erro -- número de threads incompatível\n");
        return 2;
    }

    //aloca espaco para o vetor de identificadores das threads no sistema
    tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid_sistema==NULL) {printf("--ERRO: malloc()\n"); exit(-1);}

    //cria as threads       
    for(int i=0; i<nthreads; i++)
    {  
        if (pthread_create(&tid_sistema[i], NULL, multiplicaMatrizes, (void*) i)) 
            {printf("--ERRO: pthread_create()\n"); exit(-1);}
    }

    for(int i=0; i<nthreads; i++)
        pthread_join(tid_sistema[i], NULL);

    printf("\nMatriz Resultante:\n");
    imprimeMatriz(mat);

    GET_TIME(fim);

    tempo_mult = fim - inicio;

    GET_TIME(inicio);

    descritorArquivo = fopen(argv[3], "wb");
    if (descritorArquivo == NULL)
    {
        printf("--ERRO: fopen() para arquivo de saída\n");
        exit(-1);
    }

    // Escreve a matriz resultante
    ret = fwrite(&mat->dim, sizeof(int), 1, descritorArquivo);
    ret = fwrite(mat->valores, sizeof(float), mat->dim * mat->dim, descritorArquivo);

    // Finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(matA->valores);
    free(matA);
    free(matB->valores);
    free(matB);
    free(mat->valores);
    free(mat);
    free(tid_sistema);

    GET_TIME(fim);

    tempo_fim = fim - inicio;
    
    printf("\nTempo de inicialização concorrente é de %lf\n", tempo_init);
    printf("\nTempo de multiplicação concorrente é de %lf\n", tempo_mult);
    printf("\nTempo de finalização concorrente é de %lf\n", tempo_fim);

    return 0;
}
