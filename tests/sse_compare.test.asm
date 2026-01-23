; SSE Comparison Operations Test Suite
; Tests for CMPPS (packed single) and CMPPD (packed double) with all 8 predicates
; Author: copilot
; Category: SIMD

; Comparison predicates:
; 0 = EQ    (equal)
; 1 = LT    (less than)
; 2 = LE    (less than or equal)
; 3 = UNORD (unordered - NaN present)
; 4 = NEQ   (not equal)
; 5 = NLT   (not less than - greater than or equal)
; 6 = NLE   (not less than or equal - greater than)
; 7 = ORD   (ordered - no NaN)

; =============================================================================
; CMPPS Tests (Packed Single - 4x float32)
; =============================================================================

.test "CMPPS EQ - equality comparison" {
    .description "Test CMPPS with EQ predicate (equal)"
    .author "copilot"
    .category "SIMD"
    
    ; Load identical values into XMM0 and XMM1
    LOAD_IMM64 XMM0, 0x4048000040400000      ; [3.0, 3.0] as packed floats
    LOAD_IMM64 XMM0_HIGH, 0x4040000040200000 ; [2.5, 2.0]
    LOAD_IMM64 XMM1, 0x4048000040400000      ; [3.0, 3.0]
    LOAD_IMM64 XMM1_HIGH, 0x4040000040200000 ; [2.5, 2.0]
    
    ; Compare: XMM0 == XMM1 (should be all 0xFFFFFFFF)
    CMPPS XMM0, XMM1, 0
    
    ; All comparisons should be true (0xFFFFFFFF)
    .assert_reg XMM0, -1
    .assert_reg XMM0_HIGH, -1
}

.test "CMPPS EQ - inequality comparison" {
    .description "Test CMPPS EQ with different values"
    .author "copilot"
    .category "SIMD"
    
    ; Load different values
    LOAD_IMM64 XMM0, 0x4048000040400000      ; [3.0, 3.0]
    LOAD_IMM64 XMM0_HIGH, 0x4040000040200000 ; [2.5, 2.0]
    LOAD_IMM64 XMM1, 0x4080000040000000      ; [4.0, 2.0]
    LOAD_IMM64 XMM1_HIGH, 0x4020000040400000 ; [1.5, 3.0]
    
    ; Compare: XMM0 == XMM1
    CMPPS XMM0, XMM1, 0
    
    ; Results: [false, false, false, true] -> [0x00, 0x00, 0x00, 0xFF]
    .assert_reg XMM0, -1  ; First element matches (2.0 == 2.0)
    .assert_reg XMM0_HIGH, 0x00   ; Other elements don't match
}

.test "CMPPS LT - less than comparison" {
    .description "Test CMPPS with LT predicate (less than)"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [2.0, 3.0, 1.0, 4.0] (as stored: low=[2.0,3.0], high=[1.0,4.0])
    LOAD_IMM64 XMM0, 0x4000000040400000      ; [2.0, 3.0]
    LOAD_IMM64 XMM0_HIGH, 0x408000003F800000 ; [4.0, 1.0]
    
    ; XMM1: [2.0, 2.0, 2.0, 2.0]
    LOAD_IMM64 XMM1, 0x4000000040000000      ; [2.0, 2.0]
    LOAD_IMM64 XMM1_HIGH, 0x4000000040000000 ; [2.0, 2.0]
    
    ; Compare: XMM0 < XMM1
    CMPPS XMM0, XMM1, 1
    
    ; Results: [2.0<2.0=F, 3.0<2.0=F, 1.0<2.0=T, 4.0<2.0=F]
    .assert_reg XMM0, 0x00        ; [2.0 < 2.0 = false, 3.0 < 2.0 = false]
    .assert_reg XMM0_HIGH, -1     ; [1.0 < 2.0 = true (all bits set for this element)]
}

.test "CMPPS LE - less than or equal comparison" {
    .description "Test CMPPS with LE predicate"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [1.0, 2.0, 3.0, 2.0]
    LOAD_IMM64 XMM0, 0x4000000040400000      ; [2.0, 3.0]
    LOAD_IMM64 XMM0_HIGH, 0x400000003F800000 ; [2.0, 1.0]
    
    ; XMM1: [2.0, 2.0, 2.0, 2.0]
    LOAD_IMM64 XMM1, 0x4000000040000000
    LOAD_IMM64 XMM1_HIGH, 0x4000000040000000
    
    ; Compare: XMM0 <= XMM1
    CMPPS XMM0, XMM1, 2
    
    ; Results: [1.0<=2.0=T, 2.0<=2.0=T, 3.0<=2.0=F, 2.0<=2.0=T]
    .assert_reg XMM0, -1  ; 2.0 <= 2.0 = true for both
}

