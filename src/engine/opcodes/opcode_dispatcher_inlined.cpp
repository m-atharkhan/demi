#include "opcode_dispatcher_inlined.hpp"
#include "opcode_dispatcher.hpp"
#include "../cpu.hpp"
#include "../cpu_flags.hpp"
#include "../../config.hpp"
#include "../../debug/logger.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/format.h>
#include <iomanip>

// Note: Opcode enum is already available from cpu.hpp

// Suppress pedantic warnings about computed gotos - they are intentional for performance
// Suppress pedantic warnings for performance-critical computed gotos
// This is a widely-used GCC extension for threaded code interpretation
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

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
extern void handle_mov64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
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
    DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "[DISPATCH_INLINED] ENTRY: PC=0x{:04X}", cpu.get_pc());
    
    // Declare dispatch table at function scope
    static void* dispatch_table[256] = {nullptr};
    
    DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "[DISPATCH_INLINED] About to goto init_dispatch_table");
    // Entry point - jump to dispatch table initialization
    goto init_dispatch_table;

    // Inlined NOP operation - most common, simplest operation
    op_nop: {
        uint32_t pc = cpu.get_pc();
        
        #ifndef NDEBUG
        if (Config::trace) {
            DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [NOP] PC={}", pc, pc);
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

    // Inlined LOAD_IMM operation - MODE-AWARE (6-byte format: opcode + reg + 4-byte immediate)
    op_load_imm: {
        uint32_t pc = cpu.get_pc();
        
        #ifndef NDEBUG
        if (__builtin_expect(pc + 5 >= program.size(), 0)) {
            #ifdef VM_DEBUG_BOUNDS
            Logger::instance().error() << fmt::format("[LOAD_IMM] Out of bounds at PC={}", pc) << std::endl;
            #endif
            running = false;
            return;
        }
        #endif
        
        uint8_t reg = program[pc + 1];
        // Read 32-bit value (4 bytes) in little-endian format
        uint32_t value = 0;
        value |= static_cast<uint32_t>(program[pc + 2]);
        value |= static_cast<uint32_t>(program[pc + 3]) << 8;
        value |= static_cast<uint32_t>(program[pc + 4]) << 16;
        value |= static_cast<uint32_t>(program[pc + 5]) << 24;
        
        #ifndef NDEBUG
        if (__builtin_expect(reg >= cpu.get_registers_64().size(), 0)) {
            #ifdef VM_DEBUG_BOUNDS
            DEBUG_CRITICAL(Logging::DebugCategory::MEM_BOUNDS, "[LOAD_IMM] Invalid register R{}", reg);
            #endif
            running = false;
            return;
        }
        
        if (Config::trace) {
            DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [LOAD_IMM] R{} = 0x{:08X}", pc, reg, value);
        }
        #endif
        
        // Mode-aware register write
        cpu.write_register_mode_aware(reg, static_cast<uint64_t>(value));
        cpu.set_pc(pc + 6);
        
        #ifndef NDEBUG
        if (Config::trace) {
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

    // Inlined ADD operation - very common arithmetic operation - MODE-AWARE
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
        if (__builtin_expect(reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size(), 0)) {
            #ifdef VM_DEBUG_BOUNDS
            DEBUG_CRITICAL(Logging::DebugCategory::MEM_BOUNDS, "[ADD] Invalid register R{} or R{}", reg1, reg2);
            #endif
            running = false;
            return;
        }
        
        if (Config::trace) {
            DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [ADD] R{} += R{}", pc, reg1, reg2);
        }
        #endif
        
        // Mode-aware addition
        uint64_t old_val = cpu.read_register_mode_aware(reg1);
        uint64_t val2 = cpu.read_register_mode_aware(reg2);
        uint64_t mask = cpu.get_operand_mask();
        uint64_t new_val = (old_val + val2) & mask;
        cpu.write_register_mode_aware(reg1, new_val);
        
        // Update flags - inlined for performance
        uint32_t flags = cpu.get_flags();
        flags = (new_val == 0) ? (flags | FLAG_ZERO) : (flags & ~FLAG_ZERO);
        flags = (new_val < old_val) ? (flags | FLAG_CARRY) : (flags & ~FLAG_CARRY);
        flags = (((old_val ^ new_val) & (val2 ^ new_val) & (cpu.is_64bit_mode() ? 0x8000000000000000ULL : 0x80000000ULL)) != 0) ? (flags | FLAG_OVERFLOW) : (flags & ~FLAG_OVERFLOW);
        cpu.set_flags(flags);
        
        cpu.set_pc(pc + 3);
        
        #ifndef NDEBUG
        if (Config::trace) {
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
        if (__builtin_expect(reg_dst >= cpu.get_registers_64().size() || reg_src >= cpu.get_registers_64().size(), 0)) {
            #ifdef VM_DEBUG_BOUNDS
            DEBUG_CRITICAL(Logging::DebugCategory::MEM_BOUNDS, "[MOV] Invalid register R{} or R{}", reg_dst, reg_src);
            #endif
            running = false;
            return;
        }
        
        if (Config::trace) {
            DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [MOV] R{} = R{}", pc, reg_dst, reg_src);
        }
        #endif
        
        // Mode-aware register move
        uint64_t value = cpu.read_register_mode_aware(reg_src);
        cpu.write_register_mode_aware(reg_dst, value);
        cpu.set_pc(pc + 3);
        
        #ifndef NDEBUG
        if (Config::trace) {
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
        if (Config::trace) {
            DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [HALT]", cpu.get_pc());
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
            DEBUG_CRITICAL(Logging::DebugCategory::MEM_BOUNDS, "[LOAD_IMM64] Invalid register R{}", reg);
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
        if (Config::trace) {
            DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [LOAD_IMM64] R{} = {}", pc, reg, static_cast<int64_t>(value));
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

    // Inlined DEBUG operation
    op_debug: {
        uint32_t pc = cpu.get_pc();
        
        #ifndef NDEBUG
        if (__builtin_expect(pc + 2 >= program.size(), 0)) {
            running = false;
            return;
        }
        #endif
        
        uint8_t sub_opcode = program[pc + 1];
        uint32_t next_pc = pc + 2;
        
        if (sub_opcode == 0) { // PRINT
            if (Config::debug) {
                uint8_t type = program[pc + 2];
                if (type == 1) { // String
                    uint8_t len = program[pc + 3];
                    std::string s;
                    for (int i = 0; i < len; ++i) s += (char)program[pc + 4 + i];
                    std::cout << s << std::endl;
                    next_pc = pc + 4 + len;
                } else if (type == 0) { // Register
                    uint8_t reg = program[pc + 3];
                    std::cout << cpu.get_registers()[reg] << std::endl;
                    next_pc = pc + 4;
                }
            } else {
                uint8_t type = program[pc + 2];
                if (type == 1) { // String
                    uint8_t len = program[pc + 3];
                    next_pc = pc + 4 + len;
                } else {
                    next_pc = pc + 4;
                }
            }
        } else if (sub_opcode == 1) { // BREAK
            if (Config::debug) {
                std::cout << "BREAKPOINT at 0x" << std::hex << pc << std::dec << std::endl;
            }
        } else if (sub_opcode == 2) { // DUMP
            if (Config::debug) {
                cpu.print_state("DUMP");
            }
        } else if (sub_opcode == 3) { // MEMDUMP
            // Start (4 bytes), Length (4 bytes)
            uint32_t start = 0;
            start |= (uint32_t)program[pc + 2];
            start |= (uint32_t)program[pc + 3] << 8;
            start |= (uint32_t)program[pc + 4] << 16;
            start |= (uint32_t)program[pc + 5] << 24;
            
            uint32_t len = 0;
            len |= (uint32_t)program[pc + 6];
            len |= (uint32_t)program[pc + 7] << 8;
            len |= (uint32_t)program[pc + 8] << 16;
            len |= (uint32_t)program[pc + 9] << 24;
            
            if (Config::debug) {
                const auto& mem = cpu.get_memory();
                std::cout << "Memory dump [0x" << std::hex << std::uppercase << start << " - 0x" << (start + len - 1) << "]:" << std::dec << std::endl;
                
                // 16-byte wide hex dump with ASCII
                for (uint32_t i = 0; i < len; i += 16) {
                    // Address
                    std::cout << "  0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << (start + i) << ": ";
                    
                    // Hex bytes
                    for (uint32_t j = 0; j < 16; ++j) {
                        uint32_t addr = start + i + j;
                        if (i + j < len && addr < mem.size()) {
                            std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)mem[addr];
                        } else {
                            std::cout << "  ";
                        }
                        std::cout << " ";
                        if (j == 7) std::cout << " "; // Extra space in middle
                    }
                    
                    // ASCII representation
                    std::cout << " |";
                    for (uint32_t j = 0; j < 16; ++j) {
                        uint32_t addr = start + i + j;
                        if (i + j < len && addr < mem.size()) {
                            unsigned char c = mem[addr];
                            std::cout << (char)((c >= 32 && c <= 126) ? c : '.');
                        } else {
                            std::cout << " ";
                        }
                    }
                    std::cout << "|" << std::dec << std::endl;
                }
            }
            
            next_pc = pc + 10;
        } else if (sub_opcode == 4) { // TRACE
            uint8_t val = program[pc + 2];
            if (val == 2) {
                Config::trace = !Config::trace;
            } else {
                Config::trace = (val != 0);
            }
            next_pc = pc + 3;
        } else if (sub_opcode == 5) { // ASSERT
            uint8_t type = program[pc + 2];
            uint64_t actual = 0;
            if (type == 0) { // Register
                uint8_t reg = program[pc + 3];
                actual = cpu.get_registers()[reg];
                next_pc = pc + 4;
            } else { // Memory
                uint32_t addr = 0;
                addr |= (uint32_t)program[pc + 3];
                addr |= (uint32_t)program[pc + 4] << 8;
                addr |= (uint32_t)program[pc + 5] << 16;
                addr |= (uint32_t)program[pc + 6] << 24;
                const auto& mem = cpu.get_memory();
                if (addr + 7 < mem.size()) {
                    actual = *(uint64_t*)(&mem[addr]);
                }
                next_pc = pc + 7;
            }
            uint64_t expected = 0;
            for (int i = 0; i < 8; ++i) {
                expected |= ((uint64_t)program[next_pc + i]) << (i * 8);
            }
            next_pc += 8;
            
            if (Config::debug) {
                if (actual != expected) {
                    std::cout << "ASSERTION FAILED at 0x" << std::hex << pc << ": expected 0x" << expected 
                              << ", got 0x" << actual << std::dec << std::endl;
                } else {
                    std::cout << "ASSERTION PASSED at 0x" << std::hex << pc << std::dec << std::endl;
                }
            }
        } else if (sub_opcode == 6) { // DUMPSTACK
            uint32_t depth = 0;
            depth |= (uint32_t)program[pc + 2];
            depth |= (uint32_t)program[pc + 3] << 8;
            depth |= (uint32_t)program[pc + 4] << 16;
            depth |= (uint32_t)program[pc + 5] << 24;
            
            if (Config::debug) {
                const auto& regs = cpu.get_registers();
                uint32_t sp = regs[6]; // RSP
                const auto& mem = cpu.get_memory();
                
                std::cout << "Stack dump (depth=" << depth << ", SP=0x" << std::hex << sp << "):" << std::dec << std::endl;
                for (uint32_t i = 0; i < depth && (sp + i * 8) < mem.size(); ++i) {
                    uint64_t val = 0;
                    uint32_t addr = sp + i * 8;
                    if (addr + 7 < mem.size()) {
                        val = *(uint64_t*)(&mem[addr]);
                    }
                    std::cout << "  [SP+" << (i * 8) << "] 0x" << std::hex << addr << ": 0x" << val << std::dec << std::endl;
                }
            }
            next_pc = pc + 6;
        } else if (sub_opcode == 7) { // WATCH
            uint32_t addr = 0;
            addr |= (uint32_t)program[pc + 2];
            addr |= (uint32_t)program[pc + 3] << 8;
            addr |= (uint32_t)program[pc + 4] << 16;
            addr |= (uint32_t)program[pc + 5] << 24;
            
            uint32_t len = 0;
            len |= (uint32_t)program[pc + 6];
            len |= (uint32_t)program[pc + 7] << 8;
            len |= (uint32_t)program[pc + 8] << 16;
            len |= (uint32_t)program[pc + 9] << 24;
            
            if (Config::debug) {
                std::cout << "WATCHPOINT set at 0x" << std::hex << addr << " (length " << std::dec << len << ")" << std::endl;
                // Note: Full implementation would require tracking watchpoints
            }
            next_pc = pc + 10;
        } else if (sub_opcode == 8) { // UNWATCH
            uint32_t addr = 0;
            addr |= (uint32_t)program[pc + 2];
            addr |= (uint32_t)program[pc + 3] << 8;
            addr |= (uint32_t)program[pc + 4] << 16;
            addr |= (uint32_t)program[pc + 5] << 24;
            
            if (Config::debug) {
                std::cout << "WATCHPOINT removed at 0x" << std::hex << addr << std::dec << std::endl;
            }
            next_pc = pc + 6;
        } else if (sub_opcode == 9) { // CHECKPOINT
            uint8_t len = program[pc + 2];
            std::string label;
            for (int i = 0; i < len; ++i) label += (char)program[pc + 3 + i];
            
            if (Config::debug) {
                std::cout << "CHECKPOINT: " << label << " at 0x" << std::hex << pc << std::dec << std::endl;
            }
            next_pc = pc + 3 + len;
        } else if (sub_opcode == 10) { // LOG
            uint8_t level = program[pc + 2];
            uint8_t len = program[pc + 3];
            std::string message;
            for (int i = 0; i < len; ++i) message += (char)program[pc + 4 + i];
            
            if (Config::debug) {
                const char* level_names[] = {"DEBUG", "INFO", "WARN", "ERROR"};
                const char* level_name = (level < 4) ? level_names[level] : "UNKNOWN";
                std::cout << "[" << level_name << "] " << message << std::endl;
            }
            next_pc = pc + 4 + len;
        } else if (sub_opcode == 11) { // DUMPREG
            uint8_t reg = program[pc + 2];
            
            if (Config::debug) {
                const auto& regs = cpu.get_registers();
                const char* reg_names[] = {"RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RSP", "RBP", 
                                          "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};
                const char* name = (reg < 16) ? reg_names[reg] : "UNKNOWN";
                std::cout << name << " = 0x" << std::hex << regs[reg] << " (" << std::dec << regs[reg] << ")" << std::endl;
            }
            next_pc = pc + 3;
        } else if (sub_opcode == 12) { // MEMSET
            uint32_t addr = 0;
            addr |= (uint32_t)program[pc + 2];
            addr |= (uint32_t)program[pc + 3] << 8;
            addr |= (uint32_t)program[pc + 4] << 16;
            addr |= (uint32_t)program[pc + 5] << 24;
            
            uint32_t len = 0;
            len |= (uint32_t)program[pc + 6];
            len |= (uint32_t)program[pc + 7] << 8;
            len |= (uint32_t)program[pc + 8] << 16;
            len |= (uint32_t)program[pc + 9] << 24;
            
            uint8_t value = program[pc + 10];
            
            if (Config::debug) {
                auto& mem = cpu.get_memory();
                for (uint32_t i = 0; i < len && (addr + i) < mem.size(); ++i) {
                    mem[addr + i] = value;
                }
                std::cout << "MEMSET: filled " << len << " bytes at 0x" << std::hex << addr 
                          << " with 0x" << (int)value << std::dec << std::endl;
            }
            next_pc = pc + 11;
        } else if (sub_opcode == 13) { // STEP
            uint32_t count = 0;
            count |= (uint32_t)program[pc + 2];
            count |= (uint32_t)program[pc + 3] << 8;
            count |= (uint32_t)program[pc + 4] << 16;
            count |= (uint32_t)program[pc + 5] << 24;
            
            if (Config::debug) {
                std::cout << "STEP: executing " << count << " instruction(s) starting at 0x" 
                          << std::hex << pc << std::dec << std::endl;
                // Note: Full implementation would require step-mode control
            }
            next_pc = pc + 6;
        }
        
        cpu.set_pc(next_pc);
        
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    // Generic handler for non-inlined operations
    op_handler: {
        uint8_t opcode = program[cpu.get_pc()];
        
        // Switch to appropriate handler for non-inlined opcodes
        switch (opcode) {
            case static_cast<uint8_t>(Opcode::SUB): handle_sub(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::JMP): handle_jmp(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::LOAD): handle_load(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::STORE): handle_store(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::PUSH): handle_push(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::POP): handle_pop(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::CMP): handle_cmp(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::JZ): handle_jz(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::JNZ): handle_jnz(cpu, program, running); break;
            
            // 64-bit operations (0x50-0x5F range)
            case static_cast<uint8_t>(Opcode::MOV64): handle_mov64(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::MUL64): handle_mul64(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::DIV64): handle_div64(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::AND64): handle_and64(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::CMP64): handle_cmp64(cpu, program, running); break;
            
            // Extended operations (0x60-0x6F range)  
            case static_cast<uint8_t>(Opcode::LOADEX): handle_loadex(cpu, program, running); break;
            case static_cast<uint8_t>(Opcode::STOREX): handle_storex(cpu, program, running); break;
            
            default:
                DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "[OP_HANDLER] Falling back to consolidated dispatcher for opcode 0x{:02X}", opcode);
                // Fall back to consolidated dispatcher for unhandled opcodes
                dispatch_opcode(cpu, program, running);
                DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "[OP_HANDLER] Returned from consolidated dispatcher");
                return;
        }
        
        DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "[OP_HANDLER] Handler completed, continuing to next instruction");
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
        DEBUG_CRITICAL(Logging::DebugCategory::CPU_EXECUTION, "Invalid opcode: 0x{:02X} at PC={}", opcode, cpu.get_pc());
        running = false;
        
        // Throw exception to match consolidated dispatcher behavior and allow tests to catch it
        std::string opcode_hex = fmt::format("{:02X}", static_cast<int>(opcode));
        throw CPUException("Invalid opcode: 0x" + opcode_hex);
    }

    // Dispatch table initialization
init_dispatch_table:
    DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "[DISPATCH_INLINED] Reached init_dispatch_table");
    static bool initialized = false;
    if (!initialized) {
        DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "[DISPATCH_INLINED] Initializing dispatch table");
        // Initialize dispatch table with inlined operations first
        for (int i = 0; i < 256; i++) {
            dispatch_table[i] = &&op_invalid;
        }
        
        // Most common operations get inlined handlers
        dispatch_table[static_cast<uint8_t>(Opcode::NOP)] = &&op_nop;
        dispatch_table[static_cast<uint8_t>(Opcode::LOAD_IMM)] = &&op_load_imm;
        dispatch_table[static_cast<uint8_t>(Opcode::ADD)] = &&op_add;
        dispatch_table[static_cast<uint8_t>(Opcode::MOV)] = &&op_mov;
        dispatch_table[static_cast<uint8_t>(Opcode::LOAD_IMM64)] = &&op_load_imm64;
        dispatch_table[static_cast<uint8_t>(Opcode::INT)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::IRET)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::CLI)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::STI)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::HALT)] = &&op_halt;
        
        // Less common operations use generic handler
        dispatch_table[static_cast<uint8_t>(Opcode::SUB)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JMP)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::LOAD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::STORE)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PUSH)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::POP)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::CMP)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JZ)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JNZ)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JNS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JC)] = &&op_handler;
        
        // Basic arithmetic operations
        dispatch_table[static_cast<uint8_t>(Opcode::MUL)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::DIV)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::INC)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::DEC)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::AND)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::OR)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::XOR)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::NOT)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::SHL)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::SHR)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::CALL)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::RET)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PUSH_ARG)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::POP_ARG)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PUSH_FLAG)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::POP_FLAG)] = &&op_handler;
        
        // Address and jump operations
        dispatch_table[static_cast<uint8_t>(Opcode::LEA)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::SWAP)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JNC)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JO)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JNO)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JG)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JL)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JGE)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::JLE)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MOD)] = &&op_handler;
        
        // I/O operations
        dispatch_table[static_cast<uint8_t>(Opcode::IN)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::OUT)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::INB)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::OUTB)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::INW)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::OUTW)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::INL)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::OUTL)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::INSTR)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::OUTSTR)] = &&op_handler;
        
        // Data and memory operations
        dispatch_table[static_cast<uint8_t>(Opcode::DB)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::LOADR)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::DEBUG)] = &&op_debug;
        dispatch_table[static_cast<uint8_t>(Opcode::STORER)] = &&op_handler;
        
        // 64-bit operations (0x50-0x5F range)
        dispatch_table[static_cast<uint8_t>(Opcode::ADD64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::SUB64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MOV64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MUL64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::DIV64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::AND64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::OR64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::XOR64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::NOT64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::SHL64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::SHR64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::CMP64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::INC64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::DEC64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MOD64)] = &&op_handler;
        
        // Extended operations (0x60-0x6F range)
        dispatch_table[static_cast<uint8_t>(Opcode::MOVEX)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::ADDEX)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::SUBEX)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MULEX)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::DIVEX)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::CMPEX)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::LOADEX)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::STOREX)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PUSHEX)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::POPEX)] = &&op_handler;
        
        // CPU mode operations
        dispatch_table[static_cast<uint8_t>(Opcode::MODE32)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MODE64)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MODECMP)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MODEFLAG)] = &&op_handler;
        
        // SIMD Operations (0x80-0x9F range)
        dispatch_table[static_cast<uint8_t>(Opcode::MOVAPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MOVUPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::ADDPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::SUBPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MULPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::DIVPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::SQRTPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MAXPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MINPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::ANDPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::ORPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::XORPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::CMPPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MOVAPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MOVUPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::ADDPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::SUBPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MULPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::DIVPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::SQRTPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MAXPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::MINPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::ANDPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::ORPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::XORPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::CMPPD)] = &&op_handler;
        
        // FPU Operations (0xA0-0xBF range)
        dispatch_table[static_cast<uint8_t>(Opcode::FLD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FST)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FSTP)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FILD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FIST)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FISTP)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FADD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FSUB)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FMUL)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FDIV)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FSIN)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FCOS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FTAN)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FSQRT)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FABS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FCHS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FINIT)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FCLEX)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FSTCW)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FLDCW)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FSTSW)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FCOMPP)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::FUCOMPP)] = &&op_handler;
        
        // AVX Operations (0xC0-0xDF range)
        dispatch_table[static_cast<uint8_t>(Opcode::VADDPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VSUBPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VMULPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VDIVPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VSQRTPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VMAXPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VMINPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VANDPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VORPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VXORPS)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VADDPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VSUBPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VMULPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VDIVPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VSQRTPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VMAXPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VMINPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VANDPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VORPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VXORPD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VADD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VMUL)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VDOT)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VMAX)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VBROADCAST)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::VCMPGT)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PACKB)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::UNPACKB)] = &&op_handler;
        
        // MMX Operations (0xE0-0xEF range)
        dispatch_table[static_cast<uint8_t>(Opcode::MOVQ)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PADDB)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PADDW)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PADDD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PSUBB)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PSUBW)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PSUBD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PCMPEQB)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PCMPEQW)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::PCMPEQD)] = &&op_handler;
        dispatch_table[static_cast<uint8_t>(Opcode::EMMS)] = &&op_handler;
        
        initialized = true;
        DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "Inlined threaded dispatcher initialized");
    } else {
        DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "[DISPATCH_INLINED] Dispatch table already initialized");
    }

    DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "[DISPATCH_INLINED] About to goto dispatch_start");
    // Jump to main dispatch loop
    goto dispatch_start;

