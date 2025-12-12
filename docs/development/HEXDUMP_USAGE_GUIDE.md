# Hexdump Usage and Implementation Guide

## Overview

This document describes the hexdump functionality in Demi Engine, including current implementation, usage patterns, and recommendations for additional hexdump points throughout the codebase.

**Status**: Implemented (v1.0)  
**Last Updated**: 2025-12-03  
**Category**: Debug & Development Tools

## Current Implementation

### Hexdump Flag

**Command Line Option**: `--hexdump`

**Purpose**: Enable bytecode hexdump output after assembly

**Implemented In**: `src/main.cpp`

```cpp
// Hexdump argument
parser.add_action_arg("hexdump", "--hexdump", "", "Enable bytecode hex dump output after assembly",
    [this]() {
        Config::debug = true;
        Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::ASM_HEXDUMP);
    });
```

### Current Hexdump Location

**Location**: Assembly Mode (`run_assembly_mode()` function)

**Trigger**: After bytecode assembly, before execution

**Output Format**:

```
Assembled bytecode hex dump (256 bytes):
0x0000: 01 00 0a 00 00 00 01 01 05 00 00 00 02 00 01 ff
0x0010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
...
```

**Implementation**:

```cpp
// Print hex dump of assembled bytecode (using ASM_HEXDUMP category)
{
    std::ostringstream hexdump;
    hexdump << "Assembled bytecode hex dump (" << bytecode.size() << " bytes):\n";
    for (size_t i = 0; i < bytecode.size(); ++i) {
        if (i % 16 == 0) {
            if (i > 0) hexdump << "\n";
            hexdump << "0x" << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
        }
        hexdump << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(bytecode[i]) << " ";
    }
    DEBUG_INFO(Logging::DebugCategory::ASM_HEXDUMP, "{}", hexdump.str());
}
```

### Debug Category

**Category**: `ASM_HEXDUMP` (0x107)

**Subsystem**: Assembly/Parser (0x100-0x199)

**Default State**: Disabled (must be explicitly enabled with `--hexdump`)

## Usage Examples

### Basic Assembly with Hexdump

```bash
# Assemble and show hexdump
./bin/demi-engine-debug --hexdump -A program.asm

# Combine with debug mode for full verbosity
./bin/demi-engine-debug --hexdump --debug -A program.asm

# Hexdump with specific debug level
./bin/demi-engine-debug --hexdump --debug-level detail -A program.asm
```

### Viewing Only Hexdump (Minimal Output)

```bash
# Show hexdump without execution logs
./bin/demi-engine-debug --hexdump --debug-quiet -A program.asm
```

## Recommended Additional Hexdump Points

### 1. Memory Operation Hexdumps

**Category**: `MEM_ACCESS` or new `MEM_HEXDUMP` (0x206)

**Use Cases**:

- Memory writes (STORE operations)
- Memory reads (LOAD operations)
- Memory initialization
- Stack operations (PUSH/POP)

**Suggested Implementation**:

```cpp
// In cpu.cpp - write_mem32()
void CPU::write_mem32(uint32_t addr, uint32_t value) {
    if (addr + 4 > memory.size()) {
        throw CPUException("Memory write out of bounds");
    }

    // Hexdump before write
    if (DebugHandler::instance().is_category_enabled(DebugCategory::MEM_HEXDUMP)) {
        std::ostringstream dump;
        dump << "Memory write at 0x" << std::hex << addr << ": "
             << "old=[";
        for (int i = 0; i < 4; i++) {
            dump << std::hex << std::setw(2) << std::setfill('0')
                 << static_cast<int>(memory[addr + i]) << " ";
        }
        dump << "] new=[";
        for (int i = 0; i < 4; i++) {
            dump << std::hex << std::setw(2) << std::setfill('0')
                 << static_cast<int>((value >> (i * 8)) & 0xFF) << " ";
        }
        dump << "]";
        DEBUG_DETAIL(DebugCategory::MEM_HEXDUMP, "{}", dump.str());
    }

    // Perform write
    memory[addr] = value & 0xFF;
    memory[addr + 1] = (value >> 8) & 0xFF;
    memory[addr + 2] = (value >> 16) & 0xFF;
    memory[addr + 3] = (value >> 24) & 0xFF;
}
```