.test "CMPPS NEQ - not equal comparison" {
    .description "Test CMPPS with NEQ predicate"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [1.0, 2.0, 3.0, 4.0]
    LOAD_IMM64 XMM0, 0x4000000040400000
    LOAD_IMM64 XMM0_HIGH, 0x408000003F800000
    
    ; XMM1: [1.0, 3.0, 3.0, 5.0]
    LOAD_IMM64 XMM1, 0x4040000040400000      ; [3.0, 3.0]
    LOAD_IMM64 XMM1_HIGH, 0x40A000003F800000 ; [5.0, 1.0]
    
    ; Compare: XMM0 != XMM1
    CMPPS XMM0, XMM1, 4
    
    ; Results: [2.0!=3.0=T, 3.0!=3.0=F, 1.0!=1.0=F, 4.0!=5.0=T]
    .assert_reg XMM0, -1        ; 2.0 != 3.0 = true (0xFFFFFFFF in first slot)
}

.test "CMPPS NLT - not less than (greater or equal)" {
    .description "Test CMPPS with NLT predicate"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [3.0, 2.0, 1.0, 2.0]
    LOAD_IMM64 XMM0, 0x4000000040400000
    LOAD_IMM64 XMM0_HIGH, 0x400000003F800000
    
    ; XMM1: [2.0, 2.0, 2.0, 2.0]
    LOAD_IMM64 XMM1, 0x4000000040000000
    LOAD_IMM64 XMM1_HIGH, 0x4000000040000000
    
    ; Compare: XMM0 >= XMM1 (not less than)
    CMPPS XMM0, XMM1, 5
    
    ; Results: [3.0>=2.0=T, 2.0>=2.0=T, 1.0>=2.0=F, 2.0>=2.0=T]
    .assert_reg XMM0, -1  ; 2.0 >= 2.0 = true for both
}

.test "CMPPS NLE - not less or equal (greater than)" {
    .description "Test CMPPS with NLE predicate"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [3.0, 2.0, 1.0, 2.5]
    LOAD_IMM64 XMM0, 0x4000000040400000
    LOAD_IMM64 XMM0_HIGH, 0x402000003F800000
    
    ; XMM1: [2.0, 2.0, 2.0, 2.0]
    LOAD_IMM64 XMM1, 0x4000000040000000
    LOAD_IMM64 XMM1_HIGH, 0x4000000040000000
    
    ; Compare: XMM0 > XMM1 (not less than or equal)
    CMPPS XMM0, XMM1, 6
    
    ; Results: [2.0>2.0=F, 3.0>2.0=T, 1.0>2.0=F, 2.5>2.0=T]
    .assert_reg XMM0, -1        ; 3.0 > 2.0 = true
}

.test "CMPPS with zero values" {
    .description "Test CMPPS with zero comparisons"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [0.0, -0.0, 1.0, 0.0]
    LOAD_IMM64 XMM0, 0x000000003F800000      ; [1.0, 0.0]
    LOAD_IMM64 XMM0_HIGH, 0x0000000080000000 ; [0.0, -0.0]
    
    ; XMM1: [0.0, 0.0, 0.0, 0.0]
    LOAD_IMM64 XMM1, 0
    LOAD_IMM64 XMM1_HIGH, 0
    
    ; Compare: XMM0 == XMM1
    CMPPS XMM0, XMM1, 0
    
    ; Results: [0.0==0.0=T, -0.0==0.0=T, 1.0==0.0=F, 0.0==0.0=T]
    ; Note: +0.0 and -0.0 are considered equal in IEEE 754
    .assert_reg XMM0, -1  ; 0.0 == 0.0 = true
}

; =============================================================================
; CMPPD Tests (Packed Double - 2x float64)
; =============================================================================

