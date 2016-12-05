#include <stdio.h>
#include <stdlib.h>

void sort(int *begin, int *end)
{
	if(begin + 1 >= end) return;
	int *left = begin, *right = end - 1;
	int pivot = *left;
	while(left < right)
	{
		while(left < right && *right > pivot) right--;
		if(left < right)
		{
			*left = *right;
			left++;
		}
		while(left < right && *left < pivot) left++;
		if(left < right)
		{
			*right = *left;
			right--;
		}
	}
	*left = pivot;
	sort(begin, left);
	sort(left+1, end);
}

int main(int argc, char **argv)
{
	int num = 10;
	//clock_t start, finish;
	int A[] = {3, 5, 1, 3, 7, 4, 3, 7, 8, 1};
	int i = 0;
	printf("A[] = {%d", A[0]);
	for(i = 1; i < num; i++)
	{
		printf(", %d", A[i]);
	}
	printf("}\n");
	//start = clock();
	sort(A, A+num);
	printf("A[] = {%d", A[0]);
	for(i = 1; i < num; i++)
	{
		printf(", %d", A[i]);
	}
	printf("}\n");
	//finish = clock();
	//double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//printf("Duration: %lf ms\n", duration*1000);
	return 0;
}
