#include <systemc.h>
#include "rv_tlm_0.h"
#include "globals.h"

using namespace std;


void rv_tlm::init() {
    cout << "INIT" << endl;
    breg[SP] = 0x00003ffc;                  // stack pointer
    breg[GP] = 0x00001800;                  // global pointer
    pmem->load_mem("memory_content/code.bin", 0);
    pmem->load_mem("memory_content/data.bin", 0x2000);
    go_fetch.notify(1, SC_NS);
}

void rv_tlm::fetch() {
    while (true) {
        wait(go_fetch);
        ri = pmem->mem[pc >> 2];
        global_ic.ri = ri;
        global_ic.pc = pc;
        pc += 4;
        go_decode.notify(1, SC_NS);
    }
}


uint32_t rv_tlm::unsign(int32_t x) {
    return (uint32_t)x;
}

void  rv_tlm::pc_branch(int32_t offset, instruction_context_st& ic) {
    ic.pc += offset;
}

void rv_tlm::decode() {
    while (true) {
        wait(go_decode);
        int32_t tmp;
        global_opcode = ri & 0x7F;
        rs2 = (ri >> 20) & 0x1F;
        rs1 = (ri >> 15) & 0x1F;
        rd = (ri >> 7) & 0x1F;
        shamt = (ri >> 20) & 0x1F;
        funct3 = (ri >> 12) & 0x7;
        funct7 = (ri >> 25);
        imm12_i = ((int32_t) ri) >> 20;
        tmp = rv_get_field(ri, 7, 0x1f);
        imm12_s = rv_set_field(imm12_i, 0, 0x1f, tmp);
        imm13 = imm12_s;
        imm13 = rv_set_bit(imm13, 11, imm12_s & 1);
        imm13 = imm13 & ~1;
        imm20_u = ri & (~0xFFF);
        // mais aborrecido...
        imm21 = (int32_t) ri >> 11;            // estende sinal
        tmp = rv_get_field(ri, 12, 0xFF);        // le campo 19:12
        imm21 = rv_set_field(imm21, 12, 0xFF, tmp);    // escreve campo em imm21
        tmp = rv_get_bit(ri, 20);                // le o bit 11 em ri(20)
        imm21 = rv_set_bit(imm21, 11, tmp);            // posiciona bit 11
        tmp = rv_get_field(ri, 21, 0x3FF);
        imm21 = rv_set_field(imm21, 1, 0x3FF, tmp);
        imm21 = imm21 & ~1;                    // zero bit 0
        global_ic.ins_code = get_instr_code(global_opcode, funct3, funct7);
        global_ic.ins_format = get_i_format(global_opcode);
        global_ic.rs1 = (REGISTERS) rs1;
        global_ic.rs2 = (REGISTERS) rs2;
        global_ic.rd = (REGISTERS) rd;
        global_ic.shamt = shamt;
        global_ic.imm12_i = imm12_i;
        global_ic.imm12_s = imm12_s;
        global_ic.imm13 = imm13;
        global_ic.imm21 = imm21;
        global_ic.imm20_u = imm20_u;
        go_execute.notify(1, SC_NS);
    }
}

