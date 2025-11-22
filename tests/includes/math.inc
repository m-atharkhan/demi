# Math utility macros for DemiEngine

.include "common.inc"

# Basic math operations macros
.define ADD_IMMEDIATE(reg, value) LOAD_IMM R7, value\nADD reg, R7
.define SUB_IMMEDIATE(reg, value) LOAD_IMM R7, value\nSUB reg, R7
.define MUL_BY_2(reg) SHL reg, 1
.define DIV_BY_2(reg) SHR reg, 1
.define MUL_BY_10(reg) LOAD_IMM R7, 10\nMUL reg, R7

# Advanced math (only if enabled)
.ifdef ENABLE_ADVANCED_ARITHMETIC
    .define SQUARE(reg) MOV R7, reg\nMUL reg, R7
    .define ABS(reg) CMP reg, 0\nJNC skip_abs\nNOT reg\nINC reg\nskip_abs:
.endif

# Conditional floating point macros
.ifdef ENABLE_FLOATING_POINT
    .define LOAD_FLOAT(reg, value) FLD value\nFST reg
.else
    .define LOAD_FLOAT(reg, value) # Floating point disabled
.endif