# Common constants and macros for DemiEngine assembly
# This file demonstrates advanced preprocessing capabilities

# Version information
.define DEMI_VERSION 1.0
.define DEMI_VERSION_MAJOR 1
.define DEMI_VERSION_MINOR 0
.define DEMI_BUILD_DATE "November 7, 2025"

# Common register aliases
.define COUNTER EAX
.define ACCUMULATOR EBX
.define TEMP ECX
.define STATUS EDX

# Useful constants
.define TRUE 1
.define FALSE 0
.define MAX_ITERATIONS 100

# Platform-specific settings
.ifdef __DEMI_ENGINE__
    .define MEMORY_SIZE 255
    .define STACK_SIZE 64
.else
    .define MEMORY_SIZE 255  
    .define STACK_SIZE 128
.endif

# Function macros with parameters
.define LOAD_IMMEDIATE(reg, value) LOAD_IMM reg, value
.define CLEAR_REG(reg) LOAD_IMM reg, 0
.define COPY_REG(dest, src) MOV dest, src
.define ADD_IMMEDIATE(reg, value) LOAD_IMM EDI, value\nADD reg, EDI
.define INCREMENT_BY(reg, value) LOAD_IMM EDI, value\nADD reg, EDI

# Additional register aliases
.define VERSION_REG ESI
.define FLAGS EDI

# Complex macro for loop setup
.define SETUP_LOOP(counter_reg, limit_val) LOAD_IMM counter_reg, 0\nLOAD_IMM TEMP, limit_val

# Complex multi-line macro
.define COMPLEX_OPERATION(reg1, reg2) LOAD_IMM reg1, 42\nLOAD_IMM reg2, 21

# Math utility macros
.define MUL_BY_10(reg) LOAD_IMM EDI, 10\nMUL reg, EDI

# Conditional compilation examples
.ifdef DEBUG
    .define LOG_INSTRUCTION(msg) # In debug mode, we could add logging
.else
    .define LOG_INSTRUCTION(msg) # No-op in release mode
.endif

# Feature flags
.define ENABLE_ADVANCED_ARITHMETIC TRUE
.define ENABLE_FLOATING_POINT FALSE