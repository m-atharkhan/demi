; Test different number format parsing
#test "test_binary_format" {
    #description "Test binary number format parsing"
    #category "Debug"
    #tag "formats"
    
    LOAD_IMM R0, 0b10101010  ; 170 in binary
    #assert_reg R0, 170
    HALT
}

#test "test_hex_format" {
    #description "Test hexadecimal number format parsing"  
    #category "Debug"
    #tag "formats"
    
    LOAD_IMM R0, 0xAA        ; 170 in hex
    #assert_reg R0, 170
    HALT
}

#test "test_decimal_format" {
    #description "Test decimal number format parsing"
    #category "Debug" 
    #tag "formats"
    
    LOAD_IMM R0, 170         ; 170 in decimal
    #assert_reg R0, 170
    HALT
}