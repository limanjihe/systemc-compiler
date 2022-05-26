//==============================================================================
//
// The code is generated by Intel Compiler for SystemC, version 1.4.24
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

// Variables generated for SystemC signals
logic rst;
logic [3:0] minst_s[2];
logic minst_clk[2];
logic minst_rst[2];

// Assignments generated for C++ channel arrays
assign minst_clk[0] = clk;
assign minst_clk[1] = clk;
assign minst_rst[0] = rst;
assign minst_rst[1] = rst;

//------------------------------------------------------------------------------
// Clocked THREAD: minst_thread_member_ro (test_cthread_mif_array_ptr.cpp:49:5) 

// Next-state combinational logic
always_comb begin : minst_thread_member_ro_comb     // test_cthread_mif_array_ptr.cpp:49:5
    minst_thread_member_ro_func;
end
function void minst_thread_member_ro_func;
    logic [3:0] a;
    a = minst_s[0];
endfunction

// Synchronous register update
always_ff @(posedge minst_clk[0] or posedge minst_rst[0]) 
begin : minst_thread_member_ro_ff
    if ( minst_rst[0] ) begin
    end
    else begin
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: minst_thread_member_comb (test_cthread_mif_array_ptr.cpp:60:5) 

// Thread-local variables
logic [3:0] minst_v[2];

// Next-state combinational logic
always_comb begin : minst_thread_member_comb_comb     // test_cthread_mif_array_ptr.cpp:60:5
    minst_thread_member_comb_func;
end
function void minst_thread_member_comb_func;
    logic [3:0] a;
    minst_v[0] = 1;
    a = minst_v[0];
endfunction

// Synchronous register update
always_ff @(posedge minst_clk[0] or posedge minst_rst[0]) 
begin : minst_thread_member_comb_ff
    if ( minst_rst[0] ) begin
        logic [63:0] b;
        logic [3:0] minst_v;
        minst_v = 0;
        b = minst_v + 1;
    end
    else begin
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: minst_thread_member_reg (test_cthread_mif_array_ptr.cpp:74:5) 

// Thread-local variables
logic [63:0] b0;
logic [63:0] b_next;
logic [4:0] minst_va[2];
logic [4:0] minst_va_next[2];
logic [4:0] a0;
logic [4:0] a_next;
logic thread_member_reg_PROC_STATE;
logic thread_member_reg_PROC_STATE_next;

// Next-state combinational logic
always_comb begin : minst_thread_member_reg_comb     // test_cthread_mif_array_ptr.cpp:74:5
    minst_thread_member_reg_func;
end
function void minst_thread_member_reg_func;
    a_next = a0;
    b_next = b0;
    minst_va_next[0] = minst_va[0];
    thread_member_reg_PROC_STATE_next = thread_member_reg_PROC_STATE;
    
    case (thread_member_reg_PROC_STATE)
        0: begin
            a_next = minst_va_next[0] + b_next;
            thread_member_reg_PROC_STATE_next = 1; return;    // test_cthread_mif_array_ptr.cpp:82:13;
        end
        1: begin
            minst_va_next[0] = a_next;
            a_next = minst_va_next[0] + b_next;
            thread_member_reg_PROC_STATE_next = 1; return;    // test_cthread_mif_array_ptr.cpp:82:13;
        end
    endcase
endfunction

// Synchronous register update
always_ff @(posedge minst_clk[0] or posedge minst_rst[0]) 
begin : minst_thread_member_reg_ff
    if ( minst_rst[0] ) begin
        minst_va[0] <= 0;
        b0 <= minst_va[0] + 1;
        thread_member_reg_PROC_STATE <= 0;    // test_cthread_mif_array_ptr.cpp:78:9;
    end
    else begin
        b0 <= b_next;
        minst_va[0] <= minst_va_next[0];
        a0 <= a_next;
        thread_member_reg_PROC_STATE <= thread_member_reg_PROC_STATE_next;
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: minst_thread_loc_reg (test_cthread_mif_array_ptr.cpp:87:5) 

// Thread-local variables
logic [5:0] a1;
logic [5:0] a_next0;

// Next-state combinational logic
always_comb begin : minst_thread_loc_reg_comb     // test_cthread_mif_array_ptr.cpp:87:5
    minst_thread_loc_reg_func;
end
function void minst_thread_loc_reg_func;
    logic [5:0] b;
    a_next0 = a1;
    b = a_next0;
    a_next0++;
endfunction

// Synchronous register update
always_ff @(posedge minst_clk[0] or posedge minst_rst[0]) 
begin : minst_thread_loc_reg_ff
    if ( minst_rst[0] ) begin
        a1 <= 0;
    end
    else begin
        a1 <= a_next0;
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: minst_thread_member_ro0 (test_cthread_mif_array_ptr.cpp:49:5) 

