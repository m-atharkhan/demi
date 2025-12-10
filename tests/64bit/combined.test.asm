; Comprehensive 64-bit Operations Tests
; Combined test scenarios using multiple 64-bit operations

.memory 8192  ; Allocate 8KB of memory for larger address range

.test "combined mul64 and storex" {
    .description "Test multiplication followed by extended store"
    .author "bobrossrtx"
    .category "64-bit Operations"
    .tag "combined"
    .tag "mul64"
    .tag "storex"
    
    LOAD_IMM64 EAX, 1000
    LOAD_IMM64 EBX, 500
    MUL64 ECX, EAX, EBX      ; ECX = 1000 * 500 = 500,000
    STOREX ECX, 1000       ; Store result to memory
    LOADEX EDX, 1000       ; Load back to verify
    HALT
    
    .assert_reg EDX, 500000
}

.test "combined div64 and loadex" {
    .description "Test division followed by extended load operations"
    .author "bobrossrtx"
    .category "64-bit Operations"
    .tag "combined"
    .tag "div64"
    .tag "loadex"
    
    ; Store dividend in memory
    LOAD_IMM64 EAX, 100000
    STOREX EAX, 2000
    
    ; Load dividend and perform division
    LOADEX EBX, 2000
    LOAD_IMM64 ECX, 1000
    DIV64 EDX, EBX, ECX      ; EDX = 100,000 / 1,000 = 100
    HALT
    
    .assert_reg EDX, 100
}

.test "combined and64 with memory ops" {
    .description "Test bitwise AND with memory operations"
    .author "bobrossrtx"
    .category "64-bit Operations"
    .tag "combined"
    .tag "and64"
    .tag "memory"
    
    ; Store mask in memory
    LOAD_IMM64 EAX, 255      ; Mask: all lower 8 bits set
    STOREX EAX, 3000
    
    ; Load mask and apply to value
    LOAD_IMM64 EBX, 511      ; Value: 9 bits set (0x1FF)
    LOADEX ECX, 3000       ; Load mask
    AND64 EDX, EBX, ECX      ; EDX = 511 & 255 = 255
    HALT
    
    .assert_reg EDX, 255
}

.test "64bit arithmetic chain" {
    .description "Test chained 64-bit arithmetic operations"
    .author "bobrossrtx"
    .category "64-bit Operations"
    .tag "combined"
    .tag "chain"
    
    ; Start with base values
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 50
    
    ; Chain of operations: (100 * 50) / 25 = 200
    MUL64 ECX, EAX, EBX      ; ECX = 100 * 50 = 5,000
    LOAD_IMM EDX, 25
    DIV64 ESI, ECX, EDX      ; ESI = 5,000 / 25 = 200
    HALT
    
    .assert_reg ESI, 200
}

.test "64bit memory stress test" {
    .description "Store and load multiple 64-bit values"
    .author "bobrossrtx"
    .category "64-bit Operations"
    .tag "combined"
    .tag "stress"
    .tag "memory"
    
    ; Store multiple values
    LOAD_IMM EAX, 123
    STOREX EAX, 1000
    LOAD_IMM EAX, 200
    STOREX EAX, 2000
    LOAD_IMM EAX, 111
    STOREX EAX, 3000
    
    ; Load and verify second value
    LOADEX EBX, 2000
    HALT
    
    .assert_reg EBX, 200
}

.test "64bit boundary test" {
    .description "Test operations near reasonable value boundaries"
    .author "bobrossrtx"
    .category "64-bit Operations"
    .tag "combined"
    .tag "boundary"
    .tag "large-values"
    
    ; Test with moderately large numbers that won't overflow
    LOAD_IMM64 EAX, 100000
    LOAD_IMM64 EBX, 1000
    MUL64 ECX, EAX, EBX      ; ECX = 100,000,000
    
    ; Verify we can store and retrieve this large value
    STOREX ECX, 4000
    LOADEX EDX, 4000
    HALT
    
    .assert_reg EDX, 100000000
}