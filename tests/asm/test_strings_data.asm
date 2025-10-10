; String and Data Manipulation Tests
; Tests for string operations and data processing

#test "string_copy" {
    #description "Test copying a string from one location to another"
    #author "DemiEngine Team"
    #category "Strings"
    #tag "copy"
    #tag "manipulation"
    
    ; Source string "HI" at 0xA0
    LOAD_IMM R0, 72  ; 'H'
    STORE R0, 0xA0
    LOAD_IMM R0, 73  ; 'I'
    STORE R0, 0xA1
    LOAD_IMM R0, 0   ; null
    STORE R0, 0xA2
    
    ; Copy to 0xB0
    LOAD_IMM R1, 0xA0  ; Source
    LOAD_IMM R2, 0xB0  ; Dest
    
strcpy_loop:
    LOAD R3, R1
    STORE R3, R2
    
    CMP R3, 0
    JZ strcpy_done
    
    INC R1
    INC R2
    JMP strcpy_loop
    
strcpy_done:
    ; Verify copy
    LOAD R4, 0xB0
    #assert_reg R4, 72
    
    LOAD R5, 0xB1
    #assert_reg R5, 73
    
    HALT
}

#test "string_compare" {
    #description "Test comparing two strings for equality"
    #author "DemiEngine Team"
    #category "Strings"
    #tag "compare"
    #tag "equality"
    
    ; String A: "OK" at 0xC0
    LOAD_IMM R0, 79  ; 'O'
    STORE R0, 0xC0
    LOAD_IMM R0, 75  ; 'K'
    STORE R0, 0xC1
    LOAD_IMM R0, 0
    STORE R0, 0xC2
    
    ; String B: "OK" at 0xD0
    LOAD_IMM R0, 79  ; 'O'
    STORE R0, 0xD0
    LOAD_IMM R0, 75  ; 'K'
    STORE R0, 0xD1
    LOAD_IMM R0, 0
    STORE R0, 0xD2
    
    ; Compare
    LOAD_IMM R1, 0xC0
    LOAD_IMM R2, 0xD0
    LOAD_IMM R3, 1   ; Equal flag
    
strcmp_loop:
    LOAD R4, R1
    LOAD R5, R2
    
    CMP R4, R5
    JNZ strings_not_equal
    
    CMP R4, 0
    JZ strcmp_done
    
    INC R1
    INC R2
    JMP strcmp_loop
    
strings_not_equal:
    LOAD_IMM R3, 0
    
strcmp_done:
    #assert_reg R3, 1
    HALT
}

#test "string_reverse" {
    #description "Test reversing a string in place"
    #author "DemiEngine Team"
    #category "Strings"
    #tag "reverse"
    #tag "manipulation"
    
    ; String "ABC" at 0xE0-0xE3
    LOAD_IMM R0, 65  ; 'A'
    STORE R0, 0xE0
    LOAD_IMM R0, 66  ; 'B'
    STORE R0, 0xE1
    LOAD_IMM R0, 67  ; 'C'
    STORE R0, 0xE2
    LOAD_IMM R0, 0
    STORE R0, 0xE3
    
    ; Reverse: swap 0xE0 with 0xE2
    LOAD R1, 0xE0
    LOAD R2, 0xE2
    STORE R2, 0xE0
    STORE R1, 0xE2
    
    ; Verify "CBA"
    LOAD R3, 0xE0
    #assert_reg R3, 67  ; 'C'
    
    LOAD R4, 0xE2
    #assert_reg R4, 65  ; 'A'
    
    HALT
}

#test "byte_array_fill" {
    #description "Test filling a byte array with a value"
    #author "DemiEngine Team"
    #category "Data"
    #tag "fill"
    #tag "array"
    
    LOAD_IMM R0, 255  ; Fill value
    LOAD_IMM R1, 0xF0 ; Start address
    LOAD_IMM R2, 0xF5 ; End address
    
fill_loop:
    CMP R1, R2
    JZ fill_done
    
    STORE R0, R1
    INC R1
    JMP fill_loop
    
