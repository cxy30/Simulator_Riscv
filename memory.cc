#include "memory.h"
#include <memory.h>

Memory::Memory(char* filename, bool simulate_)
{
	simulate = simulate_;
	memset(simumem, 0, MEMSIZE);
	if(!filename)
		return;
	inisp=MEMSIZE-8;
	char buff_ehdr[70];
	Elf64_Ehdr* p_ehdr;
	FILE* fd=fopen(filename,"rb");
	fread(buff_ehdr,1,64,fd);
	p_ehdr=(Elf64_Ehdr*)buff_ehdr;//pointer to elf header;
	entry = p_ehdr->e_entry;// entry address
	Elf64_Off	prooff=p_ehdr->e_phoff;
	Elf64_Half	pronum=p_ehdr->e_phnum;//56
	fseek(fd,prooff,SEEK_SET);
	for(int i=0;i<pronum;i++)
	{
		char buff_phdr[60];
		Elf64_Phdr* p_phdr;
		fread(buff_phdr,1,56,fd);
		p_phdr=(Elf64_Phdr*)buff_phdr;
		if(p_phdr->p_type==PT_LOAD)
		{
			Elf64_Addr SegAd=p_phdr->p_vaddr;
			Elf64_Off SegOff=p_phdr->p_offset;
			Elf64_Xword SegSize=p_phdr->p_memsz;
			fseek(fd,SegOff,SEEK_SET);
			fread(simumem+SegAd,1,SegSize,fd);
		}
	}
	fclose(fd);


    stats_.access_counter = 0;
    stats_.miss_num = 0;
    stats_.access_time = 0;
    stats_.replace_num = 0;
    stats_.fetch_num = 0;
    stats_.prefetch_num = 0;
}
void Memory::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, bool prefetch) {
	if (prefetch == false){
  		stats_.access_counter ++;
  		stats_.access_time += latency_.hit_latency + latency_.bus_latency;
  	}
  	if (simulate){
	  	//printf("memory addr: %llx\n", addr);
	  	if(read==1)
	  	{
	  		memcpy(content,simumem+addr,bytes);
	  	}
	  	else
	  	{
	  		memcpy(simumem+addr,content,bytes);
	  	}
  	}
}

