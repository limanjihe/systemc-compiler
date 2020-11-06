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
logic c;


//------------------------------------------------------------------------------

A a_mod
(
  .a(a),
  .b(b),
  .c(c)
);

endmodule



//==============================================================================
//
// Module: A (test_dowhile.cpp:161:5)
//
module A // "b_mod.a_mod"
(
    input logic a,
    output logic b,
    output logic c
);

// SystemC signals

//------------------------------------------------------------------------------
// Method process: do_while1 (test_dowhile.cpp:40:5) 

always_comb 
begin : do_while1     // test_dowhile.cpp:40:5
    integer i;
    i = 0;
    do
    begin
        i++;
    end
    while (i < 2);
end

//------------------------------------------------------------------------------
// Method process: do_while2 (test_dowhile.cpp:49:5) 

// Process-local variables
logic signed [31:0] m;

always_comb 
begin : do_while2     // test_dowhile.cpp:49:5
    integer j;
    integer i;
    j = 1;
    i = 0;
    do
    begin
        i++;
        if (m > 0)
        begin
            j = 2;
        end else begin
            j = 3;
        end
    end
    while (i < 3);
    j = 4;
end

//------------------------------------------------------------------------------
// Method process: do_while3 (test_dowhile.cpp:66:5) 

// Process-local variables
logic signed [31:0] k;
logic signed [31:0] m;

always_comb 
begin : do_while3     // test_dowhile.cpp:66:5
    integer i;
    integer j;
    i = 0;
    j = 1;
    k = 0;
    if (m > 0)
    begin
        j = 2;
    end else begin
        j = 3;
    end
    do
    begin
        k++;
        i++;
    end
    while (i < 2);
    j = 4;
end

//------------------------------------------------------------------------------
// Method process: do_while4 (test_dowhile.cpp:83:5) 

// Process-local variables
logic signed [31:0] k;
logic signed [31:0] m;

always_comb 
begin : do_while4     // test_dowhile.cpp:83:5
    integer i;
    integer j;
    i = 0;
    j = 1;
    k = 0;
    if (m > 0)
    begin
        j = 2;
    end
    do
    begin
        k++;
        i++;
    end
    while (i < 2);
    j = 3;
end

//------------------------------------------------------------------------------
// Method process: do_while5 (test_dowhile.cpp:99:5) 

// Process-local variables
logic signed [31:0] k;

always_comb 
begin : do_while5     // test_dowhile.cpp:99:5
    integer i;
    integer j;
    k = 0;
    i = 0;
    do
    begin
        j = 0;
        i++;
        do
        begin
            k = k + 1;
            j++;
        end
        while (j < 3);
    end
    while (i < 2);
end

//------------------------------------------------------------------------------
// Method process: do_while6 (test_dowhile.cpp:114:5) 

// Process-local variables
logic signed [31:0] k;
logic signed [31:0] m;

always_comb 
begin : do_while6     // test_dowhile.cpp:114:5
    integer i;
    k = 0;
    i = 0;
    if (m > 0)
    begin
        do
        begin
            k = k + 1;
            i++;
        end
        while (i < 2);
    end else begin
        k = 3;
    end
end

//------------------------------------------------------------------------------
// Method process: do_while_fcall (test_dowhile.cpp:145:5) 

// Process-local variables
logic signed [31:0] k;

always_comb 
begin : do_while_fcall     // test_dowhile.cpp:145:5
    integer TMP_0;
    integer i;
    k = 0;
    do
    begin
        i = k;
        // Call f() begin
        TMP_0 = i + 1;
        // Call f() end
        k = TMP_0;
    end
    while (k < 3);
end

endmodule


