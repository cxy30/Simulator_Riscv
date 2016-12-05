#define SYS_write 64
#define SYS_read 63
#define SYS_gettimeofday 169
#define SYS_exit 93
#define SYS_close 57
#define SYS_fstat 80
#define SYS_brk 214
#define SYS_lseek 62

#include "memory_monitor.h"
#include "register_file.h"
#include "machine.h"

extern void syscall(lint reg10, lint reg11, lint reg12, lint reg13, lint reg17, MemoryMonitor *memory, RegisterFile *file, Machine *machine);