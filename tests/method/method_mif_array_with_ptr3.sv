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
    input logic clk
);

// SystemC signals
logic signed [31:0] t;
logic signed [31:0] z;
logic signed [31:0] pt;
logic signed [31:0] pz;
logic signed [31:0] minst_in[2];
logic signed [31:0] minst_out[2];
logic signed [31:0] minst_pin[2];
logic signed [31:0] minst_pout[2];

assign minst_in[0] = t;
assign minst_in[1] = z;
assign z = minst_out[1];
assign minst_pin[0] = pt;
assign minst_pin[1] = pz;
assign pt = minst_pout[0];

//------------------------------------------------------------------------------
// Method process: minst_ptrProc (test_mif_array_with_ptr3.cpp:32:5) 

always_comb 
begin : minst_ptrProc     // test_mif_array_with_ptr3.cpp:32:5
    minst_out[0] = minst_in[0];
    minst_pout[0] = minst_pin[0];
end

//------------------------------------------------------------------------------
// Method process: minst_ptrProc0 (test_mif_array_with_ptr3.cpp:32:5) 

always_comb 
begin : minst_ptrProc0     // test_mif_array_with_ptr3.cpp:32:5
    minst_out[1] = minst_in[1];
    minst_pout[1] = minst_pin[1];
end

//------------------------------------------------------------------------------
// Method process: top_meth (test_mif_array_with_ptr3.cpp:67:5) 

always_comb 
begin : top_meth     // test_mif_array_with_ptr3.cpp:67:5
    integer i;
    i = z + pt;
end

endmodule