fill_done:
    ; Verify
    LOAD R3, 0xF0
    #assert_reg R3, 255
    
    LOAD R4, 0xF4
    #assert_reg R4, 255
    
    HALT
}

#test "checksum_calculation" {
    #description "Test calculating a simple checksum of data"
    #author "DemiEngine Team"
    #category "Data"
    #tag "checksum"
    #tag "validation"
    
    ; Data: [10, 20, 30, 40, 50] at 0x20-0x24
    LOAD_IMM R0, 10
    STORE R0, 0x20
    LOAD_IMM R0, 20
    STORE R0, 0x21
    LOAD_IMM R0, 30
    STORE R0, 0x22
    LOAD_IMM R0, 40
    STORE R0, 0x23
    LOAD_IMM R0, 50
    STORE R0, 0x24
    
    ; Calculate checksum (sum)
    LOAD_IMM R1, 0    ; Checksum
    LOAD_IMM R2, 0x20 ; Address
    LOAD_IMM R3, 0x25 ; End
    
checksum_loop:
    CMP R2, R3
    JZ checksum_done
    
    LOAD R4, R2
    ADD R1, R4
    
    INC R2
    JMP checksum_loop
    
checksum_done:
    ; Checksum = 10+20+30+40+50 = 150
    #assert_reg R1, 150
    HALT
}

#test "data_packing" {
    #description "Test packing multiple values into memory efficiently"
    #author "DemiEngine Team"
    #category "Data"
    #tag "packing"
    #tag "compact"
    
    ; Pack 4 values (each 2 bits: 0-3) into one byte
    ; Values: 3, 2, 1, 0 -> byte: 0b11100100 = 228
    
    LOAD_IMM R0, 3
    SHL R0, 6      ; R0 = 0b11000000
    
    LOAD_IMM R1, 2
    SHL R1, 4      ; R1 = 0b00100000
    OR R0, R1      ; R0 = 0b11100000
    
    LOAD_IMM R2, 1
    SHL R2, 2      ; R2 = 0b00000100
    OR R0, R2      ; R0 = 0b11100100
    
    LOAD_IMM R3, 0 ; R3 = 0b00000000
    OR R0, R3      ; R0 = 0b11100100 = 228
    
    #assert_reg R0, 228
    
    HALT
}

#test "data_unpacking" {
    #description "Test unpacking values from a packed byte"
    #author "DemiEngine Team"
    #category "Data"
    #tag "unpacking"
    #tag "extract"
    
    ; Packed byte: 0b11011001 = 217
    ; Extract: bits 7-6=3, bits 5-4=1, bits 3-2=2, bits 1-0=1
    
    LOAD_IMM R0, 217
    
    ; Extract bits 7-6
    MOV R1, R0
    SHR R1, 6
    #assert_reg R1, 3
    
    ; Extract bits 5-4
    MOV R2, R0
    SHR R2, 4
    LOAD_IMM R3, 3  ; Mask 0b11
    AND R2, R3
    #assert_reg R2, 1
    
    HALT
}

#test "circular_buffer_simulation" {
    #description "Test circular buffer operations"
    #author "DemiEngine Team"
    #category "Data"
    #tag "buffer"
    #tag "circular"
    
    ; Circular buffer of size 4 at 0x30-0x33
    LOAD_IMM R0, 0    ; Write index
    LOAD_IMM R1, 0    ; Read index
    LOAD_IMM R2, 4    ; Buffer size
    LOAD_IMM R3, 0x30 ; Base address
    
    ; Write value 10
    LOAD_IMM R4, 10
    MOV R5, R3
    ADD R5, R0
    STORE R4, R5
    INC R0
    MOD R0, R2  ; Wrap around
    
    ; Write value 20
    LOAD_IMM R4, 20
    MOV R5, R3
    ADD R5, R0
    STORE R4, R5
    INC R0
    MOD R0, R2
    
    ; Read first value
    MOV R5, R3
    ADD R5, R1
    LOAD R6, R5
    #assert_reg R6, 10
    
    HALT
}
