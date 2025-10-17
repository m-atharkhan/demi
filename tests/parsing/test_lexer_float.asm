; Test basic floating point parsing
; Testing lexer improvements

.data
    pi_value: .dd 3.14159265    ; Using .dd directive

.text
main:
    ; Test floating point immediate
    FLD 2.5
    FLD 13.0
    FADD 1.5
    HALT