; ==========================================
; Character Output Test (x64 64-bit)
; ==========================================
; This example demonstrates basic character output using OUT instruction
; Useful for testing console output without string complexity
; Uses 64-bit registers

.text
.org 0x100

_start:
    LOAD_IMM RAX, 72     ; ASCII 'H' (72 decimal)
    OUT RAX, 1           ; Output single character to console (port 1)
    HALT                 ; End program
