#pragma warning( disable : 26812 )
#ifndef RV_TLM_0_GLOBALS_H
#define RV_TLM_0_GLOBALS_H
#include <stdint.h>
#include <iostream>
#include <map>

using namespace std;

#define rv_get_bit(word, index) (word >> index)&1
#define rv_set_bit(word, index, val) ((word & ~(1 << index)) | ((val&1) << index))
#define rv_get_field(word, index, mask) (word >> index)&mask
#define rv_set_field(word, index, mask, value) (word & ~(mask << index)) | (value << index)

enum OPCODES {
    LUI = 0x37,		AUIPC = 0x17,		// atribui 20 msbits
    ILType = 0x03,						// Load type
    BType = 0x63,						// branch condicional
    JAL = 0x6F,		JALR = 0x67,		// jumps
    StoreType = 0x23,					// store
    ILAType = 0x13,						// logico-aritmeticas com imediato
    RegType = 0x33,
    ECALL = 0x73
};

enum FUNCT3 {
    BEQ3=0,		BNE3=01,	BLT3=04,	BGE3=05,	BLTU3=0x06, BGEU3=07,
    LB3=0,		LH3=01,		LW3=02,		LBU3=04,	LHU3=05,
    SB3=0,		SH3=01,		SW3=02,
    ADDSUB3=0,	SLL3=01,	SLT3=02,	SLTU3=03,
    XOR3=04,	SR3=05,		OR3=06,		AND3=07,
    ADDI3=0,	ORI3=06,	SLTI3=02,	XORI3=04,	ANDI3=07,
    SLTIU3=03,	SLLI3=01,	SRI3=05
};

enum FUNCT7 {
    ADD7=0,	SUB7=0x20,	SRA7=0x20,	SRL7=0, SRLI7=0x00,	SRAI7=0x20
};

enum FORMATS { RType, IType, SType, SBType, UType, UJType, NullFormat, NOPType };

enum INSTRUCTIONS {
    I_add,	I_addi, I_and,  I_andi, I_auipc,
    I_beq,	I_bge,	I_bgeu, I_blt,  I_bltu,
    I_bne,  I_jal,	I_jalr, I_lb,	I_lbu,
    I_lw,   I_lh,   I_lhu,  I_lui,  I_sb,
    I_sh,   I_sw,   I_sll,  I_slt,  I_slli,
    I_srl,  I_sra,  I_sub,  I_slti, I_sltiu,
    I_xor,	I_or,	I_srli, I_srai,  I_sltu,
    I_ori, I_ecall, I_xori, I_nop
};


//
// identificação dos registradores do banco do RV32I
//
enum REGISTERS {
    RA=1,	SP=2,	GP=3,
   A0=10,	A7=17,
};

//
//	Memory
//
enum { MEM_SIZE = 4096 };

struct instruction_context_st {
    uint32_t ri, pc;
    INSTRUCTIONS ins_code;
    FORMATS ins_format;
    REGISTERS rs1, rs2, rd;
    int32_t shamt, imm12_i, imm12_s, imm13, imm20_u, imm21;
};



#endif //RV_TLM_0_GLOBALS_H
