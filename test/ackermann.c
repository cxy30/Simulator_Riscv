#include <stdio.h>

int ackermann(int m, int n)
{
	if(m == 0) return n + 1;
	if(m > 0 && n == 0) return ackermann(m-1, 1);
	return ackermann(m-1, ackermann(m, n-1));
}

int main()
{
	//clock_t start, finish;
	int res;
	//start = clock();
	res = ackermann(3, 8);
	//finish = clock();
	//double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//printf("Duration: %lf ms\n", duration*1000);
	printf("ackermann(3, 8) = %d\n", res);
	return 0;
}
