#ifndef RV_TLM_0_RV_TLM_0_H
#define RV_TLM_0_RV_TLM_0_H

#include <systemc.h>
#include "mem_rv.h"
SC_MODULE(rv_tlm) {

    sc_port<mem_rv> pmem;
    sc_event go_fetch, go_decode, go_execute;

    int32_t breg[32];
    uint32_t pc = 0x00000000;				// contador de programa
    uint32_t ri = 0x00000000;				// registrador de intrucao

    uint32_t hi = 0x00000000;
    uint32_t lo = 0x00000000;

    int32_t		imm12_i,				// constante 12 bits
    imm12_s,				// constante 12 bits
    imm13,					// constante 13 bits
    imm20_u,				// constante 20 bis mais significativos
    imm21;					// constante 21 bits

    uint32_t	global_opcode,					// codigo da operacao
    rs1,					// indice registrador rs
    rs2,					// indice registrador rt
    rd,						// indice registrador rd
    shamt,					// deslocamento
    funct3,					// campos auxiliares
    funct7;					// constante instrucao tipo J

    instruction_context_st global_ic;

    SC_CTOR(rv_tlm) {
        SC_THREAD(init)
        SC_THREAD(fetch)
        SC_THREAD(decode)
        SC_THREAD(execute)
    }

    void init();

    void fetch();

    void decode();

    void execute();

    uint32_t unsign(int32_t x);
    void pc_branch(int32_t offset, instruction_context_st& ic);

    int logSh3(int32_t x, int32_t n);

    __attribute__((unused)) __attribute__((unused)) void print_instr(instruction_context_st& ic);
    INSTRUCTIONS get_instr_code(uint32_t opcode, uint32_t func3, uint32_t func7);
    FORMATS get_i_format(uint32_t opcode);

    __attribute__((unused)) __attribute__((unused)) __attribute__((unused)) void debug_decode(instruction_context_st& ic);

    __attribute__((unused)) __attribute__((unused)) void dump_reg(char format, bool include_zero);

    __attribute__((unused)) __attribute__((unused)) void dump_mem(int start_byte, int end_byte, char format);
};

#endif //RV_TLM_0_RV_TLM_0_H
