; Comprehensive 64-bit Operations Tests
; Combined test scenarios using multiple 64-bit operations

.memory 8192  ; Allocate 8KB of memory for larger address range

.test "combined mul64 and storex" {
    .description "Test multiplication followed by extended store"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "combined"
    .tag "mul64"
    .tag "storex"
    
    LOAD_IMM64 R0, 1000
    LOAD_IMM64 R1, 500
    MUL64 R2, R0, R1      ; R2 = 1000 * 500 = 500,000
    STOREX R2, 1000       ; Store result to memory
    LOADEX R3, 1000       ; Load back to verify
    HALT
    
    .assert_reg R3, 500000
}

.test "combined div64 and loadex" {
    .description "Test division followed by extended load operations"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "combined"
    .tag "div64"
    .tag "loadex"
    
    ; Store dividend in memory
    LOAD_IMM64 R0, 100000
    STOREX R0, 2000
    
    ; Load dividend and perform division
    LOADEX R1, 2000
    LOAD_IMM64 R2, 1000
    DIV64 R3, R1, R2      ; R3 = 100,000 / 1,000 = 100
    HALT
    
    .assert_reg R3, 100
}

.test "combined and64 with memory ops" {
    .description "Test bitwise AND with memory operations"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "combined"
    .tag "and64"
    .tag "memory"
    
    ; Store mask in memory
    LOAD_IMM64 R0, 255      ; Mask: all lower 8 bits set
    STOREX R0, 3000
    
    ; Load mask and apply to value
    LOAD_IMM64 R1, 511      ; Value: 9 bits set (0x1FF)
    LOADEX R2, 3000       ; Load mask
    AND64 R3, R1, R2      ; R3 = 511 & 255 = 255
    HALT
    
    .assert_reg R3, 255
}

.test "64bit arithmetic chain" {
    .description "Test chained 64-bit arithmetic operations"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "combined"
    .tag "chain"
    
    ; Start with base values
    LOAD_IMM R0, 100
    LOAD_IMM R1, 50
    
    ; Chain of operations: (100 * 50) / 25 = 200
    MUL64 R2, R0, R1      ; R2 = 100 * 50 = 5,000
    LOAD_IMM R3, 25
    DIV64 R4, R2, R3      ; R4 = 5,000 / 25 = 200
    HALT
    
    .assert_reg R4, 200
}

.test "64bit memory stress test" {
    .description "Store and load multiple 64-bit values"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "combined"
    .tag "stress"
    .tag "memory"
    
    ; Store multiple values
    LOAD_IMM R0, 123
    STOREX R0, 1000
    LOAD_IMM R0, 200
    STOREX R0, 2000
    LOAD_IMM R0, 111
    STOREX R0, 3000
    
    ; Load and verify second value
    LOADEX R1, 2000
    HALT
    
    .assert_reg R1, 200
}

.test "64bit boundary test" {
    .description "Test operations near reasonable value boundaries"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "combined"
    .tag "boundary"
    .tag "large-values"
    
    ; Test with moderately large numbers that won't overflow
    LOAD_IMM64 R0, 100000
    LOAD_IMM64 R1, 1000
    MUL64 R2, R0, R1      ; R2 = 100,000,000
    
    ; Verify we can store and retrieve this large value
    STOREX R2, 4000
    LOADEX R3, 4000
    HALT
    
    .assert_reg R3, 100000000
}