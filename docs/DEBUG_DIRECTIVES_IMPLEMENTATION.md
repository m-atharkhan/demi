# Debug Directives Implementation Summary

## Overview
Successfully implemented 14 debug directives for the Demi VM/assembler system. All directives only produce output when the `-d` (debug) flag is used.

## Debug Directives

### Original 5 Directives (0-4)

1. **.print** (Sub-opcode 0)
   - Syntax: `.print "string"` or `.print register`
   - Prints string literals or register values to stdout
   - Example: `.print "Hello"`, `.print rax`

2. **.break** (Sub-opcode 1)
   - Syntax: `.break`
   - Sets a breakpoint, prints PC address
   - Example output: `BREAKPOINT at 0x100`

3. **.dump** (Sub-opcode 2)
   - Syntax: `.dump`
   - Dumps all CPU registers and flags
   - Uses CPU's print_state() method

4. **.memdump** (Sub-opcode 3)
   - Syntax: `.memdump address, length`
   - 16-byte wide hex dump with ASCII representation
   - Professional format with address, hex bytes, and ASCII column
   - Example: `.memdump rsp, 32`

5. **.trace** (Sub-opcode 4)
   - Syntax: `.trace 0|1|2` (0=off, 1=on, 2=toggle)
   - Controls instruction-level tracing
   - Uses Config::trace flag (separate from Config::debug)

### New 9 Directives (5-13)

6. **.assert** (Sub-opcode 5)
   - Syntax: `.assert register, expected_value`
   - Checks if register equals expected value
   - Example: `.assert rax, 42`
   - Output: `ASSERTION PASSED` or `ASSERTION FAILED`

7. **.dumpstack** (Sub-opcode 6)
   - Syntax: `.dumpstack [depth]` (default depth: 16)
   - Dumps stack contents with SP-relative addressing
   - Shows hex and decimal values for each stack entry
   - Example: `.dumpstack 8`

8. **.watch** (Sub-opcode 7)
   - Syntax: `.watch address, length`
   - Sets a memory watchpoint (notification only)
   - Example: `.watch 500, 16`
   - Output: `WATCHPOINT set at 0x1f4 (length 16)`

9. **.unwatch** (Sub-opcode 8)
   - Syntax: `.unwatch address`
   - Removes a memory watchpoint
   - Example: `.unwatch 500`
   - Output: `WATCHPOINT removed at 0x1f4`

10. **.checkpoint** (Sub-opcode 9)
    - Syntax: `.checkpoint "label"`
    - Marks a checkpoint in execution
    - Example: `.checkpoint "Before loop"`
    - Output: `CHECKPOINT: Before loop at 0xc2`

11. **.log** (Sub-opcode 10)
    - Syntax: `.log level, "message"` (level: 0=DEBUG, 1=INFO, 2=WARN, 3=ERROR)
    - Leveled logging for categorized messages
    - Example: `.log 2, "Warning message"`
    - Output: `[WARN] Warning message`

12. **.dumpreg** (Sub-opcode 11)
    - Syntax: `.dumpreg register`
    - Dumps single register with name, hex, and decimal
    - Example: `.dumpreg rax`
    - Output: `RAX = 0x2a (42)`

13. **.memset** (Sub-opcode 12)
    - Syntax: `.memset address, length, value`
    - Fills memory region with byte value
    - Example: `.memset 600, 32, 0xFF`
    - Output: `MEMSET: filled 32 bytes at 0x258 with 0xff`

14. **.step** (Sub-opcode 13)
    - Syntax: `.step [count]` (default count: 1)
    - Marks single-stepping execution points
    - Example: `.step 5`
    - Output: `STEP: executing 5 instruction(s) starting at 0x164`

## Technical Details

### Opcode Structure
- **Opcode:** 0x42 (DEBUG)
- **Format:** `DEBUG sub_opcode [operands...]`
- **Sub-opcodes:** 0-13 (14 total directives)

### Bytecode Encoding
Each directive emits:
1. Opcode byte (0x42)
2. Sub-opcode byte (0-13)
3. Directive-specific operands (varies by directive)

### Configuration
- **Config::debug** - CLI flag (-d), controls all debug directive output
- **Config::trace** - Runtime flag, controls instruction-level tracing (set by .trace directive)

## Files Modified

### Frontend (Parsing & Assembly)
- `src/assembler/token.hpp` - Added 9 new token types
- `src/assembler/lexer.cpp` - Added 9 new keywords
- `src/assembler/parser.hpp` - Declared parse_debug_directive()
- `src/assembler/parser.cpp` - Extended parse_debug_directive() to handle all 14 directives
- `src/assembler/assembler.hpp` - Added emit_qword() declaration
- `src/assembler/assembler.cpp` - Extended DEBUG instruction emission for all sub-opcodes

### Backend (Execution)
- `src/engine/opcodes/opcode_dispatcher_inlined.cpp` - Implemented execution logic for all sub-opcodes
- `src/config.hpp` - Added Config::trace flag

### Testing
- `tests/debug_new.test.asm` - Comprehensive test of all 14 directives
- `examples/debug_directives.asm` - Example usage (original 5 directives)

## Usage Example

```asm
.memory 1024

load_imm64 rax, 100
.print "RAX value:"
.print rax

.assert rax, 100
.log 1, "Starting computation"

load_imm64 rbx, 200
.checkpoint "Before operation"
add64 rax, rbx

.dumpreg rax
.dump

.memdump 0, 64
```

Run with: `./bin/demi-engine-debug -d -A program.asm`

## Notes
- All directives are no-ops when not in debug mode (no -d flag)
- Memory operations (watch/unwatch/memset) are informational only in current implementation
- Full watchpoint tracking and step-mode execution would require additional CPU state management
- All directives have been tested and verified working

## Build Instructions
```bash
make clean && make
```

## Test Instructions
```bash
./bin/demi-engine-debug -d -A tests/debug_new.test.asm
```
