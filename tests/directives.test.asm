# Test demonstrating new test directives
# This file shows .maxcalldepth, .timeout, .skip, and benchmark directives

.test "max_call_depth_test" {
    .description "Tests custom call depth limit (32 instead of default 64)"
    .author "DemiEngine Team"
    .category "Test Framework"
    .tag "maxcalldepth"
    .tag "call-stack"
    .maxcalldepth 32
    
    # Recursive function that calls itself 31 times (should succeed)
    LOAD_IMM R0, 31         # Counter
    LOAD_IMM R1, 0          # Recursion depth tracker
    
loop:
    CMP R0, R1
    JZ end
    INC R1
    CALL recurse
    JMP loop
    
recurse:
    RET
    
end:
    HALT
}

.test "timeout_test" {
    .description "Tests timeout directive (currently informational)"
    .author "DemiEngine Team"
    .category "Test Framework"
    .tag "timeout"
    .timeout 1000
    
    # Simple test that completes quickly
    LOAD_IMM R0, 42
    HALT
    
    .assert_reg R0, 42
}

.test "skipped_test" {
    .description "This test is skipped"
    .author "DemiEngine Team"
    .category "Test Framework"
    .tag "skip"
    .skip
    
    # This code won't execute
    LOAD_IMM R0, 999
    HALT
}

.test "benchmark_example" {
    .description "Benchmark example (infrastructure ready, timing not yet implemented)"
    .author "DemiEngine Team"
    .category "Benchmarks"
    .tag "benchmark"
    .tag "arithmetic"
    .benchmark
    .warmup 5
    .iterations 100
    .measure "time"
    
    # Simple arithmetic loop
    LOAD_IMM R0, 0
    LOAD_IMM R1, 100
    
bench_loop:
    CMP R0, R1
    JZ bench_end
    INC R0
    JMP bench_loop
    
bench_end:
    HALT
    
    .assert_reg R0, 100
}

.test "combined_metadata" {
    .description "Test with all metadata directives"
    .author "DemiEngine Team"
    .category "Test Framework"
    .tag "comprehensive"
    .tag "metadata"
    .maxsteps 500
    .maxcalldepth 16
    
    LOAD_IMM R0, 123
    LOAD_IMM R1, 200  # Fixed: within 0-255 range
    ADD R0, R1
    HALT
    
    .assert_reg R0, 67  # 123 + 200 = 323, but overflows in 8-bit to 67 (323 % 256)
}
