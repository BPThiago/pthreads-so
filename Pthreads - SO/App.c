#pragma once
#define _CRT_SECURE_NO_WARNINGS 1 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib,"pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MATRIX_WIDTH 10000
#define MATRIX_HEIGHT 10000
#define BLOCK_WIDTH 1
#define BLOCK_HEIGHT 1

// Global variables
int** matrix;
int total_primos = 0;

// Prototypes
int **allocate_matrix();
void fill_matrix(int **m);
void free_matrix(int **m);
int is_prime(int n);

int main(int argc, char* argv[]) {
	clock_t t_inicio, t_fim;
	double tempo;

	t_inicio = clock();

	matrix = allocate_matrix();

	if (matrix == NULL)
		return 1;

	fill_matrix(matrix);

	for (int i = 0; i < MATRIX_HEIGHT; i++)
		for (int j = 0; j < MATRIX_WIDTH; j++)
			if (is_prime(matrix[i][j])) total_primos++;
	
	printf("Primos encontrados: %d\n", total_primos);

	free_matrix(matrix);

	t_fim = clock();
	tempo = (double)(t_fim - t_inicio)/CLOCKS_PER_SEC;
	printf("Tempo total: %f segundos.\n", tempo);

	return 0;
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
	if (m == NULL)
		return;

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