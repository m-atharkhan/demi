; Stack Operations Test Suite
; Tests for stack push, pop, and stack pointer operations

.test "push and pop single value" {
    .description "Tests basic PUSH and POP stack operations with single value"
    .author "bobrossrtx"
    .category "Stack"
    .tag "basic"
    .tag "push"
    .tag "pop"
    LOAD_IMM EAX, 42
    PUSH EAX
    LOAD_IMM EAX, 0   ; Clear EAX
    POP EBX
    .assert_reg EBX, 42
}

.test "push and pop multiple values" {
    .description "Tests pushing and popping multiple values maintains correct order"
    .author "bobrossrtx"
    .category "Stack"
    .tag "push"
    .tag "pop"
    .tag "multi-value"
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 20
    LOAD_IMM ECX, 30
    
    PUSH EAX
    PUSH EBX
    PUSH ECX
    
    POP EDX   ; Should be 30 (last in, first out)
    POP ESI   ; Should be 20
    POP EDI   ; Should be 10
    
    .assert_reg EDX, 30
    .assert_reg ESI, 20
    .assert_reg EDI, 10
}

.test "stack LIFO order" {
    .description "Tests that stack follows Last-In-First-Out ordering"
    .author "bobrossrtx"
    .category "Stack"
    .tag "push"
    .tag "pop"
    .tag "lifo"
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 200
    
    PUSH EAX
    PUSH EBX
    
    ; Pop in reverse order
    POP ECX   ; Should get 200
    POP EDX   ; Should get 100
    
    .assert_reg ECX, 200
    .assert_reg EDX, 100
}

.test "push and pop flags" {
    .description "Tests PUSH_FLAG and POP_FLAG instructions for saving/restoring CPU flags"
    .author "bobrossrtx"
    .category "Stack"
    .tag "flags"
    .tag "push"
    .tag "pop"
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 5
    CMP EAX, EBX      ; Set flags (EAX > EBX)
    PUSH_FLAG       ; Save current flags
    
    LOAD_IMM EAX, 5
    LOAD_IMM EBX, 10
    CMP EAX, EBX      ; Change flags (EAX < EBX)
    
    POP_FLAG        ; Restore original flags (EAX > EBX)
    JG greater      ; Should jump since original comparison was greater
    LOAD_IMM ECX, 99
    JMP end
greater:
    LOAD_IMM ECX, 42
end:
    .assert_reg ECX, 42
}

.test "call and return" {
    .description "Tests CALL and RET instructions for subroutine functionality"
    .author "bobrossrtx"
    .category "Stack Operations"
    .tag "call"
    .tag "return"
    .tag "subroutine"
    
    LOAD_IMM EAX, 10
    CALL subroutine
    .assert_reg EAX, 20  ; Should be doubled by subroutine
    JMP end_test
    
subroutine:
    ADD EAX, EAX      ; Double the value in EAX
    RET
    
end_test:
    ; Test completed
}

.test "stack overflow on excessive push" {
    .description "Tests that stack overflow is detected when pushing too many values"
    .author "bobrossrtx"
    .category "Stack Overflow"
    .tag "error"
    .tag "overflow"
    .tag "push"
    .memory 256
    .maxsteps 500
    .expect_error true
    
    ; Push enough values to overflow the stack (needs ~62 pushes for SP < 8)
    LOAD_IMM EAX, 42
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX
    PUSH EAX  ; 64th push should trigger overflow
}

.test "stack underflow on excess pop" {
    .description "Tests that stack underflow is detected when popping from empty stack"
    .author "bobrossrtx"
    .category "Stack Underflow"
    .tag "error"
    .tag "underflow"
    .tag "pop"
    .memory 256
    .maxsteps 50
    .expect_error true
    
    ; Push one value then pop twice (second pop should underflow)
    LOAD_IMM EAX, 100
    PUSH EAX
    POP EBX
    POP ECX  ; This should trigger stack underflow
}

.test "call stack overflow (deep recursion)" {
    .description "Tests that excessive call nesting triggers call stack overflow"
    .author "bobrossrtx"
    .category "Stack Overflow"
    .tag "error"
    .tag "overflow"
    .tag "call"
    .tag "recursion"
    .memory 2048
    .maxsteps 2000
    .expect_error true
    
recurse:
    CALL recurse  ; Infinite recursion - should hit max call depth
    RET
}

.test "ret without call" {
    .description "Tests that RET without a matching CALL triggers stack underflow"
    .author "bobrossrtx"
    .category "Stack Underflow"
    .tag "error"
    .tag "underflow"
    .tag "ret"
    .memory 256
    .maxsteps 10
    .expect_error true
    
    RET  ; No matching CALL - should cause stack underflow
}