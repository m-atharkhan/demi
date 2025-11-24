# Error Handling & Debug System Implementation Report

## 1. IMPLEMENTED ERROR HANDLING SYSTEM

### Current Error Handling Architecture

The DemiEngine now features a comprehensive, centralized error handling system implemented in `src/debug/error_handler.hpp` and `src/debug/error_handler.cpp`.

#### 1.1 ErrorHandler Class

**Core Features:**
- ✅ Centralized error reporting through singleton pattern
- ✅ Structured error codes categorized by subsystem
- ✅ Context-rich error messages with CPU state
- ✅ Multiple severity levels (Info, Warning, Error, Critical)
- ✅ Consistent error formatting across the codebase
- ✅ Quiet mode support for test execution

**Error Code Categories:**
- **0x001-0x099**: Parser/Lexer errors
- **0x100-0x199**: Assembly errors  
- **0x200-0x299**: Runtime/CPU errors
- **0x300-0x399**: Memory errors
- **0x400-0x499**: I/O errors
- **0x500-0x599**: Test framework errors

#### 1.2 Error Context Information

The system now provides rich contextual information:

```cpp
// Example runtime error with full context
ErrorHandler::instance().report_runtime(
    ErrorCode::CPU_DIVISION_BY_ZERO,
    "Division by zero detected",
    cpu.get_pc(),           // Program counter
    cpu.get_registers(),    // Register state
    cpu.get_stack_trace()   // Call stack
);
```

**Context includes:**
- Program counter (PC) at time of error
- Complete register state dump
- Call stack trace for runtime errors
- Line/column information for parse errors
- File path for I/O errors
- Memory access patterns for bounds violations

### 1.3 Error Message Examples

**Before Implementation:**
```
Error: Division by zero
Memory out of bounds
Unexpected token
```

**After Implementation:**
```
[ERROR:0x200] [CPU] [CRITICAL] Division by zero at PC=0x1234
Context: R0=5, R1=0, SP=0x7FF8, FLAGS=0x0002
Stack: main+0x1234 -> arithmetic_test+0x45

[ERROR:0x202] [CPU] [ERROR] Memory access violation  
Context: Attempted read at 0x8000 (available: 0x0000-0x7FFF)
PC=0x0456, instruction: LOAD R0, [R1]

[ERROR:0x006] [PARSE] [ERROR] Unexpected token '}' at line 15:8
Context: Expected 'number' or 'identifier' in operand
File: tests/example.test.asm
```

## 2. IMPLEMENTED DEBUG SYSTEM

### 2.1 DebugHandler Architecture

The debug system is implemented in `src/debug/debug_handler.hpp` with structured debugging categories and context tracking.

#### Debug Categories
- **0x001-0x099**: CPU/Engine debug
- **0x100-0x199**: Assembly/Parser debug
- **0x200-0x299**: Memory/Storage debug  
- **0x300-0x399**: I/O/Device debug
- **0x400-0x499**: Test/Execution debug
- **0x500-0x599**: GUI/Interface debug
- **0x600-0x699**: Performance/Profiling debug

#### Debug Context Tracking
```cpp
// Structured debug output with context
DEBUG_INFO(DebugCategory::CPU_EXECUTION, 
    "Executing instruction at PC={:04X}: {} R{}={}", 
    pc, opcode_name, reg, value);
```

### 2.2 Enhanced Logger System

**Features Implemented:**
- ✅ Structured logging with categories
- ✅ Context-aware debug messages
- ✅ Thread-safe logging operations
- ✅ Performance profiling integration
- ✅ Quiet mode for test execution
- ✅ Conditional debug output based on categories

**Debug Output Format:**
```
[DEBUG:0x001] [CPU] Executing ADD R0, R1 at PC=0x0012
  Before: R0=0x05, R1=0x03, FLAGS=0x0000
  After:  R0=0x08, FLAGS=0x0000 (No carry/overflow)
```

## 3. INTEGRATION WITH TEST FRAMEWORK

### 3.1 Assembly Test Error Handling

The assembly test executor now uses the centralized error handling:

```cpp
// In assembly_test_executor.cpp
Logging::ErrorHandler::instance().set_quiet_mode(true);
auto bytecode = assembler.assemble_string(asm_code, base_path);
Logging::ErrorHandler::instance().set_quiet_mode(false);
```

