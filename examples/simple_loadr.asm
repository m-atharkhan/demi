; Test LOADR instruction - simple program
main:
    ; Store value 42 at memory address 100
    LOAD_IMM R0, 42
    STORE R0, 100

    ; Store address 100 in register R2
    LOAD_IMM R2, 100

    ; Use LOADR to load from address stored in R2
    LOADR R3, R2

    ; R3 should now contain 42
    HALT