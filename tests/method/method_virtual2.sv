//==============================================================================
//
// The code is generated by Intel Compiler for SystemC, version 1.3.7
// see more information at https://github.com/intel/systemc-compiler
//
//==============================================================================

//==============================================================================
//
// Module: B_top ()
//
module B_top // "b_mod"
(
);


//------------------------------------------------------------------------------

C c_mod
(

);

D d_mod
(

);

endmodule



//==============================================================================
//
// Module: C (test_virtual2.cpp:76:5)
//
module C // "b_mod.c_mod"
(
);

// SystemC signals
logic s;

//------------------------------------------------------------------------------
// Method process: virt_call (test_virtual2.cpp:31:5) 

// Process-local variables
logic signed [31:0] m;

always_comb 
begin : virt_call     // test_virtual2.cpp:31:5
    // Call f() begin
    m = m + 2;
    s = 1;
    // Call f() end
end

endmodule



//==============================================================================
//
// Module: D (test_virtual2.cpp:77:5)
//
module D // "b_mod.d_mod"
(
);

// SystemC signals
logic s;

//------------------------------------------------------------------------------
// Method process: virt_call (test_virtual2.cpp:31:5) 

// Process-local variables
logic signed [31:0] m;

always_comb 
begin : virt_call     // test_virtual2.cpp:31:5
    // Call f() begin
    m = m + 3;
    s = 0;
    // Call f() end
end

//------------------------------------------------------------------------------
// Method process: virt_call_d (test_virtual2.cpp:67:5) 

// Process-local variables
logic signed [31:0] m;

always_comb 
begin : virt_call_d     // test_virtual2.cpp:67:5
    // Call f() begin
    m = m + 3;
    s = 0;
    // Call f() end
end

endmodule


