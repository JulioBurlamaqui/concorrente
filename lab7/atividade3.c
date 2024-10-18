/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Buffer maluco! */
/* Aluno: Júlio Ricardo Burlamaqui dos Santos */
/* DRE: 121125214*/

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 3
#define TAM1 15
#define TAM2 200        

typedef struct 
{
    int tamanho;
    char *conteudo;
} Mensagem;

// Variaveis globais
sem_t estado1, estado2, estado3;  //semaforos para coordenar a ordem de execucao das threads
char buffer1[TAM1], buffer2[TAM2];  //buffers malucos!
Mensagem *mensagem;
int n = 0; // controla posição de escrita no buffer2


void* carregaBuffer1(void *arg)
{//Carrega trechos do arquivo no buffer 1
    int cont = 0;

    while(cont < mensagem->tamanho)
    {   
        sem_wait(&estado1);
        
            for(int i = 0; i < TAM1; i++)
                buffer1[i] = mensagem->conteudo[cont++];

        sem_post(&estado2);
    }

    pthread_exit(NULL);
}

void* adicionaCaracter(void *arg)
{//Copia conteúdo do buffer 1 para o buffer 2 adicionando \n conforme solicitado
    buffer2[0] = '\0'; // inicializar o buffer2
    int count = 0, linha = 0;
    //contador de números inseridos (que não é a mesma coisa de n) e contador de elementos por linha
  
    while (count < mensagem->tamanho) 
    {
        sem_wait(&estado2);

        for (int i = 0; i < TAM1 && count < mensagem->tamanho; i++) 
        {
            buffer2[n++] = buffer1[i];
            count++;

            if (linha < 10) 
            {
                if (count == (linha * linha + linha + 1))
                {  //fórmula dos números triangulares (faz o padrão 2n+1)
                    buffer2[n++] = '\n';
                    linha++;
                }
            } 
            else if (count % 10 == 0)
                // Depois da décima linha, adiciona uma quebra de linha a cada 10 caracteres
                buffer2[n++] = '\n';
        }

        buffer2[n] = '\0'; // finaliza a string

        sem_post(&estado1);
    }

    sem_post(&estado3); 

    pthread_exit(NULL);
}

void* imprime(void *arg)
{//Imprime buffer 2
    sem_wait(&estado3);

        printf("%s", buffer2);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    FILE *arq; //arquivo de entrada
    size_t ret; //retorno da funcao de leitura no arquivo de entrada
    pthread_t tid[NTHREADS];
    //int tam_buffer1;

    //inicia os semaforos
    sem_init(&estado1, 0, 1);
    sem_init(&estado2, 0, 0);
    sem_init(&estado3, 0, 0);

    mensagem = (Mensagem*) malloc(sizeof(Mensagem));

    //valida e recebe os valores de entrada
    if(argc < 3) { printf("Use: %s <tamanho da mensagem> <arquivo da mensagem>\n", argv[0]); exit(-1); }

    mensagem->tamanho = atoi(argv[1]);

    //abre o arquivo de entrada com a mensagem 
    arq = fopen(argv[2], "rb");
    if(arq==NULL) { printf("--ERRO: fopen()\n"); exit(-1);}

    mensagem->conteudo = (char*) malloc(sizeof(char) * mensagem->tamanho);

    //le a mensagem
    ret = fread(mensagem->conteudo, sizeof(char), mensagem->tamanho, arq);
    if(!ret) {
        fprintf(stderr, "Erro de leitura na mensagem.\n");
        return 3;
    }

    //cria as tres threads
    if (pthread_create(&tid[2], NULL, carregaBuffer1, NULL)) { 
        printf("--ERRO: pthread_create()\n"); exit(-1); }
    if (pthread_create(&tid[1], NULL, adicionaCaracter, NULL)) { 
        printf("--ERRO: pthread_create()\n"); exit(-1); }
    if (pthread_create(&tid[0], NULL, imprime, NULL)) { 
        printf("--ERRO: pthread_create()\n"); exit(-1); }

    //--espera todas as threads terminarem
    for (int t=0; t<NTHREADS; t++) {
        if (pthread_join(tid[t], NULL)) {
                printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    } 

    fclose(arq);

    sem_destroy(&estado1);
    sem_destroy(&estado2);
    sem_destroy(&estado3);  

    free(mensagem->conteudo);
    free(mensagem);

    return 0;
}