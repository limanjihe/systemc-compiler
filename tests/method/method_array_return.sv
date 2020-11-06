//==============================================================================
//
// The code is generated by Intel Compiler for SystemC, version 1.3.7
// see more information at https://github.com/intel/systemc-compiler
//
//==============================================================================

//==============================================================================
//
// Module: test ()
//
module test // "t_inst"
(
);

// SystemC signals

//------------------------------------------------------------------------------
// Method process: array_return (test_array_return.cpp:31:5) 

always_comb 
begin : array_return     // test_array_return.cpp:31:5
    integer c[3];
    integer TMP_0;
    // Call arr_ret1() begin
    c[0] = 1;
    c[1] = 2;
    c[2] = 3;
    TMP_0 = c;
    // Call arr_ret1() end
end

endmodule


