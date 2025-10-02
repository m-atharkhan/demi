; simple_label_test.asm - Demonstrates DB labels and LOAD_IMM64
; 
; This example shows how to:
; - Define data using DB directive with a label
; - Reference the label to get the data address
; - Use LOAD_IMM64 for addresses that don't fit in 8-bit LOAD_IMM
; - Output strings using OUTSTR

.org 0x100

; Define a string with a label - the label points to where the string data starts
mystring: DB 'Hello!', 6

; Program code that references the labeled data
LOAD_IMM64 R0, mystring    ; R0 = address of string data (uses 64-bit load for larger addresses)
OUTSTR R0, 1               ; Output string at [R0] to console (port 1)
HALT                       ; End program