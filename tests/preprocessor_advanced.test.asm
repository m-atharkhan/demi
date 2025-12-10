# Platform-specific test file demonstrating preprocessor capabilities
# Tests platform-specific macros and conditional compilation

.include "includes/common.inc"

# Define platform-specific features
.define PLATFORM_DEMI
.define ENABLE_DEBUG_MODE
.define ENABLE_ADVANCED_ARITHMETIC

.test "preprocessor_platform_specific" {
    .description "Tests platform-specific conditional compilation"
    .author "bobrossrtx" 
    .category "Platform Tests"
    .tag "platform"
    .tag "conditional"
    
    CLEAR_REG(STATUS)
    
    # Platform-specific code - this should be included
    .ifdef PLATFORM_DEMI
        LOAD_IMM STATUS, 1
        .ifdef ENABLE_DEBUG_MODE
            LOAD_IMM ACCUMULATOR, 255  # Debug marker (max value for LOAD_IMM)
        .endif
    .endif
    
    .ifdef PLATFORM_X86
        LOAD_IMM STATUS, 2
    .endif
    
    .ifdef PLATFORM_ARM
        LOAD_IMM STATUS, 3
    .endif
    
    HALT
    
    .assert_reg EDX, 1     # STATUS should be 1 (PLATFORM_DEMI)
    .assert_reg EBX, 255   # ACCUMULATOR should be debug marker
}

.test "preprocessor_version_check" {
    .description "Tests version-specific compilation"
    .author "bobrossrtx"
    .category "Version Tests"
    .tag "version"
    .tag "conditional"
    
    CLEAR_REG(VERSION_REG)
    
    # Use version macros from common.inc
    LOAD_IMM VERSION_REG, DEMI_VERSION_MAJOR
    MUL_BY_10(VERSION_REG)  # Should be 10 (1 * 10)
    ADD_IMMEDIATE(VERSION_REG, DEMI_VERSION_MINOR)  # Should be 10 + 0 = 10
    
    HALT
    
    .assert_reg ESI, 10  # VERSION_REG should be 10 (1.0 encoded)
}

.test "preprocessor_complex_macros" {
    .description "Tests complex multi-line macro expansion"
    .author "bobrossrtx"
    .category "Macro Tests" 
    .tag "macros"
    .tag "complex"
    
    # Initialize registers
    CLEAR_REG(COUNTER)
    CLEAR_REG(ACCUMULATOR)
    
    # Use the complex COMPLEX_OPERATION macro
    COMPLEX_OPERATION(COUNTER, ACCUMULATOR)
    
    HALT
    
    .assert_reg EAX, 42   # COUNTER should be set to 42
    .assert_reg EBX, 21   # ACCUMULATOR should be 42/2 = 21
}

.test "preprocessor_register_aliases" {
    .description "Tests register alias definitions"
    .author "bobrossrtx"
    .category "Register Tests"
    .tag "registers"
    .tag "aliases"
    
    # Use register aliases defined in common.inc
    LOAD_IMM COUNTER, 10
    LOAD_IMM ACCUMULATOR, 20
    LOAD_IMM TEMP, 30
    LOAD_IMM STATUS, 40
    LOAD_IMM VERSION_REG, 50
    LOAD_IMM FLAGS, 60
    
    HALT
    
    .assert_reg EAX, 10   # COUNTER
    .assert_reg EBX, 20   # ACCUMULATOR  
    .assert_reg ECX, 30   # TEMP
    .assert_reg EDX, 40   # STATUS
    .assert_reg ESI, 50   # VERSION_REG
    .assert_reg EDI, 60   # FLAGS
}

# Test the mathematical macros if available
.ifdef ENABLE_ADVANCED_ARITHMETIC
    .include "includes/math.inc"
    
    .test "preprocessor_math_operations" {
        .description "Tests mathematical operation macros"
        .author "bobrossrtx"
        .category "Math Tests"
        .tag "math"
        .tag "arithmetic"
        .benchmark
        .iterations 5
        .warmup 2
        
        CLEAR_REG(ACCUMULATOR)
        LOAD_IMMEDIATE(ACCUMULATOR, 10)
        
        # Use math macros from math.inc
        SQUARE(ACCUMULATOR)      # Should be 100
        DIV_BY_2(ACCUMULATOR)    # Should be 50
        ADD_IMMEDIATE(ACCUMULATOR, 25)  # Should be 75
        
        HALT
        
        .assert_reg EBX, 75
    }
.endif

.test "preprocessor_macro_nesting" {
    .description "Tests nested macro expansion"
    .author "bobrossrtx" 
    .category "Complex Tests"
    .tag "nesting"
    .tag "macros"
    
    CLEAR_REG(COUNTER)
    
    # Nested macro usage - INCREMENT_BY calls ADD_IMMEDIATE
    INCREMENT_BY(COUNTER, 5)
    INCREMENT_BY(COUNTER, 3)
    INCREMENT_BY(COUNTER, 2)
    
    HALT
    
    .assert_reg EAX, 10  # COUNTER should be 5+3+2=10
}