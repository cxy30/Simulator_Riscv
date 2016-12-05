#ifndef _PARAM_H
#define _PARAM_H

#define RISCV64

#ifdef RISCV32
typedef unsigned int lint;
#endif

#ifdef RISCV64
typedef unsigned long long lint;
#endif


#endif