dispatch_start:
    DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "[DISPATCH_INLINED] Reached dispatch_start, PC=0x{:04X}", cpu.get_pc());
    // Main interpreter loop
    if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
        DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "[DISPATCH_INLINED] PC out of bounds, stopping");
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
                if (Config::trace) {
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [NOP] PC={}", pc, pc) << std::endl;
                    cpu.print_state("NOP");
                }
                #endif
                cpu.set_pc(pc + 1);
                break;
            }
            
            // Inlined LOAD_IMM - MODE-AWARE (6-byte format for 32-bit registers)
            case 0x01: {
                #ifndef NDEBUG
                if (__builtin_expect(pc + 5 >= program.size(), 0)) {
                    running = false;
                    break;
                }
                #endif
                
                uint8_t reg = program[pc + 1];
                // Read 32-bit value (4 bytes) in little-endian format
                uint32_t value = 0;
                value |= static_cast<uint32_t>(program[pc + 2]);
                value |= static_cast<uint32_t>(program[pc + 3]) << 8;
                value |= static_cast<uint32_t>(program[pc + 4]) << 16;
                value |= static_cast<uint32_t>(program[pc + 5]) << 24;
                
                #ifndef NDEBUG
                if (__builtin_expect(reg >= cpu.get_registers_64().size(), 0)) {
                    running = false;
                    break;
                }
                
                if (Config::trace) {
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [LOAD_IMM] R{} = {}", pc, reg, value) << std::endl;
                }
                #endif
                
                // Mode-aware register write
                cpu.write_register_mode_aware(reg, static_cast<uint64_t>(value));
                cpu.set_pc(pc + 6);
                
                #ifndef NDEBUG
                if (Config::trace) {
                    cpu.print_state("LOAD_IMM");
                }
                #endif
                break;
            }
            
            // Inlined ADD - MODE-AWARE
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
                if (__builtin_expect(reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size(), 0)) {
                    running = false;
                    break;
                }
                
                if (Config::trace) {
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [ADD] R{} += R{}", pc, reg1, reg2) << std::endl;
                }
                #endif
                
                // Mode-aware register access
                uint64_t old_val = cpu.read_register_mode_aware(reg1);
                uint64_t val2 = cpu.read_register_mode_aware(reg2);
                uint64_t mask = cpu.get_operand_mask();
                uint64_t new_val = (old_val + val2) & mask;
                cpu.write_register_mode_aware(reg1, new_val);
                
                // Update flags
                uint32_t flags = cpu.get_flags();
                flags = (new_val == 0) ? (flags | FLAG_ZERO) : (flags & ~FLAG_ZERO);
                flags = (new_val < old_val) ? (flags | FLAG_CARRY) : (flags & ~FLAG_CARRY);
                flags = (((old_val ^ new_val) & (val2 ^ new_val) & (cpu.is_64bit_mode() ? 0x8000000000000000ULL : 0x80000000ULL)) != 0) ? (flags | FLAG_OVERFLOW) : (flags & ~FLAG_OVERFLOW);
                cpu.set_flags(flags);
                
                cpu.set_pc(pc + 3);
                
                #ifndef NDEBUG
                if (Config::trace) {
                    cpu.print_state("ADD");
                }
                #endif
                break;
            }
            
            // Inlined MOV - MODE-AWARE
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
                if (__builtin_expect(reg_dst >= cpu.get_registers_64().size() || reg_src >= cpu.get_registers_64().size(), 0)) {
                    running = false;
                    break;
                }
                
                if (Config::trace) {
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [MOV] R{} = R{}", pc, reg_dst, reg_src) << std::endl;
                }
                #endif
                
                // Mode-aware register access
                uint64_t value = cpu.read_register_mode_aware(reg_src);
                cpu.write_register_mode_aware(reg_dst, value);
                cpu.set_pc(pc + 3);
                
                #ifndef NDEBUG
                if (Config::trace) {
                    cpu.print_state("MOV");
                }
                #endif
                break;
            }
            
            // Inlined HALT
            case 0xFF: {
                #ifndef NDEBUG
                if (Config::trace) {
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [HALT]", pc) << std::endl;
                    cpu.print_state("HALT");
                }
                #endif
                running = false;
                break;
            }
            
            // Inlined DEBUG
            case 0x42: {
                #ifndef NDEBUG
                if (__builtin_expect(pc + 2 >= program.size(), 0)) {
                    running = false;
                    break;
                }
                #endif
                
                uint8_t sub_opcode = program[pc + 1];
                uint32_t next_pc = pc + 2;
                
                if (sub_opcode == 0) { // PRINT
                    uint8_t type = program[pc + 2];
                    if (type == 1) { // String
                        uint8_t len = program[pc + 3];
                        std::string s;
                        for (int i = 0; i < len; ++i) s += (char)program[pc + 4 + i];
                        std::cout << s << std::endl;
                        next_pc = pc + 4 + len;
                    } else if (type == 0) { // Register
                        uint8_t reg = program[pc + 3];
                        std::cout << cpu.get_registers()[reg] << std::endl;
                        next_pc = pc + 4;
                    }
                } else if (sub_opcode == 1) { // BREAK
                    std::cout << "BREAKPOINT at 0x" << std::hex << pc << std::dec << std::endl;
                } else if (sub_opcode == 2) { // DUMP
                    cpu.print_state("DUMP");
                } else if (sub_opcode == 3) { // MEMDUMP
                    uint32_t start = 0;
                    start |= (uint32_t)program[pc + 2];
                    start |= (uint32_t)program[pc + 3] << 8;
                    start |= (uint32_t)program[pc + 4] << 16;
                    start |= (uint32_t)program[pc + 5] << 24;
                    
                    uint32_t len = 0;
                    len |= (uint32_t)program[pc + 6];
                    len |= (uint32_t)program[pc + 7] << 8;
                    len |= (uint32_t)program[pc + 8] << 16;
                    len |= (uint32_t)program[pc + 9] << 24;
                    
                    cpu.print_memory(start, start + len);
                    next_pc = pc + 10;
                } else if (sub_opcode == 4) { // TRACE
                    uint8_t val = program[pc + 2];
                    if (val == 2) {
                        Config::debug = !Config::debug;
                    } else {
                        Config::debug = (val != 0);
                    }
                    next_pc = pc + 3;
                }
                
                cpu.set_pc(next_pc);
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
                if (Config::trace) {
                    Logger::instance().debug() << fmt::format(
                        "[PC=0x{:04X}] [LOAD_IMM64] R{} = {}", pc, reg, value) << std::endl;
                }
                #endif
                
                // Store as int64_t (signed 64-bit)
                cpu.get_registers()[reg] = static_cast<int64_t>(value);
                cpu.set_pc(pc + 10); // opcode + register + 8 bytes
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
#pragma GCC diagnostic pop