**Benefits:**
- Clean test output without debug noise
- Proper error reporting for test failures
- Structured error messages in test results

### 3.2 Test Result Visualization

**Current Test Output:**
```
📁 Testing file: tests/algorithms.test.asm
fibonacci calculation ✓
factorial calculation ✓
maximum of three numbers ✓
sum array of numbers ✓
binary search simulation ✓

   5 tests: 5 passed, 0 failed

📊 Overall Summary
Files tested: 31
Total tests: 118
Passed: 116
Failed: 0

🎉 All tests passed!
```

## 4. PERFORMANCE AND RELIABILITY

### 4.1 Error Handling Performance
- **Zero-cost when no errors occur** (conditional compilation)
- **Thread-safe error reporting** with minimal locking overhead
- **Structured error codes** enable programmatic error handling

### 4.2 Debug Performance  
- **Conditional debug compilation** with DEBUG_INFO macros
- **Category-based filtering** reduces debug noise
- **Lazy string formatting** only when debug is enabled

## 5. CURRENT IMPLEMENTATION STATUS

### ✅ Completed Features

#### Error Handling System:
- [x] ErrorHandler singleton class
- [x] Structured error codes (0x001-0x5FF)
- [x] Context-rich error messages
- [x] Multiple severity levels
- [x] Runtime error reporting with CPU state
- [x] Parse error reporting with file/line info
- [x] I/O error reporting with path context
- [x] Quiet mode for test execution

#### Debug System:
- [x] DebugHandler with category-based debugging
- [x] Structured debug categories (0x001-0x6FF)
- [x] Context tracking for debug messages
- [x] Performance profiling integration
- [x] Thread-safe debug operations
- [x] Conditional debug compilation

#### Logger Integration:
- [x] Enhanced Logger class with structured output
- [x] Error/debug message formatting
- [x] Test framework integration
- [x] Clean test result visualization

### 🚧 Future Enhancements

#### Interactive Debugging (Planned):
- [ ] GDB-style interactive debugger (`--gdb` flag)
- [ ] Breakpoint system (PC-based, opcode-based)
- [ ] Step/continue execution control
- [ ] Register/memory inspection commands
- [ ] Call stack inspection
- [ ] Watch points and conditional debugging

#### Advanced Error Recovery:
- [ ] Error recovery mechanisms (continue after parse errors)
- [ ] Error suppression categories
- [ ] Context-aware error suggestions
- [ ] Error statistics and reporting

## 6. ERROR CODE REFERENCE

### Parser/Lexer Errors (0x001-0x099)
- `0x001`: Duplicate label definition
- `0x002`: Unknown directive  
- `0x003`: Invalid operand format
- `0x004`: Missing required operand
- `0x005`: Invalid number format
- `0x006`: Unexpected token
- `0x007`: Generic parse error

### Assembly Processing (0x100-0x199)
- `0x100`: Unknown instruction
- `0x101`: Invalid register name
- `0x102`: Invalid immediate value (out of range)
- `0x103`: Symbol not found (undefined label)
- `0x104`: Forward reference resolution failed
- `0x105`: Instruction encoding not implemented
- `0x106`: Directive processing error

### Runtime/CPU Errors (0x200-0x299)
- `0x200`: Division by zero
- `0x201`: Modulo by zero
- `0x202`: Memory out of bounds access
- `0x203`: Invalid register access
- `0x204`: Call stack overflow
- `0x205`: Stack overflow (PUSH)
- `0x206`: Stack underflow (POP)
- `0x207`: Invalid jump address
- `0x208`: Invalid opcode

### Memory Errors (0x300-0x399)
- `0x300`: Memory allocation failure
- `0x301`: Memory deallocation error
- `0x302`: Memory corruption detected
- `0x303`: Invalid memory access pattern

### I/O Errors (0x400-0x499)
- `0x400`: File not found
- `0x401`: Directory not found
- `0x402`: File read error
- `0x403`: File write error
- `0x404`: Invalid path
- `0x405`: Permission denied

### Test Framework (0x500-0x599)
- `0x500`: Test exceeded max steps (infinite loop)
- `0x501`: Test assertion failed
- `0x502`: Unexpected error in test execution
- `0x503`: Invalid test format
- `0x504`: Test compilation failed

