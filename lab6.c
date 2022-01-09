#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


int nthreads, *vetor;

// Variáveis para barreira
int waiting = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

void barreira(int nthreads) {
    pthread_mutex_lock(&x_mutex); 
    if (waiting == (nthreads-1)) { 
      
      pthread_cond_broadcast(&x_cond);
      waiting=0;
    } else {
      waiting++;
      pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex);
}

void * FuncaoDaThread(void * arg) {
    int id = (int) arg;

    for(int i = 0; i < nthreads; i++) {
        long int somatorio = 0;

        for(int j = 0; j < nthreads; j++) {
            somatorio += vetor[j];
        }
        
        printf("Thread %d somou %ld na %dº iteração e parou na barreira.\n", id, somatorio, i + 1);

        barreira(nthreads);

        printf("Thread %d na iteração %d saiu da barreira.\n", id, i + 1);

        int novoValor = rand();
        vetor[id] = novoValor;
        printf("Thread %d na iteração %d adicionou %d no vetor e parou na barreira.\n", id, i + 1, novoValor);

        barreira(nthreads);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    if(argc < 1) {
        fprintf(stderr, "Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }

    nthreads = atoi(argv[1]);

    vetor = malloc(sizeof(int) * nthreads);

    for(int i = 0; i < nthreads; i++) {
        vetor[i] = rand();
    }

    pthread_t *tid;
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid==NULL) {
      fprintf(stderr, "ERRO - Número de threads muito grande");
      return 1;
   }

   for(int i=0; i<nthreads; i++) {
      if(pthread_create(tid+i, NULL, FuncaoDaThread, (void*) i)){
         fprintf(stderr, "Erro ao criar thread\n");
         return 1;
      }
   }

   for (int i =0; i<nthreads; i++) {
        if (pthread_join(tid[i], NULL)) {
            printf("--ERRO: pthread_join() \n"); 
            exit(-1); 
        } 
    }
}