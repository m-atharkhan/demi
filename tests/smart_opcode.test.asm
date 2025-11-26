.test "Smart Opcode Selection" {
    .description "Verify that MOV is promoted to MOV64 when using 64-bit registers"
    .author "DemiEngine Team"
    .category "Assembler"
    
    ; This should be encoded as MOV64 (0x52) because RAX and RBX are 64-bit
    ; RAX is R0 (index 0), RBX is R3 (index 3)
    MOV RAX, RBX
    
    ; Verify the opcode at address 0 is 0x52 (MOV64)
    .assert_mem 0, 0x52
    
    ; Verify the operands (assuming standard encoding: Opcode Dest Src)
    ; This depends on the exact encoding scheme, but checking the opcode is the primary goal
    .assert_mem 1, 0x00 ; Dest R0
    .assert_mem 2, 0x03 ; Src R3
}

