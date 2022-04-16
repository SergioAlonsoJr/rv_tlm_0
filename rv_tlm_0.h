#ifndef RV_TLM_0_RV_TLM_0_H
#define RV_TLM_0_RV_TLM_0_H

#include <systemc.h>

SC_MODULE(rv_tlm) {

//    sc_port<mem_if> pmem;
    sc_event go_fetch, go_decode, go_execute;
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
};

#endif //RV_TLM_0_RV_TLM_0_H
