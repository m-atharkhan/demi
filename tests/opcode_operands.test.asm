; Opcode Operands Test Suite
; Tests various operand formats: decimal, hex, binary, registers, memory

.test "MOV with decimal immediates" {
    .description "Verify MOV handles decimal immediate values"
    .author "bobrossrtx"
    .category "Core Instructions"
    
    ; Small decimal value
    MOV EAX, 42
    .assert_reg EAX, 42
    
    ; Larger decimal value  
    MOV EBX, 1000
    .assert_reg EBX, 1000
    
    ; Zero
    MOV ECX, 0
    .assert_reg ECX, 0
    
    ; Maximum 8-bit value
    MOV EDX, 255
    .assert_reg EDX, 255
    
    halt
}

.test "MOV with hex immediates" {
    .description "Verify MOV handles hexadecimal immediate values"
    .author "bobrossrtx"
    .category "Core Instructions"
    
    ; Small hex value
    MOV EAX, 0x2A      ; 42 in decimal
    .assert_reg EAX, 42
    
    ; Medium hex value
    MOV EBX, 0xFF      ; 255 in decimal
    .assert_reg EBX, 255
    
    ; Larger hex value
    MOV ECX, 0x1000    ; 4096 in decimal
    .assert_reg ECX, 4096
    
    ; Hex with lowercase letters
    MOV EDX, 0xabcd    ; 43981 in decimal
    .assert_reg EDX, 43981
    
    ; Hex with uppercase letters
    MOV ESI, 0xABCD
    .assert_reg ESI, 43981
    
    halt
}

.test "MOV with binary immediates" {
    .description "Verify MOV handles binary immediate values"
    .author "bobrossrtx"
    .category "Core Instructions"
    
    ; Simple binary value
    MOV EAX, 0b1010    ; 10 in decimal
    .assert_reg EAX, 10
    
    ; Full byte binary
    MOV EBX, 0b11111111  ; 255 in decimal
    .assert_reg EBX, 255
    
    ; Binary with leading zeros concept
    MOV ECX, 0b00001111  ; 15 in decimal
    .assert_reg ECX, 15
    
    ; Alternating pattern
    MOV EDX, 0b10101010  ; 170 in decimal
    .assert_reg EDX, 170
    
    halt
}

.test "MOV with octal immediates" {
    .description "Verify MOV handles octal immediate values (0o prefix)"
    .author "bobrossrtx"
    .category "Core Instructions"
    
    ; Simple octal value
    MOV EAX, 0o12      ; 10 in decimal (1*8 + 2)
    .assert_reg EAX, 10
    
    ; Larger octal value
    MOV EBX, 0o377     ; 255 in decimal (3*64 + 7*8 + 7)
    .assert_reg EBX, 255
    
    ; Another octal
    MOV ECX, 0o100     ; 64 in decimal (1*64)
    .assert_reg ECX, 64
    
    ; Octal 777
    MOV EDX, 0o777     ; 511 in decimal
    .assert_reg EDX, 511
    
    halt
}

.test "MOV register to register" {
    .description "Verify MOV copies values between registers"
    .author "bobrossrtx"
    .category "Core Instructions"
    
    ; Setup source register
    MOV EAX, 42
    
    ; Copy to another register
    MOV ECX, EAX
    .assert_reg ECX, 42
    
    ; Verify source unchanged
    .assert_reg EAX, 42
    
    ; Chain of copies
    MOV EBX, 100
    MOV EDX, EBX
    MOV ESI, EDX
    .assert_reg ESI, 100
    .assert_reg EDX, 100
    .assert_reg EBX, 100
    
    halt
}

.test "MOV with memory addresses" {
    .description "Verify MOV handles memory read/write with hex addresses"
    .author "bobrossrtx"
    .category "Core Instructions"
    
    ; Store to memory at hex address
    MOV EAX, 42
    MOV [0x80], EAX      ; Address 128 in decimal
    
    ; Load from memory
    MOV EBX, 0           ; Clear destination
    MOV EBX, [0x80]
    .assert_reg EBX, 42
    
    ; Different address format - same value
    MOV ECX, 99
    MOV [0x90], ECX      ; Address 144 in decimal
    MOV EDX, [0x90]
    .assert_reg EDX, 99
    
    halt
}

.test "Address vs Immediate disambiguation" {
    .description "Verify hex numbers in brackets are addresses, bare hex are immediates"
    .author "bobrossrtx"
    .category "Core Instructions"
    
    ; 0xFF as immediate value (255)
    MOV EAX, 0xFF
    .assert_reg EAX, 255
    
    ; Store value to address 0x50 (80 decimal)
    MOV [0x50], EAX
    
    ; [0x50] as memory address
    MOV EBX, [0x50]
    .assert_reg EBX, 255
    
    ; Verify the distinction: immediate 0x50 = 80
    MOV ECX, 0x50
    .assert_reg ECX, 80
    
    ; And memory at 0x50 = 255 (value we stored)
    MOV EDX, [0x50]
    .assert_reg EDX, 255
    
    halt
}

.test "Mixed operand formats" {
    .description "Test mixing different number formats in same program"
    .author "bobrossrtx"
    .category "Core Instructions"
    
    ; Decimal
    MOV EAX, 100
    
    ; Hex
    MOV EBX, 0x64        ; Also 100 in decimal
    
    ; Binary  
    MOV ECX, 0b1100100   ; Also 100 in decimal
    
    ; All should be equal
    .assert_reg EAX, 100
    .assert_reg EBX, 100
    .assert_reg ECX, 100
    
    ; Now test memory with mixed formats
    MOV [0x70], EAX      ; Hex address
    MOV EDX, [0x70]
    .assert_reg EDX, 100
    
    halt
}

.test "Edge case: zero values" {
    .description "Test zero values in different formats"
    .author "bobrossrtx"
    .category "Core Instructions"
    
    ; Zero in different formats - using different registers
    MOV EAX, 0
    MOV EBX, 0x0
    MOV ECX, 0b0
    .assert_reg EAX, 0
    .assert_reg EBX, 0
    .assert_reg ECX, 0
    
    halt
}

.test "Edge case: one values" {
    .description "Test value 1 in different formats"
    .author "bobrossrtx"
    .category "Core Instructions"
    
    ; One in different formats
    MOV EAX, 1
    MOV EBX, 0x1
    MOV ECX, 0b1
    .assert_reg EAX, 1
    .assert_reg EBX, 1
    .assert_reg ECX, 1
    
    halt
}

.test "Edge case: larger values" {
    .description "Test larger immediate values"
    .author "bobrossrtx"
    .category "Core Instructions"
    
    ; 16-bit range values
    MOV EAX, 65535       ; Max 16-bit
    .assert_reg EAX, 65535
    
    MOV EBX, 0xFFFF
    .assert_reg EBX, 65535
    
    ; 32-bit range values
    MOV ECX, 0x10000     ; 65536
    .assert_reg ECX, 65536
    
    MOV EDX, 100000
    .assert_reg EDX, 100000
    
    halt
}
