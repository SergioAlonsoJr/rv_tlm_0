#include "rv_tlm_0.h"
#include "mem_rv.h"
#include <systemc.h>
#include "globals.h"


int sc_main(int, char* []) {
    rv_tlm rv("Risc_V");
    mem_rv memory("Memory");
    rv.pmem(memory);
    sc_start(10000, SC_NS);
    return 0;
}

