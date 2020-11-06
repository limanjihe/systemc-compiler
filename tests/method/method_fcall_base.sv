//==============================================================================
//
// The code is generated by Intel Compiler for SystemC, version 1.3.7
// see more information at https://github.com/intel/systemc-compiler
//
//==============================================================================

//==============================================================================
//
// Module: B ()
//
module B // "b_mod"
(
);

// SystemC signals

//------------------------------------------------------------------------------
// Method process: virt_meth0 (test_fcall_base.cpp:49:5) 

always_comb 
begin : virt_meth0     // test_fcall_base.cpp:49:5
    integer k;
    integer TMP_0;
    integer i;
    integer TMP_1;
    integer i_1;
    integer TMP_2;
    i = 1;
    // Call vf() begin
    TMP_0 = i + 1;
    // Call vf() end
    k = TMP_0;
    assert (k == 2) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i_1 = 1;
    // Call vf() begin
    TMP_1 = i_1 + 2;
    // Call vf() end
    k = TMP_1;
    assert (k == 3) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i_1 = 1;
    // Call vf() begin
    TMP_2 = i_1 + 2;
    // Call vf() end
    k = TMP_2;
    assert (k == 3) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
end

//------------------------------------------------------------------------------
// Method process: meth0 (test_fcall_base.cpp:60:5) 

always_comb 
begin : meth0     // test_fcall_base.cpp:60:5
    integer k;
    integer TMP_0;
    integer i;
    integer TMP_1;
    integer TMP_2;
    i = 1;
    // Call f() begin
    TMP_0 = i + 1;
    // Call f() end
    k = TMP_0;
    assert (k == 2) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i = 1;
    // Call f() begin
    TMP_1 = i + 1;
    // Call f() end
    k = TMP_1;
    assert (k == 2) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i = 1;
    // Call f() begin
    TMP_2 = i + 1;
    // Call f() end
    k = TMP_2;
    assert (k == 2) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
end

//------------------------------------------------------------------------------
// Method process: virt_meth1 (test_fcall_base.cpp:108:5) 

always_comb 
begin : virt_meth1     // test_fcall_base.cpp:108:5
    integer k;
    integer TMP_0;
    integer i;
    integer TMP_1;
    integer i_1;
    integer TMP_2;
    integer TMP_3;
    integer TMP_4;
    i = 1;
    // Call vf() begin
    TMP_0 = i + 1;
    // Call vf() end
    k = TMP_0;
    assert (k == 2) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i_1 = 1;
    // Call vf() begin
    TMP_1 = i_1 + 2;
    // Call vf() end
    k = TMP_1;
    assert (k == 3) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i_1 = 1;
    // Call vf() begin
    TMP_2 = i_1 + 2;
    // Call vf() end
    k = TMP_2;
    assert (k == 3) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i_1 = 1;
    // Call vf() begin
    TMP_3 = i_1 + 2;
    // Call vf() end
    k = TMP_3;
    assert (k == 3) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i_1 = 1;
    // Call vf() begin
    TMP_4 = i_1 + 2;
    // Call vf() end
    k = TMP_4;
    assert (k == 3) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
end

//------------------------------------------------------------------------------
// Method process: meth1 (test_fcall_base.cpp:124:5) 

always_comb 
begin : meth1     // test_fcall_base.cpp:124:5
    integer k;
    integer TMP_0;
    integer i;
    integer TMP_1;
    integer i_1;
    integer TMP_2;
    integer TMP_3;
    integer TMP_4;
    i = 1;
    // Call f() begin
    TMP_0 = i + 1;
    // Call f() end
    k = TMP_0;
    assert (k == 2) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i_1 = 1;
    // Call f() begin
    TMP_1 = i_1 + 2;
    // Call f() end
    k = TMP_1;
    assert (k == 3) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i_1 = 1;
    // Call f() begin
    TMP_2 = i_1 + 2;
    // Call f() end
    k = TMP_2;
    assert (k == 3) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i_1 = 1;
    // Call f() begin
    TMP_3 = i_1 + 2;
    // Call f() end
    k = TMP_3;
    assert (k == 3) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i = 1;
    // Call f() begin
    TMP_4 = i + 1;
    // Call f() end
    k = TMP_4;
    assert (k == 2) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
end

//------------------------------------------------------------------------------
// Method process: meth2 (test_fcall_base.cpp:140:5) 

always_comb 
begin : meth2     // test_fcall_base.cpp:140:5
    integer k;
    integer TMP_0;
    integer i;
    integer TMP_1;
    integer i_1;
    integer j;
    integer TMP_3;
    integer i_2;
    integer TMP_4;
    integer i_3;
    integer j_1;
    i = 1;
    // Call vff() begin
    i_1 = i;
    // Call vff() begin
    TMP_1 = i_1 + 1;
    // Call vff() end
    j = TMP_1;
    TMP_0 = j + 2;
    // Call vff() end
    k = TMP_0;
    assert (k == 4) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
    i_2 = 1;
    // Call ff() begin
    i_3 = i_2;
    // Call ff() begin
    TMP_4 = i_3 + 1;
    // Call ff() end
    j_1 = TMP_4;
    TMP_3 = j_1 + 2;
    // Call ff() end
    k = TMP_3;
    assert (k == 4) else $error("Assertion failed at test_fcall_base.cpp:15:20>");
end

endmodule


