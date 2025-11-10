; SIMD Foundation Test Suite
; Tests for SIMD vector operations (foundation implementation)

.test "vector addition" {
    .description "Test parallel addition of vector elements using VADD"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "vector"
    .tag "addition"
    
    ; Load vector A: [10, 20, 30, 40]
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    LOAD_IMM R3, 40
    
    ; Load vector B: [1, 2, 3, 4]
    LOAD_IMM R4, 1
    LOAD_IMM R5, 2
    LOAD_IMM R6, 3
    LOAD_IMM R7, 4
    
    ; Perform vector addition: A = A + B
    VADD R0, R4
    
    ; Expected results: [11, 22, 33, 44]
    .assert_reg R0, 11
    .assert_reg R1, 22
    .assert_reg R2, 33
    .assert_reg R3, 44
}

.test "vector multiplication" {
    .description "Test parallel multiplication of vector elements using VMUL"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "vector"
    .tag "multiplication"
    
    ; Load vector A: [2, 3, 4, 5]
    LOAD_IMM R0, 2
    LOAD_IMM R1, 3
    LOAD_IMM R2, 4
    LOAD_IMM R3, 5
    
    ; Load vector B: [10, 10, 10, 10]
    LOAD_IMM R4, 10
    LOAD_IMM R5, 10
    LOAD_IMM R6, 10
    LOAD_IMM R7, 10
    
    ; Perform vector multiplication: A = A * B
    VMUL R0, R4
    
    ; Expected results: [20, 30, 40, 50]
    .assert_reg R0, 20
    .assert_reg R1, 30
    .assert_reg R2, 40
    .assert_reg R3, 50
}

.test "vector dot product" {
    .description "Test dot product of two vectors using VDOT"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "vector"
    .tag "dot-product"
    
    ; Load vector A: [1, 2, 3, 4]
    LOAD_IMM R0, 1
    LOAD_IMM R1, 2
    LOAD_IMM R2, 3
    LOAD_IMM R3, 4
    
    ; Load vector B: [5, 6, 7, 8]
    LOAD_IMM R4, 5
    LOAD_IMM R5, 6
    LOAD_IMM R6, 7
    LOAD_IMM R7, 8
    
    ; Perform dot product: result = A • B
    VDOT
    
    ; Expected: 1*5 + 2*6 + 3*7 + 4*8 = 5 + 12 + 21 + 32 = 70
    .assert_reg R0, 70
}

.test "vector max" {
    .description "Test finding maximum element in vector using VMAX"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "vector"
    .tag "max"
    
    ; Load vector: [15, 42, 8, 23]
    LOAD_IMM R0, 15
    LOAD_IMM R1, 42
    LOAD_IMM R2, 8
    LOAD_IMM R3, 23
    
    ; Find maximum value
    VMAX
    
    ; Expected: 42 (maximum value in R0 after VMAX)
    .assert_reg R0, 42
}

.test "vector broadcast" {
    .description "Test broadcasting scalar to vector using VBROADCAST"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "vector"
    .tag "broadcast"
    
    ; Load scalar value
    LOAD_IMM R0, 99
    
    ; Broadcast R0 to R4-R7
    VBROADCAST
    
    ; Expected: R4-R7 all contain 99, R0 preserved
    .assert_reg R0, 99
    .assert_reg R4, 99
    .assert_reg R5, 99
    .assert_reg R6, 99
    .assert_reg R7, 99
}

.test "packed byte operations" {
    .description "Test packing and unpacking bytes using PACKB and UNPACKB"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "packed"
    .tag "bytes"
    
    ; Load values to pack: [10, 20, 30, 40]
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    LOAD_IMM R3, 40
    
    ; Pack bytes R0-R3 into R4
    PACKB
    
    ; Now unpack R4 back to R0-R3
    UNPACKB
    
    ; Expected: R0-R3 should contain 10, 20, 30, 40
    .assert_reg R0, 10
    .assert_reg R1, 20
    .assert_reg R2, 30
    .assert_reg R3, 40
}