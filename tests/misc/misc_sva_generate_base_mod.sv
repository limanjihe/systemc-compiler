//==============================================================================
//
// The code is generated by Intel Compiler for SystemC, version 1.3.7
// see more information at https://github.com/intel/systemc-compiler
//
//==============================================================================

//==============================================================================
//
// Module: A ()
//
module A // "test_top.a_mod"
(
    input logic clk,
    input logic rstn
);

// SystemC signals
logic o;
logic r;
logic signed [31:0] s;
logic signed [31:0] r0;

//------------------------------------------------------------------------------
// Clocked THREAD: thread1 (test_sva_generate_base_mod.cpp:40:5) 

// Thread-local variables
logic o_next;
logic thread1_PROC_STATE;
logic thread1_PROC_STATE_next;

// Next-state combinational logic
always_comb begin : thread1_comb     // test_sva_generate_base_mod.cpp:40:5
    thread1_func;
end
function void thread1_func;
    o_next = o;
    thread1_PROC_STATE_next = thread1_PROC_STATE;
    
    case (thread1_PROC_STATE)
        0: begin
            thread1_PROC_STATE_next = 1; return;    // test_sva_generate_base_mod.cpp:47:13;
        end
        1: begin
            thread1_PROC_STATE_next = 1; return;    // test_sva_generate_base_mod.cpp:47:13;
        end
    endcase
endfunction

// Syncrhonous register update
always_ff @(posedge clk or negedge rstn) 
begin : thread1_ff
    if ( ~rstn ) begin
        o <= 0;
        thread1_PROC_STATE <= 0;    // test_sva_generate_base_mod.cpp:43:9;
    end
    else begin
        o <= o_next;
        thread1_PROC_STATE <= thread1_PROC_STATE_next;

    `ifndef INTEL_SVA_OFF
        sctAssertLine44 : assert property ( o |=> o );
    `endif // INTEL_SVA_OFF
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: thread2 (test_sva_generate_base_mod.cpp:78:5) 

// Thread-local variables
logic signed [31:0] s_next;
logic thread2_PROC_STATE;
logic thread2_PROC_STATE_next;

// Next-state combinational logic
always_comb begin : thread2_comb     // test_sva_generate_base_mod.cpp:78:5
    thread2_func;
end
function void thread2_func;
    s_next = s;
    thread2_PROC_STATE_next = thread2_PROC_STATE;
    
    case (thread2_PROC_STATE)
        0: begin
            s_next = !(|s);
            thread2_PROC_STATE_next = 1; return;    // test_sva_generate_base_mod.cpp:87:13;
        end
        1: begin
            s_next = !(|s);
            thread2_PROC_STATE_next = 1; return;    // test_sva_generate_base_mod.cpp:87:13;
        end
    endcase
endfunction

// Syncrhonous register update
always_ff @(posedge clk or negedge rstn) 
begin : thread2_ff
    if ( ~rstn ) begin
        s <= 0;
        thread2_PROC_STATE <= 0;    // test_sva_generate_base_mod.cpp:81:9;
    end
    else begin
        s <= s_next;
        thread2_PROC_STATE <= thread2_PROC_STATE_next;

    `ifndef INTEL_SVA_OFF
        sctAssertLine83 : assert property ( |s |-> ##3 !(|s) );
    `endif // INTEL_SVA_OFF
    end
end

`ifndef INTEL_SVA_OFF
sctAssertLine74 : assert property (
    @(posedge clk) |r0 |-> ##2 |r0 );
sctAssertLine75 : assert property (
    @(posedge clk) |s |-> ##2 !(|s) );
sctAssertLine76 : assert property (
    @(posedge clk) mm == 0 |-> ##[2:3] mm == 3 );
sctAssertLine51 : assert property (
    @(clk) o |-> o );
sctAssertLine52 : assert property (
    @(posedge clk) r |-> r );
sctAssertLine53 : assert property (
    @(posedge clk) |s |-> ##[0:1] !(|s) );
sctAssertLine54 : assert property (
    @(posedge clk) m == 3 |-> ##3 m == 3 );
`endif // INTEL_SVA_OFF

endmodule


