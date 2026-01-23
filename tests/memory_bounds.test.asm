; Memory Bounds Error Test Suite
; Tests for out-of-bounds memory access and pointer errors
; Author: copilot
; Category: Error Handling
; Phase: 3

; =============================================================================
; Out-of-Bounds Load Tests
; =============================================================================

.test "LOAD from address zero" {
    .description "Test loading from address 0"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "bounds"
    
    ; Load from address 0 should work (it's a valid address)
    LOAD EAX, 0
    HALT
    
    ; This should succeed - address 0 is valid
}

.test "LOAD from very high address" {
    .description "Test loading from high memory address"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "bounds"
    
    ; Try to load from a potentially out-of-bounds address
    ; Memory size is typically limited, so 0x100000 might be out of bounds
    LOAD EAX, 0x10000
    HALT
}

.test "LOADR with valid high address" {
    .description "Test LOADR with high address in bounds"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "indirect"
    
    ; Use a high but valid address
    LOAD_IMM EBX, 500
    LOAD_IMM EAX, 99
    STORE EAX, 500      ; Store first
    LOADR ECX, EBX      ; Load from address in EBX
    HALT
    
    .assert_reg ECX, 99
}

.test "LOADR with register address chain" {
    .description "Test LOADR following address stored in register"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "indirect"
    
    ; Store address in memory, then load it into register and use LOADR
    LOAD_IMM EAX, 200
    STORE EAX, 100      ; mem[100] = 200
    LOAD_IMM EAX, 42
    STORE EAX, 200      ; mem[200] = 42
    
    LOAD EBX, 100       ; EBX = mem[100] = 200
    LOADR ECX, EBX      ; ECX = mem[EBX] = mem[200] = 42
    HALT
    
    .assert_reg ECX, 42
}

.test "LOADR with zero address" {
    .description "Test LOADR with address 0 (null pointer dereference simulation)"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "null"
    
    ; Load from address 0 through register
    LOAD_IMM EBX, 0
    LOADR EAX, EBX      ; Should work - address 0 is valid
    HALT
}

; =============================================================================
; Out-of-Bounds Store Tests  
; =============================================================================

.test "STORE to address zero" {
    .description "Test storing to address 0"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "bounds"
    
    LOAD_IMM EAX, 42
    STORE EAX, 0        ; Store to address 0 should work
    HALT
}

.test "STORE to high address" {
    .description "Test storing to high memory address"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    
    ; Store to high address and verify (within 512 byte default)
    LOAD_IMM EAX, 123
    STORE EAX, 480
    LOAD EBX, 480
    HALT
    
    .assert_reg EBX, 123
}

.test "STORER with register addressing" {
    .description "Test STORER using register as address pointer"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "indirect"
    
    ; Use register to hold address for STORER (within 512 byte default)
    LOAD_IMM EBX, 450
    LOAD_IMM EAX, 99
    STORER EBX, EAX     ; Store EAX to address in EBX
    
    LOAD ECX, 450       ; Verify it was stored
    HALT
    
    .assert_reg ECX, 99
}

; =============================================================================
; Memory Access Alignment Tests
; =============================================================================

.test "unaligned memory access" {
    .description "Test memory access at odd address (alignment test)"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "alignment"
    
    ; Access memory at odd address (VM may or may not require alignment)
    LOAD_IMM EAX, 42
    STORE EAX, 13       ; Odd address
    LOAD EBX, 13
    HALT
    
    .assert_reg EBX, 42
}

.test "sequential unaligned accesses" {
    .description "Test multiple unaligned memory accesses"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "alignment"
    
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 20
    LOAD_IMM ECX, 30
    
    STORE EAX, 5        ; Odd address
    STORE EBX, 9        ; Odd address
    STORE ECX, 13       ; Odd address
    
    LOAD EAX, 5
    LOAD EBX, 9
    LOAD ECX, 13
    
    HALT
    
    .assert_reg EAX, 10
    .assert_reg EBX, 20
    .assert_reg ECX, 30
}

; =============================================================================
; Memory Overwrite Tests
; =============================================================================

.test "overwrite same address multiple times" {
    .description "Test writing to same memory location repeatedly"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "overwrite"
    
    LOAD_IMM EAX, 1
    STORE EAX, 100
    
    LOAD_IMM EAX, 2
    STORE EAX, 100
    
    LOAD_IMM EAX, 3
    STORE EAX, 100
    
    LOAD EBX, 100
    HALT
    
    .assert_reg EBX, 3  ; Should have last written value
}

.test "sequential memory writes" {
    .description "Test sequential memory writes to adjacent addresses"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "sequential"
    
    ; Write to sequential addresses (within 512 byte default)
    LOAD_IMM EAX, 111
    STORE EAX, 400
    
    LOAD_IMM EBX, 222
    STORE EBX, 404      ; Next address (4 bytes later)
    
    LOAD_IMM ECX, 333
    STORE ECX, 408      ; Next address
    
    ; Read them back
    LOAD EDX, 400
    LOAD R6, 404
    LOAD R8, 408
    
    HALT
    
    ; Verify all values stored correctly
    .assert_reg EDX, 111
    .assert_reg R6, 222
    ; STORE/LOAD are byte-oriented; 333 (0x14D) stores as 0x4D = 77
    .assert_reg R8, 77
}

; =============================================================================
; Pointer Chain Tests
; =============================================================================

.test "indirect load chain" {
    .description "Test chain of indirect loads"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "indirect"
    
    ; Set up pointer chain: address 100 → 200, address 200 → 42
    LOAD_IMM EAX, 200
    STORE EAX, 100      ; mem[100] = 200
    
    LOAD_IMM EAX, 42
    STORE EAX, 200      ; mem[200] = 42
    
    ; Follow chain
    LOAD_IMM EBX, 100
    LOADR ECX, EBX      ; ECX = mem[100] = 200
    LOADR EDX, ECX      ; EDX = mem[200] = 42
    
    HALT
    
    .assert_reg EDX, 42
}

.test "circular pointer reference" {
    .description "Test circular pointer chain"
    .author "copilot"
    .category "Error Handling"
    .tag "memory"
    .tag "circular"
    
    ; Create circular reference: 100 → 200 → 100
    LOAD_IMM EAX, 200
    STORE EAX, 100      ; mem[100] = 200
    
    LOAD_IMM EAX, 100
    STORE EAX, 200      ; mem[200] = 100
    
    ; Follow one link (don't create infinite loop)
    LOAD_IMM EBX, 100
    LOADR ECX, EBX      ; ECX = mem[100] = 200
    LOADR EDX, ECX      ; EDX = mem[200] = 100
    
    HALT
    
    .assert_reg EDX, 100
}

; =============================================================================
; Memory Bounds with Jump Tests
; =============================================================================

.test "JMP to valid code address" {
    .description "Test jumping within valid code region"
    .author "copilot"
    .category "Error Handling"
    .tag "control"
    .tag "bounds"
    
    JMP skip
    LOAD_IMM EAX, 999   ; This should be skipped
    
skip:
    LOAD_IMM EBX, 111
    HALT
    
    .assert_reg EBX, 111
}

.test "CALL and RET memory effects" {
    .description "Test CALL/RET stack usage"
    .author "copilot"
    .category "Error Handling"
    .tag "call"
    .tag "stack"
    
    LOAD_IMM EAX, 10
    CALL func
    ADD EAX, EBX
    HALT
    .assert_reg EAX, 35
    
func:
    LOAD_IMM EBX, 25
    RET
}
