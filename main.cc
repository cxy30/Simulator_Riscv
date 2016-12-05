#include "machine.h"
//#include "system.h"
#include <stdio.h>
#define SIMULATE 1
int main(int argc, char **argv)
{
	if(argc < 2)
	{
		printf("No Input File!\n");
		return 1;
	}
	Machine *machine = new Machine(argv[1]);
	//machine->SetVerbose(true);
	//machine->SetDebug(true);
	//machine->SetEvaluate(true);
	machine->Run();
	printf("Run over!\n");
	return 0;
} 