void rv_tlm::execute() {
    while (true) {
        wait(go_execute);
        uint32_t t;
        breg[0] = 0;
        switch (global_ic.ins_code) {
            case I_add:
                breg[global_ic.rd] = breg[global_ic.rs1] + breg[global_ic.rs2];
                break;
            case I_addi:
                breg[global_ic.rd] = breg[global_ic.rs1] + global_ic.imm12_i;
                break;
            case I_and:
                breg[global_ic.rd] = breg[global_ic.rs1] & breg[global_ic.rs2];
                break;
            case I_andi:
                breg[global_ic.rd] = breg[global_ic.rs1] & global_ic.imm12_i;
                break;
            case I_auipc:
                breg[global_ic.rd] = global_ic.pc + global_ic.imm20_u;
                break;
            case I_beq:
                if (breg[global_ic.rs1] == breg[global_ic.rs2])
                    pc_branch(global_ic.imm13, global_ic);
                break;
            case I_bne:
                if (breg[global_ic.rs1] != breg[global_ic.rs2])
                    pc_branch(global_ic.imm13, global_ic);
                break;
            case I_blt:
                if (breg[global_ic.rs1] < breg[global_ic.rs2])
                    pc_branch(global_ic.imm13, global_ic);
                break;
            case I_bge:
                if (breg[global_ic.rs1] >= breg[global_ic.rs2])
                    pc_branch(global_ic.imm13, global_ic);
                break;

            case I_bltu:
                if (unsign(breg[global_ic.rs1]) < unsign(breg[global_ic.rs2]))
                    pc_branch(global_ic.imm13, global_ic);
                break;

            case I_bgeu:
                if (unsign(breg[global_ic.rs1]) >= unsign(breg[global_ic.rs2]))
                    pc_branch(global_ic.imm13, global_ic);
                break;

            case I_lb:
                breg[global_ic.rd] = pmem->lb(breg[global_ic.rs1], global_ic.imm12_i);
                break;

            case I_lh:
                breg[global_ic.rd] = pmem->lh(breg[global_ic.rs1], global_ic.imm12_i);
                break;

            case I_lw:
                breg[global_ic.rd] = pmem->lw(breg[global_ic.rs1], global_ic.imm12_i);
                break;

            case I_lbu:
                breg[global_ic.rd] = pmem->lbu(breg[global_ic.rs1], global_ic.imm12_i);
                break;

            case I_jal:
                if (global_ic.rd == 0) {
                    global_ic.rd = RA;
                }
                breg[global_ic.rd] = global_ic.pc + 4;
                pc_branch(global_ic.imm21, global_ic);
                break;

            case I_jalr:
                // Talvez exista um caso em que o rd seja o ra

                t = global_ic.pc + 4;
                global_ic.pc = (breg[global_ic.rs1] + global_ic.imm12_i);
                global_ic.pc = global_ic.pc & ~1;
                breg[global_ic.rd] = t;
                break;

            case I_sb:
                pmem->sb(breg[global_ic.rs1], global_ic.imm12_s, (int8_t) breg[global_ic.rs2]);
                break;

            case I_sw:
                pmem->sw(breg[global_ic.rs1], global_ic.imm12_s, breg[global_ic.rs2]);
                break;

            case I_ori:
                breg[global_ic.rd] = breg[global_ic.rs1] | global_ic.imm12_i;
                break;

            case I_xori:
                breg[global_ic.rd] = breg[global_ic.rs1] ^ global_ic.imm12_i;
                break;

            case I_lhu:
                breg[global_ic.rd] = unsign(pmem->lh(breg[global_ic.rs1], global_ic.imm12_s));
                break;

            case I_lui:
                breg[global_ic.rd] = global_ic.imm20_u;
                break;

            case I_slti:
                breg[global_ic.rd] = breg[global_ic.rs1] < global_ic.imm12_i;
                break;

            case I_sltiu:
                breg[global_ic.rd] =
                        unsign(breg[global_ic.rs1]) < unsign(global_ic.imm12_i);
                break;

            case I_slli:
                breg[global_ic.rd] = breg[global_ic.rs1] << global_ic.shamt;
                break;

            case I_srli:
                breg[global_ic.rd] = logSh3(breg[global_ic.rs1], global_ic.shamt);
                break;

            case I_srai:
                breg[global_ic.rd] = breg[global_ic.rs1] >> global_ic.shamt;
                break;

            case I_sub:
                breg[global_ic.rd] = breg[global_ic.rs1] - breg[global_ic.rs2];
                break;

            case I_sll:
                breg[global_ic.rd] = breg[global_ic.rs1] << breg[global_ic.rs2];
                break;

            case I_slt:
                breg[global_ic.rd] = breg[global_ic.rs1] < breg[global_ic.rs2];
                break;

            case I_sltu:
                breg[global_ic.rd] =
                        unsign(breg[global_ic.rs1]) < unsign(breg[global_ic.rs2]);
                break;

            case I_xor:
                breg[global_ic.rd] = breg[global_ic.rs1] ^ breg[global_ic.rs2];
                break;

            case I_or:
                breg[global_ic.rd] = breg[global_ic.rs1] | breg[global_ic.rs2];
                break;

            case I_ecall:
                if (breg[A7] == 1) {
                    cout << (int32_t) breg[A0] << endl;
                } else if (breg[A7] == 4) {
                    int pos, offset;
                    char a;
                    pos = breg[A0] >> 2;
                    offset = breg[A0] % 4;

                    do {
                        a = (pmem->mem[pos] >> (offset * 8)) & 0xFF;
                        cout << dec << a;
                        offset++;
                        if (offset >= 4) {
                            pos++;
                            offset = 0;
                        }
                    } while (a != 0x00);
                } else if (breg[A7] == 10) {
                    printf("Encerrando programa.");
                    exit(0);
                }
                break;

            case I_nop:
                break;
            default:
                printf("Instrucao nao implementada ", global_ic.ins_code);
                break;

        }

        if (global_ic.pc + 4 == pc) {
            global_ic.pc = pc;
        }
        else {
            pc = global_ic.pc;
        }

        go_fetch.notify(1, SC_NS);
    }
}

int rv_tlm::logSh3(int32_t x, int32_t n) {
    int mask = ~2 + 1;
    int shiftAmount = 31 + ((~n) + 1);//this evaluates to 31 - n on two's complement machines
    mask = mask << shiftAmount;
    mask = ~mask;//If n equals 0, it means we have negated all bits and hence have mask = 0
    x = x >> n;
    return x & mask;
}


