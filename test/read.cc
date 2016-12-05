#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(){
	char a[20];
	read(0, a, 15);
	//printf("a: %d\n", atoi(a));
	write(1, a, 15);
}