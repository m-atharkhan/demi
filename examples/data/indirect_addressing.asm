; Test LOADR instruction - simple program
main:
    ; Store value 42 at memory address 100
    LOAD_IMM EAX, 42
    STORE EAX, 100

    ; Store address 100 in register ECX
    LOAD_IMM ECX, 100

    ; Use LOADR to load from address stored in ECX
    LOADR EDX, ECX

    ; EDX should now contain 42
    HALT