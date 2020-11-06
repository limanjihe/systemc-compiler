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
logic signed [31:0] in_ports1[3];
logic signed [31:0] in_ports1b[3];
logic signed [31:0] in_ports2[3][3];
logic signed [31:0] in_ports2b[3][3];
logic out_ports1b[3];
logic signed [31:0] out_ports1[3];
logic signed [31:0] out_ports2[3][3];
logic out_ports2b[3][3];
logic [4:0] in_ports_p1[3];
logic signed [31:0] in_ports_p2[3][3];
logic signed [31:0] out_ports_p2[3][3];


//------------------------------------------------------------------------------

A a_mod
(
  .in_ports1(in_ports1),
  .in_ports1b(in_ports1b),
  .in_ports2(in_ports2),
  .in_ports2b(in_ports2b),
  .out_ports1b(out_ports1b),
  .out_ports1(out_ports1),
  .out_ports2(out_ports2),
  .out_ports2b(out_ports2b),
  .in_ports_p1(in_ports_p1),
  .out_ports_p1(in_ports_p1),
  .in_ports_p2(in_ports_p2),
  .out_ports_p2(out_ports_p2)
);

endmodule



//==============================================================================
//
// Module: A (test_array.cpp:302:5)
//
module A // "b_mod.a_mod"
(
    input logic signed [31:0] in_ports1[3],
    input logic signed [31:0] in_ports1b[3],
    input logic signed [31:0] in_ports2[3][3],
    input logic signed [31:0] in_ports2b[3][3],
    output logic out_ports1b[3],
    output logic signed [31:0] out_ports1[3],
    output logic signed [31:0] out_ports2[3][3],
    output logic out_ports2b[3][3],
    input logic [4:0] in_ports_p1[3],
    output logic [4:0] out_ports_p1[3],
    input logic signed [31:0] in_ports_p2[3][3],
    output logic signed [31:0] out_ports_p2[3][3]
);

// SystemC signals
logic [4:0] sig1[3];
logic sig1b[3];
logic signed [31:0] sig2[3][3];
logic [4:0] sig2a[3][3];
logic sig2b[3][3];
logic signed [31:0] sig_p1[3];
logic signed [31:0] sig_p2[3][3];
logic signed [3:0] sig_p2a[3][3];

//------------------------------------------------------------------------------
// Method process: mem_array (test_array.cpp:100:5) 

// Process-local variables
logic signed [31:0] m[3];
logic signed [31:0] n[3][3];

always_comb 
begin : mem_array     // test_array.cpp:100:5
    integer i;
    i = m[0];
    m[0] = i;
    m[1] = m[0] + m[2];
    m[2] = 0;
    n[1][m[1]] = n[m[0]][i];
    n[n[1][2]][2] = n[1][n[2][0]];
    for (integer j = 0; j < 3; j++)
    begin
        m[j] = j;
        for (integer k = 0; k < 3; k++)
        begin
            n[j][k] = j * k;
        end
    end
end

//------------------------------------------------------------------------------
// Method process: var_array (test_array.cpp:117:5) 

always_comb 
begin : var_array     // test_array.cpp:117:5
    integer ln[2][3];
    integer lm[3];
    integer i;
    integer j;
    lm[0] = 3; lm[1] = 2; lm[2] = 1;
    j = 0;
    i = lm[0];
    i = ln[1][2];
    lm[i] = i + 1;
    ln[i][j] = i;
    ln[i][j] = lm[0];
    lm[1] = ln[i][j];
end

//------------------------------------------------------------------------------
// Method process: in_port_array (test_array.cpp:130:5) 

always_comb 
begin : in_port_array     // test_array.cpp:130:5
    integer i;
    integer j;
    logic f;
    j = 1;
    i = in_ports1[1];
    i = in_ports1[2];
    i = in_ports1b[1];
    f = |in_ports2b[1][j];
    i = in_ports2[j][2];
    i = in_ports2[1][i];
    i = in_ports1[j - 1];
    i = in_ports2[i + j][2 + i];
end

//------------------------------------------------------------------------------
// Method process: out_port_array (test_array.cpp:146:5) 

always_comb 
begin : out_port_array     // test_array.cpp:146:5
    integer i;
    integer j;
    j = 0;
    out_ports1[j] = i;
    out_ports1[j] = i;
    out_ports1b[j] = 1;
    out_ports1b[j] = |i;
    out_ports2[1][i] = 1;
    out_ports2[j][2] = i + j;
    out_ports2b[j][i] = 0;
    out_ports2b[1][2] = |(j - i);
    out_ports1[i + j] = 1;
    out_ports2[i + 1][j - i] = i;
    out_ports2[i + 1][j - i] = 1;
    out_ports1[1] = in_ports1[2];
    out_ports2[i + 1][j - i] = in_ports1[i + 3];
    out_ports1b[i] = |in_ports2b[1][2];
    out_ports1[in_ports1[2]] = 3;
    out_ports1[in_ports1[i]] = in_ports2[1][2];
