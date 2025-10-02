; char_test.asm - Simple character output test
;
; This example demonstrates basic character output using OUT instruction.
; Useful for testing console output without string complexity.

.org 0x100

LOAD_IMM64 R0, 72      ; ASCII 'H' (72 decimal)
OUT R0, 1              ; Output single character to console (port 1)
HALT                   ; End program