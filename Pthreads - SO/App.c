#pragma once
#define _CRT_SECURE_NO_WARNINGS 1 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib,"pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC
#define NUM_THREADS 2

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MATRIX_WIDTH 10000
#define MATRIX_HEIGHT 10000
#define BLOCK_WIDTH 10
#define BLOCK_HEIGHT 10

// Global variables
int** matrix;
int total_primos = 0, pos_x = 0, pos_y = 0;
pthread_mutex_t primos_mutex;
pthread_mutex_t pos_mutex;

// Prototypes
int **allocate_matrix();
void fill_matrix(int **m);
void free_matrix(int **m);
int is_prime(int n);
void *runner(void *param);

int main(int argc, char* argv[]) {
	clock_t t_inicio, t_fim;
	double tempo;

	srand(42);
	t_inicio = clock();

	matrix = allocate_matrix();
	
	if (matrix == NULL) return 1;

	fill_matrix(matrix);

	/* PThreads Code <---<3 * "a. nucleos fisicos x nucleos logicos" */  
	pthread_t workers[NUM_THREADS];
	pthread_attr_t attr; 

	// Creation and initialization of mutex
	pthread_mutex_init(&primos_mutex, NULL);
	pthread_mutex_init(&pos_mutex, NULL);

	// Initialization of default ajustments for threads
	pthread_attr_init(&attr);

	// Initialization of each thread
	for (int i = 0; i < NUM_THREADS; i++) pthread_create(&workers[i], &attr, runner, NULL);
	
	for (int i = 0; i < NUM_THREADS; i++) pthread_join(workers[i], NULL);

	printf("Total primos: %d\n", total_primos);

	// Destruicao do mutex utilizado para liberacao de sua memoria
	pthread_mutex_destroy(&primos_mutex);
	pthread_mutex_destroy(&pos_mutex);

	free_matrix(matrix);
	
	t_fim = clock();
	tempo = (double)(t_fim - t_inicio) / CLOCKS_PER_SEC;
	printf("Tempo total: %f segundos.\n", tempo);

	return 0;
}

void* runner() {
	int base_x, base_y, limit_x, limit_y, n_primos;
	while (pos_y < MATRIX_HEIGHT) {
		pthread_mutex_lock(&pos_mutex);
		base_x = pos_x;
		base_y = pos_y;
		
		if (base_x + BLOCK_WIDTH >= MATRIX_WIDTH) { 
			pos_x = 0;
			pos_y += BLOCK_HEIGHT;
		}
		else pos_x += BLOCK_WIDTH;
		pthread_mutex_unlock(&pos_mutex);

		limit_x = MIN(base_x + BLOCK_WIDTH, MATRIX_WIDTH);
		limit_y = MIN(base_y + BLOCK_HEIGHT, MATRIX_HEIGHT);

		n_primos = 0;
		for (int i = base_y; i < limit_y; i++) {
			for (int j = base_x; j < limit_x; j++) {
				n_primos += is_prime(matrix[i][j]);
			}
		}

		pthread_mutex_lock(&primos_mutex);
		total_primos += n_primos;
		pthread_mutex_unlock(&primos_mutex);
	}

	pthread_exit(0);
}

int **allocate_matrix() {
	if (MATRIX_HEIGHT < 1 || MATRIX_WIDTH < 1) {
		printf("ERRO: LARGURA OU ALTURA INVALIDOS.");
		return NULL;
	}

	int **m = (int**)calloc(MATRIX_HEIGHT, sizeof(int*));

	if (m == NULL) {
		printf("ERRO: MEMORIA INSUFICIENTE.");
		return NULL;
	}

	for (int i = 0; i < MATRIX_HEIGHT; i++) {
		m[i] = (int*)calloc(MATRIX_WIDTH, sizeof(int));

		if (m[i] == NULL) {
			printf("ERRO: MEMORIA INSUFICIENTE.");
			return NULL;
		}
	}
	return m;
}

void fill_matrix(int **m) {
	for (int i = 0; i < MATRIX_HEIGHT; i++)
		for (int j = 0; j < MATRIX_WIDTH; j++)
			m[i][j] = rand() % 32000;
}

void free_matrix(int **m) {
	if (m == NULL) return;

	for (int i = 0; i < MATRIX_HEIGHT; i++)
		free(m[i]);
	free(m);
}

int is_prime(int n) {
	if (n % 2 == 0 && n != 2) return 0;
	for (int i = 3, upper = sqrt(n); i <= upper; i += 2)
		if (n % i == 0) return 0;
	return 1;
}