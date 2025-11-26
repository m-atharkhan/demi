# Visual Calculator Demo - Include Demonstration
# Demonstrates: .inc files, .asm files, cross-file labels, macro expansion
# Features: Visual output showing calculations step by step

.include "simple_utils.inc"
.include "math_operations.asm"

# Entry point - execution starts here  
start:
    # Print header: "Calculator Demo"
    PRINT_CHAR(67)              # 'C'
    PRINT_CHAR(97)              # 'a'  
    PRINT_CHAR(108)             # 'l'
    PRINT_CHAR(99)              # 'c'
    PRINT_CHAR(117)             # 'u'
    PRINT_CHAR(108)             # 'l'
    PRINT_CHAR(97)              # 'a'
    PRINT_CHAR(116)             # 't'
    PRINT_CHAR(111)             # 'o'
    PRINT_CHAR(114)             # 'r'
    PRINT_SPACE
    PRINT_CHAR(68)              # 'D'
    PRINT_CHAR(101)             # 'e'
    PRINT_CHAR(109)             # 'm'
    PRINT_CHAR(111)             # 'o'
    PRINT_NEWLINE
    PRINT_NEWLINE

    # Test 1: Addition (3 + 4)
    PRINT_CHAR(84)              # 'T'
    PRINT_CHAR(101)             # 'e'
    PRINT_CHAR(115)             # 's'
    PRINT_CHAR(116)             # 't'
    PRINT_SPACE
    PRINT_CHAR(49)              # '1'
    PRINT_CHAR(58)              # ':'
    PRINT_SPACE
    
    # Setup operands
    SET_VALUE(OPERAND_A, 3)     # EAX = 3  
    SET_VALUE(OPERAND_B, 4)     # EBX = 4
    
    # Print equation: dynamically print operands and result
    PRINT_DIGIT(OPERAND_A)      # Print actual value of EAX as ASCII digit
    PRINT_SPACE
    PRINT_CHAR(ASCII_PLUS)      # '+'
    PRINT_SPACE
    PRINT_DIGIT(OPERAND_B)      # Print actual value of EBX as ASCII digit
    PRINT_SPACE
    PRINT_EQUALS                # '='
    PRINT_SPACE
    
    # Perform addition (using same logic as static_addition from math_operations.asm)
    CLEAR_REG(RESULT)           # ECX = 0
    MOV RESULT, OPERAND_A       # ECX = EAX = 3
    ADD RESULT, OPERAND_B       # ECX = 3 + 4 = 7
    MOV EBP, RESULT              # Store in EBP
    
    # Print result dynamically
    PRINT_DIGIT(RESULT)         # Print actual calculated value as ASCII digit
    PRINT_NEWLINE

    # Test 2: Subtraction (9 - 2)
    PRINT_CHAR(84)              # 'T'
    PRINT_CHAR(101)             # 'e'
    PRINT_CHAR(115)             # 's'
    PRINT_CHAR(116)             # 't'
    PRINT_SPACE
    PRINT_CHAR(50)              # '2'
    PRINT_CHAR(58)              # ':'
    PRINT_SPACE
    
    # Setup operands
    SET_VALUE(OPERAND_A, 9)     # EAX = 9
    SET_VALUE(OPERAND_B, 2)     # EBX = 2
    
    # Print equation: dynamically print operands and result
    PRINT_DIGIT(OPERAND_A)      # Print actual value of EAX as ASCII digit
    PRINT_SPACE
    PRINT_CHAR(ASCII_MINUS)     # '-'
    PRINT_SPACE
    PRINT_DIGIT(OPERAND_B)      # Print actual value of EBX as ASCII digit
    PRINT_SPACE
    PRINT_EQUALS                # '='
    PRINT_SPACE
    
    # Perform subtraction (using same logic as static_subtraction from math_operations.asm)
    CLEAR_REG(RESULT)           # ECX = 0
    MOV RESULT, OPERAND_A       # ECX = EAX = 9
    SUB RESULT, OPERAND_B       # ECX = 9 - 2 = 7
    MOV R8, RESULT              # Store in R8
    
    # Print result dynamically
    PRINT_DIGIT(RESULT)         # Print actual calculated value as ASCII digit
    PRINT_NEWLINE

    # Test 3: Multiplication (2 * 3)
    PRINT_CHAR(84)              # 'T'
    PRINT_CHAR(101)             # 'e'
    PRINT_CHAR(115)             # 's'
    PRINT_CHAR(116)             # 't'
    PRINT_SPACE
    PRINT_CHAR(51)              # '3'
    PRINT_CHAR(58)              # ':'
    PRINT_SPACE
    
    # Setup operands
    SET_VALUE(OPERAND_A, 2)     # EAX = 2
    SET_VALUE(OPERAND_B, 3)     # EBX = 3
    
    # Print equation: dynamically print operands and result
    PRINT_DIGIT(OPERAND_A)      # Print actual value of EAX as ASCII digit
    PRINT_SPACE
    PRINT_CHAR(ASCII_MULTIPLY)  # '*'
    PRINT_SPACE
    PRINT_DIGIT(OPERAND_B)      # Print actual value of EBX as ASCII digit
    PRINT_SPACE
    PRINT_EQUALS                # '='
    PRINT_SPACE
    
    # Perform multiplication (using same logic as static_multiplication from math_operations.asm)
    CLEAR_REG(RESULT)           # ECX = 0
    MOV RESULT, OPERAND_A       # ECX = EAX = 2
    MUL RESULT, OPERAND_B       # ECX = 2 * 3 = 6
    MOV R9, RESULT              # Store in R9
    
    # Print result dynamically
    PRINT_DIGIT(RESULT)         # Print actual calculated value as ASCII digit
    PRINT_NEWLINE
    PRINT_NEWLINE
    
    # Verification section
    PRINT_CHAR(86)              # 'V'
    PRINT_CHAR(101)             # 'e'
    PRINT_CHAR(114)             # 'r'
    PRINT_CHAR(105)             # 'i'
    PRINT_CHAR(102)             # 'f'
    PRINT_CHAR(121)             # 'y'
    PRINT_CHAR(105)             # 'i'
    PRINT_CHAR(110)             # 'n'
    PRINT_CHAR(103)             # 'g'
    PRINT_CHAR(46)              # '.'
    PRINT_CHAR(46)              # '.'
    PRINT_CHAR(46)              # '.'
    PRINT_NEWLINE
    
    # Verification - all should be 7, 7, 6
    SET_VALUE(TEMP_REG, 7)      # Expected value for addition and subtraction
    
    CMP EBP, TEMP_REG            # Check addition result (3+4=7)
    JNZ test_failed
    CMP R8, TEMP_REG            # Check subtraction result (9-2=7)  
    JNZ test_failed
    
    SET_VALUE(TEMP_REG, 6)      # Expected value for multiplication
    CMP R9, TEMP_REG            # Check multiplication result (2*3=6)
    JNZ test_failed
    
    # All tests passed!
    PRINT_CHAR(83)              # 'S'
    PRINT_CHAR(85)              # 'U'
    PRINT_CHAR(67)              # 'C'
    PRINT_CHAR(67)              # 'C'
    PRINT_CHAR(69)              # 'E'
    PRINT_CHAR(83)              # 'S'
    PRINT_CHAR(83)              # 'S'
    PRINT_CHAR(33)              # '!'
    PRINT_SPACE
    PRINT_CHAR(65)              # 'A'
    PRINT_CHAR(108)             # 'l'
    PRINT_CHAR(108)             # 'l'
    PRINT_SPACE
    PRINT_CHAR(116)             # 't'
    PRINT_CHAR(101)             # 'e'
    PRINT_CHAR(115)             # 's'
    PRINT_CHAR(116)             # 't'
    PRINT_CHAR(115)             # 's'
    PRINT_SPACE
    PRINT_CHAR(112)             # 'p'
    PRINT_CHAR(97)              # 'a'
    PRINT_CHAR(115)             # 's'
    PRINT_CHAR(115)             # 's'
    PRINT_CHAR(101)             # 'e'
    PRINT_CHAR(100)             # 'd'
    PRINT_CHAR(33)              # '!'
    PRINT_NEWLINE
    
    HALT
    
test_failed:
    PRINT_CHAR(70)              # 'F'
    PRINT_CHAR(65)              # 'A'
    PRINT_CHAR(73)              # 'I'
    PRINT_CHAR(76)              # 'L'
    PRINT_CHAR(69)              # 'E'
    PRINT_CHAR(68)              # 'D'
    PRINT_CHAR(33)              # '!'
    PRINT_NEWLINE
    
    HALT