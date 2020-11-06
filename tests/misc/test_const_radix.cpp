/******************************************************************************
* Copyright (c) 2020, Intel Corporation. All rights reserved.
* 
* SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.
* 
*****************************************************************************/

#include "sct_assert.h"
#include <systemc.h>
#include <iostream>

// Constant radix for local and member constants and constant arrays
// Error reported as non defined variables @j and @z are used
template <unsigned N, class TRAITS>
struct A : public sc_module 
{
    sc_in<bool> clk{"clk"};
    sc_signal<sc_uint<32>> s;

    SC_HAS_PROCESS(A);
    
    explicit A(const sc_module_name& name) : sc_module(name) 
    {
        SC_METHOD(declMeth);
        sensitive << s;
        
        SC_METHOD(assignMeth);
        sensitive << s;
        
        SC_METHOD(constMeth);
        sensitive << s;

        SC_METHOD(templMeth);
        sensitive << s;
        
        SC_METHOD(binaryMeth);
        sensitive << s;

        SC_METHOD(unaryMeth);
        sensitive << s;

        SC_METHOD(partSelMeth);
        sensitive << s;

        SC_METHOD(funcCallMeth);
        sensitive << s;
    }
   
    
    // Simple declaration
    void declMeth() 
    {
        unsigned d = 15;
        unsigned o = 015;
        unsigned h1 = 0xF;
        int h2 = -0XF;
        unsigned bi = 0b01111;
        
        bool b1 = 0b1;
        bool b2 = 0x12;
        bool b3 = 012;
        
        sc_uint<10> arr1[3] = {0x42, 042, 42};
        int arr2[3] = {-0x42, -042, -42};
        
        static const int l1 = 0xA5;
        h2 = l1;
    }
    
    void assignMeth() 
    {
        unsigned d; d = 15;
        unsigned o; o = 015;
        unsigned h1; h1 = 0xF;
        int h2; h2 = -0XF;
        unsigned bi; bi = 0b01111;
        
        bool b1; b1 = 0b1;
        bool b2; b2 = 0x12;
        bool b3; b3 = 012;
        
        sc_uint<10> arr1[3]; 
        arr1[1] = 0x42; arr1[2] =42;
        int arr2[3]; 
        arr2[1] = -0x42; arr2[2] = -42;
    }
    
    const unsigned G1 = 0x100;
    const unsigned G2 = 0b1100;
    const unsigned G3 = G2 + 0b1;
    static const unsigned G4 = 0xFF;
    static const unsigned G5 = 0xFF >> 2;
    const int G6 = -0x1F + 1;
    const sc_uint<16> G7 = 0xFFF1;
    static constexpr unsigned GE1 = 0b101;
    static constexpr unsigned GE2 = G4 - 1;    
    
    void constMeth() 
    {
        const unsigned h1 = 0xFF; unsigned hh1 = h1;
        const unsigned h2 = 0x100; unsigned hh2 = h2;
        const int h3 = -0XFF + 1; int hh3 = h3;
        const int h4 = -0X100 - 10; int hh4 = h4;
        const unsigned h5 = G5;
        
        unsigned g1 = G1;
        unsigned g2; g2 = G2 + G3;
        unsigned g4; g4 = G4 / 2;
        unsigned g5 = G5 + h5;    // 0x7E
        int g6 = G6;
        g6 = G6 + 10;
        sc_uint<16> g7 = G7;
        
        unsigned ge1 = GE1;
        unsigned ge2 = GE2;
        sc_uint<8> ge3 = GE2+1;
    }

    // Constants from template
    static const unsigned GT1 = N;
    static const unsigned GT2 = N+1;
    static const unsigned GT3 = TRAITS::M;
    static const unsigned GT4 = 1+TRAITS::M;

    void templMeth()
    {
        unsigned a1 = GT1;
        unsigned a2 = GT2;
        unsigned a3 = GT3;
        unsigned a4 = GT4;
    }
    
    // Range and bit
    void partSelMeth() 
    {
        sc_uint<32> x;
        x.range(07,0x3) = 0;
        bool b = x.bit(0b1011);
    }
    
    void binaryMeth() 
    {
        sc_uint<32> x = G1 + 1;
        x = 41 / G1;
        x = G2 - 10;
        x = G1 & G2;
        sct_assert_const(x == 0);
        x = G1 & 0x300;
        sct_assert_const(x == 0x100);
        std::cout << hex << x << std::endl;
        x = G1 >> 2;
        x = 2 << G2;
    }

    void unaryMeth() 
    {
        sc_uint<32> x = ~G1;
        bool b = G7.and_reduce();
        x = G7.nor_reduce();
    }
    
    // Function call 
    int f(int par) {
        int j = par + 1;
        return 0x42;
    }
    
    void g(const sc_uint<8>& par) {
        int j = par + 1;
    }
   
    void funcCallMeth() 
    {
        int j = f(0x42);
        g(042);
        j = f(0b1101);
        g(42);
    }

};

struct TR {
    static const unsigned M = 0x4;
};

struct Top : public sc_module 
{
    sc_in<bool>     clk{"clk"};
    A<0x3, TR>      modA{"modA"};

    SC_CTOR(Top)  
    {
        modA.clk(clk);
    }
};

int sc_main(int argc, char **argv) {

    sc_clock clock_gen{"clock_gen", 10, SC_NS};
    Top mod{"mod"};
    mod.clk(clock_gen);
    sc_start();

    return 0;
}

