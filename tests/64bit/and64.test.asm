; 64-bit Logical AND Tests  
; Tests for AND64 instruction with various scenarios

.test "and64 basic bitwise and" {
    .description "Basic 64-bit bitwise AND test"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "and64"
    
    LOAD_IMM R0, 15      ; Binary: 1111
    LOAD_IMM R1, 7       ; Binary: 0111
    AND64 R2, R0, R1     ; R2 = 1111 & 0111 = 0111 = 7
    HALT
    
    .assert_reg R2, 7
}

.test "and64 all bits set" {
    .description "Test AND with all bits set"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "and64"
    .tag "all-bits"
    
    LOAD_IMM R0, 255     ; All 8 bits set
    LOAD_IMM R1, 255     ; All 8 bits set
    AND64 R2, R0, R1     ; R2 = 255 & 255 = 255
    HALT
    
    .assert_reg R2, 255
}

.test "and64 with zero" {
    .description "Test AND with zero (should always be zero)"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "and64"
    .tag "zero"
    
    LOAD_IMM64 R0, 12345
    LOAD_IMM R1, 0
    AND64 R2, R0, R1     ; R2 = 12345 & 0 = 0
    HALT
    
    .assert_reg R2, 0
}

.test "and64 mask operation" {
    .description "Test AND as bit mask operation"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "and64"
    .tag "mask"
    
    LOAD_IMM R0, 170     ; Binary: 10101010
    LOAD_IMM R1, 85      ; Binary: 01010101 (complementary mask)
    AND64 R2, R0, R1     ; R2 = 10101010 & 01010101 = 00000000 = 0
    HALT
    
    .assert_reg R2, 0
}

.test "and64 partial mask" {
    .description "Test AND with partial bit mask"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "and64"
    .tag "partial-mask"
    
    LOAD_IMM R0, 255     ; Binary: 11111111
    LOAD_IMM R1, 15      ; Binary: 00001111 (lower 4 bits mask)
    AND64 R2, R0, R1     ; R2 = 11111111 & 00001111 = 00001111 = 15
    HALT
    
    .assert_reg R2, 15
}

.test "and64 large numbers" {
    .description "Test AND with large 64-bit numbers"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "and64"
    .tag "large-values"
    
    LOAD_IMM64 R0, 1048575  ; Binary: many bits set
    LOAD_IMM64 R1, 524287   ; Binary: overlapping bit pattern
    AND64 R2, R0, R1        ; R2 = bitwise AND result
    HALT
    
    ; 1048575 = 0xFFFFF (20 bits set)
    ; 524287  = 0x7FFFF (19 bits set)  
    ; Result  = 0x7FFFF = 524287
    .assert_reg R2, 524287
}