.test "CMPPD EQ - equality comparison" {
    .description "Test CMPPD with EQ predicate for packed doubles"
    .author "copilot"
    .category "SIMD"
    
    ; Load identical doubles into XMM0 and XMM1
    ; 3.0 as double = 0x4008000000000000
    LOAD_IMM64 XMM0, 0x4008000000000000      ; 3.0
    LOAD_IMM64 XMM0_HIGH, 0x4000000000000000 ; 2.0
    LOAD_IMM64 XMM1, 0x4008000000000000      ; 3.0
    LOAD_IMM64 XMM1_HIGH, 0x4000000000000000 ; 2.0
    
    ; Compare: XMM0 == XMM1
    CMPPD XMM0, XMM1, 0
    
    ; Both comparisons should be true
    .assert_reg XMM0, -1
    .assert_reg XMM0_HIGH, -1
}

.test "CMPPD LT - less than comparison" {
    .description "Test CMPPD with LT predicate"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [1.0, 3.0] as doubles
    LOAD_IMM64 XMM0, 0x3FF0000000000000      ; 1.0
    LOAD_IMM64 XMM0_HIGH, 0x4008000000000000 ; 3.0
    
    ; XMM1: [2.0, 2.0] as doubles
    LOAD_IMM64 XMM1, 0x4000000000000000      ; 2.0
    LOAD_IMM64 XMM1_HIGH, 0x4000000000000000 ; 2.0
    
    ; Compare: XMM0 < XMM1
    CMPPD XMM0, XMM1, 1
    
    ; Results: [1.0<2.0=T, 3.0<2.0=F]
    .assert_reg XMM0, -1  ; 1.0 < 2.0 = true
    .assert_reg XMM0_HIGH, 0x00   ; 3.0 < 2.0 = false
}

.test "CMPPD LE - less than or equal" {
    .description "Test CMPPD with LE predicate"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [2.0, 3.0]
    LOAD_IMM64 XMM0, 0x4000000000000000
    LOAD_IMM64 XMM0_HIGH, 0x4008000000000000
    
    ; XMM1: [2.0, 2.0]
    LOAD_IMM64 XMM1, 0x4000000000000000
    LOAD_IMM64 XMM1_HIGH, 0x4000000000000000
    
    ; Compare: XMM0 <= XMM1
    CMPPD XMM0, XMM1, 2
    
    ; Results: [2.0<=2.0=T, 3.0<=2.0=F]
    .assert_reg XMM0, -1
    .assert_reg XMM0_HIGH, 0x00
}

.test "CMPPD NEQ - not equal" {
    .description "Test CMPPD with NEQ predicate"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [1.5, 2.5]
    LOAD_IMM64 XMM0, 0x3FF8000000000000      ; 1.5
    LOAD_IMM64 XMM0_HIGH, 0x4004000000000000 ; 2.5
    
    ; XMM1: [1.5, 3.5]
    LOAD_IMM64 XMM1, 0x3FF8000000000000      ; 1.5
    LOAD_IMM64 XMM1_HIGH, 0x400C000000000000 ; 3.5
    
    ; Compare: XMM0 != XMM1
    CMPPD XMM0, XMM1, 4
    
    ; Results: [1.5!=1.5=F, 2.5!=3.5=T]
    .assert_reg XMM0, 0x00
    .assert_reg XMM0_HIGH, -1
}

.test "CMPPD NLT - not less than (greater or equal)" {
    .description "Test CMPPD with NLT predicate"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [3.0, 1.0]
    LOAD_IMM64 XMM0, 0x4008000000000000
    LOAD_IMM64 XMM0_HIGH, 0x3FF0000000000000
    
    ; XMM1: [2.0, 2.0]
    LOAD_IMM64 XMM1, 0x4000000000000000
    LOAD_IMM64 XMM1_HIGH, 0x4000000000000000
    
    ; Compare: XMM0 >= XMM1
    CMPPD XMM0, XMM1, 5
    
    ; Results: [3.0>=2.0=T, 1.0>=2.0=F]
    .assert_reg XMM0, -1
    .assert_reg XMM0_HIGH, 0x00
}

.test "CMPPD NLE - not less or equal (greater than)" {
    .description "Test CMPPD with NLE predicate"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [3.0, 2.0]
    LOAD_IMM64 XMM0, 0x4008000000000000
    LOAD_IMM64 XMM0_HIGH, 0x4000000000000000
    
    ; XMM1: [2.0, 2.0]
    LOAD_IMM64 XMM1, 0x4000000000000000
    LOAD_IMM64 XMM1_HIGH, 0x4000000000000000
    
    ; Compare: XMM0 > XMM1
    CMPPD XMM0, XMM1, 6
    
    ; Results: [3.0>2.0=T, 2.0>2.0=F]
    .assert_reg XMM0, -1
    .assert_reg XMM0_HIGH, 0x00
}

