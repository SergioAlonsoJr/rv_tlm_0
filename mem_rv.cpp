#include "mem_rv.h"

using namespace std;

void mem_rv::check_address_range(uint32_t address) {
    // endereco em bytes, memoria em words
    if ((address >> 2) > MEM_SIZE) {
        cout << "Erro: Endereco fora dos limites da memoria - " << address;
        exit(-1);
    }
}

int32_t mem_rv::read(uint32_t address) {
    check_address_range(address);
    return mem[address];
}

int32_t mem_rv::lb(uint32_t address, int32_t kte) {
    auto target_addr = address + kte;

    check_address_range(target_addr);
    auto* pb = (int8_t*)&mem;

    return (*(pb + target_addr));
}

int32_t mem_rv::lbu(uint32_t address, int32_t kte) {
    auto target_addr = address + kte;

    check_address_range(target_addr);
    auto* pb = (uint8_t*)&mem;

    return (*(pb + target_addr));
}

int32_t mem_rv::lh(uint32_t address, int32_t kte) {
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

int32_t mem_rv::lw(uint32_t address, int32_t kte) {
    auto target_addr = address + kte;

    check_address_range(target_addr);

    return mem[target_addr >> 2];
}



void mem_rv::sb(uint32_t address, int32_t kte, int8_t dado) {
    auto target_addr = address + kte;

    check_address_range(target_addr);


    auto* pb = (uint8_t*)&mem[target_addr >> 2];
    auto bi = (target_addr % 4);
    pb += bi;

    *pb = (uint8_t)dado;
}

void mem_rv::sw(uint32_t address, int32_t kte, int32_t dado){
    auto target_address = address + kte;

    check_address_range(target_address);

    mem[target_address >> 2] = dado;

}

int mem_rv::load_mem(const char* fn, int start) {
    FILE* file_ptr;
    auto* memory_ptr = mem + (start >> 2);
    int size = 0;

    file_ptr = fopen(fn, "rb");
    if (!file_ptr) {
        printf("Arquivo nao encontrado!");
        return -1;
    }else {
        while (!feof(file_ptr)) {
            fread(memory_ptr, 4, 1, file_ptr);
            memory_ptr++;
            size++;
        }
        fclose(file_ptr);
    }
    return size;
}