// Next-state combinational logic
always_comb begin : minst_thread_member_ro0_comb     // test_cthread_mif_array_ptr.cpp:49:5
    minst_thread_member_ro0_func;
end
function void minst_thread_member_ro0_func;
    logic [3:0] a;
    a = minst_s[1];
endfunction

// Synchronous register update
always_ff @(posedge minst_clk[1] or posedge minst_rst[1]) 
begin : minst_thread_member_ro0_ff
    if ( minst_rst[1] ) begin
    end
    else begin
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: minst_thread_member_comb0 (test_cthread_mif_array_ptr.cpp:60:5) 

// Next-state combinational logic
always_comb begin : minst_thread_member_comb0_comb     // test_cthread_mif_array_ptr.cpp:60:5
    minst_thread_member_comb0_func;
end
function void minst_thread_member_comb0_func;
    logic [3:0] a;
    minst_v[1] = 1;
    a = minst_v[1];
endfunction

// Synchronous register update
always_ff @(posedge minst_clk[1] or posedge minst_rst[1]) 
begin : minst_thread_member_comb0_ff
    if ( minst_rst[1] ) begin
        logic [63:0] b;
        logic [3:0] minst_v;
        minst_v = 0;
        b = minst_v + 1;
    end
    else begin
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: minst_thread_member_reg0 (test_cthread_mif_array_ptr.cpp:74:5) 

// Thread-local variables
logic [63:0] b1;
logic [63:0] b_next0;
logic [4:0] a2;
logic [4:0] a_next1;
logic thread_member_reg_PROC_STATE0;
logic thread_member_reg_PROC_STATE_next0;

// Next-state combinational logic
always_comb begin : minst_thread_member_reg0_comb     // test_cthread_mif_array_ptr.cpp:74:5
    minst_thread_member_reg0_func;
end
function void minst_thread_member_reg0_func;
    a_next1 = a2;
    b_next0 = b1;
    minst_va_next[1] = minst_va[1];
    thread_member_reg_PROC_STATE_next0 = thread_member_reg_PROC_STATE0;
    
    case (thread_member_reg_PROC_STATE0)
        0: begin
            a_next1 = minst_va_next[1] + b_next0;
            thread_member_reg_PROC_STATE_next0 = 1; return;    // test_cthread_mif_array_ptr.cpp:82:13;
        end
        1: begin
            minst_va_next[1] = a_next1;
            a_next1 = minst_va_next[1] + b_next0;
            thread_member_reg_PROC_STATE_next0 = 1; return;    // test_cthread_mif_array_ptr.cpp:82:13;
        end
    endcase
endfunction

// Synchronous register update
always_ff @(posedge minst_clk[1] or posedge minst_rst[1]) 
begin : minst_thread_member_reg0_ff
    if ( minst_rst[1] ) begin
        minst_va[1] <= 0;
        b1 <= minst_va[1] + 1;
        thread_member_reg_PROC_STATE0 <= 0;    // test_cthread_mif_array_ptr.cpp:78:9;
    end
    else begin
        b1 <= b_next0;
        minst_va[1] <= minst_va_next[1];
        a2 <= a_next1;
        thread_member_reg_PROC_STATE0 <= thread_member_reg_PROC_STATE_next0;
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: minst_thread_loc_reg0 (test_cthread_mif_array_ptr.cpp:87:5) 

// Thread-local variables
logic [5:0] a3;
logic [5:0] a_next2;

// Next-state combinational logic
always_comb begin : minst_thread_loc_reg0_comb     // test_cthread_mif_array_ptr.cpp:87:5
    minst_thread_loc_reg0_func;
end
function void minst_thread_loc_reg0_func;
    logic [5:0] b;
    a_next2 = a3;
    b = a_next2;
    a_next2++;
endfunction

// Synchronous register update
always_ff @(posedge minst_clk[1] or posedge minst_rst[1]) 
begin : minst_thread_loc_reg0_ff
    if ( minst_rst[1] ) begin
        a3 <= 0;
    end
    else begin
        a3 <= a_next2;
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: top_thread_comb (test_cthread_mif_array_ptr.cpp:145:5) 

// Thread-local variables
logic [3:0] minst_vc[2];

// Next-state combinational logic
always_comb begin : top_thread_comb_comb     // test_cthread_mif_array_ptr.cpp:145:5
    top_thread_comb_func;
end
function void top_thread_comb_func;
    logic [3:0] a;
    minst_vc[1] = 2;
    a = minst_vc[1];
