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
module A // "a_mod"
(
    input logic clk
);

// SystemC signals
logic nrst;

//------------------------------------------------------------------------------
// Method process: nonconst_ptr_method (test_pointer_warning.cpp:42:5) 

// Process-local variables
logic signed [31:0] q0;
logic [3:0] v0;

always_comb 
begin : nonconst_ptr_method     // test_pointer_warning.cpp:42:5
    integer j;
    q0 = 1;
    v0 = 2;
    j = q0 + signed'({1'b0, v0});
end

//------------------------------------------------------------------------------
// Clocked THREAD: nonconst_ptr_thread (test_pointer_warning.cpp:47:5) 

// Thread-local variables
logic [3:0] v1;
logic [3:0] v1_next;
logic signed [31:0] q1;
logic signed [31:0] q1_next;

// Next-state combinational logic
always_comb begin : nonconst_ptr_thread_comb     // test_pointer_warning.cpp:47:5
    nonconst_ptr_thread_func;
end
function void nonconst_ptr_thread_func;
    integer j;
    q1_next = q1;
    v1_next = v1;
    j = q1_next + signed'({1'b0, v1_next}) + q1_next;
endfunction

// Syncrhonous register update
always_ff @(posedge clk or negedge nrst) 
begin : nonconst_ptr_thread_ff
    if ( ~nrst ) begin
        q1 <= 1;
        v1 <= 2;
    end
    else begin
        v1 <= v1_next;
        q1 <= q1_next;
    end
end

endmodule


