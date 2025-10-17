#test "test_direct_addressing" {
    #description "Test direct memory addressing modes"
    #category "Debug"
    #tag "memory"
    
    ; Setup test data
    LOAD_IMM R0, 123
    STORE R0, 0x50
    LOAD_IMM R0, 456  
    STORE R0, 0x51
    
    ; Test direct addressing with immediate addresses
    LOAD R1, 0x50     ; Should load 123
    LOAD R2, 0x51     ; Should load 456
    
    #assert_reg R1, 123
    #assert_reg R2, 456
    HALT
}

#test "test_indirect_addressing" {
    #description "Test indirect memory addressing modes"
    #category "Debug"
    #tag "memory"
    
    ; Setup test data
    LOAD_IMM R0, 123
    STORE R0, 0x60
    LOAD_IMM R0, 456
    STORE R0, 0x61
    
    ; Test indirect addressing with register addresses
    LOAD_IMM R3, 0x60
    LOAD R4, R3       ; Should load from address in R3 (0x60) = 123
    
    LOAD_IMM R3, 0x61
    LOAD R5, R3       ; Should load from address in R3 (0x61) = 456
    
    #assert_reg R4, 123
    #assert_reg R5, 456
    HALT
}