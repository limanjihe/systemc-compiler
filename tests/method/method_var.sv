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

// SystemC signals
logic a;
logic b;
logic c;
logic signed [31:0] e;
logic signed [31:0] d;
logic signed [31:0] p1;
logic signed [31:0] p2;


//------------------------------------------------------------------------------

A a_mod
(
  .a(a),
  .b(b),
  .c(c),
  .d(d),
  .e(e),
  .oip(p1),
  .iip(p2)
);

endmodule



//==============================================================================
//
// Module: A (test_var.cpp:256:5)
//
module A // "b_mod.a_mod"
(
    input logic a,
    output logic b,
    output logic c,
    input logic signed [31:0] d,
    output logic signed [31:0] e,
    output logic signed [31:0] oip,
    input logic signed [31:0] iip
);

// SystemC signals
logic signed [31:0] si;
logic sb;
logic signed [31:0] sip;
logic signed [31:0] sig;

//------------------------------------------------------------------------------
// Method process: unknown_ref (test_var.cpp:78:5) 

always_comb 
begin : unknown_ref     // test_var.cpp:78:5
    integer i;
    i = 1;
    i = sig;
end

//------------------------------------------------------------------------------
// Method process: var1 (test_var.cpp:87:5) 

always_comb 
begin : var1     // test_var.cpp:87:5
    integer k_1;
    integer i;
    integer j;
    i = 3;
    j = i + 1 + k_1;
    b = |j;
end

//------------------------------------------------------------------------------
// Method process: var2 (test_var.cpp:95:5) 

always_comb 
begin : var2     // test_var.cpp:95:5
    integer i;
    integer i_1;
    i = a;
    i_1 = a + 1;
    b = |i_1;
    i = i + 2;
    b = |i;
end

//------------------------------------------------------------------------------
// Method process: var3 (test_var.cpp:107:5) 

// Process-local variables
logic signed [31:0] m;

always_comb 
begin : var3     // test_var.cpp:107:5
    integer i;
    integer i_1;
    integer i_2;
    i = a;
    if (m > 0)
    begin
        i_1 = 1;
    end else begin
        i_2 = a;
        b = |i_2;
    end
    b = |i;
end

//------------------------------------------------------------------------------
// Method process: var4 (test_var.cpp:119:5) 

always_comb 
begin : var4     // test_var.cpp:119:5
    integer i;
    integer j;
    integer k_1;
    i = 0;
    while (i < 2)
    begin
        i++;
        j = i;
        k_1 = i + j;
        b = |k_1;
    end
    b = |i;
end

//------------------------------------------------------------------------------
// Method process: signal1 (test_var.cpp:131:5) 

always_comb 
begin : signal1     // test_var.cpp:131:5
    integer i;
    i = si;
    si = i;
    e = si;
    si = d;
end

//------------------------------------------------------------------------------
// Method process: signal2 (test_var.cpp:141:5) 

always_comb 
begin : signal2     // test_var.cpp:141:5
    integer i;
    logic f;
    logic f1;
    integer i1;
    i = si;
    si = i;
    i = sb;
    sb = |i;
    f = sb;
    sb = f;
    f = |si;
    si = f;
    f1 = |si;
    i1 = sb;
end

//------------------------------------------------------------------------------
// Method process: signal3 (test_var.cpp:162:5) 

always_comb 
begin : signal3     // test_var.cpp:162:5
    integer i;
    i = sip;
    sip = i;
    i = sip;
    sip = i;
    i = sip;
    sip = i;
    oip = sip;
    sip = oip;
    sip = iip;
    oip = iip;
    sip = sip;
    oip = oip;
end

//------------------------------------------------------------------------------
// Method process: port1 (test_var.cpp:183:5) 

always_comb 
begin : port1     // test_var.cpp:183:5
    integer i;
    i = a;
    b = |i;
end

//------------------------------------------------------------------------------
// Method process: port2 (test_var.cpp:190:5) 

always_comb 
begin : port2     // test_var.cpp:190:5
    integer i;
    logic f;
    i = d;
    e = i;
    i = a;
    b = |i;
    f = a;
    b = f;
    f = |d;
    e = f;
end

//------------------------------------------------------------------------------
// Method process: port3 (test_var.cpp:207:5) 

always_comb 
begin : port3     // test_var.cpp:207:5
    integer i;
    i = oip;
    oip = i;
    i = oip;
    oip = i;
    oip = i;
    i = oip;
    i = oip;
    oip = i;
end

//------------------------------------------------------------------------------
// Method process: pointer_compare (test_var.cpp:222:5) 

always_comb 
begin : pointer_compare     // test_var.cpp:222:5
    integer i;
    logic b2;
    b2 = 1;
end

//------------------------------------------------------------------------------
// Method process: pointer_compare2 (test_var.cpp:233:5) 

always_comb 
begin : pointer_compare2     // test_var.cpp:233:5
    integer i;
    logic b2;
    b2 = 1;
end

endmodule