## 7. USAGE EXAMPLES

### Error Handling Usage:
```cpp
// Runtime error with context
ErrorHandler::instance().report_runtime(
    ErrorCode::CPU_DIVISION_BY_ZERO,
    "Attempted division by zero in arithmetic test",
    cpu.get_pc(),
    cpu.get_registers()
);

// Parse error with file context
ErrorHandler::instance().report_parse(
    ErrorCode::PARSE_UNEXPECTED_TOKEN,
    "Expected register operand",
    "tests/example.asm", 15, 8
);

// I/O error with path context  
ErrorHandler::instance().report_io(
    ErrorCode::IO_FILE_NOT_FOUND,
    "/path/to/missing/file.asm",
    "Assembly source file not found"
);
```

### Debug System Usage:
```cpp
// Conditional debug output
DEBUG_INFO(DebugCategory::CPU_EXECUTION, 
    "ADD instruction: R{} ({}) + R{} ({}) = {} (flags: {})",
    reg1, val1, reg2, val2, result, flags);

// Performance profiling
DEBUG_PROF(DebugCategory::PERF_INSTRUCTION,
    "Instruction execution time: {}μs", duration);

// Memory access debugging
DEBUG_MEM(DebugCategory::MEM_ACCESS,
    "Memory read: address=0x{:04X}, value=0x{:02X}", addr, value);
```

## Summary

**Implementation Status:**
- ✅ **Error Handling**: Complete centralized system with structured error codes and rich context
- ✅ **Debug System**: Comprehensive debug categories with context tracking
- ✅ **Logger Integration**: Enhanced formatting and test framework integration  
- ✅ **Test Framework**: Clean error reporting and quiet mode support
- ✅ **Performance**: Zero-cost when disabled, thread-safe operations
- 🚧 **Interactive Debugging**: Planned for future release

The error handling and debug systems provide a solid foundation for development, debugging, and production use of the DemiEngine virtual machine.

3. **Redundant/Vague Messages**
   - "Test execution failed: {e.what()}" - doesn't say which test
   - "Unknown exception" - completely unhelpful
   - "Unexpected token" - doesn't show what token was found

4. **No Error Categories/Codes**
   - Can't programmatically handle specific errors
   - Can't distinguish recoverable vs. fatal errors
   - No severity levels

5. **Duplicate Error Reporting**
   - Errors logged to both Logger and Config::error_count
   - Some errors log then throw, then caught and logged again
   - stderr and Logger both used inconsistently

### 1.3 Current Error Handling Patterns

**Pattern 1: Error + Exception**
```cpp
Logger::instance().error() << error_msg << std::endl;
Config::error_count++;
throw CPUException(error_msg);  // Redundant - already logged
```

**Pattern 2: Try-Catch with Generic Message**
```cpp
catch (const std::exception& e) {
    std::cerr << "Error: Exception: " << e.what() << std::endl;
    Config::error_count++;
}
```

**Pattern 3: Direct stderr**
```cpp
std::cerr << "Error: Test path not found: " << filepath << std::endl;
```

---

## 2. ERROR HANDLING IMPROVEMENT PLAN

### 2.1 Immediate Improvements (High Priority)

**A) Create Consistent Error Format**
```cpp
// Proposed format:
[ERROR_CODE:0x001] [SUBSYSTEM] [SEVERITY] Message
Context: PC=0x1234, R0=0xAB, SP=0x5000
Suggestion: Fix your code like this...
```

**B) Define Error Code System**
- 0x001-0x099: Parser/Lexer errors
- 0x100-0x199: Assembly errors
- 0x200-0x299: Runtime/CPU errors
- 0x300-0x399: Memory errors
- 0x400-0x499: I/O errors
- 0x500-0x599: Test framework errors

**C) Centralize Error Handling**
- Create `ErrorHandler` class
- Standardize all error reporting through it
- Prevent duplicate logging

**D) Improve Specific Error Messages**

