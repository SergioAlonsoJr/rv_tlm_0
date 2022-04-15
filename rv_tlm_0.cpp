
#include <systemc.h>
//#include "memory.h"
using namespace std;


SC_MODULE(rv_tlm) {

//    sc_port<mem_if> pmem;
    sc_event go_fetch, go_decode, go_execute;
    SC_CTOR(rv_tlm) {
        SC_THREAD(init)
        SC_THREAD(fetch)
        SC_THREAD(decode)
        SC_THREAD(execute)

    }

    void init() {
        cout << "INIT" << endl;
        // TODO: chamar os métodos do trabalho anterior
        go_fetch.notify(1, SC_NS);
    }

    void fetch() {
        while (true) {
            wait(go_fetch);
            cout << "fetch" << endl;
            go_decode.notify(1, SC_NS);
        }
    }
    void decode() {
        while (true) {
            wait(go_decode);
            cout << "decode" << endl;
            go_execute.notify(1, SC_NS);
        }
    }
    void execute() {
        while (true) {
            wait(go_execute);
            cout << "execute" << endl;
            go_fetch.notify(1, SC_NS);
        }
    }
};

int sc_main(int, char* []) {
    rv_tlm rv("Risc_V");

    sc_start(100, SC_NS);
    return 0;
}
