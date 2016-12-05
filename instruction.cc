#include "instruction.h"

Instruction::Instruction(unsigned int ins)
{
	content = ins;
	opcode = ins & 127;
	rd = (ins >> 7) & 31;
	rs1 = (ins >> 15) & 31;
	rs2 = (ins >> 20) & 31;
	rs3 = (ins >> 27) & 31;
	funct3 = (ins >> 12) & 7;
	funct7 = (ins >> 25) & 127;
	funct5 = (ins >> 27) & 31;
	funct12 = (ins >> 20) & ((1 << 12) - 1);
	shamt = rs2;
}

Instruction::~Instruction()
{
	
}