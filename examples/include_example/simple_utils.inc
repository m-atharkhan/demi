# Simple Utilities for Calculator
# Register aliases and utility macros

# Register aliases for clarity
.define OPERAND_A R0
.define OPERAND_B R1 
.define RESULT R2
.define ASCII_OFFSET_REG R4
.define TEMP_REG R5
.define PRINT_REG R6

# Operation constants
.define OP_ADD 1
.define OP_SUBTRACT 2
.define OP_MULTIPLY 3

# ASCII constants for printing
.define ASCII_NEWLINE 10
.define ASCII_SPACE 32
.define ASCII_EQUALS 61
.define ASCII_PLUS 43
.define ASCII_MINUS 45
.define ASCII_MULTIPLY 42

# Utility macros
.define SET_VALUE(reg, val) LOAD_IMM reg, val
.define CLEAR_REG(reg) LOAD_IMM reg, 0

# Print macros for visual output
.define PRINT_CHAR(ascii_val) LOAD_IMM PRINT_REG, ascii_val\nOUT PRINT_REG, 1
.define PRINT_NEWLINE PRINT_CHAR(ASCII_NEWLINE)
.define PRINT_SPACE PRINT_CHAR(ASCII_SPACE)
.define PRINT_EQUALS PRINT_CHAR(ASCII_EQUALS)
.define PRINT_DIGIT(reg) MOV PRINT_REG, reg\nLOAD_IMM ASCII_OFFSET_REG, 48\nADD PRINT_REG, ASCII_OFFSET_REG\nOUT PRINT_REG, 1