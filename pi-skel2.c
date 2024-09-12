/* 
 ** Universidade Federal de São Carlos
 ** Departamento de Computação
 ** Prof. Hélio Crestana Guardia
 */

/*
 ** Programa : 
 ** Comando: 
 ** Objetivo: observar passagem de valores para função da thread.
              Nesse caso, usa-se vetor em que cada thread preenche 
              os resultados da sua operação. 
*/

/* 
 Algoritmo de Monte Carlo para cálculo do valor de pi 
 Dados um círculo de raio 1 inserido em um quadrado de lado 2.
 Relação entre pontos aleatórios que estão dentro do círculo 
 e pontos no quadrado pode ser usada para cálculo de pi.

 Relação entre as áreas: (p*r^2)/(4*r^2)
 Considerando r=1: (p/4)=pontos_no_círculo/total_pontos
 p=4*pontos_no_círculo/total_pontos

 Considerando a geração de valores aleatórios entre 0 e 1 (raio)
 para as coordenadas x e y de um ponto, usa-se a equação do teorema 
 de Pitágoras para determinar se ponto está dentro do círculo ou não:
 z^2=x^2+y^2
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define N_THREADS 8
#define SAMPLES (2<<25)

// declarando o mutex e a variavel hits (numero total de pontos que caem dentro do ciruclo) para o calculo do valor de pi (devemos tratar sua sincronizacao)
long hits = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *calcula(void *arg) {
    long amostras = *((long *)arg);
    unsigned short seed[3];
    seed[0] = 0.5; seed[1] = 0.5; seed[2] = (unsigned short)time(NULL) + pthread_self();

    double x, y;
    long local_hits = 0;

    for (long i = 0; i < amostras; i++) {
        x = erand48(seed);
        y = erand48(seed);

        if (x * x + y * y <= 1.0)
            local_hits++;
    }

    pthread_mutex_lock(&mutex);
    hits += local_hits;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    long amostras = SAMPLES;
    pthread_t threads[N_THREADS];

    if (argc > 1)
        amostras = atoi(argv[1]);

    long amostras_por_thread = amostras / N_THREADS;

    for (int i = 0; i < N_THREADS; i++) {
        pthread_create(&threads[i], NULL, calcula, (void *)&amostras_por_thread);
    }

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("hits: %ld\n",hits);
    double pi = (double)(4 * (double)((double)hits / (double)amostras));

    printf("Pi: %.16f\n", pi);

    return 0;
}
