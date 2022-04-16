#include "memoria.h"

using namespace std;

int32_t mem[MEM_SIZE];

void check_address_range(uint32_t address) {
    // endereco em bytes, memoria em words
    if ((address >> 2) > MEM_SIZE) {
        cout << "Erro: Endereco fora dos limites da memoria - " << address;
        exit(-1);
    }
}

int32_t lb(uint32_t address, int32_t kte) {
    auto target_addr = address + kte;

    check_address_range(target_addr);
    auto* pb = (int8_t*)&mem;

    return (*(pb + target_addr));
}

int32_t lbu(uint32_t address, int32_t kte) {
    auto target_addr = address + kte;

    check_address_range(target_addr);
    auto* pb = (uint8_t*)&mem;

    return (*(pb + target_addr));
}

int32_t lh(uint32_t address, int32_t kte) {
    auto target_addr = address + kte;

    check_address_range(target_addr);


    if ((target_addr % 2) != 0) {
        cout << "Erro: endereco de meia palavra desalinhado!" << endl;
        return -1;
    }
    auto  word = mem[target_addr >> 2];
    word = (word >> 8 * (target_addr & 2));

    return (word & 0xFFFF);
}

int32_t lw(uint32_t address, int32_t kte) {
    auto target_addr = address + kte;

    check_address_range(target_addr);

    return mem[target_addr >> 2];
}



void sb(uint32_t address, int32_t kte, int8_t dado) {
    auto target_addr = address + kte;

    check_address_range(target_addr);


    auto* pb = (uint8_t*)&mem[target_addr >> 2];
    auto bi = (target_addr % 4);
    pb += bi;

    *pb = (uint8_t)dado;
}

void sw(uint32_t address, int32_t kte, int32_t dado){
    auto target_address = address + kte;

    check_address_range(target_address);

    mem[target_address >> 2] = dado;

}
