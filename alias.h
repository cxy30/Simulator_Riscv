#ifndef _ALIAS_H
#define _ALIAS_H

/*
#define ZERO 0
#define RA 1
#define S0 2
#define FP 2
#define S1 3
#define S2 4
#define S3 5
#define S4 6
#define S5 7
#define S6 8
#define S7 9
#define S8 10
#define S9 11
#define S10 12
#define S11 13
#define SP 14
#define TP 15
#define V0 16
#define V1 17
#define A0 18
#define A1 19
#define A2 20
#define A3 21
#define A4 22
#define A5 23
#define A6 24
#define A7 25
#define T0 26
#define T1 27
#define T2 28
#define T3 29
#define T4 30
#define GP 31
#define PC 32
*/

#define ZERO 0
#define RA 1
#define SP 2
#define GP 3
#define TP 4
#define T0 5
#define T1 6
#define T2 7
#define S0 8
#define FP 8
#define S1 9
#define A0 10
#define A1 11
#define A2 12
#define A3 13
#define A4 14
#define A5 15
#define A6 16
#define A7 17
#define S2 18
#define S3 19
#define S4 20
#define S5 21
#define S6 22
#define S7 23
#define S8 24
#define S9 25
#define S10 26
#define S11 27
#define T3 28
#define T4 29
#define T5 30
#define T6 31
#define PC 32

#define FT0 0
#define FT1 1
#define FT2 2
#define FT3 3
#define FT4 4
#define FT5 5
#define FT6 6
#define FT7 7
#define FS0 8
#define FS1 9
#define FA0 10
#define FA1 11
#define FA2 12
#define FA3 13
#define FA4 14
#define FA5 15
#define FA6 16
#define FA7 17
#define FS2 18
#define FS3 19
#define FS4 20
#define FS5 21
#define FS6 22
#define FS7 23
#define FS8 24
#define FS9 25
#define FS10 26
#define FS11 27
#define FT8 28
#define FT9 29
#define FT10 30
#define FT11 31


#define LOAD 3
#define LOAD_FP 7
#define MISC_MEM 15
#define OP_IMM 19
#define AUIPC 23
#define OP_IMM_32 27
#define STORE 35
#define STORE_FP 39
#define AMO 47
#define OP 51
#define LUI 55
#define OP_32 59
#define MADD 67
#define MSUB 71
#define NMSUB 75
#define NMADD 79
#define OP_FP 83
#define BRANCH 99
#define JALR 103
#define JAL 111
#define SYSTEM 115

#define BEQ 0
#define BNE 1
#define BLT 4
#define BGE 5
#define BLTU 6
#define BGEU 7
#define LB 0
#define LH 1
#define LW 2
#define LBU 4
#define LHU 5
#define SB 0
#define SH 1
#define SW 2
#define ADDI 0
#define SLTI 2
#define SLTIU 3
#define XORI 4
#define ORI 6
#define ANDI 7

//#define SLLI 0
//#define SRLI 0
//#define SRAI 32
#define ADD 0
#define SUB 32
#define SLL 0
#define SLT 0
#define SLTU 0
#define XOR 0
#define SRL 0
#define SRA 32
#define OR 0
#define AND 0

#define ECALL 0
#define EBREAK 1



/*
#define ADDSUB 0
#define SLL 1
#define SLT 2
#define SLTU 3
#define XOR 4
#define SRX 5
#define OR 6
#define AND 7
#define FENCE 0
#define FENCE_I 1
*/

#define LWU 6
#define LD 3
#define SD 3
#define ADDIW 0

#define SLLI 0
#define SRLI 0
#define SRAI 16
#define SLLIW 0
#define SRLIW 0
#define SRAIW 32
#define ADDW 0
#define SUBW 32
#define SLLW 0
#define SRLW 0
#define SRAW 32

/*
#define SLLIW 1
#define SRXIW 5
#define ADDSUBW 0
#define SLLW 1
#define SRXW 5

#define SRLI 0
#define SRAI 32
#define ADD 0
#define SUB 32
#define SRL 0
#define SRA 32
#define ECALL 0
#define EBREAK 1
#define SRLIW 0
#define SRAIW 32
#define SRLW 0
#define SRAW 32
*/

#define MUL 1
#define MULH 1
#define MULHSU 1
#define MULHU 1
#define DIV 1
#define DIVU 1
#define REM 1
#define REMU 1

#define MULW 1
#define DIVW 1
#define DIVUW 1
#define REMW 1
#define REMUW 1

#define FLW 2
#define FSW 2

#define FLD 3
#define FSD 3
#define FMADD_D 1
#define FMSUB_D 1
#define FNMSUB_D 1
#define FNMADD_D 1
#define FADD_D 1
#define FSUB_D 5
#define FMUL_D 9
#define FDIV_D 13
#define FSQRT_D 45
#define FSGNJ_D 17
#define FSGNJN_D 17
#define FSGNJX_D 17
#define FMIN_D 21
#define FMAX_D 21
#define FCVT_S_D 32
#define FCVT_D_S 33
#define FEQ_D 81
#define FLT_D 81
#define FLE_D 81
#define FCLASS_D 113
#define FCVT_W_D 97
#define FCVT_WU_D 97
#define FCVY_D_W 105
#define FCVT_D_WU 105
#define FMV_X_D 113
#define FMV_D_X 121

#define FMUL_S 8
#define FDIV_S 12

#define PRIV 0


#endif