**CLI Flag**: `--hexdump-memory` or `--memdump-hex`

### 2. Device I/O Hexdumps

**Category**: `IO_HEXDUMP` (0x306)

**Use Cases**:

- Port read/write operations
- File device operations
- RAMDisk block transfers
- Network packet inspection

**Suggested Implementation**:

```cpp
// In device_manager.cpp
void DeviceManager::writePortDWord(uint8_t port, uint32_t value) {
    auto it = devices.find(port);
    if (it != devices.end()) {
        // Hexdump I/O operation
        if (DebugHandler::instance().is_category_enabled(DebugCategory::IO_HEXDUMP)) {
            std::ostringstream dump;
            dump << "I/O Write Port 0x" << std::hex << static_cast<int>(port)
                 << " <- [" << std::hex << std::setw(8) << std::setfill('0') << value << "] "
                 << "(device: " << it->second->get_name() << ")";
            DEBUG_DETAIL(DebugCategory::IO_HEXDUMP, "{}", dump.str());
        }

        it->second->write(value);
    }
}
```

**CLI Flag**: `--hexdump-io`

### 3. Stack Frame Hexdumps

**Category**: `CPU_STACK` or `STACK_HEXDUMP` (0x004 or new)

**Use Cases**:

- Function call stack frames
- Stack overflow detection
- Stack unwinding debugging
- Return address verification

**Suggested Implementation**:

```cpp
// In cpu.cpp - for CALL instruction
void handle_call(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // ... existing call logic ...

    // Hexdump stack after CALL
    if (DebugHandler::instance().is_category_enabled(DebugCategory::STACK_HEXDUMP)) {
        std::ostringstream dump;
        uint32_t sp = cpu.get_sp();
        dump << "Stack after CALL (SP=0x" << std::hex << sp << "):\n";

        // Show top 8 stack entries
        for (int i = 0; i < 8 && sp + (i * 4) < cpu.get_memory().size(); i++) {
            uint32_t addr = sp + (i * 4);
            uint32_t value = cpu.read_mem32(addr);
            dump << "  [SP+" << std::dec << (i * 4) << "] 0x" << std::hex
                 << std::setw(8) << std::setfill('0') << value << "\n";
        }
        DEBUG_DETAIL(DebugCategory::STACK_HEXDUMP, "{}", dump.str());
    }
}
```

**CLI Flag**: `--hexdump-stack`

### 4. Instruction Fetch Hexdumps

**Category**: `CPU_EXECUTION` or `EXEC_HEXDUMP` (0x001 or new)

**Use Cases**:

- Instruction stream analysis
- Opcode verification
- Jump target verification
- Self-modifying code detection

**Suggested Implementation**:

```cpp
// In cpu.cpp - execute() loop
void CPU::execute(const std::vector<uint8_t>& program, uint32_t entry_address) {
    // ... initialization ...

    while (running && registers[static_cast<size_t>(Register::RIP)] < program.size()) {
        uint32_t pc = registers[static_cast<size_t>(Register::RIP)];
        uint8_t opcode = program[pc];

        // Hexdump instruction fetch
        if (DebugHandler::instance().is_category_enabled(DebugCategory::EXEC_HEXDUMP)) {
            std::ostringstream dump;
            dump << "Fetch PC=0x" << std::hex << std::setw(4) << std::setfill('0') << pc
                 << ": opcode=0x" << std::hex << std::setw(2) << std::setfill('0')
                 << static_cast<int>(opcode);

            // Show next few bytes (operands)
            dump << " [";
            for (size_t i = 0; i < 8 && pc + i < program.size(); i++) {
                dump << std::hex << std::setw(2) << std::setfill('0')
                     << static_cast<int>(program[pc + i]) << " ";
            }
            dump << "]";
            DEBUG_TRACE(DebugCategory::EXEC_HEXDUMP, "{}", dump.str());
        }

        // ... dispatch opcode ...
    }
}
```

