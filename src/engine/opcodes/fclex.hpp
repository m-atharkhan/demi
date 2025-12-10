#pragma once
#include "../cpu.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/format.h>

// FCLEX: Clear FPU exceptions
// Clears all exception flags in the FPU status word
// No operands needed
inline void handle_FCLEX(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    // Get current status word
    uint16_t status = cpu.get_fpu_status_word();
    
    // Clear exception flags (bits 0-5)
    // Bit 0: Invalid Operation
    // Bit 1: Denormalized Operand
    // Bit 2: Zero Divide
    // Bit 3: Overflow
    // Bit 4: Underflow
    // Bit 5: Precision
    status &= ~0x003F; // Clear bits 0-5
    
    // Update status word
    cpu.set_fpu_status_word(status);
    
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC={:#06x}] [FCLEX] cleared FPU exceptions, status={:#06x}", 
        cpu.get_pc(), status), Logging::DebugLevel::DETAIL);
    
    // Increment program counter
    cpu.set_pc(cpu.get_pc() + 1);
}
