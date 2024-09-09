/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório: 1 */
/* Codigo: Implementar o seu primeiro programa concorrente! */
/* Autor: Júlio Burlamaqui*/
/* DRE: 121125214*/

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
   int idThread, inicio, fim; //M = numero de threads
   int* vetor;
} t_Args;

void criaVetor(int* vetor, int N) // N = tamanho do vetor
{
  int i;
  printf("Vetor original:\n");

  for(i=0; i<N; i++)
  {
    vetor[i] = i;
    printf("Posição %d de %d igual a %d\n", i, N-1, vetor[i]);
  }
}

void imprimeVetor(int* vetor, int N)
{
  int i;
  printf("\nVetor alterado:\n");

  for(i=0; i<N; i++)
    printf("Posição %d de %d igual a %d\n", i, N-1, vetor[i]);
}

//funcao executada pelas threads
void *SomaVetor (void *arg) {
  t_Args *args = (t_Args *) arg;
  int i;

  printf("\nThread %d\n", args->idThread);

  for(i=args->inicio; i<args->fim; i++)
    {
        args->vetor[i] = args->vetor[i] + 1;
        printf("Posição %d de %d igual a %d\n", i, args->fim, args->vetor[i]);
    }
  
  free(arg); //libera a alocacao feita na main
  pthread_exit(NULL);
}

//funcao principal do programa
int main(int argc, char* argv[]) {
  t_Args *args; //receberá os argumentos para a thread

  int M, N; //qtde de threads que serao criadas E tamanho do vetor (recebidas na linha de comando)
  int *vetor;
  int i;

  //verifica se o argumento 'qtde de threads' foi passado e armazena seu valor
  if(argc<3) 
  {
    printf("--ERRO: informe a qtde de threads <%s> <M> e tamanho do vetor <%s> <N>\n", argv[1], argv[2]);
    return 1;
  }
  M = atoi(argv[1]);
  N = atoi(argv[2]);
  if(M < 2 || N < M)
  {
    printf("--ERRO: número de threads e tamanho do vetor incompatíveis.");
    return 1;
  }

  // aloca, checa e cria vetor
  vetor = (int *)malloc(N * sizeof(int));    
  if (vetor == NULL) 
  {
    printf("--ERRO: não foi possível alocar memória.\n");
    return 1;
  }
  criaVetor(vetor, N);  

  //identificadores das threads no sistema
  pthread_t tid_sistema[M];
  int elementosPorThread = N/M; // quantidade de elementos que cada matriz interagirá
  int resto = N%M; // caso a divisão não seja exata, elementos sobresalentes serão passados para as primeiras threads
  
  //cria as threads
  for(i=1; i<=M; i++) 
  {
    printf("--Aloca e preenche argumentos para thread %d\n", i);
    args = malloc(sizeof(t_Args));
    if (args == NULL) 
    {
      printf("--ERRO: malloc()\n"); 
      return 1;
    }

    args->idThread = i-1;
    if (args->idThread < resto) {
        args->inicio = args->idThread * elementosPorThread + args->idThread;
        args->fim = args->inicio + elementosPorThread + 1;
    } else {
        args->inicio = args->idThread * elementosPorThread + resto;
        args->fim = args->inicio + elementosPorThread;
    }
    args->vetor = vetor;

    printf("--Cria a thread %d\n", i);
    if (pthread_create(&tid_sistema[i-1], NULL, SomaVetor, (void*) args)) 
    {
      printf("--ERRO: pthread_create()\n"); 
      return 2;
    }
  }

  //espera todas as threads terminarem
  for (int i=0; i<M; i++) {
    if (pthread_join(tid_sistema[i], NULL)) {
         printf("--ERRO: pthread_join() da thread %d\n", i); 
    } 
  }

  //log da função principal
  printf("--Thread principal terminou\n");

  imprimeVetor(vetor, N);

  free(vetor);

  return 0;
}