Current → Improved Examples:
- "Invalid Division Division by zero" → "Runtime error at PC=0x1234: Division by zero (R0 = 5, R1 = 0)"
- "Memory out of bounds" → "Memory access error: Attempted to read at 0xFFFF, available memory: 0x0000-0x7FFF"
- "Unexpected token" → "Unexpected token '}' at line 15, column 8. Expected 'number' or 'identifier'"
- "Stack underflow" → "Stack underflow at PC=0x0456: POP attempted with SP=0x0100 (memory ends at 0x00FF)"

### 2.2 Future Enhancements (Next Release)

- [ ] Error recovery mechanisms (partial compilation, continue after errors)
- [ ] Error suppression categories (warnings vs. errors)
- [ ] Context-aware suggestions (hint system for common mistakes)
- [ ] Error aggregation report (summary of all errors at end)
- [ ] Colored error output with highlighted problem areas
- [ ] Stack trace for runtime errors
- [ ] Memory snapshot on critical errors (dump registers/memory state)
- [ ] Error statistics per category

---

## 3. DEBUG/LOGGING SYSTEM ANALYSIS

### 3.1 Current Debug Message Assessment

**Debug Messages by Category:**

| Category | Location | Count | Quality |
|----------|----------|-------|---------|
| Opcode Execution | opcodes_consolidated.cpp | 50+ | VERBOSE - logs every step |
| Memory Operations | cpu.cpp | 6 | MODERATE - missing context |
| Assembly Processing | assembler.cpp | 6 | LOW - vague messages |
| Instruction Fetch | cpu.cpp | 4 | HIGH - clear state info |
| Device I/O | devices/*.hpp | 8 | MODERATE - format inconsistent |
| Test Framework | assembly_test_executor.cpp | 4 | LOW - missing test context |

### 3.2 Debug Message Issues

1. **Too Verbose for Operand Execution**
   - Every ADD, SUB, etc. logs before AND after
   - Creates noise that drowns out important debug info
   - Hard to find the actual problem

2. **Inconsistent Timestamps**
   - Some have PC prefixes, some don't
   - Makes tracing execution flow difficult

3. **Missing Correlation IDs**
   - Can't trace which test/file caused a debug message
   - Multiple tests running concurrently would be confusing

4. **Incomplete Information**
   - [FETCH_OPERAND] logs PC but not the operand address
   - Memory access logs address but not what's stored there
   - Arithmetic ops log values but not flags (carry, overflow)

---

## 4. PROPOSED NEW DEBUG SYSTEM ARCHITECTURE

### 4.1 Enhanced Logger Redesign

```cpp
// New structured logging format:
class DebugContext {
    std::string test_name;        // Which test is running
    uint32_t pc;                  // Program counter
    uint32_t instruction_count;   // Instruction number since entry
    std::map<std::string, std::string> vars;  // Contextual variables
};

// Usage:
DEBUG_CONTEXT.set_pc(cpu.get_pc());
Logger::instance().debug()
    .with_context(DEBUG_CONTEXT)
    .format("ADD", "R0={} R1={} -> R0={} (C={}, O={})")
    .args(cpu.get_register(0), cpu.get_register(1), 
          cpu.get_register(0), carry_flag, overflow_flag)
    .flush();
```

### 4.2 GDB-Style Debugger Mode (`--gdb` / `-g`)

**Three-Letter Flag Design:**
- `-g` or `--gdb`: Enter interactive debugger mode

**Core Features:**
- **Breakpoints**: `break PC=0x1234` or `break opcode=JMP`
- **Step Execution**: `step` (next instruction), `stepover` (skip functions)
- **Register Inspection**: `registers`, `register R0`, `memory 0x1000 0x1010`
- **Call Stack**: `backtrace`, `frame N`
- **Conditional Debugging**: `when R0 > 100`, `on_error`
- **Tracing**: `trace on|off`, `trace_filter opcode=ADD`
- **Watch Points**: `watch R0`, `watch_memory 0x5000`

**Interactive Session Format:**
```
(demi-gdb) break 0x1234
Breakpoint 1 set at PC=0x1234

(demi-gdb) run tests/arithmetic.test.asm
Starting test: addition
PC=0x0000: LOAD_IMM R0, 0x05
  R0 = 0x00000000
PC=0x0002: LOAD_IMM R1, 0x03
  R0 = 0x00000005

Breakpoint 1 hit at PC=0x1234
(demi-gdb) registers
R0  = 0x00000008  (decimal: 8)
R1  = 0x00000003  (decimal: 3)
...

(demi-gdb) memory 0x5000 0x5010
0x5000: 00 01 02 03 04 05 06 07 | 08 09 0A 0B 0C 0D 0E 0F

(demi-gdb) continue
```

### 4.3 Logger Visual Improvements

**Current Output:**
```
[25-11-17 11:52:05.152] [INFO] === Running tests from tests/algorithms.test.asm ===
[25-11-17 11:52:05.152] [INFO] fibonacci calculation ✓
```

**Proposed Output:**
```
┌─────────────────────────────────────────────────────┐
│  DEMI Engine Test Suite                             │
│  File: tests/algorithms.test.asm                    │
│  Tests: 5  Passed: 5  Failed: 0  [████████████] 100% │
└─────────────────────────────────────────────────────┘

  ✓ fibonacci calculation (334ms)
  ✓ factorial calculation (189ms)
  ✓ maximum of three numbers (42ms)
  ✓ sum array of numbers (25ms)
  ✓ binary search simulation (71ms)

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  Summary:
  ├─ Total Tests:      5
  ├─ Assertions:      10 passed
  ├─ Total Time:     661ms
  └─ Category:       Algorithms
```

---

## 5. IMPLEMENTATION ROADMAP

### Phase 1: Error Handling Improvements (1-2 hours)
- [ ] Create `ErrorHandler` class with error codes
- [ ] Standardize error message format
- [ ] Add context information to runtime errors
- [ ] Consolidate error reporting (no duplicates)
- [ ] Add error recovery hints

### Phase 2: Debug System Redesign (2-3 hours)
- [ ] Create `DebugContext` for thread-safe context tracking
- [ ] Redesign Logger output format with structure
- [ ] Implement visual table/box drawing for test results
- [ ] Add progress indicators and timing information

### Phase 3: GDB-Style Debugger (3-4 hours)
- [ ] Create `DebuggerREPL` class for interactive commands
- [ ] Implement breakpoint system (PC-based, opcode-based)
- [ ] Add register/memory inspection commands
- [ ] Implement step/continue execution control
- [ ] Add call stack inspection

### Phase 4: Testing & Polish (1 hour)
- [ ] Test error messages with various scenarios
- [ ] Verify debugger works with test suite
- [ ] Performance validation
- [ ] Documentation updates

---

## 6. ERROR CODE REFERENCE

### Assembly/Parser (0x001-0x099)
- 0x001: Duplicate label definition
- 0x002: Unknown directive
- 0x003: Invalid operand format
- 0x004: Missing required operand
- 0x005: Invalid number format
- 0x006: Unexpected token
- 0x007: Parse error

### Assembly Processing (0x100-0x199)
- 0x100: Unknown instruction
- 0x101: Invalid register name
- 0x102: Invalid immediate value (out of range)
- 0x103: Symbol not found (undefined label)
- 0x104: Forward reference resolution failed
- 0x105: Instruction encoding not implemented

### Memory/CPU Runtime (0x200-0x299)
- 0x200: Division by zero
- 0x201: Memory out of bounds
- 0x202: Invalid register access
- 0x203: Call stack overflow
- 0x204: Stack overflow (PUSH)
- 0x205: Stack underflow (POP)
- 0x206: Invalid jump address
- 0x207: Invalid opcode

### I/O Errors (0x400-0x499)
- 0x400: File not found
- 0x401: Directory not found
- 0x402: File read error
- 0x403: File write error
- 0x404: Invalid path
- 0x405: Permission denied

### Test Framework (0x500-0x599)
- 0x500: Test exceeded max steps (infinite loop)
- 0x501: Test assertion failed
- 0x502: Unexpected error in test
- 0x503: Invalid test format

---

## Summary

**Current State:**
- ✓ Error handling exists but inconsistent
- ✗ Error messages lack context and structure
- ✗ No systematic error categorization
- ✗ Debug output is verbose and hard to parse
- ✗ No interactive debugging capability

**Proposed State:**
- ✓ Structured error messages with codes
- ✓ Rich context (register state, memory, etc.)
- ✓ New logger visual format
- ✓ Interactive GDB-style debugger
- ✓ Better developer experience
