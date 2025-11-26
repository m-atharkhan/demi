# Mathematical Operations Module - Simplified
# Demonstrates cross-file function definitions (static functions)

.include "simple_utils.inc"

# Static addition function (not called, just demonstrates cross-file includes)
static_addition:
    CLEAR_REG(RESULT)           # ECX = 0
    MOV RESULT, OPERAND_A       # ECX = EAX  
    ADD RESULT, OPERAND_B       # ECX = EAX + EBX
    # No return jump - this is just for demonstration of includes
    
# Static subtraction function (not called, just demonstrates cross-file includes)
static_subtraction:
    CLEAR_REG(RESULT)           # ECX = 0
    MOV RESULT, OPERAND_A       # ECX = EAX
    SUB RESULT, OPERAND_B       # ECX = EAX - EBX
    # No return jump - this is just for demonstration of includes

# Static multiplication function (not called, just demonstrates cross-file includes)
static_multiplication:
    CLEAR_REG(RESULT)           # ECX = 0  
    MOV RESULT, OPERAND_A       # ECX = EAX
    MUL RESULT, OPERAND_B       # ECX = EAX * EBX
    # No return jump - this is just for demonstration of includes