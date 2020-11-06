/******************************************************************************
* Copyright (c) 2020, Intel Corporation. All rights reserved.
* 
* SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.
* 
*****************************************************************************/

#include <systemc.h>

using namespace sc_core;

// Dynamically allocated ports in another module
class A : public sc_module {
public:
    sc_in<bool>*            p1[3];
    sc_out<bool>**          p2;
    sc_signal<int>*         s1;

    sc_signal<bool>         dummy;
    
    SC_CTOR(A) {
        SC_METHOD(proc1); sensitive << dummy;
    }
    
    void proc1() {
        s1[0] = p1[0]->read() ? 1 : 2;
        *(p2[0]) = s1[0].read();
    }     
    
};

class B_top : public sc_module {
public:
    sc_signal<bool>         sig1[3];
    sc_signal<bool>         sig2[3];
    
    A a_mod{"a_mod"};

    SC_CTOR(B_top) {
        // Allocate signal in another module directly
        for (int i = 0; i < 3; i++) {
            a_mod.p1[i] = new sc_in<bool>("p1");
        }

        a_mod.p2 = sc_new_array<sc_out<bool>*>(3);
        for (int i = 0; i < 3; i++) {
            a_mod.p2[i] = new sc_out<bool>("p2");
        }
        
        a_mod.s1 = new sc_signal<int>[3];
        
        for (int i = 0; i < 3; i++) {
            a_mod.p1[i]->bind(sig1[i]);
            a_mod.p2[i]->bind(sig2[i]);
        }
    }
};

int sc_main(int argc, char *argv[]) {
    B_top b_mod{"b_mod"};
    sc_start();
    return 0;
}


