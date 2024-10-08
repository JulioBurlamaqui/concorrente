//Programa concorrente que cria e faz operacoes thread-safe com prioridade sobre uma lista de inteiros
//Aluno: Júlio Ricardo Burlamaqui dos Santos
//DRE: 121125214

#include <stdio.h>
#include <stdlib.h>
#include "list_int.h"
#include <pthread.h>
#include "timer.h"

#define QTDE_OPS 10000000 //quantidade de operacoes sobre a lista (insercao, remocao, consulta)
#define QTDE_INI 100 //quantidade de insercoes iniciais na lista
#define MAX_VALUE 100 //valor maximo a ser inserido

//lista compartilhada iniciada 
struct list_node_s* head_p = NULL; 
//qtde de threads no programa
int nthreads;
int escritor_ativo = 0;
int escritores_fila = 0; 
int leitores = 0;

pthread_mutex_t mutex;
pthread_cond_t cond_leitor, cond_escritor;

//tarefa das threads
void* tarefa(void* arg) {
    long int id = (long int) arg;
    int op;
    int in, out, read; 
    in=out=read = 0; 

    //realiza operacoes de consulta (98%), insercao (1%) e remocao (1%)
    for(long int i=id; i<QTDE_OPS; i+=nthreads)
    {
        op = rand() % 100;
        if(op<98) 
        { // leitura
        printf("Sou a thread %ld, tentando ler...\n", id);
            pthread_mutex_lock(&mutex);
                while(escritor_ativo || escritores_fila)
                    pthread_cond_wait(&cond_leitor, &mutex);
                leitores++;
                Member(i%MAX_VALUE, head_p);  /* Ignore return value */  
                leitores--;
            pthread_mutex_unlock(&mutex);

            printf("Sou a thread %ld, e li.\n", id);
            read++;  
        }
        else if(98<=op && op<99) 
        { // inserção
            printf("Sou a thread %ld, tentando inserir...\n", id);
            pthread_mutex_lock(&mutex);
                escritores_fila++;
                while(leitores || escritor_ativo)
                    pthread_cond_wait(&cond_escritor, &mutex);
                escritores_fila--;
                escritor_ativo = 1;

                Insert(i%MAX_VALUE, &head_p);  /* Ignore return value */
            
                escritor_ativo = 0;
                
                if(escritores_fila)
                    pthread_cond_signal(&cond_escritor);
                if(leitores)
                    pthread_cond_broadcast(&cond_leitor);
                    
            pthread_mutex_unlock(&mutex);  

            printf("Sou a thread %ld e inseri %ld.\n", id, i%MAX_VALUE); 
            in++;
        }
        else if(op>=99) 
        {  // deleção
            printf("Sou a thread %ld, tentando apagar...\n", id);
            pthread_mutex_lock(&mutex);
                escritores_fila++;
                while(leitores || escritor_ativo)
                    pthread_cond_wait(&cond_escritor, &mutex);
                escritores_fila--;
                escritor_ativo = 1;
                
                Delete(i%MAX_VALUE, &head_p);  /* Ignore return value */
            
                escritor_ativo = 0;
                
                if(escritores_fila)
                    pthread_cond_signal(&cond_escritor);
                if(leitores)
                    pthread_cond_broadcast(&cond_leitor);

            pthread_mutex_unlock(&mutex);

            printf("Sou a thread %ld e apaguei %ld.\n", id, i%MAX_VALUE); 
            out++;
        }
    }
    //registra a qtde de operacoes realizadas por tipo
    printf("Thread %ld: in=%d out=%d read=%d\n", id, in, out, read);
    pthread_exit(NULL);
}

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    pthread_t *tid;
    double ini, fim, delta;

    //verifica se o numero de threads foi passado na linha de comando
    if(argc<2) {
        printf("Digite: %s <numero de threads>\n", argv[0]); return 1;
    }
    nthreads = atoi(argv[1]);

    //insere os primeiros elementos na lista
    for(int i=0; i<QTDE_INI; i++)
        Insert(i%MAX_VALUE, &head_p);  /* Ignore return value */


    //aloca espaco de memoria para o vetor de identificadores de threads no sistema
    tid = malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {  
        printf("--ERRO: malloc()\n"); return 2;
    }

    //tomada de tempo inicial
    GET_TIME(ini);
    //inicializa a variavel mutex
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_escritor, NULL);
    pthread_cond_init(&cond_leitor, NULL);
    
    //cria as threads
    for(long int i=0; i<nthreads; i++) {
        if(pthread_create(tid+i, NULL, tarefa, (void*) i)) {
            printf("--ERRO: pthread_create()\n"); return 3;
        }
    }

    //aguarda as threads terminarem
    for(int i=0; i<nthreads; i++) {
        if(pthread_join(*(tid+i), NULL)) {
            printf("--ERRO: pthread_join()\n"); return 4;
        }
    }

    //tomada de tempo final
    GET_TIME(fim);
    delta = fim-ini;
    printf("Tempo: %lf\n", delta);

    //libera os mutex e condicionais 
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_leitor);
    pthread_cond_destroy(&cond_escritor);
    //libera o espaco de memoria do vetor de threads
    free(tid);
    //libera o espaco de memoria da lista
    Free_list(&head_p);

    return 0;
}  /* main */