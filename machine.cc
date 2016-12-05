#include "machine.h"
#include "alias.h"
#include "syscall.h"
//#include "param.h"
#include <stdio.h>
#include <map>
#include <string>

char *regTable[32] = {"zr", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0",
	 "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5",
	 "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

char *floatTable[32] = {"ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7",
	"fs0", "fs1", "fa0", "fa1", "fa2", "fa3", "fa4", "fa5", "fa6", "fa7", "fs2", 
	"fs3", "fs4", "fs5", "fs6", "fs7", "fs8", "fs9", "fs10", "fs11", "ft8",
	"ft9", "ft10", "ft11"};



void Machine::addIns(char *name)
{
	std::string n = name;
	mp[n] ++;
}

void Machine::Evaluate()
{
	if(evaluate)
	{
		printf("Total instruction number: %d\nThe instructions that used in this program are listed below:\n", sumIns);
		printf("Ins\tNum\tProp\n");
		std::map<std::string, unsigned int>::iterator it;
		for(it = mp.begin(); it != mp.end(); it ++)
		{
			printf("%s:\t%d\t%.2lf%%\n", (it->first).c_str(), it->second, (double)(it->second) / sumIns * 100);
		}

	}
}


Machine::Machine(char *filename)
{
	//printf("a1 %s\n", filename);
	registerFile = new RegisterFile();
	memory = new MemoryMonitor(filename);
	//printf("a2\n");
	instruction = NULL;
	registerFile->setPC(memory->entry);
	registerFile->setInteger(SP, memory->inisp);
	state = Running;
	verbose = false;
	debug = false;
	evaluate = false;
	sumIns = 0;
	mp.clear();
}

Machine::~Machine()
{

}

void Machine::setPC(lint newPC)
{
	registerFile->setPC(newPC);
}

void Machine::Run()
{
	//printf("run\n");
	while(state == Running)
	{
		//printf("Run: %lx\n", registerFile->getInteger(8));
		if(debug) PrintReg();//printf("O");
		//printf("O");
		Fetch();//printf("F");
		Decode();//printf("D");
		Execute();//printf("E");
		MemoryAccess();//printf("M");
		WriteBack();//printf("W");
		registerFile->setPC(registerFile->getPC()+4);//printf("S\n");
		sumIns ++;
		//if(verbose) printf("end ins\n");
	}
}

void Machine::Fetch()
{
	if(instruction != NULL)
	{
		delete instruction;
	}
	lint address = registerFile->getPC();
	//registerFile->setPC((lint)address + 4);
	unsigned int *ins = (unsigned int*)memory->Load(address, 4);
	instruction = new Instruction(*ins);
	//printf("ins: %u\n", instruction->opcode);
	//printf("Fetch: %lu\n", registerFile->getInteger(8));
	delete ins;
}

void Machine::Decode()
{
	lint imms, immu;
	switch(instruction->opcode)
	{
	case OP_IMM:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = WWords;
		rs1 = instruction->rs1;
		val1 = registerFile->getInteger(rs1);
		rd = instruction->rd;
		aluSrc = Imm;
		imms = (long long)((int)instruction->content >> 20);
		immu = (instruction->content >> 20);
		switch(instruction->funct3)
		{
		case ADDI:
			aluFun = Add;
			imm = imms;
			if(verbose) printf("addi $%s $%s %ld\n", regTable[rd], regTable[rs1], imm);
			addIns("addi");
			break;
		case SLTI:
			aluFun = Less;
			imm = imms;
			if(verbose) printf("slti $%s $%s %ld\n", regTable[rd], regTable[rs1], imm);
			addIns("slti");
			break;
		case SLTIU:
			aluFun = Lessu;
			imm = immu;
			if(verbose) printf("sltiu $%s $%s %lu\n", regTable[rd], regTable[rs1], imm);
			addIns("sltiu");
			break;
		case ANDI:
			aluFun = And;
			imm = immu;
			if(verbose) printf("andi $%s $%s %lu\n", regTable[rd], regTable[rs1], imm);
			addIns("andi");
			break;
		case ORI:
			aluFun = Or;
			imm = immu;
			if(verbose) printf("ori $%s $%s %lu\n", regTable[rd], regTable[rs1], imm);
			addIns("ori");
			break;
		case XORI:
			aluFun = Xor;
			imm = immu;
			if(verbose) printf("xori $%s $%s %lu\n", regTable[rd], regTable[rs1], imm);
			addIns("xori");
			break;
		case 1:
			switch(((instruction->content) >> 26))
			{
			case SLLI:
				aluFun = Shl;
				shamt = (((instruction->content) >> 20) & 63);
				if(verbose) printf("slli $%s $%s %u\n", regTable[rd], regTable[rs1], shamt);
				addIns("slli");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 5:
			switch(((instruction->content) >> 26))
			{
			case SRLI:
				aluFun = Shrl;
				shamt = (((instruction->content) >> 20) & 63);
				if(verbose) printf("srli $%s $%s %u\n", regTable[rd], regTable[rs1], shamt);
				addIns("srli");
				break;
			case SRAI:
				aluFun = Shra;
				shamt = (((instruction->content) >> 20) & 63);
				if(verbose) printf("srai $%s $%s %u\n", regTable[rd], regTable[rs1], shamt);
				addIns("srai");
				break;
			default:
				BadCode();
				break;
			}
			break;
		default:
			BadCode();
			break;
		}
		break;
	case LUI:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = WWords;
		aluFun = ANull;
		imm = (instruction->content & (~((1 << 12) - 1)));
		imm = (long long)((int)((unsigned int)imm));
		vald = imm;
		rd = instruction->rd;
		if(verbose) printf("lui $%s %lu\n", regTable[rd], imm);
		addIns("lui");
		break;
	case AUIPC:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = WWords;
		aluFun = Add;
		imm = (instruction->content & (~((1 << 12) - 1)));
		imm = (long long)((int)((unsigned int)imm));
		rd = instruction->rd;
		aluSrc = Imm;
		val1 = registerFile->getPC();
		if(verbose) printf("auipc $%s %lu\n", regTable[rd], imm);
		addIns("aupic");
		break;
	case OP:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = WWords;
		rs1 = instruction->rs1;
		val1 = registerFile->getInteger(rs1);
		rs2 = instruction->rs2;
		val2 = registerFile->getInteger(rs2);
		rd = instruction->rd;
		aluSrc = Rs2;
		switch(instruction->funct3)
		{
		case 0:
			switch(instruction->funct7)
			{
			case ADD:
				aluFun = Add;
				if(verbose) printf("add $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("add");
				break;
			case SUB:
				aluFun = Sub;
				if(verbose) printf("sub $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("sub");
				break;
			case MUL:
				aluFun = Mul;
				if(verbose) printf("mul $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("mul");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 1:
			switch(instruction->funct7)
			{
			case SLL:
				aluFun = Shl;
				shamt = (val2 & 63);
				if(verbose) printf("sll $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("sll");
				break;
			case MULH:
				aluFun = Mulh;
				if(verbose) printf("mulh $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("mulh");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 2:
			switch(instruction->funct7)
			{
			case SLT:
				aluFun = Less;
				if(verbose) printf("slt $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("slt");
				break;
			case MULHSU:
				aluFun = Mulhsu;
				if(verbose) printf("mulhsu $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("mulhsu");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 3:
			switch(instruction->funct7)
			{
			case SLTU:
				aluFun = Lessu;
				if(verbose) printf("sltu $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("sltu");
				break;
			case MULHU:
				aluFun = Mulhu;
				if(verbose) printf("mulhu $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("mulhu");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 4:
			switch(instruction->funct7)
			{
			case XOR:
				aluFun = Xor;
				if(verbose) printf("xor $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("xor");
				break;
			case DIV:
				aluFun = Div;
				if(verbose) printf("div $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("div");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 5:
			switch(instruction->funct7)
			{
			case SRL:
				aluFun = Shrl;
				shamt = (val2 & 63);
				if(verbose) printf("srl $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("srl");
				break;
			case SRA:
				aluFun = Shra;
				shamt = (val2 & 63);
				if(verbose) printf("sra $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("sra");
				break;
			case DIVU:
				aluFun = Divu;
				if(verbose) printf("divu $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("divu");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 7:
			switch(instruction->funct7)
			{
			case AND:
				aluFun = And;
				if(verbose) printf("and $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("and");
				break;
			case REMU:
				aluFun = Remu;
				if(verbose) printf("remu $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("remu");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 6:
			switch(instruction->funct7)
			{
			case OR:
				aluFun = Or;
				if(verbose) printf("or $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("or");
				break;
			case REM:
				aluFun = Rem;
				if(verbose) printf("rem $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("rem");
				break;
			default:
				BadCode();
				break;
			}
			break;
		default:
			BadCode();
			break;
		}
		break;
	case JAL:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = WWords;
		aluFun = Add;
		aluSrc = Val4;
		val1 = registerFile->getPC();
		{lint content = instruction->content;
		imm = 0;
		imm += ((content >> 20) & ((1 << 11) - 2));
		imm += ((content >> 9) & (1 << 11));
		imm += (content & ((1 << 20) - (1 << 12)));
		imm += ((content >> 11) & (1 << 20));}
		imm = ((long long)imm << 43 >> 43);
		rd = instruction->rd;
		registerFile->setPC(val1 + imm - 4);
		if(verbose) printf("jal %ld $%s\n", imm, regTable[rd]);
		addIns("jal");
		break;
	case JALR:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = WWords;
		aluFun = Add;
		aluSrc = Val4;
		val1 = registerFile->getPC();
		imm = (long long)((int)instruction->content >> 20);
		rd = instruction->rd;
		rs1 = instruction->rs1;
		{lint address = imm + registerFile->getInteger(instruction->rs1);
		//address = (address & (~1));
		address = (address >> 1 << 1);
		registerFile->setPC(address - 4);}
		if(verbose) printf("jalr $%s %ld\n", regTable[rs1], imm);
		addIns("jalr");
		break;
	case BRANCH:
		memFun = MNull;
		writeFun = WNull;
		aluFun = ANull;
		imm = 0;
		{lint content = instruction->content;
		imm += ((content >> 7) & ((1 << 5) - 2));
		imm += ((content >> 20) & ((1 << 11) - (1 << 5)));
		imm += ((content << 4) & (1 << 11));
		imm += ((content >> 19) & (1 << 12));}
		imm = ((long long)imm << 51 >> 51);
		{lint nowPC = registerFile->getPC();
		rs1 = instruction->rs1;
		rs2 = instruction->rs2;
		val1 = registerFile->getInteger(instruction->rs1);
		val2 = registerFile->getInteger(instruction->rs2);
		switch(instruction->funct3)
		{
		case BEQ:
			if(val1 == val2)
			{
				registerFile->setPC(imm + nowPC - 4);
			}
			if(verbose) printf("beq $%s $%s %ld\n", regTable[rs1], regTable[rs2], imm);
			addIns("beq");
			break;
		case BNE:
			if(val1 != val2)
			{
				registerFile->setPC(imm + nowPC - 4);
			}
			if(verbose) printf("bne $%s $%s %ld\n", regTable[rs1], regTable[rs2], imm);
			addIns("bne");
			break;
		case BLT:
			if((long long)val1 < (long long)val2)
			{
				registerFile->setPC(imm + nowPC - 4);
			}
			if(verbose) printf("blt $%s $%s %ld\n", regTable[rs1], regTable[rs2], imm);
			addIns("blt");
			break;
		case BLTU:
			if(val1 < val2)
			{
				//printf("bltu: %lu %lu\n", val1, val2);
				registerFile->setPC(imm + nowPC - 4);
			}
			if(verbose) printf("bltu $%s $%s %ld\n", regTable[rs1], regTable[rs2], imm);
			addIns("bltu");
			break;
		case BGE:
			if((long long)val1 >= (long long)val2)
			{
				registerFile->setPC(imm + nowPC - 4);
			}
			if(verbose) printf("bge $%s $%s %ld\n", regTable[rs1], regTable[rs2], imm);
			addIns("bge");
			break;
		case BGEU:
			if(val1 >= val2)
			{
				registerFile->setPC(imm + nowPC - 4);
			}
			if(verbose) printf("bgeu $%s $%s %ld\n", regTable[rs1], regTable[rs2], imm);
			addIns("bgeu");
			break;
		default:
			BadCode();
			break;
		}}
		break;
	case LOAD:
		//printf("LOAD: %lu\n", registerFile->getInteger(8));
		memFun = Read;
		writeFun = WWrite;
		aluFun = Add;
		aluSrc = Imm;
		imm = (long long)((int)instruction->content >> 20);
		rs1 = instruction->rs1;
		val1 = registerFile->getInteger(rs1);
		rd = instruction->rd;
		
		switch(instruction->funct3)
		{
		case LW:
			memSize = MWords;
			writeSize = WWords;
			if(verbose) printf("lw $%s %ld($%s)\n", regTable[rd], imm, regTable[rs1]);
			addIns("lw");
			break;
		case LH:
			memSize = MHalfWords;
			writeSize = WHalfWords;
			if(verbose) printf("lh $%s %ld($%s)\n", regTable[rd], imm, regTable[rs1]);
			addIns("lh");
			break;
		case LB:
			memSize = MBytes;
			writeSize = WBytes;
			if(verbose) printf("lb $%s %ld($%s)\n", regTable[rd], imm, regTable[rs1]);
			addIns("lb");
			break;
		case LD:
			memSize = MDouble;
			writeSize = WDouble;
			if(verbose) printf("ld $%s %ld($%s)\n", regTable[rd], imm, regTable[rs1]);
			addIns("ld");
			break;
		case LWU:
			memSize = MWordsu;
			writeSize = WWords;
			if(verbose) printf("lwu $%s %ld($%s)\n", regTable[rd], imm, regTable[rs1]);
			addIns("lwu");
			break;
		case LHU:
			memSize = MHalfWordsu;
			writeSize = WHalfWords;
			if(verbose) printf("lhu $%s %ld($%s)\n", regTable[rd], imm, regTable[rs1]);
			addIns("lhu");
			break;
		case LBU:
			memSize = MBytesu;
			writeSize = WBytes;
			if(verbose) printf("lbu $%s %ld($%s)\n", regTable[rd], imm, regTable[rs1]);
			addIns("lbu");
			break;
		default:
			BadCode();
			break;
		}
		break;
	case STORE:
		memFun = MWrite;
		writeFun = WNull;
		aluFun = Add;
		aluSrc = Imm;
		//imm = (long long)((int)instruction->content >> 20);
		//imm = (imm & (~((1 << 5) - 1)));
		//imm += ((instruction->content >> 7) & ((1 << 5) - 1));
		{
			lint content = instruction->content;
			imm = 0;
			imm += ((content >> 7) & ((1 << 5) - 1));
			imm += ((content >> 20) & ((1 << 12) - (1 << 5)));
			imm = ((long long)imm << 52 >> 52);
		}
		
		rs1 = instruction->rs1;
		val1 = registerFile->getInteger(rs1);
		rs2 = instruction->rs2;
		val2 = registerFile->getInteger(rs2);
		//printf("STORE: %u\n", instruction->funct3);
		switch(instruction->funct3)
		{
		case SW:
			memSize = MWords;
			if(verbose) printf("sw $%s %ld($%s)\n", regTable[rs2], imm, regTable[rs1]);
			addIns("sw");
			break;
		case SH:
			memSize = MHalfWords;
			if(verbose) printf("sh $%s %ld($%s)\n", regTable[rs2], imm, regTable[rs1]);
			addIns("sh");
			break;
		case SB:
			memSize = MBytes;
			if(verbose) printf("sb $%s %ld($%s)\n", regTable[rs2], imm, regTable[rs1]);
			addIns("sb");
			break;
		case SD:
			memSize = MDouble;
			if(verbose) printf("sd $%s %ld($%s)\n", regTable[rs2], imm, regTable[rs1]);
			addIns("sd");
			break;
		default:
			BadCode();
			break;
		}
		break;
	case OP_IMM_32:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = WHalfWords;
		aluSrc = Imm;
		rs1 = instruction->rs1;
		val1 = registerFile->getInteger(rs1);
		val1 = (unsigned int)val1;
		rd = instruction->rd;
		imm = (long long)(((int)instruction->content >> 20));
		switch(instruction->funct3)
		{
		case ADDIW:
			aluFun = Add;
			val1 = ((long long)((int)((unsigned int)val1)));
			if(verbose) printf("addiw $%s $%s %ld\n", regTable[rd], regTable[rs1], imm);
			addIns("addiw");
			break;
		case 1:
			switch(instruction->funct7)
			{
			case SLLIW:
				aluFun = Shlw;
				shamt = ((instruction->content >> 20) & 31);
				if(verbose) printf("slliw $%s $%s %lu\n", regTable[rd], regTable[rs1], shamt);
				addIns("slliw");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 5:
			switch(instruction->funct7)
			{
			case SRLIW:
				aluFun = Shrlw;
				shamt = ((instruction->content >> 20) & 31);
				if(verbose) printf("srliw $%s $%s %lu\n", regTable[rd], regTable[rs1], shamt);
				addIns("srliw");
				break;
			case SRAIW:
				aluFun = Shraw;
				shamt = ((instruction->content >> 20) & 31);
				if(verbose) printf("sraiw $%s $%s %lu\n", regTable[rd], regTable[rs1], shamt);
				addIns("sraiw");
				break;
			default:
				BadCode();
				break;
			}
			break;
		default:
			BadCode();
			break;
		}
		break;
	case OP_32:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = WHalfWords;
		rs1 = instruction->rs1;
		rs2 = instruction->rs2;
		rd = instruction->rd;
		val1 = registerFile->getInteger(rs1);
		val2 = registerFile->getInteger(rs2);
		val1 = (unsigned int)val1;
		val2 = (unsigned int)val2;
		aluSrc = Rs2;
		switch(instruction->funct3)
		{
		case 0:
			switch(instruction->funct7)
			{
			case ADDW:
				aluFun = Add;
				val1 = ((long long)((int)((unsigned int)val1)));
				val2 = ((long long)((int)((unsigned int)val2)));
				if(verbose) printf("addw $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("addw");
				break;
			case SUBW:
				aluFun = Sub;
				val1 = ((long long)((int)((unsigned int)val1)));
				val2 = ((long long)((int)((unsigned int)val2)));
				if(verbose) printf("subw $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("subw");
				break;
			case MULW:
				aluFun = Mul;
				val1 = ((long long)((int)((unsigned int)val1)));
				val2 = ((long long)((int)((unsigned int)val2)));
				if(verbose) printf("mulw $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("mulw");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 1:
			switch(instruction->funct7)
			{
			case SLLW:
				aluFun = Shlw;
				shamt = (val2 & 31);
				if(verbose) printf("sllw $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("sllw");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 4:
			switch(instruction->funct7)
			{
			case DIVW:
				aluFun = Div;
				val1 = ((long long)((int)((unsigned int)val1)));
				val2 = ((long long)((int)((unsigned int)val2)));
				if(verbose) printf("divw $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("divw");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 5:
			switch(instruction->funct7)
			{
			case SRLW:
				aluFun = Shrlw;
				shamt = (val2 & 31);
				if(verbose) printf("srlw $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("srlw");
				break;
			case SRAW:
				aluFun = Shraw;
				shamt = (val2 & 31);
				if(verbose) printf("sraw $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("sraw");
				break;
			case DIVUW:
				aluFun = Divu;
				val1 = ((long long)((int)((unsigned int)val1)));
				val2 = ((long long)((int)((unsigned int)val2)));
				if(verbose) printf("divuw $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("divuw");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 6:
			switch(instruction->funct7)
			{
			case REMW:
				aluFun = Rem;
				val1 = ((long long)((int)((unsigned int)val1)));
				val2 = ((long long)((int)((unsigned int)val2)));
				if(verbose) printf("remw $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("remw");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 7:
			switch(instruction->funct7)
			{
			case REMUW:
				aluFun = Remu;
				if(verbose) printf("remuw $%s $%s $%s\n", regTable[rd], regTable[rs1], regTable[rs2]);
				addIns("remuw");
				break;
			default:
				BadCode();
				break;
			}
			break;
		default:
			BadCode();
			break;
		}
		break;
	case LOAD_FP:
		memFun = Read;
		aluFun = Add;
		aluSrc = Imm;
		writeFun = WWrite;
		rs1 = instruction->rs1;
		val1 = registerFile->getInteger(rs1);
		rd = instruction->rd;
		imm = (long long)((int)instruction->content >> 20);
		switch(instruction->funct3)
		{
		case FLW:
			memSize = MWords;
			writeSize = FWords;
			if(verbose) printf("flw $%s %ld($%s)\n", floatTable[rd], imm, regTable[rs1]);
			addIns("flw");
			break;
		case FLD:
			memSize = MDouble;
			writeSize = FDouble;
			if(verbose) printf("fld $%s %ld($%s)\n", floatTable[rd], imm, regTable[rs1]);
			addIns("fld");
			break;
		default:
			BadCode();
			break;
		}
		break;
	case STORE_FP:
		memFun = MWrite;
		aluFun = Add;
		aluSrc = Imm;
		writeFun = WNull;
		rs1 = instruction->rs1;
		val1 = registerFile->getInteger(rs1);
		rs2 = instruction->rs2;
		val2 = registerFile->getFloat(rs2);
		imm = ((instruction->content >> 7) & 31);
		imm += ((instruction->content >> 20) & ((1 << 12) - (1 << 5)));
		imm = ((long long)imm << 52 >> 52);
		switch(instruction->funct3)
		{
		case FSW:
			memSize = MWords;
			if(verbose) printf("fsw $%s %ld($%s)\n", floatTable[rs2], imm, regTable[rs1]);
			addIns("fsw");
			break;
		case FSD:
			memSize = MDouble;
			if(verbose) printf("fsd $%s %ld($%s)\n", floatTable[rs2], imm, regTable[rs1]);
			addIns("fsd");
			break;
		default:
			BadCode();
			break;
		}
		break;
	case OP_FP:
		memFun = MNull;
		aluSrc = Rs2;
		writeFun = WWrite;
		writeSize = FDouble;
		rs1 = instruction->rs1;
		rs2 = instruction->rs2;
		rd = instruction->rd;
		val1 = registerFile->getFloat(rs1);
		val2 = registerFile->getFloat(rs2);
		switch(instruction->funct7)
		{
		case FADD_D:
			aluFun = Fadd;
			if(verbose) printf("fadd.d $%s $%s $%s\n", floatTable[rd], floatTable[rs1], regTable[rs2]);
			addIns("fadd.d");
			break;
		case FSUB_D:
			aluFun = Fsub;
			if(verbose) printf("fsub.d $%s $%s $%s\n", floatTable[rd], floatTable[rs1], regTable[rs2]);
			addIns("fsub.d");
			break;
		case FMUL_D:
			aluFun = Fmul;
			if(verbose) printf("fmul.d $%s $%s $%s\n", floatTable[rd], floatTable[rs1], regTable[rs2]);
			addIns("fmul.d");
			break;
		case FDIV_D:
			aluFun = Fdiv;
			if(verbose) printf("fdiv.d $%s $%s $%s\n", floatTable[rd], floatTable[rs1], regTable[rs2]);
			addIns("fdiv.d");
			break;
		case FSQRT_D:
			aluFun = Fsqrt;
			if(verbose) printf("fsqrt.d $%s $%s\n", floatTable[rd], floatTable[rs1]);
			addIns("fsqrt.d");
			break;
		case FMUL_S:
			aluFun = Fmuls;
			if(verbose) printf("fmul.s $%s $%s $%s\n", floatTable[rd], floatTable[rs1], regTable[rs2]);
			addIns("fmul.s");
			break;
		case FDIV_S:
			aluFun = Fdivs;
			if(verbose) printf("fdiv.s $%s $%s $%s\n", floatTable[rd], floatTable[rs1], regTable[rs2]);
			addIns("fdiv.s");
			break;
		case 96:
			switch(instruction->rs2)
			{
			case 0:
				aluFun = Fcvtws;
				writeSize = WWords;
				if(verbose) printf("fcvt.w.s $%s $%s\n", regTable[rd], floatTable[rs1]);
				addIns("fcvt.w.s");
				break;
			case 1:
				aluFun = Fcvtwus;
				writeSize = WWords;
				if(verbose) printf("fcvt.wu.s $%s $%s\n", regTable[rd], floatTable[rs1]);
				addIns("fcvt.wu.s");
				break;
			case 2:
				aluFun = Fcvtls;
				writeSize = WDouble;
				if(verbose) printf("fcvt.l.s $%s $%s\n", regTable[rd], floatTable[rs1]);
				addIns("fcvt.l.s");
				break;
			case 3:
				aluFun = Fcvtlus;
				writeSize = WDouble;
				if(verbose) printf("fcvt.lu.s $%s $%s\n", regTable[rd], floatTable[rs1]);
				addIns("fcvt.lu.s");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 97:
			switch(instruction->rs2)
			{
			case 0:
				aluFun = Fcvtwd;
				writeSize = WDouble;
				if(verbose) printf("fcvt.w.d $%s $%s\n", regTable[rd], floatTable[rs1]);
				addIns("fcvt.w.d");
				break;
			case 1:
				aluFun = Fcvtwud;
				writeSize = WDouble;
				if(verbose) printf("fcvt.wu.d $%s $%s\n", regTable[rd], floatTable[rs1]);
				addIns("fcvt.wu.d");
				break;
			case 2:
				aluFun = Fcvtld;
				writeSize = WDouble;
				if(verbose) printf("fcvt.l.d $%s $%s\n", regTable[rd], floatTable[rs1]);
				addIns("fcvt.l.d");
				break;
			case 3:
				aluFun = Fcvtlud;
				writeSize = WDouble;
				if(verbose) printf("fcvt.lu.d $%s $%s\n", regTable[rd], floatTable[rs1]);
				addIns("fcvt.lu.d");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 104:
			switch(instruction->rs2)
			{
			case 0:
				aluFun = Fcvtsw;
				val1 = registerFile->getInteger(rs1);
				val1 = (long long)((int)((unsigned int)val1));
				if(verbose) printf("fcvt.s.w $%s $%s\n", floatTable[rd], regTable[rs1]);
				addIns("fcvt.s.w");
				break;
			case 1:
				aluFun = Fcvtswu;
				val1 = registerFile->getInteger(rs1);
				val1 = (unsigned int)val1;
				if(verbose) printf("fcvt.s.wu $%s $%s\n", floatTable[rd], regTable[rs1]);
				addIns("fcvt.s.wu");
				break;
			case 2:
				aluFun = Fcvtsl;
				val1 = registerFile->getInteger(rs1);
				if(verbose) printf("fcvt.s.l $%s $%s\n", floatTable[rd], regTable[rs1]);
				addIns("fcvt.s.l");
				break;
			case 3:
				aluFun = Fcvtslu;
				val1 = registerFile->getInteger(rs1);
				if(verbose) printf("fcvt.s.lu $%s $%s\n", floatTable[rd], regTable[rs1]);
				addIns("fcvt.s.lu");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 105:
			switch(instruction->rs2)
			{
			case 0:
				aluFun = Fcvtdw;
				val1 = registerFile->getInteger(rs1);
				val1 = (long long)((int)((unsigned int)val1));
				if(verbose) printf("fcvt.d.w $%s $%s\n", floatTable[rd], regTable[rs1]);
				addIns("fcvt.d.w");
				break;
			case 1:
				aluFun = Fcvtdwu;
				val1 = registerFile->getInteger(rs1);
				val1 = (unsigned int)val1;
				if(verbose) printf("fcvt.d.wu $%s $%s\n", floatTable[rd], regTable[rs1]);
				addIns("fcvt.d.wu");
				break;
			case 2:
				aluFun = Fcvtdl;
				val1 = registerFile->getInteger(rs1);
				if(verbose) printf("fcvt.d.l $%s $%s\n", floatTable[rd], regTable[rs1]);
				addIns("fcvt.d.l");
				break;
			case 3:
				aluFun = Fcvtdlu;
				val1 = registerFile->getInteger(rs1);
				if(verbose) printf("fcvt.d.lu $%s $%s\n", floatTable[rd], regTable[rs1]);
				addIns("fcvt.d.lu");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case FMV_X_D:
			aluFun = ANull;
			vald = val1;
			writeSize = WDouble;
			if(verbose) printf("fmv.x.d $%s $%s\n", regTable[rd], floatTable[rs1]);
			addIns("fmv.x.d");
			break;
		case FMV_D_X:
			aluFun = ANull;
			vald = registerFile->getInteger(rs1);
			if(verbose) printf("fmv.d.x $%s $%s\n", floatTable[rd], regTable[rs1]);
			addIns("fmv.d.x");
			break;
		case FCVT_S_D:
			aluFun = Fcvtsd;
			if(verbose) printf("fcvt.s.d $%s $%s\n", floatTable[rd], floatTable[rs1]);
			addIns("fcvt.s.d");
			break;
		case FCVT_D_S:
			aluFun = Fcvtds;
			if(verbose) printf("fcvt.d.s $%s $%s\n", floatTable[rd], floatTable[rs1]);
			addIns("fcvt.d.s");
			break;
		case 17:
			switch(instruction->funct3)
			{
			case 0:
				aluFun = Fsgnjd;
				if(verbose) printf("fsgnj.d $%s $%s $%s\n", floatTable[rd], floatTable[rs1], floatTable[rs2]);
				addIns("fsgnj.d");
				break;
			case 1:
				aluFun = Fsgnjnd;
				if(verbose) printf("fsgnjn.d $%s $%s $%s\n", floatTable[rd], floatTable[rs1], floatTable[rs2]);
				addIns("fsgnjn.d");
				break;
			case 2:
				aluFun = Fsgnjxd;
				if(verbose) printf("fsgnjx.d $%s $%s $%s\n", floatTable[rd], floatTable[rs1], floatTable[rs2]);
				addIns("fsgnjx.d");
				break;
			default:
				BadCode();
				break;
			}
			break;
		case 81:
			switch(instruction->funct3)
			{
			case 2:
				aluFun = Feqd;
				writeSize = WDouble;
				if(verbose) printf("feq.d $%s $%s $%s\n", regTable[rd], floatTable[rs1], floatTable[rs2]);
				addIns("feq.d");
				break;
			case 1:
				aluFun = Fltd;
				writeSize = WDouble;
				if(verbose) printf("flt.d $%s $%s $%s\n", regTable[rd], floatTable[rs1], floatTable[rs2]);
				addIns("flt.d");
				break;
			case 0:
				aluFun = Fled;
				writeSize = WDouble;
				if(verbose) printf("fle.d $%s $%s $%s\n", regTable[rd], floatTable[rs1], floatTable[rs2]);
				addIns("fle.d");
				break;
			default:
				BadCode();
				break;
			}
			break;
		default:
			BadCode();
			break;
		}
		break;
	case MADD:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = FDouble;
		rs1 = instruction->rs1;
		rs2 = instruction->rs2;
		val1 = registerFile->getFloat(rs1);
		val2 = registerFile->getFloat(rs2);
		rd = instruction->rd;
		switch(((instruction->content) >> 25) & 3)
		{
		case 1:
			aluFun = Fmaddd;
			if(verbose) printf("fmadd.d $%s $%s $%s\n", regTable[rd], floatTable[rs1], floatTable[rs2]);
			addIns("fmadd.d");
			break;
		default:
			BadCode();
			break;
		}
		break;
	case MSUB:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = FDouble;
		rs1 = instruction->rs1;
		rs2 = instruction->rs2;
		val1 = registerFile->getFloat(rs1);
		val2 = registerFile->getFloat(rs2);
		rd = instruction->rd;
		switch(((instruction->content) >> 25) & 3)
		{
		case 1:
			aluFun = Fmsubd;
			if(verbose) printf("fmsub.d $%s $%s $%s\n", regTable[rd], floatTable[rs1], floatTable[rs2]);
			addIns("fmsub.d");
			break;
		default:
			BadCode();
			break;
		}
		break;
	case NMADD:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = FDouble;
		rs1 = instruction->rs1;
		rs2 = instruction->rs2;
		val1 = registerFile->getFloat(rs1);
		val2 = registerFile->getFloat(rs2);
		rd = instruction->rd;
		switch(((instruction->content) >> 25) & 3)
		{
		case 1:
			aluFun = Fnmaddd;
			if(verbose) printf("nfmadd.d $%s $%s $%s\n", regTable[rd], floatTable[rs1], floatTable[rs2]);
			addIns("nfmadd.d");
			break;
		default:
			BadCode();
			break;
		}
		break;
	case NMSUB:
		memFun = MNull;
		writeFun = WWrite;
		writeSize = FDouble;
		rs1 = instruction->rs1;
		rs2 = instruction->rs2;
		val1 = registerFile->getFloat(rs1);
		val2 = registerFile->getFloat(rs2);
		rd = instruction->rd;
		switch(((instruction->content) >> 25) & 3)
		{
		case 1:
			aluFun = Fnmsubd;
			if(verbose) printf("nfmsub.d $%s $%s $%s\n", regTable[rd], floatTable[rs1], floatTable[rs2]);
			addIns("nfmsub.d");
			break;
		default:
			BadCode();
			break;
		}
		break;
	case SYSTEM:
		memFun = MNull;
		aluFun = ANull;
		writeFun = WNull;
		switch(instruction->funct3)
		{
		case PRIV:
			switch(instruction->funct12)
			{
			case ECALL:
				if(verbose) printf("ecall\n");
				{lint v10 = registerFile->getInteger(10);
				lint v11 = registerFile->getInteger(11);
				lint v12 = registerFile->getInteger(12);
				lint v13 = registerFile->getInteger(13);
				lint v17 = registerFile->getInteger(17);
				syscall(v10, v11, v12, v13, v17, memory, registerFile, this);}
				addIns("ecall");
				break;
			case EBREAK:
				if(verbose) printf("ebreak\n");
				addIns("ebreak");
				break;
			default:
				BadCode();
				break;
			}
			break;
		default:
			BadCode();
			break;
		}
		break;
	default:
		BadCode();
		break;
	}
}

void Machine::Execute()
{
	//if(verbose) printf("Execute\n");
	if(aluFun == ANull)
		return;

	lint v1 = val1;
	lint v2 = val2;
	
	switch(aluSrc)
	{
	case Rs2:
		v2 = val2;
		break;
	case Imm:
		v2 = imm;
		break;
	case Val4:
		v2 = 4;
		break;
	}

	//printf("Execute: %lu %lu\n", v1, v2);

	int neg;
	double f1 = *(double *)(&v1);
	double f2 = *(double *)(&v2);
	lint v3 = registerFile->getFloat(instruction->rs3);
	double f3 = *(double *)(&v3);
	double fd;
	float a1 = *(float *)(&v1);
	float a2 = *(float *)(&v2);
	float ad;

	switch(aluFun)
	{
	case Add:
		vald = v1 + v2;
		//printf("vald: %lu\n", vald);
		break;
	case Sub:
		vald = v1 - v2;
		break;
	case Mul:
		vald = (long long)v1 * (long long)v2;
		break;
	case Div:
		vald = (long long)v1 / (long long)v2;
		break;
	case Fadd:
		fd = f1 + f2;
		vald = *(lint *)(&fd);
		break;
	case Fsub:
		fd = f1 - f2;
		vald = *(lint *)(&fd);
		break;
	case Fmul:
		fd = f1 * f2;
		vald = *(lint *)(&fd);
		break;
	case Fdiv:
		fd = f1 / f2;
		vald = *(lint *)(&fd);
		break;
	case Fsqrt:
		fd = sqrt(f1);
		vald = *(lint *)(&fd);
		break;
	case And:
		vald = v1 & v2;
		break;
	case Or:
		vald = v1 | v2;
		break;
	case Xor:
		vald = v1 ^ v2;
		break;
	case Less:
		vald = ((long long)v1 < (long long)v2) ? 1 : 0;
		break;
	case Lessu:
		vald = (v1 < v2) ? 1 : 0;
		break;
	case Shl:
		vald = (v1 << shamt);
		break;
	case Shrl:
		vald = (v1 >> shamt);
		break;
	case Shra:
		vald = ((long long)v1 >> shamt);
		break;
	case Shlw:
		{unsigned int s1 = v1;
		s1 = (s1 << shamt);
		vald = (long long)((int)s1);}
		break;
	case Shrlw:
		{unsigned int s1 = v1;
		s1 = (s1 >> shamt);
		vald = (long long)((int)s1);}
		break;
	case Shraw:
		{int s1 = (int)((unsigned int)v1);
		s1 = (s1 >> shamt);
		vald = (long long)s1;}
		break;
	case Mulh:
		{neg = 1;
		if((long long)v1 < 0) 
		{
			neg = -neg;
			v1 = -v1;
		}
		if((long long)v2 < 0) 
		{
			neg = -neg;
			v2 = -v2;
		}
		unsigned int x1 = v1;
		unsigned int x2 = (v1 >> 32);
		unsigned int y1 = v2;
		unsigned int y2 = (v2 >> 32);
		unsigned int z1 = 0, z2 = 0, z3 = 0, z4 = 0;
		lint tmp = (lint)x1 * (lint)y1;
		z1 += tmp;
		z2 += (tmp >> 32);
		tmp = (lint)x1 * (lint)y2;
		if(z2 + (unsigned int)tmp < z2)
		{
			z3 ++;
		}
		z2 += tmp;
		if(z3 + (tmp >> 32) < z3)
		{
			z4 ++;
		}
		z3 += (tmp >> 32);
		tmp = (lint)x2 * (lint)y1;
		if(z2 + (unsigned int)tmp < z2)
		{
			z3 ++;
		}
		z2 += tmp;
		if(z3 + (tmp >> 32) < z3)
		{
			z4 ++;
		}
		z3 += (tmp >> 32);
		tmp = (lint)x2 * (lint)y2;
		if(z3 + (unsigned int)tmp < z3)
		{
			z4 ++;
		}
		z3 += tmp;
		z4 += (tmp >> 32);
		vald = z3 + ((lint)z4 << 32);
		vald = (long long)vald * neg;}
		break;
	case Mulhu:
		{unsigned int x1 = v1;
		unsigned int x2 = (v1 >> 32);
		unsigned int y1 = v2;
		unsigned int y2 = (v2 >> 32);
		unsigned int z1 = 0, z2 = 0, z3 = 0, z4 = 0;
		lint tmp = (lint)x1 * (lint)y1;
		z1 += tmp;
		z2 += (tmp >> 32);
		tmp = (lint)x1 * (lint)y2;
		if(z2 + (unsigned int)tmp < z2)
		{
			z3 ++;
		}
		z2 += tmp;
		if(z3 + (tmp >> 32) < z3)
		{
			z4 ++;
		}
		z3 += (tmp >> 32);
		tmp = (lint)x2 * (lint)y1;
		if(z2 + (unsigned int)tmp < z2)
		{
			z3 ++;
		}
		z2 += tmp;
		if(z3 + (tmp >> 32) < z3)
		{
			z4 ++;
		}
		z3 += (tmp >> 32);
		tmp = (lint)x2 * (lint)y2;
		if(z3 + (unsigned int)tmp < z3)
		{
			z4 ++;
		}
		z3 += tmp;
		z4 += (tmp >> 32);
		vald = z3 + ((lint)z4 << 32);}
		break;
	case Mulhsu:
		{neg = 1;
		if((long long)v1 < 0)
		{
			neg = -neg;
			v1 = -v1;
		}
		unsigned int x1 = v1;
		unsigned int x2 = (v1 >> 32);
		unsigned int y1 = v2;
		unsigned int y2 = (v2 >> 32);
		unsigned int z1 = 0, z2 = 0, z3 = 0, z4 = 0;
		lint tmp = (lint)x1 * (lint)y1;
		z1 += tmp;
		z2 += (tmp >> 32);
		tmp = (lint)x1 * (lint)y2;
		if(z2 + (unsigned int)tmp < z2)
		{
			z3 ++;
		}
		z2 += tmp;
		if(z3 + (tmp >> 32) < z3)
		{
			z4 ++;
		}
		z3 += (tmp >> 32);
		tmp = (lint)x2 * (lint)y1;
		if(z2 + (unsigned int)tmp < z2)
		{
			z3 ++;
		}
		z2 += tmp;
		if(z3 + (tmp >> 32) < z3)
		{
			z4 ++;
		}
		z3 += (tmp >> 32);
		tmp = (lint)x2 * (lint)y2;
		if(z3 + (unsigned int)tmp < z3)
		{
			z4 ++;
		}
		z3 += tmp;
		z4 += (tmp >> 32);
		vald = z3 + ((lint)z4 << 32);
		vald = (long long)vald * neg;}
		break;
	case Divu:
		vald = v1 / v2;
		break;
	case Rem:
		vald = (long long)v1 % (long long)v2;
		break;
	case Remu:
		vald = v1 % v2;
		break;
	case Fcvtwd:
		vald = (long long)((int)f1);
		break;
	case Fcvtwud:
		vald = (lint)((unsigned int)f1);
		break;
	case Fcvtdw:
		fd = (double)((int)v1);
		vald = *(lint *)(&fd);
		break;
	case Fcvtdwu:
		fd = (double)((unsigned int)v1);
		vald = *(lint *)(&fd);
		break;
	case Fcvtld:
		vald = (long long)f1;
		break;
	case Fcvtlud:
		vald = (lint)f1;
		break;
	case Fcvtdl:
		fd = (double)((long long)v1);
		vald = *(lint *)(&fd);
		break;
	case Fcvtdlu:
		fd = (double)v1;
		vald = *(lint *)(&fd);
		break;
	case Fcvtsd:
		ad = (float)f1;
		vald = (lint)(*(unsigned int*)(&ad));
		break;
	case Fcvtds:
		fd = (double)a1;
		vald = *(lint *)(&fd);
		break;
	case Fsgnjd:
		vald = (v1 & ((1 << 63) - 1)) + (v2 & (1 << 63));
		break;
	case Fsgnjnd:
		vald = (v1 & ((1 << 63) - 1)) + ((~v2) & (1 << 63));
		break;
	case Fsgnjxd:
		vald = (v1 & ((1 << 63) - 1)) + ((v2 & (1 << 63)) ^ (v1 & (1 << 63)));
		break;
	case Feqd:
		vald = (f1 == f2) ? 1 : 0;
		break;
	case Fltd:
		vald = (f1 < f2) ? 1 : 0;
		break;
	case Fled:
		vald = (f1 <= f2) ? 1 : 0;
		break;
	case Fmaddd:
		fd = f1*f2+f3;
		vald = *(lint *)(&fd);
		break;
	case Fmsubd:
		fd = f1*f2-f3;
		vald = *(lint *)(&fd);
		break;
	case Fnmaddd:
		fd = -(f1*f2+f3);
		vald = *(lint *)(&fd);
		break;
	case Fnmsubd:
		fd = -(f1*f2-f3);
		vald = *(lint *)(&fd);
		break;
	case Fcvtws:
		vald = (long long)((int)a1);
		break;
	case Fcvtwus:
		vald = (lint)((unsigned int)a1);
		break;
	case Fcvtls:
		vald = (long long)a1;
		break;
	case Fcvtlus:
		vald = (lint)a1;
		break;
	case Fcvtsw:
		ad = (float)((int)v1);
		vald = (lint)(*(unsigned int*)(&ad));
		break;
	case Fcvtswu:
		ad = (float)((unsigned int)v1);
		vald = (lint)(*(unsigned int*)(&ad));
		break;
	case Fcvtsl:
		ad = (float)((long long)v1);
		vald = (lint)(*(unsigned int*)(&ad));
		break;
	case Fcvtslu:
		ad = (float)v1;
		vald = (lint)(*(unsigned int*)(&ad));
		break;
	case Fmuls:
		ad = a1*a2;
		vald = (lint)(*(unsigned int*)(&ad));
		break;
	case Fdivs:
		ad = a1/a2;
		vald = (lint)(*(unsigned int*)(&ad));
		break;
	}
}

void Machine::MemoryAccess()
{
	//void *address = (void *)vald;
	//printf("MemoryAccess\n");
	if(memFun == MNull)
		return;
	void *content = NULL;
	switch(memFun)
	{
	case Read:
		switch(memSize)
		{
		case MDouble:
			//printf("Debug0 %lu\n", vald);
			content = memory->Load(vald, 8);
			//printf("Debug1\n");
			vald = (*(lint *)(content));
			break;
		case MWords:
			content = memory->Load(vald, 4);
			vald = (long long)(*(int *)(content));
			break;
		case MHalfWords:
			content = memory->Load(vald, 2);
			vald = (long long)(*(short *)(content));
			break;
		case MBytes:
			content = memory->Load(vald, 1);
			vald = (long long)(*(char *)(content));
			break;
		case MWordsu:
			content = memory->Load(vald, 4);
			vald = (*(unsigned int *)(content));
			break;
		case MHalfWordsu:
			content = memory->Load(vald, 2);
			vald = (*(unsigned short *)(content));
			break;
		case MBytesu:
			content = memory->Load(vald, 1);
			vald = (*(unsigned char *)(content));
			break;

		}
		break;
	case MWrite:
		switch(memSize)
		{
		case MDouble:
			memory->Store(vald, 8, (char*)(&val2));
			break;
		case MWords:
			memory->Store(vald, 4, (char*)(&val2));
			break;
		case MHalfWords:
			memory->Store(vald, 2, (char*)(&val2));
			break;
		case MBytes:
			memory->Store(vald, 1, (char*)(&val2));
			break;
		}
		break;
	}
	if(content != NULL) delete content;
}

void Machine::WriteBack()
{
	//printf("WriteBack\n");
	if(writeFun == WNull)
		return;

	switch(writeSize)
	{
	case WDouble:
	case WWords:
	case WHalfWords:
	case WBytes:
		if(rd != ZERO) registerFile->setInteger(rd, vald);
		break;
	case FDouble:
	case FWords:
	case FHalfWords:
	case FBytes:
		registerFile->setFloat(rd, vald);
		break;
	}
}

void Machine::BadCode()
{
	printf("It is a bad code! The machine halts!\n");
	state = Halt;
	PrintReg();
}	

void Machine::PrintReg()
{
	printf("Debug:\n");
	for(int i = 0; i < 32; i++)
	{
		printf("$%s: %lx\t", regTable[i], registerFile->getInteger(i));
		if((i+1)%4 == 0)
		{
			printf("\n");
		}
	}
	printf("$%s: %lx\n", "pc", registerFile->getPC());
	printf("Please click enter to continue:\n");
	char c = 'n';
	//scanf("%c", &c);
	if(c == 'r')
	{
		debug = false;
	}
	else if(c == 'q')
	{
		state = Halt;
	}
	else if(c == 'n')
	{
		
	}
}