end

//------------------------------------------------------------------------------
// Method process: signal_array_part_sel (test_array.cpp:175:5) 

always_comb 
begin : signal_array_part_sel     // test_array.cpp:175:5
    integer i;
    integer j;
    logic [4:0] x;
    logic b;
    x = 0;
    x = sig1[1][3 : 2];
    i = sig1[j][0];
    i = sig2a[1][2][7];
    i = sig2a[1][2][9 : 5];
    b = in_ports_p1[2][3];
    i = in_ports_p1[2][2 : 1];
    x = sig_p2a[i][j][3 : 0];
    x = sig_p2a[i + j][i - j][j +: 2];
    sig1[1] = sig1[2][1 : 0];
    out_ports1[0] = in_ports_p1[1][2];
    x = sig1[0][j + i +: 2];
end

//------------------------------------------------------------------------------
// Method process: signal_array (test_array.cpp:197:5) 

always_comb 
begin : signal_array     // test_array.cpp:197:5
    integer i;
    integer j;
    j = 0;
    sig1[j] = i;
    sig1[j] = i + 1;
    sig1b[j] = 1;
    sig1b[j] = |i;
    sig2[1][i] = sig1[2];
    sig2[j][2] = sig1[1] + sig1[2];
    sig2b[j][i] = sig1b[0];
    sig2b[1][2] = !sig1b[0];
    i = sig2[1][sig1[2]];
    i = sig2[1][2];
    out_ports1[i] = sig2[1][sig1[2]];
    out_ports1[i] = sig2[sig1[2]][i];
    out_ports2[1][2] = signed'({1'b0, sig1[0]}) + sig2[1][2];
    sig1[1] = sig1[2];
    sig2[1][2] = sig2[0][1];
    sig2[0][1] = sig2[1][2] + sig2[2][1] + signed'({1'b0, sig1[0]});
end

//------------------------------------------------------------------------------
// Method process: port_pointer_array (test_array.cpp:226:5) 

always_comb 
begin : port_pointer_array     // test_array.cpp:226:5
    integer i;
    i = in_ports_p2[1][2];
    i = in_ports_p1[1];
    i = in_ports_p1[1];
    out_ports_p1[2] = i;
    out_ports_p1[2] = i;
    out_ports_p1[2] = i;
    out_ports_p1[2] = in_ports_p1[1];
    out_ports_p1[2] = in_ports_p1[1];
    out_ports_p1[2] = in_ports_p1[1];
    out_ports_p2[1][2] = in_ports_p2[0][1];
    out_ports_p2[1][2] = in_ports_p2[0][1] + in_ports_p2[2][2];
end

//------------------------------------------------------------------------------
// Method process: sig_pointer_array (test_array.cpp:245:5) 

always_comb 
begin : sig_pointer_array     // test_array.cpp:245:5
    integer i;
    integer j;
    i = sig_p1[1];
    i = sig_p1[i + 1];
    i = sig_p1[1];
    sig_p1[2] = i;
    sig_p2[i + 1][j + 2] = i;
    sig_p1[2] = i;
    out_ports_p1[2] = sig_p2[1][2];
    sig_p2[1][2] = out_ports_p1[2];
    out_ports_p1[2] = sig_p1[1];
    out_ports_p2[i][i] = sig_p1[1];
    sig_p2[1][2] = in_ports_p2[1][2];
    sig_p2[1][2] = signed'({1'b0, in_ports_p1[0]}) + in_ports_p2[1][2];
    sig_p2[0][0] = in_ports_p1[0];
    sig_p2[1][2] = sig_p2[1][2];
    sig_p2[1][2] = sig_p2[1][2];
    sig_p1[i * i] = sig_p2[i][i] + sig_p2[1][2];
end

//------------------------------------------------------------------------------
// Method process: array_init (test_array.cpp:274:5) 

always_comb 
begin : array_init     // test_array.cpp:274:5
    integer y1[3];
    integer y2[3];
    logic [2:0] x1[3];
    logic [2:0] x2[3];
    integer k1;
    integer k2;
    y2[0] = 1; y2[1] = 2; y2[2] = 3;
    x1[0] = 0; x1[1] = 0; x1[2] = 0;
    x2[0] = 1; x2[1] = 2; x2[2] = 3;
    k1 = 2;
    k2 = 2;
end

//------------------------------------------------------------------------------
// Method process: chan_array_range (test_array.cpp:85:5) 

always_comb 
begin : chan_array_range     // test_array.cpp:85:5
    integer i;
    logic b;
    logic [2:0] offset;
    offset = i;
    b = |sig1[i][offset +: 2];
end

//------------------------------------------------------------------------------
// Method process: chan_array_double_used (test_array.cpp:94:5) 

always_comb 
begin : chan_array_double_used     // test_array.cpp:94:5
    logic b;
    b = sig1b[0] ? 0 : sig1b[0];
end

endmodule


