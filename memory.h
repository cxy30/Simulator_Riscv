#ifndef CACHE_MEMORY_H_
#define CACHE_MEMORY_H_

#include <stdint.h>
#include "storage.h"
#include <elf.h>
#include <math.h> 
#define MEMSIZE 0x10000000
class Memory: public Storage {
 public:
  Memory(char* filename, bool simulate);
  ~Memory() {}

  // Main access process
  void HandleRequest(uint64_t addr, int bytes, int read,
                     char *content, int &hit, int &time);
  Elf64_Addr entry;
  Elf64_Addr inisp;
 private:
  // Memory implement
  char simumem[MEMSIZE];
  bool simulate;
  DISALLOW_COPY_AND_ASSIGN(Memory);
};

#endif //CACHE_MEMORY_H_ 
