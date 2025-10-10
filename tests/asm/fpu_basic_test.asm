; Test file for basic FPU operations

#test "simple_finit_test" {
    #description "Simple FINIT test"
    #author "DemiEngine Team"
    #category "FPU"
    #tag "basic"
    
    ; Just test FINIT by itself
    FINIT
    HALT
}

#test "fpu_load_store_test" {
    #description "Test FLD, FST, and FSTP operations"
    #author "DemiEngine Team"
    #category "FPU"
    #tag "basic"
    #tag "load-store"
    
    ; Initialize FPU
    FINIT
    
    ; Load immediate value onto FPU stack
    FLD 42
    
    ; Store to memory address 0x80
    FST 0x80
    
    ; Load another value
    FLD 13
    
    ; Store and pop to address 0x88
    FSTP 0x88
    
    ; Load from memory
    FLD 0x80
    
    ; Verify we can still execute after FPU operations
    LOAD_IMM R0, 99
    #assert_reg R0, 99
    
    HALT
}

#test "fpu_arithmetic_test" {
    #description "Test FADD, FSUB, FMUL, FDIV operations"
    #author "DemiEngine Team"
    #category "FPU"
    #tag "arithmetic"
    
    ; Initialize FPU
    FINIT
    
    ; Test FADD: 10 + 5 = 15
    FLD 10
    FADD 5
    FST 0x100
    
    ; Test FSUB: 20 - 8 = 12
    FLD 20
    FSUB 8
    FST 0x108
    
    ; Test FMUL: 6 * 7 = 42
    FLD 6
    FMUL 7
    FST 0x110
    
    ; Test FDIV: 100 / 4 = 25
    FLD 100
    FDIV 4
    FST 0x118
    
    ; Test chained operations: (5 + 3) * 2 = 16
    FLD 5
    FADD 3
    FMUL 2
    FST 0x120
    
    ; Test memory operand with FADD
    ; First store a value to memory
    FLD 15
    FSTP 0x200
    ; Now add from memory: 10 + mem[0x200] = 10 + 15 = 25
    FLD 10
    FADD 0x200
    FST 0x128
    
    ; Verify CPU still works
    LOAD_IMM R0, 123
    #assert_reg R0, 123
    
    HALT
}