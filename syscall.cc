#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include "syscall.h"
#include "memory_monitor.h"
#include "alias.h"
#include "register_file.h"
#include "machine.h"
#include <string.h>
//need include h file of load and store

void sys_exit(Machine *machine){
	printf("the program has exited successfully\n");
	machine->Evaluate();
	exit(0);
}

/*
void sys_write(lint reg10, lint reg11, lint reg12, Memory *memory){
	printf("sys_write\n");
	unsigned long long int cur_addr = reg11;
	unsigned long long int f_addr = reg12+reg11;
	char* s = (char*)memory->Load(cur_addr, reg12);
	for(unsigned long long int i = 0; i < reg12; i++)
	{

		unsigned char a = s[i];
		printf("%c", a);
	}
}*/

void sys_write(lint reg10, lint reg11, lint reg12, MemoryMonitor *memory, RegisterFile *file){
	//printf("sys_write, position: %llx\n", reg11);
	// we don't have fd
	if (reg10 != 1){
		printf("error!!! not stdout!!!\n");
		exit(0);
	}
	else{
		// need load function from Jingyue Gao
		//void* location = memory->Translate(reg11); 
		//write((int)reg10, location, 1);
		void* location = memory->Load(reg11, reg12);
		int writeNum = write((int)reg10, (char *)location, (int)reg12);
		file->setInteger(A0, writeNum);
	}
	
}

void sys_read(lint reg10, lint reg11, lint reg12, MemoryMonitor *memory, RegisterFile *file)
{
	//printf("sys_read, position: %llx\n", reg11);

	// we don't have fd
	if (reg10 != 0)
	{
		printf("error!!! not stdin!!!\n");
		exit(0);
	}
	else{
		char* readposition = (char*)malloc((int)reg12 + 5);
		int readNum = read((int)reg10, readposition, (int)reg12);
		//int readNum = read("%s", readposition);
		//printf("readposition: %s\n", readposition);
		//read((int)reg10, readposition, (int)reg12);
		// need store function from Jingyue Gao
		memory->Store(reg11, strlen(readposition) + 1, (char*)readposition);
		//void* location = memory->Load(reg11, (int)reg12);
		//printf("saved %s\n", location);
		delete readposition;
		file->setInteger(A0, readNum);
	}
	
}

void sys_gettimeofday(lint reg10, MemoryMonitor *memory){
	//printf("sys_gettimeofday\n");
	struct timeval t;
	if (gettimeofday(&t, NULL) == 0){
		memory->Store(reg10, sizeof(t), (char*)&t);
	}
	else{
		printf("gettimeofday error\n");
		exit(0);
	}
}
/*
void sys_fstat(lint reg10, lint reg11, Memory *memory, RegisterFile *file){
	//printf("sys_fstat\n");
	void* location = memory->Translate(reg11);
	int num = fstat(reg10, (struct stat *)location);
	file->setInteger(A0, num);
}

void sys_close(lint reg10, Memory *memory, RegisterFile *file){
	//printf("sys_close\n");
	int num = close(reg10);
	file->setInteger(A0, num);
}

void sys_brk(lint reg10, Memory *memory, RegisterFile *file){
	//printf("sys_brk\n");
	void* location = memory->Translate(reg10);
	int num = brk((void*)location);
	file->setInteger(A0, num);
}

void sys_lseek(lint reg10, lint reg11, lint reg12, Memory *memory,RegisterFile *file){
	//printf("sys_lseek\n");
	off_t num = lseek(reg10, reg11, reg12);
	if (reg10 != 0)
	{
		printf("error!!! not stdin!!!\n");
		exit(0);
	}
	file->setInteger(A0, num);
}*/

void syscall(lint reg10, lint reg11, lint reg12, lint reg13, lint reg17, MemoryMonitor *memory, RegisterFile *file, Machine *machine)
{
	switch(reg17){
	case SYS_exit:
		sys_exit(machine);
		break;
	case SYS_read:
		sys_read(reg10, reg11, reg12, memory, file);
		break;
	case SYS_write: 
		sys_write(reg10, reg11, reg12, memory, file);
		break;
	case SYS_gettimeofday: 
		sys_gettimeofday(reg10, memory);
		break;
	case SYS_close:
		//sys_close(reg10, memory, file);
		file->setInteger(A0, 0);
		break;
	case SYS_fstat:
		//sys_fstat(reg10, reg11, memory, file);
		file->setInteger(A0, 0);
		break;
	case SYS_brk:
		//sys_brk(reg10, memory, file);
		file->setInteger(A0, 0);
		break;
	case SYS_lseek:
		//sys_lseek(reg10, reg11, reg12, memory, file);
		file->setInteger(A0, 0);
		break;
	default:
		printf("%d syscall hasn't been defined\n", reg17);
	}
}