# Parser Tests

This directory contains tests for advanced assembler parsing features and syntax support.

## Test Files

- `test_sections_and_directives.asm` - Section directives and data declaration tests
- `test_parser_comprehensive.asm` - Comprehensive parsing feature validation
- `test_parser_stress.asm` - Edge cases and parser stress testing

## Coverage

### Section Support
- `.data` section for initialized data
- `.text` section for executable code
- Section switching and symbol table generation
- Proper memory layout organization

### Data Directives
- `.dd` directive for double word (32-bit) data
- Integer values in .dd declarations
- Floating point values in .dd declarations
- Mixed data type support

### Floating Point Parsing
- Decimal point recognition in numeric literals
- Scientific notation support (where implemented)
- Proper FloatExpression AST node generation
- Integration with FPU operations

### Expression Parsing
- Parentheses support `(` and `)`
- Size specifiers: `byte`, `word`, `dword`, `qword`
- Complex expression evaluation
- Nested parentheses handling

### Token Recognition
- LPAREN and RPAREN token types
- Enhanced numeric literal parsing
- Keyword recognition improvements
- Case insensitive parsing where appropriate

## Parser Architecture

### Lexical Analysis (Lexer)
The lexer (`src/assembler/lexer.cpp`) handles:
- Token stream generation
- Numeric literal parsing (integers and floats)
- Keyword and identifier recognition
- Special character tokenization

### Syntax Analysis (Parser)  
The parser (`src/assembler/parser.cpp`) handles:
- AST node generation
- Expression parsing with precedence
- Statement and directive parsing
- Error recovery and reporting

### AST Nodes
New expression types in `src/assembler/ast.hpp`:
- `FloatExpression` - Floating point literals
- `STRegisterExpression` - ST(n) register references
- Enhanced primary expression parsing

## Running Parser Tests

From project root:

```bash
# Run specific parser tests
./bin/demi-engine --assembly tests/parsing/test_sections_and_directives.asm
./bin/demi-engine --assembly tests/parsing/test_parser_comprehensive.asm
./bin/demi-engine --assembly tests/parsing/test_parser_stress.asm

# With verbose output to see parsing details
./bin/demi-engine --assembly tests/parsing/test_parser_comprehensive.asm --verbose=true
```

## Test Categories

### Section Directive Tests
```assembly
.data
    value dd 42.5
    counter dd 0

.text
main:
    mov eax, value
    hlt
```

### Size Specifier Tests
```assembly
mov eax, dword ptr [ebx]
mov ax, word ptr [ecx]  
mov al, byte ptr [edx]
```

### Floating Point Parsing Tests
```assembly
.data
    pi dd 3.14159
    large dd 1.23e6

.text
main:
    fld pi
    fld 2.718
    hlt
```

### Parentheses Expression Tests
```assembly
mov eax, (ebx + ecx)
add edx, (value + 10)
```

## Expected Behavior

Parser tests validate:

1. **Correct Tokenization**: All new tokens properly recognized
2. **AST Generation**: Proper expression node creation
3. **Error Handling**: Graceful handling of syntax errors
4. **Integration**: New features work with existing assembly
5. **Edge Cases**: Robust handling of complex expressions

## Error Cases Tested

- Invalid ST register indices (ST(8), ST(-1))
- Malformed floating point numbers
- Unmatched parentheses
- Invalid size specifiers
- Section directive errors