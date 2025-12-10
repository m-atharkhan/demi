; Demonstration of .maxsteps directive
; Tests that the .maxsteps directive can override the default 10,000 step limit

.test "default maxsteps (10000)" {
    .description "Test that runs with the default 10,000 step limit for loop execution"
    .author "bobrossrtx"
    .category "Test Framework"
    .tag "maxsteps"
    .tag "loop"
    
    ; This test uses the default 10,000 step limit
    ; Running 100 iterations is well within the limit
    LOAD_IMM EAX, 0
    LOAD_IMM EBX, 1
    LOAD_IMM ECX, 100
loop_start:
    ADD EAX, EBX
    CMP EAX, ECX
    JL loop_start
    .assert_reg EAX, 100
}

.test "custom maxsteps (500)" {
    .description "Test with custom .maxsteps directive set to 500 steps"
    .author "bobrossrtx"
    .category "Test Framework"
    .tag "maxsteps"
    .tag "metadata"
    .tag "loop"
    .maxsteps 500
    
    ; This test sets a custom limit of 500 steps
    ; Running 20 iterations should complete successfully
    LOAD_IMM EAX, 0
    LOAD_IMM EBX, 1
    LOAD_IMM ECX, 20
repeat:
    ADD EAX, EBX
    CMP EAX, ECX
    JL repeat
    .assert_reg EAX, 20
}

.test "infinite loop caught by limit" {
    .description "Test that infinite loops are caught by maxsteps timeout"
    .author "bobrossrtx"
    .category "Test Framework"
    .tag "maxsteps"
    .tag "error-handling"
    .tag "timeout"
    .maxsteps 50
    .expect_error true
    
    ; This test should fail because it will exceed the 50 step limit
    LOAD_IMM EAX, 0
    LOAD_IMM EBX, 1
endless:
    ADD EAX, EBX
    JMP endless
}
