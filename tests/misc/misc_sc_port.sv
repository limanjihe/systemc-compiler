//==============================================================================
//
// The code is generated by Intel Compiler for SystemC, version 1.3.7
// see more information at https://github.com/intel/systemc-compiler
//
//==============================================================================

//==============================================================================
//
// Module: Dut ()
//
module Dut // "dut"
(
    input logic clk
);

// SystemC signals
logic nrst;
logic [3:0] s;
logic [3:0] tar_s2;
logic slave_clk;
logic slave_nrst;

assign slave_clk = clk;
assign slave_nrst = nrst;

//------------------------------------------------------------------------------
// Method process: slave_methProc (test_sc_port.cpp:38:5) 

always_comb 
begin : slave_methProc     // test_sc_port.cpp:38:5
    logic [3:0] TMP_0;
    logic [3:0] val;
    val = 0;
    // Call f() begin
    tar_s2 = val;
    s = val;
    TMP_0 = val + 1;
    // Call f() end
end

//------------------------------------------------------------------------------
// Clocked THREAD: slave_threadProc (test_sc_port.cpp:43:5) 

// Thread-local variables
logic [3:0] tar_s2_next;
logic [3:0] s_next;
logic [3:0] a;
logic [3:0] a_next;

// Next-state combinational logic
always_comb begin : slave_threadProc_comb     // test_sc_port.cpp:43:5
    slave_threadProc_func;
end
function void slave_threadProc_func;
    logic [3:0] TMP_0;
    logic [3:0] val;
    logic [3:0] TMP_1;
    a_next = a;
    s_next = s;
    tar_s2_next = tar_s2;
    val = a_next;
    // Call f() begin
    tar_s2_next = val;
    s_next = val;
    TMP_1 = val + 1;
    // Call f() end
    a_next = TMP_1;
endfunction

// Syncrhonous register update
always_ff @(posedge slave_clk or negedge slave_nrst) 
begin : slave_threadProc_ff
    if ( ~slave_nrst ) begin
        logic [3:0] TMP_0;
        logic [3:0] val;
        a <= 0;
        val = 0;
        // Call f() begin
        tar_s2 <= val;
        s <= val;
        TMP_0 = val + 1;
        // Call f() end
    end
    else begin
        tar_s2 <= tar_s2_next;
        s <= s_next;
        a <= a_next;
    end
end

//------------------------------------------------------------------------------
// Method process: methProc (test_sc_port.cpp:112:5) 

always_comb 
begin : methProc     // test_sc_port.cpp:112:5
    logic [3:0] TMP_0;
    logic [3:0] a;
    // Call getSig() begin
    TMP_0 = tar_s2;
    // Call getSig() end
    a = TMP_0;
end

//------------------------------------------------------------------------------
// Clocked THREAD: threadProc (test_sc_port.cpp:116:5) 

// Thread-local variables
logic [3:0] a0;
logic [3:0] a_next0;

// Next-state combinational logic
always_comb begin : threadProc_comb     // test_sc_port.cpp:116:5
    threadProc_func;
end
function void threadProc_func;
    logic [3:0] TMP_0;
    logic [3:0] val;
    a_next0 = a0;
    val = a_next0;
    // Call g() begin
    TMP_0 = val + tar_s2;
    // Call g() end
    a_next0 = TMP_0;
endfunction

// Syncrhonous register update
always_ff @(posedge clk or negedge nrst) 
begin : threadProc_ff
    if ( ~nrst ) begin
        a0 <= 0;
    end
    else begin
        a0 <= a_next0;
    end
end

endmodule


