//==============================================================================
//
// The code is generated by Intel Compiler for SystemC, version 1.3.7
// see more information at https://github.com/intel/systemc-compiler
//
//==============================================================================

//==============================================================================
//
// Module: top ()
//
module top // "tb_inst.top0"
(
    input logic rstn,
    input logic clkin,
    input logic signed [11:0] din,
    output logic signed [11:0] inner_out,
    input logic din0,
    input logic [11:0] din1,
    output logic signed [31:0] dout
);


//------------------------------------------------------------------------------

inner inner0
(
  .clkin(clkin),
  .rstn(rstn),
  .din(din),
  .inner_out(inner_out),
  .din0(din0),
  .din1(din1),
  .dout(dout)
);

endmodule



//==============================================================================
//
// Module: inner (test_promote_ports.cpp:64:5)
//
module inner // "tb_inst.top0.inner0"
(
    input logic clkin,
    input logic rstn,
    input logic signed [11:0] din,
    output logic signed [11:0] inner_out,
    input logic din0,
    input logic [11:0] din1,
    output logic signed [31:0] dout
);

// SystemC signals
logic signed [31:0] intsig;


//------------------------------------------------------------------------------

bottom bot0
(
  .clkin(clkin),
  .din(din0)
);

bottom2 bot2
(
  .clkin(clkin),
  .din(din1),
  .dout(dout),
  .intin(intsig)
);

endmodule



//==============================================================================
//
// Module: bottom (test_promote_ports.cpp:47:5)
//
module bottom // "tb_inst.top0.inner0.bot0"
(
    input logic clkin,
    input logic din
);

endmodule



//==============================================================================
//
// Module: bottom2 (test_promote_ports.cpp:48:5)
//
module bottom2 // "tb_inst.top0.inner0.bot2"
(
    input logic clkin,
    input logic [11:0] din,
    output logic signed [31:0] dout,
    input logic signed [31:0] intin
);

endmodule


