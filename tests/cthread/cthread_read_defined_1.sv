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
// Module: A (test_cthread_read_defined_1.cpp:204:5)
//
module A // "b_mod.a_mod"
(
    input logic clk,
    input logic nrst,
    input logic a,
    output logic b
);

// SystemC signals
logic [3:0] s;

//------------------------------------------------------------------------------
// Method process: if_complex_stmt1 (test_cthread_read_defined_1.cpp:49:5) 

always_comb 
begin : if_complex_stmt1     // test_cthread_read_defined_1.cpp:49:5
    integer i;
    integer j;
    i = a;
end

//------------------------------------------------------------------------------
// Method process: if_complex_stmt2 (test_cthread_read_defined_1.cpp:59:5) 

always_comb 
begin : if_complex_stmt2     // test_cthread_read_defined_1.cpp:59:5
    integer i;
    integer j;
end

//------------------------------------------------------------------------------
// Method process: if_complex_stmt3 (test_cthread_read_defined_1.cpp:68:5) 

always_comb 
begin : if_complex_stmt3     // test_cthread_read_defined_1.cpp:68:5
    integer i;
    integer j;
end

//------------------------------------------------------------------------------
// Method process: if_complex_const1 (test_cthread_read_defined_1.cpp:77:5) 

always_comb 
begin : if_complex_const1     // test_cthread_read_defined_1.cpp:77:5
    integer i;
    integer j;
end

//------------------------------------------------------------------------------
// Method process: if_complex_const2 (test_cthread_read_defined_1.cpp:86:5) 

always_comb 
begin : if_complex_const2     // test_cthread_read_defined_1.cpp:86:5
    integer i;
    integer j;
end

//------------------------------------------------------------------------------
// Method process: if_complex_const3 (test_cthread_read_defined_1.cpp:95:5) 

always_comb 
begin : if_complex_const3     // test_cthread_read_defined_1.cpp:95:5
    integer i;
    integer j;
end

//------------------------------------------------------------------------------
// Method process: if_complex_const4 (test_cthread_read_defined_1.cpp:104:5) 

always_comb 
begin : if_complex_const4     // test_cthread_read_defined_1.cpp:104:5
    integer i;
    integer j;
end

//------------------------------------------------------------------------------
// Method process: if_complex_par1 (test_cthread_read_defined_1.cpp:113:5) 

always_comb 
begin : if_complex_par1     // test_cthread_read_defined_1.cpp:113:5
    integer i;
    integer j;
end

//------------------------------------------------------------------------------
// Method process: if_complex_par2 (test_cthread_read_defined_1.cpp:122:5) 

always_comb 
begin : if_complex_par2     // test_cthread_read_defined_1.cpp:122:5
    integer i;
    integer j;
    integer k;
end

//------------------------------------------------------------------------------
// Method process: if_complex_assign1 (test_cthread_read_defined_1.cpp:132:5) 

always_comb 
begin : if_complex_assign1     // test_cthread_read_defined_1.cpp:132:5
    integer i;
    integer j;
end

//------------------------------------------------------------------------------
// Method process: if_complex_assign2 (test_cthread_read_defined_1.cpp:140:5) 

always_comb 
begin : if_complex_assign2     // test_cthread_read_defined_1.cpp:140:5
    integer i;
    integer j;
end

//------------------------------------------------------------------------------
// Method process: cond_oper1 (test_cthread_read_defined_1.cpp:150:5) 

always_comb 
begin : cond_oper1     // test_cthread_read_defined_1.cpp:150:5
    integer i;
    integer j;
    integer c;
    c = i == 1 ? j : 0;
end

//------------------------------------------------------------------------------
// Method process: cond_oper2 (test_cthread_read_defined_1.cpp:158:5) 

always_comb 
begin : cond_oper2     // test_cthread_read_defined_1.cpp:158:5
    integer i;
    integer j;
    integer k;
    integer c;
    i = 1;
    c = j;
end

//------------------------------------------------------------------------------
// Method process: cond_oper3 (test_cthread_read_defined_1.cpp:168:5) 

always_comb 
begin : cond_oper3     // test_cthread_read_defined_1.cpp:168:5
    integer i;
    integer j;
    integer k;
    integer c;
    c = |s && |i ? 1 : j + k;
end

//------------------------------------------------------------------------------
// Method process: cond_oper4 (test_cthread_read_defined_1.cpp:177:5) 

always_comb 
begin : cond_oper4     // test_cthread_read_defined_1.cpp:177:5
    integer i;
    integer j;
    integer k;
    integer c;
    c = k;
end

//------------------------------------------------------------------------------
// Method process: cond_oper5 (test_cthread_read_defined_1.cpp:186:5) 

always_comb 
begin : cond_oper5     // test_cthread_read_defined_1.cpp:186:5
    integer i;
    integer j;
    integer k;
    integer c;
    c = 0;
end

endmodule