**CLI Flag**: `--hexdump-exec`

### 5. Register State Hexdumps

**Category**: `CPU_REGISTERS` or `REG_HEXDUMP` (0x002 or new)

**Use Cases**:

- Full register state snapshots
- Before/after comparisons
- Multi-register operation verification

**Suggested Implementation**:

```cpp
// In cpu.cpp
void CPU::hexdump_registers() {
    if (!DebugHandler::instance().is_category_enabled(DebugCategory::REG_HEXDUMP)) {
        return;
    }

    std::ostringstream dump;
    dump << "Register Hexdump:\n";

    // General purpose registers (show in groups of 4)
    for (size_t i = 0; i < 16; i += 4) {
        dump << "  ";
        for (size_t j = 0; j < 4 && i + j < 16; j++) {
            dump << RegisterNames[i + j] << "=0x" << std::hex << std::setw(16)
                 << std::setfill('0') << registers[i + j] << " ";
        }
        dump << "\n";
    }

    DEBUG_DETAIL(DebugCategory::REG_HEXDUMP, "{}", dump.str());
}
```

**CLI Flag**: `--hexdump-registers`

### 6. Interrupt Handler Hexdumps

**Category**: `IO_INTERRUPT` or `INT_HEXDUMP` (0x305 or new)

**Use Cases**:

- Interrupt vector table dumps
- Interrupt context switching
- ISR entry/exit verification

**Suggested Implementation**:

```cpp
// In interrupt_controller.cpp
void InterruptController::trigger_interrupt(uint8_t vector) {
    // Hexdump IVT entry
    if (DebugHandler::instance().is_category_enabled(DebugCategory::INT_HEXDUMP)) {
        uint32_t ivt_address = ivt_base_ + (vector * 4);
        std::ostringstream dump;
        dump << "Interrupt 0x" << std::hex << std::setw(2) << std::setfill('0')
             << static_cast<int>(vector)
             << " -> IVT[0x" << std::hex << ivt_address << "]";
        DEBUG_IMPORTANT(DebugCategory::INT_HEXDUMP, "{}", dump.str());
    }

    // ... trigger interrupt ...
}
```

**CLI Flag**: `--hexdump-interrupts`

### 7. File/Block Device Hexdumps

**Category**: `IO_FILE` or `FILE_HEXDUMP` (0x301 or new)

**Use Cases**:

- File read/write operations
- Block device transfers
- RAMDisk operations
- Virtual storage debugging

**Suggested Implementation**:

```cpp
// In file_device.cpp
void FileDevice::write(uint32_t value) {
    // ... write logic ...

    // Hexdump file operation
    if (DebugHandler::instance().is_category_enabled(DebugCategory::FILE_HEXDUMP)) {
        std::ostringstream dump;
        dump << "FileDevice Write: [" << std::hex;
        for (size_t i = 0; i < buffer.size() && i < 32; i++) {
            dump << std::setw(2) << std::setfill('0')
                 << static_cast<int>(buffer[i]) << " ";
        }
        if (buffer.size() > 32) dump << "... (" << std::dec << buffer.size() << " bytes)";
        dump << "]";
        DEBUG_DETAIL(DebugCategory::FILE_HEXDUMP, "{}", dump.str());
    }
}
```

**CLI Flag**: `--hexdump-files`

## Unified Hexdump Command

### Proposed: `--hexdump=<categories>`

**Purpose**: Enable multiple hexdump categories with a single flag

**Syntax**:

```bash
# All hexdumps
--hexdump=all

# Specific categories
--hexdump=asm,memory,io

# Assembly only (current behavior)
--hexdump=asm
--hexdump  # defaults to asm
```

**Implementation**:

