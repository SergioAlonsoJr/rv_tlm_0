#ifndef RV_TLM_0_MEMORIA_H
#define RV_TLM_0_MEMORIA_H

#include <cstdint>
#include "globals.h"

extern int32_t mem[MEM_SIZE];

int32_t lb(uint32_t address, int32_t kte);
int32_t lh(uint32_t address, int32_t kte);
int32_t lw(uint32_t address, int32_t kte);
int32_t lbu(uint32_t address, int32_t kte);

void sb(uint32_t address, int32_t kte, int8_t dado);
void sw(uint32_t address, int32_t kte, int32_t dado);

#endif //RV_TLM_0_MEMORIA_H
