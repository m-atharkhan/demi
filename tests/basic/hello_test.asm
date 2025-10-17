#test "basic output test" {
    LOAD_IMM R0, 72         ; ASCII 'H'
    OUT R0, 0x01            ; Output to console
    #assert_reg R0, 72
    
    LOAD_IMM R0, 101        ; ASCII 'e'
    OUT R0, 0x01            ; Output to console
    #assert_reg R0, 101
    
    HALT
}