```cpp
parser.add_value_arg("hexdump", "--hexdump", "",
    "Enable hexdump output (categories: all, asm, memory, io, stack, exec, registers, interrupts, files)",
    [this](const std::string& value) {
        Config::debug = true;

        std::string categories = value.empty() ? "asm" : value;

        if (categories == "all") {
            // Enable all hexdump categories
            Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::ASM_HEXDUMP);
            Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::MEM_HEXDUMP);
            Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::IO_HEXDUMP);
            Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::STACK_HEXDUMP);
            Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::EXEC_HEXDUMP);
            Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::REG_HEXDUMP);
            Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::INT_HEXDUMP);
            Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::FILE_HEXDUMP);
        } else {
            // Parse comma-separated categories
            std::istringstream ss(categories);
            std::string cat;
            while (std::getline(ss, cat, ',')) {
                if (cat == "asm") {
                    Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::ASM_HEXDUMP);
                } else if (cat == "memory" || cat == "mem") {
                    Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::MEM_HEXDUMP);
                } else if (cat == "io") {
                    Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::IO_HEXDUMP);
                } else if (cat == "stack") {
                    Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::STACK_HEXDUMP);
                } else if (cat == "exec") {
                    Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::EXEC_HEXDUMP);
                } else if (cat == "registers" || cat == "reg") {
                    Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::REG_HEXDUMP);
                } else if (cat == "interrupts" || cat == "int") {
                    Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::INT_HEXDUMP);
                } else if (cat == "files" || cat == "file") {
                    Logging::DebugHandler::instance().enable_category(Logging::DebugCategory::FILE_HEXDUMP);
                } else {
                    std::cerr << "Unknown hexdump category: " << cat << std::endl;
                }
            }
        }
    });
```

## Debug Category Extensions Needed

To implement all recommended hexdump points, add these categories to `debug_handler.hpp`:

```cpp
enum class DebugCategory {
    // ... existing categories ...

    // Memory/Storage (0x200-0x299)
    MEM_ACCESS = 0x200,
    MEM_ALLOCATION = 0x201,
    MEM_BOUNDS = 0x202,
    MEM_VIRTUAL = 0x203,
    MEM_CACHE = 0x204,
    MEM_PROTECTION = 0x205,
    MEM_HEXDUMP = 0x206,        // NEW: Memory operation hexdumps

    // I/O/Device (0x300-0x399)
    IO_DEVICE = 0x300,
    IO_FILE = 0x301,
    IO_CONSOLE = 0x302,
    IO_RAMDISK = 0x303,
    IO_NETWORK = 0x304,
    IO_INTERRUPT = 0x305,
    IO_HEXDUMP = 0x306,         // NEW: I/O operation hexdumps
    FILE_HEXDUMP = 0x307,       // NEW: File/block device hexdumps

    // CPU/Engine (0x001-0x099)
    CPU_EXECUTION = 0x001,
    CPU_REGISTERS = 0x002,
    CPU_FLAGS = 0x003,
    CPU_STACK = 0x004,
    CPU_JUMP = 0x005,
    CPU_DISPATCHER = 0x006,
    CPU_PREDICTION = 0x007,
    CPU_PIPELINE = 0x008,
    STACK_HEXDUMP = 0x009,      // NEW: Stack operation hexdumps
    EXEC_HEXDUMP = 0x00A,       // NEW: Instruction fetch hexdumps
    REG_HEXDUMP = 0x00B,        // NEW: Register state hexdumps
    INT_HEXDUMP = 0x00C,        // NEW: Interrupt handler hexdumps
};
```

## Performance Considerations

### Hexdump Overhead

Hexdumps can be **expensive** when enabled:

- String formatting operations
- I/O operations (output to console/file)
- Potential lock contention in multi-threaded scenarios

### Mitigation Strategies

1. **Conditional Compilation**:

```cpp
#ifdef DEMI_ENABLE_HEXDUMP
    if (DebugHandler::instance().is_category_enabled(DebugCategory::MEM_HEXDUMP)) {
        // ... hexdump code ...
    }
#endif
```

2. **Lazy Evaluation**:

```cpp
// Only format string if category is enabled
DEBUG_DETAIL_IF(DebugCategory::MEM_HEXDUMP, [&]() {
    std::ostringstream dump;
    // ... expensive formatting ...
    return dump.str();
});
```

