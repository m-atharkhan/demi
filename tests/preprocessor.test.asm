# Comprehensive preprocessor test file
# Tests .include, .define, .ifdef, .ifndef, .elif, .else, .endif

.include "includes/math.inc"

.test "preprocessor_basic_macros" {
    .description "Tests basic macro expansion and simple defines"
    .author "Preprocessor Team"
    .category "Preprocessor Tests"
    .tag "macros"
    .tag "define"
    
    # Use basic macros
    CLEAR_REG(COUNTER)
    LOAD_IMMEDIATE(ACCUMULATOR, 42)
    COPY_REG(COUNTER, ACCUMULATOR)
    
    HALT
    
    .assert_reg R0, 42  # COUNTER should be 42
    .assert_reg R1, 42  # ACCUMULATOR should be 42
}

.test "preprocessor_conditional_compilation" {
    .description "Tests ifdef/ifndef conditional compilation"
    .author "Preprocessor Team"
    .category "Preprocessor Tests"
    .tag "conditional"
    .tag "ifdef"
    
    CLEAR_REG(STATUS)
    
    # This should be included since __DEMI_ENGINE__ is defined
    .ifdef __DEMI_ENGINE__
        LOAD_IMM STATUS, TRUE
    .else
        LOAD_IMM STATUS, FALSE
    .endif
    
    # This should not be included since UNDEFINED_MACRO is not defined
    .ifndef UNDEFINED_MACRO
        LOAD_IMM ACCUMULATOR, 100
    .else
        LOAD_IMM ACCUMULATOR, 200
    .endif
    
    HALT
    
    .assert_reg R3, 1   # STATUS should be TRUE (1)
    .assert_reg R1, 100 # ACCUMULATOR should be 100
}

.test "preprocessor_function_macros" {
    .description "Tests function-like macros with parameters"
    .author "Preprocessor Team"
    .category "Preprocessor Tests"
    .tag "macros"
    .tag "function-macros"
    
    CLEAR_REG(COUNTER)
    
    # Use function macro with parameters
    ADD_IMMEDIATE(COUNTER, 25)
    MUL_BY_2(COUNTER)    # Should double it to 50
    
    HALT
    
    .assert_reg R0, 50  # COUNTER should be 50 (25 * 2)
}

.test "preprocessor_advanced_conditionals" {
    .description "Tests complex conditional compilation with elif/else"
    .author "Preprocessor Team"
    .category "Preprocessor Tests"
    .tag "conditional"
    .tag "elif"
    
    CLEAR_REG(STATUS)
    
    .ifdef NONEXISTENT_FEATURE
        LOAD_IMM STATUS, 1
    .elif ENABLE_ADVANCED_ARITHMETIC
        LOAD_IMM STATUS, 2
    .else
        LOAD_IMM STATUS, 3
    .endif
    
    HALT
    
    .assert_reg R3, 2   # STATUS should be 2 (ENABLE_ADVANCED_ARITHMETIC branch)
}

.test "preprocessor_nested_includes" {
    .description "Tests nested file inclusion and macro resolution"
    .author "Preprocessor Team"
    .category "Preprocessor Tests"
    .tag "include"
    .tag "nested"
    
    # Constants from common.inc should be available
    LOAD_IMM COUNTER, MAX_ITERATIONS
    
    # Should use the DEMI_ENGINE memory size (512, not 1024)
    LOAD_IMM ACCUMULATOR, MEMORY_SIZE  
    
    HALT
    
    .assert_reg R0, 100  # MAX_ITERATIONS
    .assert_reg R1, 255  # MEMORY_SIZE (DEMI_ENGINE version)
}

# Benchmark test using preprocessor macros
.test "preprocessor_benchmark" {
    .description "Benchmark test using preprocessor-generated code"
    .author "Preprocessor Team"
    .category "Benchmarks"
    .tag "benchmark"
    .tag "preprocessor"
    .benchmark
    .warmup 3
    .iterations 10
    .measure "time"
    
    # Simple test using macros
    CLEAR_REG(COUNTER)
    LOAD_IMMEDIATE(ACCUMULATOR, 25)
    ADD_IMMEDIATE(COUNTER, 25)
    
    HALT
    
    .assert_reg R0, 25  # COUNTER should be 25
}