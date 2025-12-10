; Error Handling Test Suite
; Tests for error conditions and exception handling

.test "division by zero" {
    .description "Tests that division by zero triggers an error"
    .author "bobrossrtx"
    .category "Error Handling"
    .tag "error"
    .tag "division"
    .expect_error true
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 0
    DIV EAX, EBX  ; Should trigger division by zero error
}

.test "invalid opcode" {
    .description "Tests that executing an undefined opcode triggers an error"
    .author "bobrossrtx"
    .category "Error Handling"
    .tag "error"
    .tag "opcode"
    .expect_error true
    ; Use a simpler approach - try to execute a memory location with invalid data
    LOAD_IMM EAX, 0xBB  ; Load invalid opcode value
    STORE EAX, 200      ; Store at memory location 200
    JMP 200            ; Jump to location with invalid opcode
}

.test "stack overflow" {
    .description "Tests that excessive stack pushes trigger a stack overflow error"
    .author "bobrossrtx"
    .category "Error Handling"
    .tag "error"
    .tag "stack"
    .memory 256
    .maxsteps 500
    .expect_error true
    
    ; Push enough values to overflow the stack (need 62+ pushes for SP to drop below 8)
    ; Memory size = 256, SP starts at 256, each PUSH decrements by 4
    ; Need to push until SP < 8, so (256-8)/4 = 62 pushes minimum
    LOAD_IMM EAX, 1
    
    ; Push 65 times to ensure overflow (one per line for clarity)
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
    PUSH EAX
    PUSH EAX  ; This 65th push should trigger stack overflow
}

.test "jump out of bounds" {
    .description "Tests that jumping to an invalid address triggers an error"
    .author "bobrossrtx"
    .category "Error Handling"
    .tag "error"
    .tag "jump"
    .expect_error true
    JMP 1000  ; Jump to invalid address
}

.test "return without call" {
    .description "Tests that RET without a matching CALL triggers an error"
    .author "bobrossrtx"
    .category "Error Handling"
    .tag "error"
    .tag "call-stack"
    .expect_error true
    RET  ; Return without corresponding CALL
}