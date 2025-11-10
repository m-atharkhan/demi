# README: Visual Calculator Include Example

This directory contains a comprehensive example demonstrating the DemiEngine Assembly preprocessor include system with visual output.

## Files Overview

### calculator_demo.asm
- **Purpose**: Main visual calculator program demonstrating include usage  
- **Features**:
  - `.include "simple_utils.inc"` - includes macros and constants
  - `.include "math_operations.asm"` - includes cross-file functions
  - **Visual output** showing calculations step by step
  - Macro expansion for register operations and printing
  - Verification of calculation results with success/failure messages

### simple_utils.inc
- **Purpose**: Utility macros, register aliases, and print functions
- **Content**: 
  - Register aliases: OPERAND_A=R0, OPERAND_B=R1, RESULT=R2, TEMP_REG=R5, PRINT_REG=R6
  - Operation constants: OP_ADD=1, OP_SUBTRACT=2, OP_MULTIPLY=3  
  - ASCII constants: ASCII_NEWLINE, ASCII_SPACE, ASCII_EQUALS, ASCII_PLUS, ASCII_MINUS, ASCII_MULTIPLY
  - Utility macros: SET_VALUE(reg, val), CLEAR_REG(reg)
  - **Print macros**: PRINT_CHAR(ascii_val), PRINT_NEWLINE, PRINT_SPACE, PRINT_EQUALS, **PRINT_DIGIT(reg)** for dynamic number display

### math_operations.asm  
- **Purpose**: Mathematical operation functions demonstrating cross-file includes
- **Content**: Three static math functions (static_addition, static_subtraction, static_multiplication)
- **Features**: Demonstrates that .asm files can be included for code organization

## How to Run

```bash
cd examples/include_example
../../bin/demi-engine --assembly calculator_demo.asm
```

## Visual Output

The calculator now provides **dynamic visual feedback** showing actual calculated values:

```
Calculator Demo

Test 1: 3 + 4 = 7
Test 2: 9 - 2 = 7
Test 3: 2 * 3 = 6

Verifying...
SUCCESS! All tests passed!
```

### Dynamic Features:
- **Real-time operand display**: Shows actual register values, not hard-coded text
- **Calculated results**: Dynamically converts numeric results to ASCII and displays them
- **Single-digit arithmetic**: Demonstrates number-to-ASCII conversion (0-9)
- **Live verification**: Results are computed and verified at runtime

## Key Features Demonstrated

1. **Multi-file includes**: Both .inc and .asm files
2. **Cross-file labels**: Functions defined in one file, called from another
3. **Macro expansion**: Parameter-based macros with proper substitution
4. **Register aliases**: Symbolic names for registers (OPERAND_A, RESULT, etc.)
5. **Modular programming**: Separation of utilities, math operations, and main logic
6. **🆕 Visual output**: Real-time display of calculations and results
7. **🆕 Print macros**: Reusable printing utilities for characters and numbers

## Expected Output

The calculator performs three mathematical operations with **dynamic visual feedback**:
- **Addition**: 3 + 4 = 7 (values read from registers and computed in real-time)
- **Subtraction**: 9 - 2 = 7 (values read from registers and computed in real-time)  
- **Multiplication**: 2 × 3 = 6 (values read from registers and computed in real-time)
- **Verification**: Shows success/failure status with clear messages based on actual computed results

## Technical Details

- **Preprocessor Integration**: Assembly mode includes full preprocessing
- **Symbol Resolution**: Cross-file labels properly resolved in symbol table
- **Macro Processing**: Function macros with parameters working correctly
- **Include System**: Relative path resolution with circular dependency protection
- **🆕 Output System**: Uses OUT instruction with port 1 for console output
- **🆕 ASCII Printing**: Character-by-character output for clear visual feedback  
- **🆕 Dynamic Number Display**: Real-time conversion of register values to ASCII digits (0-9)

## Educational Value

This example demonstrates:
- **Modular Assembly Programming**: Clean separation of concerns across files
- **Advanced Preprocessing**: Include system with macro expansion 
- **Cross-File Development**: Functions callable across module boundaries
- **Visual Debugging**: Output techniques for assembly program verification
- **Professional Practices**: Structured code organization and documentation