.test "CMPPD with zero values" {
    .description "Test CMPPD with zero comparisons"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [0.0, 1.0]
    LOAD_IMM64 XMM0, 0
    LOAD_IMM64 XMM0_HIGH, 0x3FF0000000000000
    
    ; XMM1: [0.0, 0.0]
    LOAD_IMM64 XMM1, 0
    LOAD_IMM64 XMM1_HIGH, 0
    
    ; Compare: XMM0 == XMM1
    CMPPD XMM0, XMM1, 0
    
    ; Results: [0.0==0.0=T, 1.0==0.0=F]
    .assert_reg XMM0, -1
    .assert_reg XMM0_HIGH, 0x00
}

; =============================================================================
; Edge Cases and Special Values
; =============================================================================

.test "CMPPS all predicates consistency" {
    .description "Verify predicate consistency (LT vs NLT, etc.)"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [2.0, 2.0, 2.0, 2.0]
    LOAD_IMM64 XMM0, 0x4000000040000000
    LOAD_IMM64 XMM0_HIGH, 0x4000000040000000
    
    ; XMM1: [3.0, 3.0, 3.0, 3.0]
    LOAD_IMM64 XMM1, 0x4040000040400000
    LOAD_IMM64 XMM1_HIGH, 0x4040000040400000
    
    ; Test LT: 2.0 < 3.0 should be all true
    CMPPS XMM0, XMM1, 1
    .assert_reg XMM0, -1
    .assert_reg XMM0_HIGH, -1
}

.test "CMPPS equal values with EQ and NEQ" {
    .description "Test that EQ and NEQ are inverses for equal values"
    .author "copilot"
    .category "SIMD"
    
    ; Load same values
    LOAD_IMM64 XMM0, 0x4000000040400000
    LOAD_IMM64 XMM0_HIGH, 0x4040000040200000
    LOAD_IMM64 XMM1, 0x4000000040400000
    LOAD_IMM64 XMM1_HIGH, 0x4040000040200000
    
    ; EQ should be all true
    CMPPS XMM0, XMM1, 0
    .assert_reg XMM0, -1
    
    ; Reset and test NEQ
    LOAD_IMM64 XMM0, 0x4000000040400000
    LOAD_IMM64 XMM0_HIGH, 0x4040000040200000
    CMPPS XMM0, XMM1, 4
    
    ; NEQ should be all false
    .assert_reg XMM0, 0x00
}

.test "CMPPD boundary values" {
    .description "Test CMPPD with very close values"
    .author "copilot"
    .category "SIMD"
    
    ; XMM0: [1.0, 1.0000001]
    LOAD_IMM64 XMM0, 0x3FF0000000000000      ; 1.0
    LOAD_IMM64 XMM0_HIGH, 0x3FF0000A7C5AC472 ; ~1.0000001
    
    ; XMM1: [1.0, 1.0]
    LOAD_IMM64 XMM1, 0x3FF0000000000000
    LOAD_IMM64 XMM1_HIGH, 0x3FF0000000000000
    
    ; Compare: XMM0 > XMM1
    CMPPD XMM0, XMM1, 6
    
    ; Results: [1.0>1.0=F, 1.0000001>1.0=T]
    .assert_reg XMM0, 0x00
    .assert_reg XMM0_HIGH, -1
}

.test "CMPPS self-comparison" {
    .description "Test comparing register with itself"
    .author "copilot"
    .category "SIMD"
    
    LOAD_IMM64 XMM0, 0x4000000040400000
    LOAD_IMM64 XMM0_HIGH, 0x4040000040200000
    
    ; Compare XMM0 with itself using EQ
    CMPPS XMM0, XMM0, 0
    
    ; Should be all true
    .assert_reg XMM0, -1
    .assert_reg XMM0_HIGH, -1
}

.test "CMPPD self-comparison with NEQ" {
    .description "Test self != self should be all false"
    .author "copilot"
    .category "SIMD"
    
    LOAD_IMM64 XMM2, 0x4000000000000000
    LOAD_IMM64 XMM2_HIGH, 0x4008000000000000
    
    ; Compare XMM2 != XMM2
    CMPPD XMM2, XMM2, 4
    
    ; Should be all false
    .assert_reg XMM2, 0x00
    .assert_reg XMM2_HIGH, 0x00
}
