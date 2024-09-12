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

#include <unistd.h>
#include <stdio.h> 
#include <pthread.h> 
#include <stdlib.h> 
#include <limits.h> 

#define N_THREADS 8
#define SAMPLES 2<<25


// declarando o mutex e a variavel hits (numero total de pontos que caem dentro do ciruclo) para o calculo do valor de pi (devemos tratar sua sincronizacao)
long hits = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *
calcula(void *arg)
{
    long amostras = *((long *)arg);
	unsigned short seed[3];		// semente usada com valores aleatórios na função erand48()

	seed[0] = 0.5; seed[1] = 0.5; seed[2] = (unsigned short)time(NULL);

	double x, y;  // coordenadas do ponto
    long local_hits = 0;

    for (long i=0; i < amostras; i++) {
		// Geração de números aleatórios entre 0 e 1, usando a função eradnd48()
		x=erand48(seed);
		y=erand48(seed);

		// verifica se coordenadas do ponto estão dentro do círculo
		if(x*x+y*y <= 1.0)
			local_hits++;
	}
    pthread_mutex_lock(&mutex);
    hits += local_hits;
    pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}


int
main(int argc, char **argv) 
{ 
	int  status;
	long amostras;

	double pi;

	int num_threads = 1;
	// vetor de pthread_t para salvamento dos identificadores das threads
	pthread_t *threads; 

	if(argc > 1)
		amostras = atoi(argv[1]);
	else
		amostras = SAMPLES;

	// passando o número de threads como argumanto para a função main...

	if(argc > 2) {
		num_threads = atoi(argv[2]);
		if(num_threads <= 0) {
			printf("Númreo de threads inválido...\n");
			return(EXIT_FAILURE);
		}
	} 
	// aloca memória para o vetor de thread IDs
	threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));	


	// Dividindo as iteraçòes (amostras) entre várias threads...
    long amostras_por_thread = amostras / num_threads;

	// O que passar como parâmetro? Número de amostras que cada uma vai calcular...

	// Loop de criacao das threads
	for (int i=0; i < num_threads; i++) {

		status = pthread_create(&threads[i], NULL, calcula, (void *)&amostras_por_thread );

		if (status) {
			printf("Falha da criacao da thread %d (%d)\n", i, status);
			return (EXIT_FAILURE);
		}
	}

	// Como coletar as somas parciais calculadas pelas threads? 
    // resposta: variavel hits (variavel global incrementada com exclusao mutua)

	// loop de espera pelo término da execução das threads
	for (int i=0; i < num_threads; i++) {

		status = pthread_join(threads[i], NULL);

		if (status) {
			printf("Erro em pthread_join (%d)\n",status);
			break;
		}
	}


	printf("hits: %ld\n",hits);

 	pi=(double) (4 * (double)((double)hits / (double)amostras));

	printf("Pi: %.16f\n",pi);

	return(0);
} 

