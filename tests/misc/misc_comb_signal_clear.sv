//==============================================================================
//
// The code is generated by Intel Compiler for SystemC, version 1.3.7
// see more information at https://github.com/intel/systemc-compiler
//
//==============================================================================

//==============================================================================
//
// Module: comb_signal_module ()
//
module comb_signal_module // "tb.mod"
(
    input logic clk,
    input logic nrst
);

// SystemC signals
logic cs;
logic csp;
logic csa[2];
logic cspa[2];

//------------------------------------------------------------------------------
// Clocked THREAD: thrdProc (test_comb_signal_clear.cpp:37:5) 

// Next-state combinational logic
always_comb begin : thrdProc_comb     // test_comb_signal_clear.cpp:37:5
    thrdProc_func;
end
function void thrdProc_func;
    cs = '0;
    csa = '{default:0};
    csp = '0;
    cspa = '{default:0};
    cs = 5;
    csa[0] = 7;
    csp = 6;
    for (integer i = 0; i < 2; i++)
    begin
        csa[i] = 7;
        cspa[i] = 8;
    end
endfunction

// Syncrhonous register update
always_ff @(posedge clk or negedge nrst) 
begin : thrdProc_ff
    if ( ~nrst ) begin
    end
    else begin
    end
end

endmodule


