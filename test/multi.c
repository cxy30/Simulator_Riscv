#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int* multi(int *A, int *B, int N)
{
	int *C = malloc(N*N*sizeof(int));
	memset(C, 0, N*N*sizeof(int));
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			for(int k = 0; k < N; k++)
			{
				C[i*N+j] += A[i*N+k] * B[k*N+j];
			}
		}
	}
	return C;
}

int main()
{
	int N = 10;
	//int *A = malloc(N*N*sizeof(int));
	//int *B = malloc(N*N*sizeof(int));
	int A[10][10];
	int B[10][10];
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			scanf("%d", &A[i][j]);
		}
	}
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			scanf("%d", &B[i][j]);
		}
	}
	//clock_t start, finish;
	//start = clock();
	int *C = multi(A, B, N);
	printf("A:\n");
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			printf("%d ", A[i][j]);
		}
		printf("\n");
	}
	printf("B:\n");
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			printf("%d ", B[i][j]);
		}
		printf("\n");
	}
	printf("C:\n");
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			printf("%d ", C[i*N+j]);
		}
		printf("\n");
	}
	//finish = clock();
	//double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//printf("Duration: %lf ms\n", duration*1000);
	//free(A);
	//free(B);
	free(C);
	return 0;
}
