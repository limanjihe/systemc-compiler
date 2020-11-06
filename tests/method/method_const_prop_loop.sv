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
logic clk;
logic nrst;


//------------------------------------------------------------------------------

A a_mod
(
  .clk(clk),
  .nrst(nrst),
  .a(a),
  .b(b)
);

endmodule



//==============================================================================
//
// Module: A (test_const_prop_loop.cpp:182:5)
//
module A // "b_mod.a_mod"
(
    input logic clk,
    input logic nrst,
    input logic a,
    output logic b
);

// SystemC signals

//------------------------------------------------------------------------------
// Method process: unstable_loop1 (test_const_prop_loop.cpp:49:5) 

// Process-local variables
logic signed [31:0] k;

always_comb 
begin : unstable_loop1     // test_const_prop_loop.cpp:49:5
    k = 0;
    for (integer i = 0; i < 100; i++)
    begin
        if (i == 99)
        begin
            k = 1;
        end
    end
end

//------------------------------------------------------------------------------
// Method process: unstable_loop2 (test_const_prop_loop.cpp:58:5) 

// Process-local variables
logic signed [31:0] k;
logic signed [31:0] m;

always_comb 
begin : unstable_loop2     // test_const_prop_loop.cpp:58:5
    logic b_1;
    integer l;
    k = 0;
    m = 0;
    b_1 = 0;
    for (integer i = 0; i < 100; i++)
    begin
        b_1 = !b_1;
        if (b_1)
        begin
            k = 1;
        end
    end
    if (b_1)
    begin
        m = 1;
    end
    if (1)
    begin
        l = 1;
    end
end

//------------------------------------------------------------------------------
// Method process: simple_for1 (test_const_prop_loop.cpp:76:5) 

// Process-local variables
logic signed [31:0] m;

always_comb 
begin : simple_for1     // test_const_prop_loop.cpp:76:5
    m = 0;
    for (integer i = 0; i < 2; i++)
    begin
        m++;
    end
end

//------------------------------------------------------------------------------
// Method process: simple_for1a (test_const_prop_loop.cpp:86:5) 

// Process-local variables
logic signed [31:0] m;

always_comb 
begin : simple_for1a     // test_const_prop_loop.cpp:86:5
    integer l;
    m = 0;
    for (integer i = 0; i < 100; i++)
    begin
        m++;
    end
    if (|m)
    begin
        l = 1;
    end
end

//------------------------------------------------------------------------------
// Method process: simple_for1b (test_const_prop_loop.cpp:96:5) 

always_comb 
begin : simple_for1b     // test_const_prop_loop.cpp:96:5
    logic b_1;
    integer l;
    b_1 = 0;
    for (integer i = 0; i < 100; i++)
    begin
        b_1 = !b_1;
    end
    if (b_1)
    begin
        l = 1;
    end
end

//------------------------------------------------------------------------------
// Clocked THREAD: simple_for2 (test_const_prop_loop.cpp:106:5) 

// Thread-local variables
logic signed [31:0] i;
logic signed [31:0] i_next;
logic signed [31:0] k;
logic signed [31:0] k_next;
logic signed [31:0] m;
logic signed [31:0] m_next;
logic [1:0] simple_for2_PROC_STATE;
logic [1:0] simple_for2_PROC_STATE_next;

// Next-state combinational logic
always_comb begin : simple_for2_comb     // test_const_prop_loop.cpp:106:5
    simple_for2_func;
end
function void simple_for2_func;
    integer l;
    i_next = i;
    k_next = k;
    m_next = m;
    simple_for2_PROC_STATE_next = simple_for2_PROC_STATE;
    
    case (simple_for2_PROC_STATE)
        0: begin
            m_next = 0;
            k_next = a;
            i_next = 0;
            if (i_next < k_next)
            begin
                m_next = 1;
                simple_for2_PROC_STATE_next = 1; return;    // test_const_prop_loop.cpp:115:17;
            end
            if (|m_next)
            begin
                l = 1;
            end
            simple_for2_PROC_STATE_next = 2; return;    // test_const_prop_loop.cpp:119:13;
        end
        1: begin
            i_next++;
            if (i_next < k_next)
            begin
                m_next = 1;
                simple_for2_PROC_STATE_next = 1; return;    // test_const_prop_loop.cpp:115:17;
            end
            if (|m_next)
            begin
                l = 1;
            end
            simple_for2_PROC_STATE_next = 2; return;    // test_const_prop_loop.cpp:119:13;
        end
        2: begin
            m_next = 0;
            k_next = a;
            i_next = 0;
            if (i_next < k_next)
            begin
                m_next = 1;
                simple_for2_PROC_STATE_next = 1; return;    // test_const_prop_loop.cpp:115:17;
            end
            if (|m_next)
            begin
                l = 1;
            end
            simple_for2_PROC_STATE_next = 2; return;    // test_const_prop_loop.cpp:119:13;
        end
    endcase
endfunction

// Syncrhonous register update
always_ff @(posedge clk or negedge nrst) 
begin : simple_for2_ff
    if ( ~nrst ) begin
        simple_for2_PROC_STATE <= 0;    // test_const_prop_loop.cpp:107:9;
    end
    else begin
        i <= i_next;
        k <= k_next;
        m <= m_next;
        simple_for2_PROC_STATE <= simple_for2_PROC_STATE_next;
    end
end

//------------------------------------------------------------------------------
// Method process: simple_for3 (test_const_prop_loop.cpp:123:5) 

// Process-local variables
logic signed [31:0] m;
logic signed [31:0] n;

always_comb 
begin : simple_for3     // test_const_prop_loop.cpp:123:5
    m = 0;
    n = 0;
    for (integer i = 0; i < 2; i++)
    begin
        m++;
        for (integer j = 0; j < m; j++)
        begin
            n++;
        end
    end
end

//------------------------------------------------------------------------------
// Method process: continue_in_for1 (test_const_prop_loop.cpp:138:5) 

// Process-local variables
logic signed [31:0] m;

always_comb 
begin : continue_in_for1     // test_const_prop_loop.cpp:138:5
    m = 0;
    for (integer i = 0; i < 2; i++)
    begin
        if (a)
        begin
            continue;
        end
        m = 1;
    end
end

//------------------------------------------------------------------------------
// Method process: continue_in_for2 (test_const_prop_loop.cpp:150:5) 

// Process-local variables
logic signed [31:0] m;

always_comb 
begin : continue_in_for2     // test_const_prop_loop.cpp:150:5
    m = 0;
    for (integer i = 0; i < 2; i++)
    begin
        if (a)
        begin
            continue;
        end else begin
            m = 1;
        end
    end
end

//------------------------------------------------------------------------------
// Method process: dowhile_loop (test_const_prop_loop.cpp:164:5) 

always_comb 
begin : dowhile_loop     // test_const_prop_loop.cpp:164:5
    integer i;
    i = 0;
    do
    begin
        i++;
    end
    while (i < 3);
end

endmodule


