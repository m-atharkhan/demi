; ==========================================
; Character Output Test (x86 32-bit)
; ==========================================
; This example demonstrates basic character output using OUT instruction
; Useful for testing console output without string complexity
; Uses 32-bit registers

.text
.org 0x100

_start:
    LOAD_IMM EAX, 72     ; ASCII 'H' (72 decimal)
    OUT EAX, 1           ; Output single character to console (port 1)
    HALT                 ; End program