endfunction

// Synchronous register update
always_ff @(posedge clk or posedge rst) 
begin : top_thread_comb_ff
    if ( rst ) begin
        logic [3:0] minst_vc[2];
        minst_vc[1] = 1;
    end
    else begin
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: top_thread_reg (test_cthread_mif_array_ptr.cpp:158:5) 

// Thread-local variables
logic [3:0] minst_vr[2];
logic [3:0] minst_vr_next[2];
logic top_thread_reg_PROC_STATE;
logic top_thread_reg_PROC_STATE_next;

// Next-state combinational logic
always_comb begin : top_thread_reg_comb     // test_cthread_mif_array_ptr.cpp:158:5
    top_thread_reg_func;
end
function void top_thread_reg_func;
    logic [3:0] a;
    minst_vr_next = minst_vr;
    top_thread_reg_PROC_STATE_next = top_thread_reg_PROC_STATE;
    
    case (top_thread_reg_PROC_STATE)
        0: begin
            minst_vr_next[1] = 2;
            top_thread_reg_PROC_STATE_next = 1; return;    // test_cthread_mif_array_ptr.cpp:165:13;
        end
        1: begin
            a = minst_vr_next[1];
            minst_vr_next[1] = 2;
            top_thread_reg_PROC_STATE_next = 1; return;    // test_cthread_mif_array_ptr.cpp:165:13;
        end
    endcase
endfunction

// Synchronous register update
always_ff @(posedge clk or posedge rst) 
begin : top_thread_reg_ff
    if ( rst ) begin
        minst_vr[1] <= 1;
        top_thread_reg_PROC_STATE <= 0;    // test_cthread_mif_array_ptr.cpp:161:9;
    end
    else begin
        minst_vr <= minst_vr_next;
        top_thread_reg_PROC_STATE <= top_thread_reg_PROC_STATE_next;
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: top_thread_ro (test_cthread_mif_array_ptr.cpp:171:5) 

// Thread-local variables
logic [3:0] minst_vo[2];
logic [3:0] minst_vo_next[2];

// Next-state combinational logic
always_comb begin : top_thread_ro_comb     // test_cthread_mif_array_ptr.cpp:171:5
    top_thread_ro_func;
end
function void top_thread_ro_func;
    logic [3:0] a;
    minst_vo_next = minst_vo;
    a = minst_vo_next[1];
endfunction

// Synchronous register update
always_ff @(posedge clk or posedge rst) 
begin : top_thread_ro_ff
    if ( rst ) begin
    end
    else begin
        minst_vo <= minst_vo_next;
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: top_thread_fcall (test_cthread_mif_array_ptr.cpp:181:5) 

// Thread-local variables
logic minst_vf[2];
logic minst_vf_next[2];
logic a4;
logic a_next3;
logic [1:0] top_thread_fcall_PROC_STATE;
logic [1:0] top_thread_fcall_PROC_STATE_next;

// Next-state combinational logic
always_comb begin : top_thread_fcall_comb     // test_cthread_mif_array_ptr.cpp:181:5
    top_thread_fcall_func;
end
function void top_thread_fcall_func;
    logic b;
    a_next3 = a4;
    minst_vf_next = minst_vf;
    top_thread_fcall_PROC_STATE_next = top_thread_fcall_PROC_STATE;
    
    case (top_thread_fcall_PROC_STATE)
        0: begin
            // Call f_loc_reg() begin
            a_next3 = minst_vf_next[1];
            top_thread_fcall_PROC_STATE_next = 1; return;    // test_cthread_mif_array_ptr.cpp:107:9;
            // Call f_loc_reg() end
        end
        1: begin
            // Call f_loc_reg() begin
            b = a_next3;
            // Call f_loc_reg() end
            top_thread_fcall_PROC_STATE_next = 2; return;    // test_cthread_mif_array_ptr.cpp:187:13;
        end
        2: begin
            // Call f_loc_reg() begin
            a_next3 = minst_vf_next[1];
            top_thread_fcall_PROC_STATE_next = 1; return;    // test_cthread_mif_array_ptr.cpp:107:9;
            // Call f_loc_reg() end
        end
    endcase
endfunction

// Synchronous register update
always_ff @(posedge clk or posedge rst) 
begin : top_thread_fcall_ff
    if ( rst ) begin
        top_thread_fcall_PROC_STATE <= 0;    // test_cthread_mif_array_ptr.cpp:183:9;
    end
    else begin
        minst_vf <= minst_vf_next;
        a4 <= a_next3;
        top_thread_fcall_PROC_STATE <= top_thread_fcall_PROC_STATE_next;
    end
end

endmodule


