; Basic Assembly Operations Test
; Simple test of fundamental assembly operations

#test "cpu basic operations" {
    .text
    main:
        ; Basic register operations
        LOAD_IMM R0, 42
        LOAD_IMM R1, 13
        ADD R0, R1                  ; R0 = 55
        #assert_reg R0, 55
        
        ; Basic control flow
        MOV R2, R0
        #assert_reg R2, 55
        CMP R2, R1
        
        ; Stack operations
        PUSH R0
        PUSH R1
        POP R3                      ; R3 = 13
        POP R4                      ; R4 = 55
        #assert_reg R3, 13
        #assert_reg R4, 55
        
        ; Basic arithmetic
        SUB R0, R1                  ; R0 = 42
        #assert_reg R0, 42
        MUL R1, R1                  ; R1 = 169
        #assert_reg R1, 169
        
        ; Logical operations
        LOAD_IMM R5, 255
        AND R5, R0                  ; R5 = 42 & 255 = 42
        #assert_reg R5, 42
        
        HALT
}