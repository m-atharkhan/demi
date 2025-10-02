; test_db_labels.asm - Multiple DB labels example
;
; This example demonstrates:
; - Multiple labeled data blocks
; - Different string lengths
; - Sequential data access

.org 0x100

; Define multiple labeled strings
greeting: DB 'Hi!\n', 3
message: DB 'Labels work!\n', 12

; Program that uses both labels
LOAD_IMM64 R0, greeting
OUTSTR R0, 1

LOAD_IMM64 R0, message  
OUTSTR R0, 1

HALT