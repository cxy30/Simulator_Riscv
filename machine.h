#ifndef _MACHINE_H
#define _MACHINE_H

#include "register_file.h"
#include "memory_monitor.h"
#include "param.h"
#include "instruction.h"
#include <string>
#include <map>

enum MachineState {Running, Error, Halt};
enum ALUFun {ANull, Add, Sub, Mul, Div, And, Or, Xor, Less, Lessu, Shl, Shrl, Shra, Shlw, Shrlw, Shraw, 
	Mulh, Mulhu, Mulhsu, Divu, Rem, Remu, Fadd, Fsub, Fmul, Fdiv, Fsqrt, Fcvtwd, Fcvtwud, Fcvtdw, Fcvtdwu, 
	Fcvtld, Fcvtlud, Fcvtdl, Fcvtdlu, Fcvtsd, Fcvtds, Fsgnjd, Fsgnjnd, Fsgnjxd, Feqd, Fltd, Fled, Fmaddd, 
	Fmsubd, Fnmaddd, Fnmsubd, Fcvtws, Fcvtwus, Fcvtsw, Fcvtswu, Fcvtls, Fcvtlus, Fcvtsl, Fcvtslu, Fmuls, Fdivs};
enum ALUSrc {Rs2, Imm, Val4};
enum MemFun {MNull, Read, MWrite};
enum MemSize {MDouble, MWords, MHalfWords, MBytes, MWordsu, MHalfWordsu, MBytesu};
enum WriteFun {WNull, WWrite};
enum WriteSize {WDouble, WWords, WHalfWords, WBytes, FDouble, FWords, FHalfWords, FBytes};

class Machine
{
public:
	Machine(char *filename);
	~Machine();
	void Run();
	void setPC(lint newPC);

	void Fetch();
	void Decode();
	void Execute();
	void MemoryAccess();
	void WriteBack();
	void SetVerbose(bool b) {verbose = b;}
	void SetDebug(bool b) {debug = b;}
	void SetEvaluate(bool b) {evaluate = b;}
	void PrintReg();
	void addIns(char *ins);
	void Evaluate();
	
private:
	RegisterFile *registerFile;
	MemoryMonitor *memory;
	MachineState state;

	Instruction *instruction;
	
	lint rs1, rs2, rd;
	lint val1, val2, vald;
	lint imm;
	unsigned int shamt;

	ALUFun aluFun;
	ALUSrc aluSrc;
	MemFun memFun;
	MemSize memSize;
	WriteFun writeFun;
	WriteSize writeSize;

	bool verbose;
	bool debug;
	bool evaluate;
	unsigned int sumIns;

	std::map<std::string, unsigned int> mp;

	void BadCode();
};

#endif