#include "opcode_dispatcher_threaded.hpp"
#include "../cpu.hpp"
#include "../../debug/debug_handler.hpp"
#include "../opcodes/opcode_profiler.hpp"
#include <fmt/format.h>
#include <atomic>

// Disable all warnings for this file to avoid clutter during implementation
// Suppress pedantic warnings for performance-critical computed gotos
// This is a widely-used GCC extension for threaded code interpretation
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-label"
#pragma GCC diagnostic ignored "-Wpedantic"

// Check for computed goto support at compile time
#if USE_THREADED_CODE

// Threaded code implementation using computed goto
// This provides better performance than switch-case by eliminating branch prediction overhead
// Requires GCC 2.95+ or Clang with computed goto extension support
void dispatch_opcode_threaded(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // Declare dispatch table at function scope to avoid scope issues with VS Code IntelliSense
    // Initialize to null pointers first
    static void* dispatch_table[256] = {nullptr};
    
    // Entry point - jump to dispatch table initialization after all labels are defined
    goto init_dispatch_table;

    // All instruction label definitions come first
    op_nop: {
        // Inline NOP implementation for maximum speed
        DEBUG_INFO(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [NOP] PC={}", cpu.get_pc(), cpu.get_pc());
        cpu.set_pc(cpu.get_pc() + 1);
        cpu.print_state("NOP");
        
        // Jump to next instruction
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_load_imm: {
        extern void handle_load_imm(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_load_imm(cpu, program, running);
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_add: {
        extern void handle_add(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_add(cpu, program, running);
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_sub: {
        extern void handle_sub(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_sub(cpu, program, running);
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_mov: {
        extern void handle_mov(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_mov(cpu, program, running);
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_jmp: {
        extern void handle_jmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_jmp(cpu, program, running);
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_load: {
        extern void handle_load(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_load(cpu, program, running);
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_loadr: {
        extern void handle_loadr(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_loadr(cpu, program, running);
        if (!running) return;
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_store: {
        extern void handle_store(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_store(cpu, program, running);
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_push: {
        extern void handle_push(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_push(cpu, program, running);
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_pop: {
        extern void handle_pop(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_pop(cpu, program, running);
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_cmp: {
        extern void handle_cmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_cmp(cpu, program, running);
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    op_out: {
        extern void handle_out(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_out(cpu, program, running);
        if (__builtin_expect(!running, 0)) return;
        if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
            running = false;
            return;
        }
        goto *dispatch_table[program[cpu.get_pc()]];
    }

    // System operations
    op_halt: {
        extern void handle_halt(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
        handle_halt(cpu, program, running);
        return; // HALT stops execution
    }

    // Invalid opcode handler
    op_invalid: {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_DISPATCHER, fmt::format("Invalid opcode: 0x{:02X} at PC={}", program[cpu.get_pc()], cpu.get_pc()), Logging::DebugLevel::CRITICAL);
        running = false;
        return;
    }

    // Dispatch table initialization (thread-safe: atomic acquire/release)
    // Note: std::call_once with lambdas cannot capture label addresses (&&label);
    // atomic acquire/release is the correct pattern for computed-goto dispatch tables.
init_dispatch_table:
        // Initialize jump table for threaded code interpretation
        // Must be done after all labels are defined to get correct addresses
        static std::atomic<bool> initialized{false};
        if (!initialized.load(std::memory_order_acquire)) {
            void* table[256] = {
                &&op_nop,       // 0x00 - NOP
                &&op_load_imm,  // 0x01 - LOAD_IMM
                &&op_add,       // 0x02 - ADD
                &&op_sub,       // 0x03 - SUB
                &&op_mov,       // 0x04 - MOV
                &&op_jmp,       // 0x05 - JMP
                &&op_load,      // 0x06 - LOAD
                &&op_store,     // 0x07 - STORE
                &&op_push,      // 0x08 - PUSH
                &&op_pop,       // 0x09 - POP
                &&op_cmp,       // 0x0A - CMP
                &&op_invalid,   // 0x0B - JE (will add later)
                &&op_invalid,   // 0x0C - JNE (will add later)
                &&op_invalid,   // 0x0D - JL (will add later)
                &&op_invalid,   // 0x0E - JG (will add later)
                &&op_invalid,   // 0x0F - JLE (will add later)
                &&op_invalid,   // 0x10 - JGE (will add later)
                &&op_invalid,   // 0x11 - INC (will add later)
                &&op_invalid,   // 0x12 - DEC (will add later)
                &&op_invalid,   // 0x13 - MUL (will add later)
                &&op_invalid,   // 0x14 - DIV (will add later)
                &&op_invalid,   // 0x15 - MOD (will add later)
                &&op_invalid,   // 0x16 - AND (will add later)
                &&op_invalid,   // 0x17 - OR (will add later)
                &&op_invalid,   // 0x18 - XOR (will add later)
                &&op_invalid,   // 0x19 - NOT (will add later)
                &&op_invalid,   // 0x1A - SHL (will add later)
                &&op_invalid,   // 0x1B - CALL (will add later)
                &&op_invalid,   // 0x1C - RET (will add later)
                &&op_invalid,   // 0x1D - PUSH_ARG (will add later)
                &&op_invalid,   // 0x1E - POP_ARG (will add later)
                &&op_invalid,   // 0x1F - PUSH_FLAG (will add later)
                &&op_invalid,   // 0x20 - POP_FLAG (will add later)
                &&op_invalid,   // 0x21 - LEA (will add later)
                &&op_invalid,   // 0x22 - SWAP (will add later)
                // Fill 0x23-0x3F with invalid
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_out,       // 0x40 - OUT
                &&op_loadr,     // 0x41 - LOADR (Load Register indirect)
                // Fill 0x42-0xFE with invalid
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid, &&op_invalid,
                &&op_halt       // 0xFF - HALT
            };
            
            // Copy the initialized table to the function-scope array
            for (int i = 0; i < 256; i++) {
                dispatch_table[i] = table[i];
            }
            initialized.store(true, std::memory_order_release);
        } // end atomic init guard

        // Now jump to the main dispatch loop
        goto dispatch_start;

dispatch_start:
    // Main interpreter loop - jump directly to instruction handler
    if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
        running = false;
        return;
    }

    {
        // opcode is uint8_t so the value is always in [0, 255]; the dispatch
        // table has exactly 256 entries, so the index itself is always in-range.
        // However we still guard against a null entry: this can happen if the
        // static table was only partially written before another thread read it
        // (a TOCTOU race on the atomic flag) or if the table initialisation
        // path is ever changed.  The check is a single pointer comparison and
        // has no measurable impact in release builds.
        uint8_t opcode = program[cpu.get_pc()];
        // Profile opcode execution (TASK-005)
        OPCODE_PROFILE(opcode);
        void* target = dispatch_table[opcode];
        if (__builtin_expect(target == nullptr, 0)) {
            Logging::DebugHandler::instance().report(
                Logging::DebugCategory::CPU_DISPATCHER,
                fmt::format("Dispatch table null entry for opcode 0x{:02X} at PC={}",
                            opcode, cpu.get_pc()),
                Logging::DebugLevel::CRITICAL);
            running = false;
            return;
        }
        goto *target;
    }
}

#else
// Fallback for compilers without computed goto support
// This will automatically use the switch-case dispatcher
void dispatch_opcode_threaded(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // Delegate to fallback implementation
    dispatch_opcode_fallback(cpu, program, running);
}
#endif

// Fallback implementation for compilers without computed goto support
// This calls the original switch-case based dispatcher
void dispatch_opcode_fallback(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // Call the original switch-case dispatcher from opcodes_consolidated.cpp
    extern void dispatch_opcode(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
    dispatch_opcode(cpu, program, running);
}

#pragma GCC diagnostic pop