__attribute__((unused)) void rv_tlm::print_instr(instruction_context_st& ic) {
    cout << "Instruction Context: \n";
    cout << "ri: " << ic.ri << endl;
    cout << "pc: " << ic.pc << endl;
    cout << "ins_code: " << ic.ins_code << endl;
    cout << "ins_format: " << ic.ins_format << endl;
    cout << "rs1: " << ic.rs1 << endl;
    cout << "rs2: " << ic.rs2 << endl;
    cout << "rd: " << ic.rd << endl;
    cout << "shamt: " << ic.shamt << endl;
    cout << "imm12_s: " << ic.imm12_s << endl;
    cout << "imm13: " << ic.imm13 << endl;
    cout << "imm20_u: " << ic.imm20_u << endl;
    cout << "imm21: " << ic.imm21 << endl;
}



INSTRUCTIONS rv_tlm::get_instr_code(uint32_t opcode, uint32_t func3, uint32_t func7) {
    switch(opcode){
        case LUI: return I_lui;
        case AUIPC: return I_auipc;
        case JAL: return I_jal;
        case JALR: return I_jalr;
        case ECALL: return I_ecall;
        case ILType:
            switch (func3) {
                case LB3: return I_lb;
                case LBU3: return I_lbu;
                case LH3: return I_lh;
                case LW3: return I_lw;
                case LHU3: return I_lhu;
            }
        case StoreType:
            switch (func3) {
                case SB3: return I_sb;
                case SH3: return I_sh;
                case SW3: return I_sw;
            }

        case RegType:
            switch (func3){
                case ADDSUB3:
                    switch (func7) {
                        case ADD7: return  I_add;
                        case SUB7: return I_sub;
                    }
                case SLL3: return I_sll;
                case SLT3: return I_slt;
                case SLTU3: return I_sltu;
                case XOR3: return I_xor;
                case OR3: return I_or;
                case AND3: return I_and;
                case SR3:
                    switch (func7) {
                        case SRA7: return I_sra;
                        case SRL7: return I_srl;
                    }
            }
        case ILAType:
            switch (func3)
            {
                case ADDI3: return I_addi;
                case ORI3: return I_ori;
                case XORI3: return I_xori;
                case SLTI3: return I_slti;
                case ANDI3: return I_andi;
                case SLTIU3: return I_sltiu;
                case SLLI3: return I_slli;
                case SRI3:
                    switch (func7) {
                        case SRAI7: return I_srai;
                        case SRLI7: return I_srli;
                    }
            }
        case BType:
            switch (func3)
            {
                case BEQ3: return I_beq;
                case BNE3: return I_bne;
                case BLT3: return I_blt;
                case BGE3: return I_bge;
                case BLTU3: return I_bltu;
                case BGEU3: return I_bgeu;
            }
        default: return I_nop;
    }
}

FORMATS rv_tlm::get_i_format(uint32_t opcode) {
    switch (opcode) {
        case LUI:
        case AUIPC: return UType;
        case JAL:
        case JALR: return UJType;
        case ECALL: return NullFormat;
        case ILType:
        case ILAType: return IType;

        case StoreType:return SType;

        case RegType: return RType;

        case BType: return SBType;
        default: return NOPType;
    }
}

__attribute__((unused)) void rv_tlm::dump_reg(char format, bool include_zero = true) {
    printf("\n");
    auto base = (format == 'h') ? hex : dec;
    for (int i = 0; i < 32; i++) {
        if (breg[i] == 0 && !include_zero)
            continue;

        cout << "BREG[" << dec<< i << "] = " << base <<breg[i] << endl;
    }

    printf("PC pra proxima instrucao: ");

    cout << "pc = " << base << rv_tlm::global_ic.pc << endl;
    cout << "hi = " << base << hi << endl;
    cout << "lo = " << base << lo << endl;

}

__attribute__((unused)) void rv_tlm::dump_mem(int start, int end, char format){
    printf("\n");
    if (format == 'h') {
        for (auto i = start; i <= end; i += 4) {
            printf("%x \t%x\n", i, pmem->lw(i, 0));
        }

    }
    else {
        for (auto i = start; i < end; i += 4) {
            printf("%x \t%d\n", i, pmem->lw(i, 0));
        }
    }
}

__attribute__((unused)) void rv_tlm::debug_decode(instruction_context_st& ic) {
    cout << "pc = " << ic.pc << endl;
    cout << "instruction = " <<  hex <<ic.ri << endl;
    cout << "format = " << ic.ins_format << endl;

    cout << hex << "opcode = " << ic.ins_code << endl;
    cout << "rs1 = " << ic.rs1 << endl;
    cout << "rs2 = " << ic.rs2 << endl;
    cout << "rd = " << ic.rd << endl;
    cout << "shamt = " << ic.shamt << endl;
    cout << "imm12_i = " << ic.imm12_i << endl;
    cout << "imm12_s = " << ic.imm12_s << endl;
    cout << "imm13 = " << ic.imm13 << endl;
    cout << "imm21 = " << ic.imm21 << endl;
    cout << "imm20_u = " << ic.imm20_u << endl;

}
