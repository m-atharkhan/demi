#include "opcode_dispatcher_unified.hpp"
#include "opcode_registry.hpp"
#include "../cpu.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/format.h>

// Suppress pedantic warnings about computed gotos - they are intentional for performance
// Suppress pedantic warnings for performance-critical computed gotos
// This is a widely-used GCC extension for threaded code interpretation
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

// Check for computed goto support at compile time
#if USE_THREADED_CODE

// Threaded code implementation using computed goto with unified registry
void dispatch_opcode_unified(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // Initialize the opcode registry
    OpcodeRegistry::instance().initialize_handlers();
    
    // Declare dispatch table at function scope
    static void* dispatch_table[256] = {nullptr};
    
    // Entry point - jump to dispatch table initialization
    goto init_dispatch_table;

    // Generic handler that calls the registered function
    op_handler: {
        uint8_t opcode = program[cpu.get_pc()];
        auto handler = OpcodeRegistry::instance().get_handler(opcode);
        
        if (handler) {
            handler(cpu, program, running);
        } else {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
                fmt::format("Invalid opcode: 0x{:02X} at PC={}", opcode, cpu.get_pc()), Logging::DebugLevel::CRITICAL);
            running = false;
            return;
        }
        
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
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
            fmt::format("Invalid opcode: 0x{:02X} at PC={}", opcode, cpu.get_pc()), Logging::DebugLevel::CRITICAL);
        running = false;
        return;
    }

    // Dispatch table initialization
init_dispatch_table:
    static bool initialized = false;
    if (!initialized) {
        // Get all handlers from registry
        const auto& handlers = OpcodeRegistry::instance().get_handlers();
        
        // Build dispatch table based on registry
        for (int i = 0; i < 256; i++) {
            if (handlers[i]) {
                dispatch_table[i] = &&op_handler;
            } else {
                dispatch_table[i] = &&op_invalid;
            }
        }
        
        initialized = true;
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
            "Unified threaded dispatcher initialized", Logging::DebugLevel::DETAIL);
    }

    // Jump to main dispatch loop
    goto dispatch_start;

dispatch_start:
    // Main interpreter loop
    if (__builtin_expect(cpu.get_pc() >= program.size(), 0)) {
        running = false;
        return;
    }

    // Get opcode and jump directly to handler
    uint8_t opcode = program[cpu.get_pc()];
    goto *dispatch_table[opcode];
}

#else
// Fallback for compilers without computed goto support
void dispatch_opcode_unified(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // Delegate to fallback implementation
    dispatch_opcode_unified_fallback(cpu, program, running);
}
#endif

// Unified fallback implementation using switch-case with registry
void dispatch_opcode_unified_fallback(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // Initialize the opcode registry
    OpcodeRegistry::instance().initialize_handlers();
    
    if (cpu.get_pc() >= program.size()) {
        running = false;
        return;
    }

    uint8_t opcode = program[cpu.get_pc()];
    auto handler = OpcodeRegistry::instance().get_handler(opcode);
    
    if (handler) {
        handler(cpu, program, running);
    } else {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
            fmt::format("Invalid opcode: 0x{:02X} at PC={}", opcode, cpu.get_pc()), Logging::DebugLevel::CRITICAL);
        running = false;
    }
}
#pragma GCC diagnostic pop