3. **Sampling**:

```cpp
// Only hexdump every Nth operation
static uint64_t counter = 0;
if ((++counter % 100) == 0) {
    // ... hexdump ...
}
```

4. **Size Limits**:

```cpp
// Limit hexdump size
const size_t MAX_HEXDUMP_SIZE = 256;
size_t dump_size = std::min(bytecode.size(), MAX_HEXDUMP_SIZE);
```

## Use Cases

### Debugging Assembly Issues

```bash
# See what bytecode was generated
./bin/demi-engine-debug --hexdump -A buggy_program.asm
```

### Memory Corruption Investigation

```bash
# Watch all memory operations
./bin/demi-engine-debug --hexdump=memory,stack -A program.asm
```

### I/O Protocol Debugging

```bash
# Monitor device communications
./bin/demi-engine-debug --hexdump=io,files -A device_test.asm
```

### Comprehensive Debugging

```bash
# Enable everything (very verbose!)
./bin/demi-engine-debug --hexdump=all --debug-verbose -A program.asm
```

### Selective Debugging

```bash
# Only specific subsystems
./bin/demi-engine-debug --hexdump=asm,exec --debug-level detail -A program.asm
```

## Best Practices

### When to Use Hexdumps

✅ **Good Use Cases**:

- Assembly output verification
- Binary protocol debugging
- Memory corruption investigation
- Device I/O troubleshooting
- Instruction stream analysis
- Security vulnerability research

❌ **Avoid For**:

- Production execution (too slow)
- Long-running programs (huge output)
- High-frequency operations (overwhelming)
- Performance testing (skews results)

### Output Management

1. **Redirect to File**:

```bash
./bin/demi-engine-debug --hexdump=all -A program.asm 2>hexdump.log
```

2. **Filter with grep**:

```bash
./bin/demi-engine-debug --hexdump=all -A program.asm 2>&1 | grep "HEXDUMP"
```

3. **Paginate Output**:

```bash
./bin/demi-engine-debug --hexdump=all -A program.asm 2>&1 | less
```

## Implementation Priority

Recommended implementation order:

1. ✅ **Assembly Hexdump** - DONE (v1.0)
2. 🔴 **Memory Operation Hexdump** - HIGH priority (debugging aid)
3. 🟡 **Stack Frame Hexdump** - MEDIUM priority (call stack debugging)
4. 🟡 **I/O Operation Hexdump** - MEDIUM priority (device debugging)
5. 🟢 **Instruction Fetch Hexdump** - LOW priority (very verbose)
6. 🟢 **Register State Hexdump** - LOW priority (covered by .dumpreg)
7. 🟢 **Interrupt Hexdump** - LOW priority (covered by debug output)
8. 🟢 **File/Block Hexdump** - LOW priority (specialized use case)

## Future Enhancements

### Interactive Hexdump Viewer

Integrate hexdumps into GUI or create interactive viewer:

- Click on address to see hexdump
- Compare before/after states
- Search/filter hexdump output
- Save hexdump snapshots

### Hexdump Diff Mode

Show only changed bytes:

```
Memory write diff at 0x1000:
  - [00 00 00 00]
  + [42 00 00 00]
```

### Hexdump Replay

Record hexdumps and replay program state:

- Capture all hexdumps during execution
- Save to file for later analysis
- Replay execution with same state

## Conclusion

Hexdump functionality is a **powerful debugging tool** when used appropriately. The current implementation provides a solid foundation with assembly hexdumps. Adding strategic hexdump points throughout the codebase will significantly enhance debugging capabilities, especially for:

- Binary protocol development
- Memory debugging
- Device driver implementation
- Security analysis
- Performance profiling

**Key Takeaway**: Implement hexdumps where **binary data inspection** provides value that textual output cannot.

---

**Document Version**: 1.0  
**Status**: Recommendations Pending Implementation  
**Next Review**: After implementation of priority items  
**Maintained By**: Development Team
