#include "opcode_dispatcher_inlined.hpp"
#include "opcode_dispatcher.hpp"
#include "../cpu.hpp"
#include "../cpu_flags.hpp"
#include "../../config.hpp"
#include "../../debug/logger.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/format.h>

// External handler declarations
extern void handle_sub(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_load(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_store(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_push(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_pop(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_cmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jz(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jnz(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// 64-bit operation handlers
extern void handle_mul64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_div64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_and64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_cmp64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_loadex(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_storex(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

extern void dispatch_opcode(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// Check for computed goto support at compile time
#if USE_THREADED_CODE

// High-performance dispatcher with aggressive inlining
// This implements the most common operations directly in the dispatcher
// to eliminate function call overhead
void dispatch_opcode_inlined(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    Logger::instance().debug() << fmt::format("[DISPATCH_INLINED] ENTRY: PC=0x{:04X}", cpu.get_pc()) << std::endl;
    
    // Declare dispatch table at function scope
    static void* dispatch_table[256] = {nullptr};
    
    Logger::instance().debug() << fmt::format("[DISPATCH_INLINED] About to goto init_dispatch_table") << std::endl;
    // Entry point - jump to dispatch table initialization
    goto init_dispatch_table;

    // Inlined NOP operation - most common, simplest operation
    op_nop: {
        uint32_t pc = cpu.get_pc();
        
        #ifndef NDEBUG
        if (Config::debug) {
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [NOP] PC={}", pc, pc) << std::endl;
            cpu.print_state("NOP");
        }
        #endif
        
        cpu.set_pc(pc + 1);
        
        // Fast bounds check and next instruction dispatch
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    // Inlined LOAD_IMM operation - very common, simple operation
    op_load_imm: {
        uint32_t pc = cpu.get_pc();
        
        #ifndef NDEBUG
        if (__builtin_expect(pc + 2 >= program.size(), 0)) {
            #ifdef VM_DEBUG_BOUNDS
            Logger::instance().error() << fmt::format("[LOAD_IMM] Out of bounds at PC={}", pc) << std::endl;
            #endif
            running = false;
            return;
        }
        #endif
        
        uint8_t reg = program[pc + 1];
        uint8_t value = program[pc + 2];
        
        #ifndef NDEBUG
        if (__builtin_expect(reg >= cpu.get_registers().size(), 0)) {
            #ifdef VM_DEBUG_BOUNDS
            Logger::instance().error() << fmt::format("[LOAD_IMM] Invalid register R{}", reg) << std::endl;
            #endif
            running = false;
            return;
        }
        
        if (Config::debug) {
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [LOAD_IMM] R{} = {}", pc, reg, value) << std::endl;
        }
        #endif
        
        cpu.get_registers()[reg] = value;
        // Also update the 64-bit register array to maintain consistency
        if (reg < cpu.get_registers_64().size()) {
            cpu.get_registers_64()[reg] = static_cast<uint64_t>(value);
        }
        cpu.set_pc(pc + 3);
        
        #ifndef NDEBUG
        if (Config::debug) {
            cpu.print_state("LOAD_IMM");
        }
        #endif
        
        // Fast next instruction dispatch
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    // Inlined ADD operation - very common arithmetic operation
    op_add: {
        uint32_t pc = cpu.get_pc();
        
        #ifndef NDEBUG
        if (__builtin_expect(pc + 2 >= program.size(), 0)) {
            #ifdef VM_DEBUG_BOUNDS
            Logger::instance().error() << fmt::format("[ADD] Out of bounds at PC={}", pc) << std::endl;
            #endif
            running = false;
            return;
        }
        #endif
        
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];
        
        #ifndef NDEBUG
        if (__builtin_expect(reg1 >= cpu.get_registers().size() || reg2 >= cpu.get_registers().size(), 0)) {
            #ifdef VM_DEBUG_BOUNDS
            Logger::instance().error() << fmt::format("[ADD] Invalid register R{} or R{}", reg1, reg2) << std::endl;
            #endif
            running = false;
            return;
        }
        
        if (Config::debug) {
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [ADD] R{} += R{}", pc, reg1, reg2) << std::endl;
        }
        #endif
        
        // Perform the addition with overflow detection for flags
        uint32_t old_val = cpu.get_registers()[reg1];
        uint32_t new_val = old_val + cpu.get_registers()[reg2];
        cpu.get_registers()[reg1] = new_val;
        
        // Update flags - inlined for performance
        uint32_t flags = cpu.get_flags();
        flags = (new_val == 0) ? (flags | FLAG_ZERO) : (flags & ~FLAG_ZERO);
        flags = (new_val < old_val) ? (flags | FLAG_CARRY) : (flags & ~FLAG_CARRY);
        flags = (((old_val ^ new_val) & (cpu.get_registers()[reg2] ^ new_val) & 0x80000000) != 0) ? (flags | FLAG_OVERFLOW) : (flags & ~FLAG_OVERFLOW);
        cpu.set_flags(flags);
        
        cpu.set_pc(pc + 3);
        
        #ifndef NDEBUG
        if (Config::debug) {
            cpu.print_state("ADD");
        }
        #endif
        
        // Fast next instruction dispatch
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    // Inlined MOV operation - very common register move
    op_mov: {
        uint32_t pc = cpu.get_pc();
        
        #ifndef NDEBUG
        if (__builtin_expect(pc + 2 >= program.size(), 0)) {
            #ifdef VM_DEBUG_BOUNDS
            Logger::instance().error() << fmt::format("[MOV] Out of bounds at PC={}", pc) << std::endl;
            #endif
            running = false;
            return;
        }
        #endif
        
        uint8_t reg_dst = program[pc + 1];
        uint8_t reg_src = program[pc + 2];
        
        #ifndef NDEBUG
        if (__builtin_expect(reg_dst >= cpu.get_registers().size() || reg_src >= cpu.get_registers().size(), 0)) {
            #ifdef VM_DEBUG_BOUNDS
            Logger::instance().error() << fmt::format("[MOV] Invalid register R{} or R{}", reg_dst, reg_src) << std::endl;
            #endif
            running = false;
            return;
        }
        
        if (Config::debug) {
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [MOV] R{} = R{}", pc, reg_dst, reg_src) << std::endl;
        }
        #endif
        
        cpu.get_registers()[reg_dst] = cpu.get_registers()[reg_src];
        cpu.set_pc(pc + 3);
        
        #ifndef NDEBUG
        if (Config::debug) {
            cpu.print_state("MOV");
        }
        #endif
        
        // Fast next instruction dispatch
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    // Inlined HALT operation - must stop execution
    op_halt: {
        #ifndef NDEBUG
        if (Config::debug) {
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [HALT]", cpu.get_pc()) << std::endl;
            cpu.print_state("HALT");
        }
        #endif
        
        running = false;
        return;
    }

    // Inlined LOAD_IMM64 operation - common for 64-bit constants
    op_load_imm64: {
        uint32_t pc = cpu.get_pc();
        
        #ifndef NDEBUG
        if (__builtin_expect(pc + 9 >= program.size(), 0)) {
            #ifdef VM_DEBUG_BOUNDS
            Logger::instance().error() << fmt::format("[LOAD_IMM64] Out of bounds at PC={}", pc) << std::endl;
            #endif
            running = false;
            return;
        }
        #endif
        
        uint8_t reg = program[pc + 1];
        
        #ifndef NDEBUG
        if (__builtin_expect(reg >= TOTAL_REGISTERS, 0)) {
            #ifdef VM_DEBUG_BOUNDS
            Logger::instance().error() << fmt::format("[LOAD_IMM64] Invalid register R{}", reg) << std::endl;
            #endif
            running = false;
            return;
        }
        #endif
        
        // Read 64-bit value (8 bytes) in little-endian format
        uint64_t value = 0;
        value |= static_cast<uint64_t>(program[pc + 2]);
        value |= static_cast<uint64_t>(program[pc + 3]) << 8;
        value |= static_cast<uint64_t>(program[pc + 4]) << 16;
        value |= static_cast<uint64_t>(program[pc + 5]) << 24;
        value |= static_cast<uint64_t>(program[pc + 6]) << 32;
        value |= static_cast<uint64_t>(program[pc + 7]) << 40;
        value |= static_cast<uint64_t>(program[pc + 8]) << 48;
        value |= static_cast<uint64_t>(program[pc + 9]) << 56;
        
        #ifndef NDEBUG
        if (Config::debug) {
            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}] [LOAD_IMM64] R{} = {}", pc, reg, static_cast<int64_t>(value)) << std::endl;
        }
        #endif
        
        // Store as int64_t (signed 64-bit) to the 64-bit register array
        cpu.set_register_64(static_cast<Register>(reg), static_cast<int64_t>(value));
        cpu.set_pc(pc + 10); // opcode + register + 8 bytes
        
        // Fast bounds check and next instruction dispatch
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    // Generic handler for non-inlined operations - calls registered function
    op_handler: {
        uint8_t opcode = program[cpu.get_pc()];
        Logger::instance().debug() << fmt::format("[OP_HANDLER] Handling opcode 0x{:02X} at PC=0x{:04X}", opcode, cpu.get_pc()) << std::endl;
        
        // Switch to appropriate handler for non-inlined opcodes
        switch (opcode) {
            case 0x03: handle_sub(cpu, program, running); break;
            case 0x05: handle_jmp(cpu, program, running); break;
            case 0x06: handle_load(cpu, program, running); break;
            case 0x07: handle_store(cpu, program, running); break;
            case 0x08: handle_push(cpu, program, running); break;
            case 0x09: handle_pop(cpu, program, running); break;
            case 0x0A: handle_cmp(cpu, program, running); break;
            case 0x0B: handle_jz(cpu, program, running); break;
            case 0x0C: handle_jnz(cpu, program, running); break;
            
            // 64-bit operations (0x50-0x5F range)
            case 0x54: handle_mul64(cpu, program, running); break;  // MUL64
            case 0x55: handle_div64(cpu, program, running); break;  // DIV64
            case 0x56: handle_and64(cpu, program, running); break;  // AND64
            case 0x5B: handle_cmp64(cpu, program, running); break;  // CMP64
            
            // Extended operations (0x60-0x6F range)  
            case 0x66: handle_loadex(cpu, program, running); break; // LOADEX
            case 0x67: handle_storex(cpu, program, running); break; // STOREX
            
            default:
                Logger::instance().debug() << fmt::format("[OP_HANDLER] Falling back to consolidated dispatcher for opcode 0x{:02X}", opcode) << std::endl;
                // Fall back to consolidated dispatcher for unhandled opcodes
                dispatch_opcode(cpu, program, running);
                Logger::instance().debug() << fmt::format("[OP_HANDLER] Returned from consolidated dispatcher") << std::endl;
                return;
        }
        
        Logger::instance().debug() << fmt::format("[OP_HANDLER] Handler completed, continuing to next instruction") << std::endl;
        // Continue to next instruction
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    // Invalid opcode handler
    op_invalid: {
        uint8_t opcode = program[cpu.get_pc()];
        Logger::instance().error() << fmt::format("Invalid opcode: 0x{:02X} at PC={}", opcode, cpu.get_pc()) << std::endl;
        running = false;
        return;
    }

    // Dispatch table initialization
init_dispatch_table:
    Logger::instance().debug() << fmt::format("[DISPATCH_INLINED] Reached init_dispatch_table") << std::endl;
    static bool initialized = false;
    if (!initialized) {
        Logger::instance().debug() << fmt::format("[DISPATCH_INLINED] Initializing dispatch table") << std::endl;
        // Initialize dispatch table with inlined operations first
        for (int i = 0; i < 256; i++) {
            dispatch_table[i] = &&op_invalid;
        }
        
        // Most common operations get inlined handlers
        dispatch_table[0x00] = &&op_nop;        // NOP - most common in many programs
        dispatch_table[0x01] = &&op_load_imm;   // LOAD_IMM - very common for constants
        dispatch_table[0x02] = &&op_add;        // ADD - very common arithmetic
        dispatch_table[0x04] = &&op_mov;        // MOV - very common register moves
        dispatch_table[0x53] = &&op_load_imm64; // LOAD_IMM64 - common for 64-bit constants
        dispatch_table[0xFF] = &&op_halt;       // HALT - important for termination
        
        // Less common operations use generic handler
        dispatch_table[0x03] = &&op_handler;    // SUB
        dispatch_table[0x05] = &&op_handler;    // JMP  
        dispatch_table[0x06] = &&op_handler;    // LOAD
        dispatch_table[0x07] = &&op_handler;    // STORE
        dispatch_table[0x08] = &&op_handler;    // PUSH
        dispatch_table[0x09] = &&op_handler;    // POP
        dispatch_table[0x0A] = &&op_handler;    // CMP
        dispatch_table[0x0B] = &&op_handler;    // JZ
        dispatch_table[0x0C] = &&op_handler;    // JNZ
        dispatch_table[0x0D] = &&op_handler;    // JS
        dispatch_table[0x0E] = &&op_handler;    // JNS
        dispatch_table[0x0F] = &&op_handler;    // JC
        
        // Basic arithmetic operations
        dispatch_table[0x10] = &&op_handler;    // MUL
        dispatch_table[0x11] = &&op_handler;    // DIV
        dispatch_table[0x12] = &&op_handler;    // INC
        dispatch_table[0x13] = &&op_handler;    // DEC
        dispatch_table[0x14] = &&op_handler;    // AND
        dispatch_table[0x15] = &&op_handler;    // OR
        dispatch_table[0x16] = &&op_handler;    // XOR
        dispatch_table[0x17] = &&op_handler;    // NOT
        dispatch_table[0x18] = &&op_handler;    // SHL
        dispatch_table[0x19] = &&op_handler;    // SHR
        dispatch_table[0x1A] = &&op_handler;    // CALL
        dispatch_table[0x1B] = &&op_handler;    // RET
        dispatch_table[0x1C] = &&op_handler;    // PUSH_ARG
        dispatch_table[0x1D] = &&op_handler;    // POP_ARG
        dispatch_table[0x1E] = &&op_handler;    // PUSH_FLAG
        dispatch_table[0x1F] = &&op_handler;    // POP_FLAG
        
        // Address and jump operations
        dispatch_table[0x20] = &&op_handler;    // LEA
        dispatch_table[0x21] = &&op_handler;    // SWAP
        dispatch_table[0x22] = &&op_handler;    // JNC
        dispatch_table[0x23] = &&op_handler;    // JO
        dispatch_table[0x24] = &&op_handler;    // JNO
        dispatch_table[0x25] = &&op_handler;    // JG
        dispatch_table[0x26] = &&op_handler;    // JL
        dispatch_table[0x27] = &&op_handler;    // JGE
        dispatch_table[0x28] = &&op_handler;    // JLE
        dispatch_table[0x29] = &&op_handler;    // MOD
        
        // I/O operations
        dispatch_table[0x30] = &&op_handler;    // IN
        dispatch_table[0x31] = &&op_handler;    // OUT
        dispatch_table[0x32] = &&op_handler;    // INB
        dispatch_table[0x33] = &&op_handler;    // OUTB
        dispatch_table[0x34] = &&op_handler;    // INW
        dispatch_table[0x35] = &&op_handler;    // OUTW
        dispatch_table[0x36] = &&op_handler;    // INL
        dispatch_table[0x37] = &&op_handler;    // OUTL
        dispatch_table[0x38] = &&op_handler;    // INSTR
        dispatch_table[0x39] = &&op_handler;    // OUTSTR
        
        // Data and memory operations
        dispatch_table[0x40] = &&op_handler;    // DB
        dispatch_table[0x41] = &&op_handler;    // LOADR
        
        // 64-bit operations (0x50-0x5F range)
        dispatch_table[0x50] = &&op_handler;    // ADD64
        dispatch_table[0x51] = &&op_handler;    // SUB64
        dispatch_table[0x52] = &&op_handler;    // MOV64
        dispatch_table[0x54] = &&op_handler;    // MUL64
        dispatch_table[0x55] = &&op_handler;    // DIV64
        dispatch_table[0x56] = &&op_handler;    // AND64
        dispatch_table[0x57] = &&op_handler;    // OR64
        dispatch_table[0x58] = &&op_handler;    // XOR64
        dispatch_table[0x59] = &&op_handler;    // NOT64
        dispatch_table[0x5A] = &&op_handler;    // SHL64
        dispatch_table[0x5B] = &&op_handler;    // SHR64
        dispatch_table[0x5C] = &&op_handler;    // CMP64
        dispatch_table[0x5D] = &&op_handler;    // INC64
        dispatch_table[0x5E] = &&op_handler;    // DEC64
        dispatch_table[0x5F] = &&op_handler;    // MOD64
        
        // Extended operations (0x60-0x6F range)
        dispatch_table[0x60] = &&op_handler;    // MOVEX
        dispatch_table[0x61] = &&op_handler;    // ADDEX
        dispatch_table[0x62] = &&op_handler;    // SUBEX
        dispatch_table[0x63] = &&op_handler;    // MULEX
        dispatch_table[0x64] = &&op_handler;    // DIVEX
        dispatch_table[0x65] = &&op_handler;    // CMPEX
        dispatch_table[0x66] = &&op_handler;    // LOADEX
        dispatch_table[0x67] = &&op_handler;    // STOREX
        dispatch_table[0x68] = &&op_handler;    // PUSHEX
        dispatch_table[0x69] = &&op_handler;    // POPEX
        
        // CPU mode operations
        dispatch_table[0x70] = &&op_handler;    // MODE32
        dispatch_table[0x71] = &&op_handler;    // MODE64
        dispatch_table[0x72] = &&op_handler;    // MODECMP
        dispatch_table[0x73] = &&op_handler;    // MODEFLAG
        
        // SIMD Operations (0x80-0x9F range)
        for (int i = 0x80; i <= 0x99; i++) {
            dispatch_table[i] = &&op_handler;
        }
        
        // FPU Operations (0xA0-0xBF range)
        for (int i = 0xA0; i <= 0xB6; i++) {
            dispatch_table[i] = &&op_handler;
        }
        
        // AVX Operations (0xC0-0xDF range)
        for (int i = 0xC0; i <= 0xDF; i++) {
            dispatch_table[i] = &&op_handler;
        }
        
        // MMX Operations (0xE0-0xEF range)
        for (int i = 0xE0; i <= 0xEA; i++) {
            dispatch_table[i] = &&op_handler;
        }
        
        initialized = true;
        Logger::instance().debug() << "Inlined threaded dispatcher initialized" << std::endl;
    } else {
        Logger::instance().debug() << fmt::format("[DISPATCH_INLINED] Dispatch table already initialized") << std::endl;
    }

    Logger::instance().debug() << fmt::format("[DISPATCH_INLINED] About to goto dispatch_start") << std::endl;
    // Jump to main dispatch loop
    goto dispatch_start;

dispatch_start:
    Logger::instance().debug() << fmt::format("[DISPATCH_INLINED] Reached dispatch_start, PC=0x{:04X}", cpu.get_pc()) << std::endl;
    // Main interpreter loop
    if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
        Logger::instance().debug() << fmt::format("[DISPATCH_INLINED] PC out of bounds, stopping") << std::endl;
        running = false;
        return;
    }

    // Get opcode and jump directly to handler
    uint8_t opcode = program[cpu.get_pc()];
    Logger::instance().debug() << fmt::format("[DISPATCH_INLINED] About to dispatch opcode 0x{:02X} at PC=0x{:04X}", opcode, cpu.get_pc()) << std::endl;
    goto *dispatch_table[opcode];
}

#else
// Fallback for compilers without computed goto support
void dispatch_opcode_inlined(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // Use optimized switch-case fallback with inlined operations
    dispatch_opcode_inlined_fallback(cpu, program, running);
}
#endif

// Optimized fallback implementation with inlined operations
void dispatch_opcode_inlined_fallback(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    while (running && cpu.get_pc() < program.size()) {
        uint8_t opcode = program[cpu.get_pc()];
        uint32_t pc = cpu.get_pc();
        
        switch (opcode) {
            // Inlined NOP
            case 0x00: {
                #ifndef NDEBUG
                if (Config::debug) {
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [NOP] PC={}", pc, pc) << std::endl;
                    cpu.print_state("NOP");
                }
                #endif
                cpu.set_pc(pc + 1);
                break;
            }
            
            // Inlined LOAD_IMM  
            case 0x01: {
                #ifndef NDEBUG
                if (__builtin_expect(pc + 2 >= program.size(), 0)) {
                    running = false;
                    break;
                }
                #endif
                
                uint8_t reg = program[pc + 1];
                uint8_t value = program[pc + 2];
                
                #ifndef NDEBUG
                if (__builtin_expect(reg >= cpu.get_registers().size(), 0)) {
                    running = false;
                    break;
                }
                
                if (Config::debug) {
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [LOAD_IMM] R{} = {}", pc, reg, value) << std::endl;
                }
                #endif
                
                cpu.get_registers()[reg] = value;
                // Also update the 64-bit register array to maintain consistency
                if (reg < cpu.get_registers_64().size()) {
                    cpu.get_registers_64()[reg] = static_cast<uint64_t>(value);
                }
                cpu.set_pc(pc + 3);
                
                #ifndef NDEBUG
                if (Config::debug) {
                    cpu.print_state("LOAD_IMM");
                }
                #endif
                break;
            }
            
            // Inlined ADD
            case 0x02: {
                #ifndef NDEBUG
                if (__builtin_expect(pc + 2 >= program.size(), 0)) {
                    running = false;
                    break;
                }
                #endif
                
                uint8_t reg1 = program[pc + 1];
                uint8_t reg2 = program[pc + 2];
                
                #ifndef NDEBUG
                if (__builtin_expect(reg1 >= cpu.get_registers().size() || reg2 >= cpu.get_registers().size(), 0)) {
                    running = false;
                    break;
                }
                
                if (Config::debug) {
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [ADD] R{} += R{}", pc, reg1, reg2) << std::endl;
                }
                #endif
                
                uint32_t old_val = cpu.get_registers()[reg1];
                uint32_t new_val = old_val + cpu.get_registers()[reg2];
                cpu.get_registers()[reg1] = new_val;
                
                // Update flags
                uint32_t flags = cpu.get_flags();
                flags = (new_val == 0) ? (flags | FLAG_ZERO) : (flags & ~FLAG_ZERO);
                flags = (new_val < old_val) ? (flags | FLAG_CARRY) : (flags & ~FLAG_CARRY);
                flags = (((old_val ^ new_val) & (cpu.get_registers()[reg2] ^ new_val) & 0x80000000) != 0) ? (flags | FLAG_OVERFLOW) : (flags & ~FLAG_OVERFLOW);
                cpu.set_flags(flags);
                
                cpu.set_pc(pc + 3);
                
                #ifndef NDEBUG
                if (Config::debug) {
                    cpu.print_state("ADD");
                }
                #endif
                break;
            }
            
            // Inlined MOV
            case 0x04: {
                #ifndef NDEBUG
                if (__builtin_expect(pc + 2 >= program.size(), 0)) {
                    running = false;
                    break;
                }
                #endif
                
                uint8_t reg_dst = program[pc + 1];
                uint8_t reg_src = program[pc + 2];
                
                #ifndef NDEBUG
                if (__builtin_expect(reg_dst >= cpu.get_registers().size() || reg_src >= cpu.get_registers().size(), 0)) {
                    running = false;
                    break;
                }
                
                if (Config::debug) {
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [MOV] R{} = R{}", pc, reg_dst, reg_src) << std::endl;
                }
                #endif
                
                cpu.get_registers()[reg_dst] = cpu.get_registers()[reg_src];
                cpu.set_pc(pc + 3);
                
                #ifndef NDEBUG
                if (Config::debug) {
                    cpu.print_state("MOV");
                }
                #endif
                break;
            }
            
            // Inlined LOAD_IMM64
            case 0x53: {
                #ifndef NDEBUG
                if (__builtin_expect(pc + 9 >= program.size(), 0)) {
                    running = false;
                    break;
                }
                #endif
                
                uint8_t reg = program[pc + 1];
                
                #ifndef NDEBUG
                if (__builtin_expect(reg >= cpu.get_registers().size(), 0)) {
                    running = false;
                    break;
                }
                #endif
                
                // Read 64-bit value (8 bytes) in little-endian format
                uint64_t value = 0;
                value |= static_cast<uint64_t>(program[pc + 2]);
                value |= static_cast<uint64_t>(program[pc + 3]) << 8;
                value |= static_cast<uint64_t>(program[pc + 4]) << 16;
                value |= static_cast<uint64_t>(program[pc + 5]) << 24;
                value |= static_cast<uint64_t>(program[pc + 6]) << 32;
                value |= static_cast<uint64_t>(program[pc + 7]) << 40;
                value |= static_cast<uint64_t>(program[pc + 8]) << 48;
                value |= static_cast<uint64_t>(program[pc + 9]) << 56;
                
                #ifndef NDEBUG
                if (Config::debug) {
                    Logger::instance().debug() << fmt::format(
                        "[PC=0x{:04X}] [LOAD_IMM64] R{} = {}", pc, reg, value) << std::endl;
                }
                #endif
                
                // Store as int64_t (signed 64-bit)
                cpu.get_registers()[reg] = static_cast<int64_t>(value);
                cpu.set_pc(pc + 10); // opcode + register + 8 bytes
                break;
            }
            
            // Inlined HALT
            case 0xFF: {
                #ifndef NDEBUG
                if (Config::debug) {
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [HALT]", pc) << std::endl;
                    cpu.print_state("HALT");
                }
                #endif
                running = false;
                break;
            }
            
            // Non-inlined operations - delegate to original handlers
            default: {
                dispatch_opcode(cpu, program, running);
                break;
            }
        }
    }
}