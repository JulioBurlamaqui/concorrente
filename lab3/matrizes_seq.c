/* Disciplina: Computacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Lab3:  */
/* Codigo: Faz o produto de matrizes quadrados de forma sequêncial */
/* Aluno: Júlio Ricardo Burlamaqui dos Santos */
/* DRE: 121125214 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct 
{
    int dim;
    float *valores;
} Matriz;

void alocaMatriz(Matriz *matriz)
{
    //aloca memoria para a matriz
    matriz->valores = (float*) malloc(sizeof(float) * matriz->dim * matriz->dim);
    if(!matriz->valores) 
    {
        fprintf(stderr, "Erro de alocao da memoria da matriz\n");
        exit(-1);
    }
}

Matriz* leMatriz(const char* filename)
{
    int n, m; // dimensão das matrizes
    size_t ret; //retorno da funcao de leitura no arquivo de entrada
    Matriz *matriz = (Matriz*) malloc(sizeof(Matriz)); // struct da matriz lida
    
    FILE *arq = fopen(filename, "rb");
    if(arq==NULL)
    {
        printf("--ERRO: fopen()\n"); 
        exit(-1);
    }

    ret = fread(&n, sizeof(int), 1, arq);
    if(!ret) 
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz\n");
        exit(-1);
    }
    ret = fread(&m, sizeof(int), 1, arq);
    if(!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz\n");
        exit(-1);
    }
    // verifica se matriz é quadrada
    if(n != m)
    {
        printf("A matriz deve ser quadrada!");
        exit(-1);
    }
    matriz->dim = n;

    alocaMatriz(matriz);

    //carrega a matriz do binario
    ret = fread(matriz->valores, sizeof(float), matriz->dim * matriz->dim, arq);
    if(ret < matriz->dim * matriz->dim) 
    {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");        
        exit(-1);
    }

    fclose(arq);

    return matriz;
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

void multiplicaMatrizes(Matriz *resultado, Matriz *matA, Matriz *matB)
{
    int i, j, k;

    resultado->dim = matA->dim;
    alocaMatriz(resultado);

    for(i = 0; i < matA->dim; i++)
    {
        for(j = 0; j < matA->dim; j++)
        {
            resultado->valores[i*matA->dim+j] = 0; // inicializando a matriz resultado com 0
            for(k = 0; k < matA->dim; k++)   
                resultado->valores[i*matA->dim+j] += matA->valores[i*matA->dim+k] * matB->valores[k*matA->dim+j];
        }
    }
}

int main(int argc, char *argv[])
{
    Matriz *mat, *matA, *matB; // matrizes
    FILE *descritorArquivo; //descritor do arquivo de saida
    size_t ret; //retorno da funcao de escrita no arquivo de saida
    clock_t inicio, fim; 
    double tempo_init, tempo_mult, tempo_fim; //timestamps

    inicio = clock();

    //valida e recebe os valores de entrada
    if(argc < 4) {printf("Use: %s <arquivo de entrada da primeira matriz> <arquivo de entrada da segunda matriz> <arquivo de saída> \n", argv[0]); exit(-1); }

    matA = leMatriz(argv[1]);
    printf("\nMatriz A\n");
    imprimeMatriz(matA);

    matB = leMatriz(argv[2]);
    printf("\nMatriz B:\n");
    imprimeMatriz(matB);

    if(matA->dim != matB->dim)
    {
        printf("\nAs matrizes devem ter mesma dimensão\n");
        return 1;
    }

    fim = clock();

    tempo_init = fim - inicio;

    inicio = clock();

    mat = (Matriz*) malloc(sizeof(Matriz));
    multiplicaMatrizes(mat, matA, matB);
    printf("\nMatriz Resultante:\n");
    imprimeMatriz(mat);   

    fim = clock();

    tempo_mult = fim - inicio;

    inicio = clock();

    // Abre o arquivo de saída
    descritorArquivo = fopen(argv[3], "wb");
    if(descritorArquivo == NULL)
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

    fim = clock();

    tempo_fim = fim - inicio;

    printf("\nTempo de inicialização sequencial é de %lf milissegundos.\n", tempo_init);
    printf("\nTempo de multiplicação sequencial é de %lf milissegundos.\n", tempo_mult);
    printf("\nTempo de finalização sequencial é de %lf milissegundos.\n", tempo_fim);

    return 0;
}