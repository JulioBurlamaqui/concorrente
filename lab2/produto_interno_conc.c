/* Disciplina: Computacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Lab2:  */
/* Codigo: Faz o produto interno de dois vetores de forma concorrente */
/* Aluno: Júlio Ricardo Burlamaqui dos Santos */
/* DRE: 121125214 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h> 
#include <math.h>

//variaveis globais
//tamanho do vetor
long int dim;
//vetor de elementos
float *vetA, *vetB;
//numero de threads
int nthreads;

//funcao executada pelas threads
//estrategia de divisao de tarefas: blocos de n/nthreads elementos
void *ProdutoInterno (void *tid) {
    long int id = (long int) tid; //identificador da thread
    int ini, fim, bloco; //auxiliares para divisao do vetor em blocos
    float produto_local, *ret; //somatorio local

    bloco = dim/nthreads; //tamanho do bloco de dados de cada thread
    ini = id*bloco; //posicao inicial do vetor
    fim = ini + bloco; //posicao final do vetor
    if (id==(nthreads-1)) fim = dim; //a ultima thread trata os elementos restantes no caso de divisao nao exata

    //soma os valores 
    for(int i=ini; i<fim; i++) 
        produto_local += vetA[i] * vetB[i];

    //retorna o resultado da soma
    ret = malloc(sizeof(float));
    if (ret!=NULL) *ret = produto_local;
    else printf("--ERRO: malloc() thread\n");

    pthread_exit((void*) ret);
}

//funcao principal do programa
int main(int argc, char *argv[]) {
    FILE *arq; //arquivo de entrada
    size_t ret; //retorno da funcao de leitura no arquivo de entrada
            
    double produto_ori; //produto registrado no arquivo
    float produto_concorrente = 0;
    float *produto_retorno_threads; //auxiliar para retorno das threads
    double variacao; // variação relativa = mod((produto sequencial - produto concorrente)/produto sequencial)

    pthread_t *tid_sistema; //vetor de identificadores das threads no sistema

    //valida e recebe os valores de entrada
    if(argc < 3) { printf("Use: %s <arquivo de entrada> <numero de threads> \n", argv[0]); exit(-1); }

    //abre o arquivo de entrada com os valores para serem multiplicados 
    arq = fopen(argv[1], "rb");
    if(arq==NULL) { printf("--ERRO: fopen()\n"); exit(-1); }

    //le o tamanho do vetor (primeira linha do arquivo)
    ret = fread(&dim, sizeof(long int), 1, arq);
    if(!ret) {
        fprintf(stderr, "Erro de leitura das dimensoes do vetor arquivo \n");
        return 3;
    }

    //aloca espaco de memoria e carrega o vetor A
    vetA = malloc (sizeof(float) * dim);
    if(vetA==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
    ret = fread(vetA, sizeof(float), dim, arq);
    if(ret < dim) {
        fprintf(stderr, "Erro de leitura dos elementos do vetor A\n");
        return 4;
    }

    //aloca espaco de memoria e carrega o vetor B
    vetB = malloc (sizeof(float) * dim);
    if(vetB==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
    ret = fread(vetB, sizeof(float), dim, arq);
    if(ret < dim) {
        fprintf(stderr, "Erro de leitura dos elementos do vetor B\n");
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

    //le o numero de threads da entrada do usuario 
    nthreads = atoi(argv[2]);
    //limita o numero de threads ao tamanho do vetor
    if(nthreads>dim || nthreads < 2) {
        fprintf(stderr, "Erro -- número de threads incompatível\n");
        return 4;
    }

    //aloca espaco para o vetor de identificadores das threads no sistema
    tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid_sistema==NULL) {printf("--ERRO: malloc()\n"); exit(-1);}

    //ERRO AQUI
    //cria as threads
    for(long int i=0; i<nthreads; i++)
    {  
        if (pthread_create(&tid_sistema[i], NULL, ProdutoInterno, (void*) i)) 
            {printf("--ERRO: pthread_create()\n"); exit(-1);}
    }


    //espera todas as threads terminarem e calcula a soma total das threads
    //retorno = (float*) malloc(sizeof(float));
    for(int i=0; i<nthreads; i++) 
    {
        if (pthread_join(tid_sistema[i], (void *) &produto_retorno_threads)) 
          {printf("--ERRO: pthread_join()\n"); exit(-1);}

        produto_concorrente += *produto_retorno_threads;
        free(produto_retorno_threads);
    }

    printf("\n\n");
    //imprime os resultados
    printf("Produto concorrente             = %.26f\n", produto_concorrente);

    //le o somatorio registrado no arquivo
    ret = fread(&produto_ori, sizeof(double), 1, arq); 
    printf("Produto sequencial              = %.26lf\n", produto_ori);

    variacao = fabs((produto_ori - (double) produto_concorrente)/produto_ori);
    printf("Variação relativa               = %.32lf", variacao);

    //desaloca os espacos de memoria
    free(vetA);
    free(vetB);
    free(tid_sistema);
    //fecha o arquivo
    fclose(arq);

    return 0;

}