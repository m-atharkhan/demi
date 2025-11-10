; FPU Floating-Point Test Suite
; Tests for floating-point arithmetic operations

.test "basic FPU load and store" {
    .description "Test basic FLD and FST operations"
    .author "DemiEngine Team"
    .category "FPU"
    .tag "basic"
    .tag "load"
    .tag "store"
    
    ; Initialize FPU
    FINIT
    
    ; Load immediate value onto FPU stack
    FLD 50
    
    ; Store to memory address 0x80
    FST 0x80
    
    ; Verify CPU still works after FPU operations
    LOAD_IMM R1, 50
    .assert_reg R1, 50
}

.test "floating-point addition" {
    .description "Test floating-point addition using FADD"
    .author "DemiEngine Team"
    .category "FPU"
    .tag "arithmetic"
    .tag "addition"
    
    ; Initialize FPU
    FINIT
    
    ; Test FADD: 10 + 5 = 15
    FLD 10
    FADD 5
    FST 0x100
    
    ; Verify CPU still works
    LOAD_IMM R0, 123
    .assert_reg R0, 123
}

.test "floating-point subtraction" {
    .description "Test floating-point subtraction using FSUB"
    .author "DemiEngine Team"
    .category "FPU"
    .tag "arithmetic"
    .tag "subtraction"
    
    ; Initialize FPU
    FINIT
    
    ; Test FSUB: 20 - 8 = 12
    FLD 20
    FSUB 8
    FST 0x108
    
    ; Verify CPU still works
    LOAD_IMM R0, 200
    .assert_reg R0, 200
}

.test "floating-point multiplication" {
    .description "Test floating-point multiplication using FMUL"
    .author "DemiEngine Team"
    .category "FPU"
    .tag "arithmetic"
    .tag "multiplication"
    
    ; Initialize FPU
    FINIT
    
    ; Test FMUL: 6 * 7 = 42
    FLD 6
    FMUL 7
    FST 0x110
    
    ; Verify CPU still works
    LOAD_IMM R0, 42
    .assert_reg R0, 42
}

.test "floating-point division" {
    .description "Test floating-point division using FDIV"
    .author "DemiEngine Team"
    .category "FPU"
    .tag "arithmetic"
    .tag "division"
    
    ; Initialize FPU
    FINIT
    
    ; Test FDIV: 100 / 4 = 25
    FLD 100
    FDIV 4
    FST 0x118
    
    ; Verify CPU still works
    LOAD_IMM R0, 100
    .assert_reg R0, 100
}

.test "trigonometric functions" {
    .description "Test trigonometric functions FSIN and FCOS"
    .author "DemiEngine Team"
    .category "FPU"
    .tag "trigonometry"
    .tag "transcendental"
    
    FINIT
    
    ; Test FSIN: sin(0) = 0
    FLD 0
    FSIN
    FST 0x200
    
    ; Test FCOS: cos(0) = 1
    FLD 0
    FCOS
    FST 0x208
    
    ; Verify CPU still functional
    LOAD_IMM R1, 0
    LOAD_IMM R2, 1
    .assert_reg R1, 0
    .assert_reg R2, 1
}

.test "square root function" {
    .description "Test square root function using FSQRT"
    .author "DemiEngine Team"
    .category "FPU"
    .tag "mathematical"
    .tag "sqrt"
    
    FINIT
    
    ; Test FSQRT: sqrt(16.0) = 4.0
    FLD 16
    FSQRT
    FST 0x160
    
    ; Test FSQRT: sqrt(25.0) = 5.0
    FLD 25
    FSQRT
    FST 0x168
    
    ; Verify CPU still works
    LOAD_IMM R1, 8
    .assert_reg R1, 8
}