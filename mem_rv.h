#ifndef RV_TLM_0_MEM_RV_H
#define RV_TLM_0_MEM_RV_H

#include <cstdint>
#include "globals.h"
#include <systemc.h>

struct mem_if : public sc_interface {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
    virtual int32_t	read(uint32_t address) = 0;
#pragma clang diagnostic pop

    virtual int32_t lb(uint32_t address, int32_t kte) = 0;
    virtual int32_t lh(uint32_t address, int32_t kte) = 0;
    virtual int32_t lw(uint32_t address, int32_t kte) = 0;
    virtual int32_t lbu(uint32_t address, int32_t kte) = 0;

    virtual void sb(uint32_t address, int32_t kte, int8_t dado) = 0;
    virtual void sw(uint32_t address, int32_t kte, int32_t dado) = 0;

};

struct mem_rv : public sc_module, public mem_if {

    SC_CTOR(mem_rv){

    }


    int32_t mem[MEM_SIZE];

    void check_address_range(uint32_t address);
    int32_t read(uint32_t address);

    int32_t lb(uint32_t address, int32_t kte);
    int32_t lh(uint32_t address, int32_t kte);
    int32_t lw(uint32_t address, int32_t kte);
    int32_t lbu(uint32_t address, int32_t kte);

    void sb(uint32_t address, int32_t kte, int8_t dado);
    void sw(uint32_t address, int32_t kte, int32_t dado);

    int load_mem(const char *fn, int start);
};

#endif //RV_TLM_0_MEM_RV_H
