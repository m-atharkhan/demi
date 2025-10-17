; String manipulation operations
#test "test_string_copy_simple" {
    #description "Test simple string copy operation"
    #category "Debug" 
    #tag "strings"
    #tag "memory"
    
    ; Setup source string "AB" at 0x80
    LOAD_IMM R0, 65     ; 'A'
    STORE R0, 0x80
    LOAD_IMM R0, 66     ; 'B' 
    STORE R0, 0x81
    LOAD_IMM R0, 0      ; null terminator
    STORE R0, 0x82
    
    ; Copy to destination at 0x90
    LOAD R1, 0x80       ; Load 'A'
    STORE R1, 0x90      ; Store 'A'
    
    LOAD R1, 0x81       ; Load 'B'
    STORE R1, 0x91      ; Store 'B'
    
    LOAD R1, 0x82       ; Load null
    STORE R1, 0x92      ; Store null
    
    ; Verify copy worked
    LOAD R2, 0x90       ; Should be 'A' (65)
    LOAD R3, 0x91       ; Should be 'B' (66)
    LOAD R4, 0x92       ; Should be 0
    
    #assert_reg R2, 65
    #assert_reg R3, 66
    #assert_reg R4, 0
    
    HALT
}