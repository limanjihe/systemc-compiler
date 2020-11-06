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
logic [31:0] indx;

//------------------------------------------------------------------------------
// Method process: array_unknown_ref (test_fcall_ref_arr_unkwn.cpp:48:5) 

// Process-local variables
logic signed [31:0] gindx;

always_comb 
begin : array_unknown_ref     // test_fcall_ref_arr_unkwn.cpp:48:5
    logic larr[3];
    logic TMP_0;
    logic b;
    logic TMP_2;
    larr[0] = 1; larr[1] = 0; larr[2] = 1;
    gindx = indx;
    // Call ref_elem() begin
    gindx++;
    TMP_0 = larr[0];
    // Call ref_elem() end
    b = TMP_0;
    // Call ref_elem() begin
    gindx++;
    TMP_2 = larr[gindx];
    // Call ref_elem() end
    b = TMP_2;
end

//------------------------------------------------------------------------------
// Method process: array_unknown_ptr (test_fcall_ref_arr_unkwn.cpp:64:5) 

// Process-local variables
logic signed [31:0] mindx;
logic parr[3];

always_comb 
begin : array_unknown_ptr     // test_fcall_ref_arr_unkwn.cpp:64:5
    logic TMP_0;
    logic b;
    logic TMP_2;
    mindx = indx;
    // Call ptr_elem() begin
    mindx++;
    TMP_0 = parr[mindx];
    // Call ptr_elem() end
    b = TMP_0;
    // Call ptr_elem() begin
    mindx++;
    TMP_2 = parr[1];
    // Call ptr_elem() end
    b = TMP_2;
end

//------------------------------------------------------------------------------
// Clocked THREAD: array_ref_wait (test_fcall_ref_arr_unkwn.cpp:77:5) 

// Thread-local variables
logic arr[3];
logic arr_next[3];
logic [1:0] array_ref_wait_PROC_STATE;
logic [1:0] array_ref_wait_PROC_STATE_next;

// Next-state combinational logic
always_comb begin : array_ref_wait_comb     // test_fcall_ref_arr_unkwn.cpp:77:5
    array_ref_wait_func;
end
function void array_ref_wait_func;
    logic TMP_0;
    logic b;
    logic TMP_2;
    arr_next = arr;
    array_ref_wait_PROC_STATE_next = array_ref_wait_PROC_STATE;
    
    case (array_ref_wait_PROC_STATE)
        0: begin
            // Call ref_wait() begin
            array_ref_wait_PROC_STATE_next = 1; return;    // test_fcall_ref_arr_unkwn.cpp:73:9;
            // Call ref_wait() end
        end
        1: begin
            // Call ref_wait() begin
            TMP_0 = arr_next[indx];
            // Call ref_wait() end
            b = TMP_0;
            array_ref_wait_PROC_STATE_next = 2; return;    // test_fcall_ref_arr_unkwn.cpp:84:13;
        end
        2: begin
            // Call ref_wait() begin
            array_ref_wait_PROC_STATE_next = 1; return;    // test_fcall_ref_arr_unkwn.cpp:73:9;
            // Call ref_wait() end
        end
    endcase
endfunction

// Syncrhonous register update
always_ff @(posedge clk or negedge nrst) 
begin : array_ref_wait_ff
    if ( ~nrst ) begin
        array_ref_wait_PROC_STATE <= 0;    // test_fcall_ref_arr_unkwn.cpp:80:9;
    end
    else begin
        arr <= arr_next;
        array_ref_wait_PROC_STATE <= array_ref_wait_PROC_STATE_next;
    end
end

endmodule


