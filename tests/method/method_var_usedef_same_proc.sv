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
logic rst;
logic signed [31:0] t;


//------------------------------------------------------------------------------

Child child
(
  .clk(clk),
  .rst(rst),
  .in(t),
  .out(t)
);

endmodule



//==============================================================================
//
// Module: Child (test_var_usedef_same_proc.cpp:50:5)
//
module Child // "top.child"
(
    input logic clk,
    input logic rst,
    input logic signed [31:0] in,
    output logic signed [31:0] out
);

// SystemC signals
logic signed [31:0] s;
logic signed [31:0] t;

//------------------------------------------------------------------------------
// Method process: methA (test_var_usedef_same_proc.cpp:30:5) 

always_comb 
begin : methA     // test_var_usedef_same_proc.cpp:30:5
    s = in;
    t = s;
end

//------------------------------------------------------------------------------
// Method process: methB (test_var_usedef_same_proc.cpp:35:5) 

always_comb 
begin : methB     // test_var_usedef_same_proc.cpp:35:5
    integer j;
    if (|s)
    begin
        j = out;
    end
    out = in;
end

endmodule


