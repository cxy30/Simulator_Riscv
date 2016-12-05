#ifndef _INSTRUCTION_H
#define _INSTRUCTION_H

enum InstructionType {R, I, S, SB, U, UJ};

class Instruction
{
public:
	Instruction(unsigned int ins);
	~Instruction();

	unsigned int content;
	//InstructionType type;
	unsigned int opcode;
	unsigned int rd;
	unsigned int rs1;
	unsigned int rs2;
	unsigned int rs3;
	unsigned int funct3;
	unsigned int funct7;
	unsigned int funct5;
	unsigned int funct12;
	//unsigned int imm;
	unsigned int shamt;
};

#endif