# Mathematical Operations Module - Simplified
# Demonstrates cross-file function definitions (static functions)

.include "simple_utils.inc"

# Static addition function (not called, just demonstrates cross-file includes)
static_addition:
    CLEAR_REG(RESULT)           # R2 = 0
    MOV RESULT, OPERAND_A       # R2 = R0  
    ADD RESULT, OPERAND_B       # R2 = R0 + R1
    # No return jump - this is just for demonstration of includes
    
# Static subtraction function (not called, just demonstrates cross-file includes)
static_subtraction:
    CLEAR_REG(RESULT)           # R2 = 0
    MOV RESULT, OPERAND_A       # R2 = R0
    SUB RESULT, OPERAND_B       # R2 = R0 - R1
    # No return jump - this is just for demonstration of includes

# Static multiplication function (not called, just demonstrates cross-file includes)
static_multiplication:
    CLEAR_REG(RESULT)           # R2 = 0  
    MOV RESULT, OPERAND_A       # R2 = R0
    MUL RESULT, OPERAND_B       # R2 = R0 * R1
    # No return jump - this is just for demonstration of includes