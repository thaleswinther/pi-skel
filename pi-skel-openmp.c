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




int
main(int argc, char **argv) 
{ 
	int  status;
	long amostras, fracao, parcial;
	long hits = 0;

	double pi;

	if(argc > 1)
		amostras = atoi(argv[1]);
	else
		amostras = SAMPLES;



	// semente usada com valores aleatórios na função erand48()
	unsigned short seed[3];		// semente usada com valores aleatórios na função erand48()

	seed[0] = 0.5; seed[1] = 0.5; seed[2] = (unsigned short)time(NULL);

	double x, y;  // coordenadas do ponto
	
	#pragma omp parallel for 
	for (long i=0; i < amostras; i++) {
		// Geração de números aleatórios entre 0 e 1, usando a função eradnd48()
		x=erand48(seed);
		y=erand48(seed);

		// verifica se coordenadas do ponto estão dentro do círculo
		if(x*x+y*y <= 1.0)
			#pragma omp atomic
			hits++;
	}

	printf("hits: %ld\n",hits);

 	pi=(double) (4 * (double)((double)hits / (double)amostras));

	printf("Pi: %.16f\n",pi);

	return(0);
} 

