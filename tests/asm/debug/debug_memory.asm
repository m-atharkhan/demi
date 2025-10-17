; Simple test to see memory values
LOAD_IMM R0, 10
STORE R0, 0x60
LOAD_IMM R0, 20  
STORE R0, 0x61

; Load back and check
LOAD R1, 0x60  ; Should be 10
LOAD R2, 0x61  ; Should be 20

HALT