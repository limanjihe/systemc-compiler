//==============================================================================
//
// The code is generated by Intel Compiler for SystemC, version 1.3.7
// see more information at https://github.com/intel/systemc-compiler
//
//==============================================================================

//==============================================================================
//
// Module: Top ()
//
module Top // "top"
(
);


//------------------------------------------------------------------------------

mod_with_name m0
(

);

endmodule



//==============================================================================
//
// Module: mod_with_name (test_ctor_name.cpp:47:5)
//
module mod_with_name // "top.mod2"
(
);

// SystemC signals
logic s;

//------------------------------------------------------------------------------
// Method process: proc (test_ctor_name.cpp:34:5) 

always_comb 
begin : proc     // test_ctor_name.cpp:34:5
    logic b;
    b = s;
end

endmodule


