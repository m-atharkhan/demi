; ==========================================
; SWAP Opcode Test Suite
; Tests swapping values between register and memory
; NOTE: SWAP only supports R0-R7 and memory addresses < 0x200
; Format: SWAP reg, [address] - swaps reg value with memory[address]
; ==========================================

.test "swap_basic" {
    .description "Tests basic SWAP between register and memory"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "swap"
    .tag "basic"
    
    LOAD_IMM RAX, 42
    SWAP RAX, [0x100]   ; Swap RAX (42) with memory[0x100] (0)
    
    .assert_reg RAX, 0
    .assert_mem 0x100, 42
}

.test "swap_zero_values" {
    .description "Tests SWAP with zero values"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "swap"
    .tag "zero"
    
    LOAD_IMM RBX, 0
    SWAP RBX, [0x110]
    
    .assert_reg RBX, 0
    .assert_mem 0x110, 0
}

.test "swap_twice_restores" {
    .description "Tests SWAP twice restores original values"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "swap"
    .tag "twice"
    
    LOAD_IMM RCX, 77
    SWAP RCX, [0x120]  ; First swap: RCX=0, mem=77
    SWAP RCX, [0x120]  ; Second swap: RCX=77, mem=0
    
    .assert_reg RCX, 77
    .assert_mem 0x120, 0
}

.test "swap_max_value" {
    .description "Tests SWAP with maximum byte value"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "swap"
    .tag "max"
    
    LOAD_IMM RDX, 0xFF
    SWAP RDX, [0x130]  ; Swap 0xFF with 0
    
    .assert_reg RDX, 0
    .assert_mem 0x130, 0xFF
}

.test "swap_sequential" {
    .description "Tests sequential SWAP operations"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "swap"
    .tag "sequential"
    
    LOAD_IMM R5, 30
    LOAD_IMM R6, 40
    SWAP R5, [0x140]    ; R5=0, mem[0x140]=30
    SWAP R6, [0x148]    ; R6=0, mem[0x148]=40
    
    .assert_reg R5, 0
    .assert_reg R6, 0
    .assert_mem 0x140, 30
    .assert_mem 0x148, 40
}

.test "swap_power_of_two" {
    .description "Tests SWAP with power of 2 values"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "swap"
    .tag "power2"
    
    LOAD_IMM R7, 128
    SWAP R7, [0x150]
    
    .assert_reg R7, 0
    .assert_mem 0x150, 128
}

.test "swap_with_different_registers" {
    .description "Tests SWAP with different register pairs"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "swap"
    .tag "different"
    
    LOAD_IMM RAX, 10
    LOAD_IMM RBX, 20
    LOAD_IMM RCX, 30
    SWAP RAX, [0x160]
    SWAP RBX, [0x168]
    SWAP RCX, [0x170]
    
    .assert_reg RAX, 0
    .assert_reg RBX, 0
    .assert_reg RCX, 0
    .assert_mem 0x160, 10
    .assert_mem 0x168, 20
    .assert_mem 0x170, 30
}
