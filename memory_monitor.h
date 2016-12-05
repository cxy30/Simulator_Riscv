#ifndef _Memory_Monitor_H
#define _Memory_Monitor_H
#include "param.h"
#include "memory.h"
#include "cache.h"
#include <elf.h>
#include <math.h>


class MemoryMonitor
{
	public:
		MemoryMonitor(char* filename);
		~MemoryMonitor();
		void* Load(lint ad,int length);
		void Store(lint ad,int length,char*content);
		//void* Translate(lint ad);
		Elf64_Addr entry;
		Elf64_Addr inisp;
	private:
		char simumem[MEMSIZE];// virtual memory
		Memory *memory;
		Cache *l1, *l2, *llc;
};
#endif