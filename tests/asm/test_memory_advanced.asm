; Memory Management Tests
; Tests memory operations, bounds checking, and edge cases

#test "memory_boundary_access" {
    #description "Test memory access at boundaries"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "bounds"
    #tag "edge-cases"
    
    ; Store at low memory
    LOAD_IMM R0, 42
    STORE R0, 0x00
    
    ; Load from low memory
    LOAD R1, 0x00
    #assert_reg R1, 42
    
    ; Store at high memory (address 255)
    LOAD_IMM R2, 99
    STORE R2, 0xFF
    
    ; Load from high memory
    LOAD R3, 0xFF
    #assert_reg R3, 99
    
    HALT
}

#test "sequential_memory_operations" {
    #description "Test sequential memory writes and reads"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "sequential"
    #tag "pattern"
    
    ; Write sequence 10, 20, 30, 40, 50 to memory
    LOAD_IMM R0, 10
    STORE R0, 0x10
    
    LOAD_IMM R0, 20
    STORE R0, 0x11
    
    LOAD_IMM R0, 30
    STORE R0, 0x12
    
    LOAD_IMM R0, 40
    STORE R0, 0x13
    
    LOAD_IMM R0, 50
    STORE R0, 0x14
    
    ; Read back and verify
    LOAD R1, 0x10
    #assert_reg R1, 10
    
    LOAD R2, 0x12
    #assert_reg R2, 30
    
    LOAD R3, 0x14
    #assert_reg R3, 50
    
    HALT
}

#test "memory_copy_pattern" {
    #description "Test copying data from one memory location to another"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "copy"
    #tag "pattern"
    
    ; Write source data
    LOAD_IMM R0, 123
    STORE R0, 0x20
    
    ; Copy to destination
    LOAD R1, 0x20
    STORE R1, 0x30
    
    ; Verify copy
    LOAD R2, 0x30
    #assert_reg R2, 123
    
    ; Modify source
    LOAD_IMM R3, 200
    STORE R3, 0x20
    
    ; Verify destination unchanged
    LOAD R4, 0x30
    #assert_reg R4, 123
    
    HALT
}

#test "memory_swap_operation" {
    #description "Test swapping values between memory locations"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "swap"
    #tag "pattern"
    
    ; Initialize two memory locations
    LOAD_IMM R0, 10
    STORE R0, 0x40
    
    LOAD_IMM R1, 20
    STORE R1, 0x41
    
    ; Swap using temporary register
    LOAD R2, 0x40      ; R2 = 10
    LOAD R3, 0x41      ; R3 = 20
    STORE R3, 0x40     ; mem[0x40] = 20
    STORE R2, 0x41     ; mem[0x41] = 10
    
    ; Verify swap
    LOAD R4, 0x40
    #assert_reg R4, 20
    
    LOAD R5, 0x41
    #assert_reg R5, 10
    
    HALT
}

#test "memory_fill_pattern" {
    #description "Test filling memory with a pattern"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "fill"
    #tag "pattern"
    
    ; Fill 5 consecutive bytes with value 77
    LOAD_IMM R0, 77
    
    STORE R0, 0x50
    STORE R0, 0x51
    STORE R0, 0x52
    STORE R0, 0x53
    STORE R0, 0x54
    
    ; Verify pattern
    LOAD R1, 0x50
    #assert_reg R1, 77
    
    LOAD R2, 0x52
    #assert_reg R2, 77
    
    LOAD R3, 0x54
    #assert_reg R3, 77
    
    HALT
}

#test "memory_zero_initialization" {
    #description "Test memory is properly initialized to zero"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "initialization"
    #tag "zero"
    
    ; Read uninitialized memory (should be 0)
    LOAD R0, 0x60
    #assert_reg R0, 0
    
    LOAD R1, 0x61
    #assert_reg R1, 0
    
    ; Write and verify
    LOAD_IMM R2, 55
    STORE R2, 0x60
    
    LOAD R3, 0x60
    #assert_reg R3, 55
    
    HALT
}

#test "memory_overwrite_test" {
    #description "Test overwriting memory locations multiple times"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "overwrite"
    #tag "modification"
    
    ; Write initial value
    LOAD_IMM R0, 100
    STORE R0, 0x70
    
    ; Overwrite with new value
    LOAD_IMM R1, 200
    STORE R1, 0x70
    
    ; Verify overwrite
    LOAD R2, 0x70
    #assert_reg R2, 200
    
    ; Overwrite again
    LOAD_IMM R3, 50
    STORE R3, 0x70
    
    ; Verify final value
    LOAD R4, 0x70
    #assert_reg R4, 50
    
    HALT
}

#test "memory_register_isolation" {
    #description "Test that memory and registers are properly isolated"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "isolation"
    #tag "registers"
    
    ; Set register value
    LOAD_IMM R0, 111
    
    ; Store to memory
    STORE R0, 0x80
    
    ; Modify register
    LOAD_IMM R0, 222
    
    ; Verify memory unchanged
    LOAD R1, 0x80
    #assert_reg R1, 111
    
    ; Verify register has new value
    #assert_reg R0, 222
    
    HALT
}
