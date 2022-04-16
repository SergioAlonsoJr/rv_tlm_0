
#include <systemc.h>
//#include "memory.h"
#include "rv_tlm_0.h"
#include "globals.h"
#include "memoria.h"

using namespace std;


void rv_tlm::init() {
    cout << "INIT" << endl;
    breg[SP] = 0x00003ffc;                  // stack pointer
    breg[GP] = 0x00001800;                  // global pointer
    load_mem("memory_content/code.bin", 0);
    load_mem("memory_content/data.bin", 0x2000);
    go_fetch.notify(1, SC_NS);
}

void rv_tlm::fetch() {
    while (true) {
        wait(go_fetch);
//        cout << "fetch" << endl;
        ri = mem[pc >> 2];
        global_ic.ri = ri;
        global_ic.pc = pc;
        pc += 4;
        go_decode.notify(1, SC_NS);
    }
}

void rv_tlm::decode() {
    while (true) {
        wait(go_decode);
//        cout << "decode" << endl;
        int32_t tmp;
        global_opcode = ri & 0x7F;
        rs2 = (ri >> 20) & 0x1F;
        rs1 = (ri >> 15) & 0x1F;
        rd = (ri >> 7) & 0x1F;
        shamt = (ri >> 20) & 0x1F;
        funct3 = (ri >> 12) & 0x7;
        funct7 = (ri >> 25);
        imm12_i = ((int32_t) ri) >> 20;
        tmp = get_field(ri, 7, 0x1f);
        imm12_s = set_field(imm12_i, 0, 0x1f, tmp);
        imm13 = imm12_s;
        imm13 = set_bit(imm13, 11, imm12_s & 1);
        imm13 = imm13 & ~1;
        imm20_u = ri & (~0xFFF);
        // mais aborrecido...
        imm21 = (int32_t) ri >> 11;            // estende sinal
        tmp = get_field(ri, 12, 0xFF);        // le campo 19:12
        imm21 = set_field(imm21, 12, 0xFF, tmp);    // escreve campo em imm21
        tmp = get_bit(ri, 20);                // le o bit 11 em ri(20)
        imm21 = set_bit(imm21, 11, tmp);            // posiciona bit 11
        tmp = get_field(ri, 21, 0x3FF);
        imm21 = set_field(imm21, 1, 0x3FF, tmp);
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
//        cout << "execute" << endl;
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
                breg[global_ic.rd] = lb(breg[global_ic.rs1], global_ic.imm12_i);
                break;

            case I_lh:
                breg[global_ic.rd] = lh(breg[global_ic.rs1], global_ic.imm12_i);
                break;

            case I_lw:
                breg[global_ic.rd] = lw(breg[global_ic.rs1], global_ic.imm12_i);
                break;

            case I_lbu:
                breg[global_ic.rd] = lbu(breg[global_ic.rs1], global_ic.imm12_i);
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
                sb(breg[global_ic.rs1], global_ic.imm12_s, (int8_t) breg[global_ic.rs2]);
                break;

            case I_sw:
                sw(breg[global_ic.rs1], global_ic.imm12_s, breg[global_ic.rs2]);
                break;

            case I_ori:
                breg[global_ic.rd] = breg[global_ic.rs1] | global_ic.imm12_i;
                break;

            case I_xori:
                breg[global_ic.rd] = breg[global_ic.rs1] ^ global_ic.imm12_i;
                break;

            case I_lhu:
                breg[global_ic.rd] = unsign(lh(breg[global_ic.rs1], global_ic.imm12_s));
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
                        a = (mem[pos] >> (offset * 8)) & 0xFF;
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
int sc_main(int, char* []) {
    rv_tlm rv("Risc_V");

    sc_start(10000, SC_NS);
    return 0;
}
#pragma clang diagnostic pop
