; ==========================================
; LEA (Load Effective Address) Opcode Test Suite
; Tests loading addresses into registers
; NOTE: LEA only supports R0-R7 (RAX-R7)
; ==========================================

.test "lea_basic" {
    .description "Tests basic LEA operation"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "lea"
    .tag "basic"
    
    LEA RAX, 0x100
    
    .assert_reg RAX, 0x100
}

.test "lea_zero_address" {
    .description "Tests LEA with address zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "lea"
    .tag "zero"
    
    LEA RBX, 0
    
    .assert_reg RBX, 0
}

.test "lea_high_address" {
    .description "Tests LEA with high memory address"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "lea"
    .tag "high"
    
    LEA RCX, 0xFFFF
    
    .assert_reg RCX, 0xFFFF
}

.test "lea_sequential_addresses" {
    .description "Tests LEA loading sequential addresses"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "lea"
    .tag "sequential"
    
    LEA RAX, 0x1000
    LEA RBX, 0x1008
    LEA RCX, 0x1010
    
    .assert_reg RAX, 0x1000
    .assert_reg RBX, 0x1008
    .assert_reg RCX, 0x1010
}

.test "lea_pointer_arithmetic_base" {
    .description "Tests LEA as base for pointer arithmetic"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "lea"
    .tag "pointer"
    
    LEA RDX, 0x2000
    LOAD_IMM R5, 16
    MOV64 R6, RDX
    ADD64 R6, R5  ; R6 = RDX + R5 = 0x2000 + 16 = 0x2010
    
    .assert_reg R6, 0x2010
}

.test "lea_overwrite" {
    .description "Tests LEA overwriting previous register value"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "lea"
    .tag "overwrite"
    
    LOAD_IMM R6, 999
    LEA R6, 0x5000
    
    .assert_reg R6, 0x5000
}

.test "lea_all_registers" {
    .description "Tests LEA with multiple registers (R0-R7)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "lea"
    .tag "multiple"
    
    LEA RAX, 0x100
    LEA RBX, 0x200
    LEA RCX, 0x300
    LEA RDX, 0x400
    
    .assert_reg RAX, 0x100
    .assert_reg RBX, 0x200
    .assert_reg RCX, 0x300
    .assert_reg RDX, 0x400
}

.test "lea_power_of_two" {
    .description "Tests LEA with power of 2 addresses"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "lea"
    .tag "power2"
    
    LEA R7, 256
    LEA R6, 512
    LEA R5, 1024
    
    .assert_reg R7, 256
    .assert_reg R6, 512
    .assert_reg R5, 1024
}
