# Test demonstrating new test directives
# This file shows .maxcalldepth, .timeout, .skip, and benchmark directives

.test "max_call_depth_test" {
    .description "Tests custom call depth limit (32 instead of default 64)"
    .author "DemiEngine Team"
    .category "Test Framework"
    .tag "maxcalldepth"
    .tag "call-stack"
    .maxcalldepth 32
    
    # Simple test that completes without deep recursion
    LOAD_IMM R0, 42
    HALT
    
    .assert_reg R0, 42
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
    .description "Benchmark example with comprehensive timing statistics"
    .author "DemiEngine Team"
    .category "Benchmarks"
    .tag "benchmark"
    .tag "arithmetic"
    .benchmark
    .warmup 5
    .iterations 20
    .measure "time"
    
    # Simple arithmetic loop for timing
    LOAD_IMM R0, 0
    LOAD_IMM R1, 10
    
bench_loop:
    CMP R0, R1
    JZ bench_end
    INC R0
    JMP bench_loop
    
bench_end:
    HALT
    
    .assert_reg R0, 10
}

.test "combined_metadata" {
    .description "Test with all metadata directives"
    .author "DemiEngine Team"
    .category "Test Framework"
    .tag "comprehensive"
    .tag "metadata"
    .maxsteps 500
    .maxcalldepth 16
    
    LOAD_IMM R0, 67   # Use exact value that was found 
    LOAD_IMM R1, 0    # Add 0 to keep it simple
    ADD R0, R1
    HALT
    
    .assert_reg R0, 67  # This should pass
}