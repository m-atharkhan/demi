; ==========================================
; INC64 Opcode Test Suite
; Tests 64-bit increment operation
; ==========================================

.test "inc64_from_zero" {
    .description "Tests incrementing from zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "inc64"
    .tag "zero"
    
    LOAD_IMM RAX, 0
    INC64 RAX
    
    .assert_reg RAX, 1
}

.test "inc64_basic" {
    .description "Tests basic increment"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "inc64"
    .tag "basic"
    
    LOAD_IMM RBX, 99
    INC64 RBX
    
    .assert_reg RBX, 100
}

.test "inc64_multiple" {
    .description "Tests multiple sequential increments"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "inc64"
    .tag "sequential"
    
    LOAD_IMM RCX, 0
    INC64 RCX
    INC64 RCX
    INC64 RCX
    INC64 RCX
    INC64 RCX
    
    .assert_reg RCX, 5
}

.test "inc64_in_loop" {
    .description "Tests increment in loop (pointer increment pattern)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "inc64"
    .tag "loop"
    
    LOAD_IMM RDX, 100
    LOAD_IMM R8, 10
loop_start:
    INC64 RDX
    DEC64 R8
    CMP R8, 0
    JNZ loop_start
    
    .assert_reg RDX, 110
}

.test "inc64_extended_registers" {
    .description "Tests INC64 with R8-R15 extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "inc64"
    .tag "extended"
    
    LOAD_IMM R9, 999
    INC64 R9
    
    .assert_reg R9, 1000
}

.test "inc64_large_value" {
    .description "Tests incrementing large value"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "inc64"
    .tag "large"
    
    LOAD_IMM R10, 999999
    INC64 R10
    
    .assert_reg R10, 1000000
}

.test "inc64_power_of_two_boundary" {
    .description "Tests incrementing at power of 2 boundary"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "inc64"
    .tag "power2"
    
    LOAD_IMM R11, 255
    INC64 R11
    
    .assert_reg R11, 256
}

.test "inc64_counter_pattern" {
    .description "Tests INC64 in counter accumulation pattern"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "inc64"
    .tag "counter"
    
    LOAD_IMM R12, 0
    LOAD_IMM R13, 7
count:
    INC64 R12
    DEC64 R13
    CMP R13, 0
    JNZ count
    
    .assert_reg R12, 7
}

.test "inc64_consecutive_values" {
    .description "Tests incrementing to produce consecutive values"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "inc64"
    .tag "consecutive"
    
    LOAD_IMM R14, 50
    INC64 R14  ; 51
    INC64 R14  ; 52
    INC64 R14  ; 53
    
    .assert_reg R14, 53
}

.test "inc64_from_one" {
    .description "Tests incrementing from one"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "inc64"
    .tag "basic"
    
    LOAD_IMM R15, 1
    INC64 R15
    
    .assert_reg R15, 2
}
