#pragma once
#include "../cpu.hpp"
#include "../../debug/debug_handler.hpp"
#include <cmath>
#include <fmt/format.h>

// FSQRT - Floating Point Square Root
// Replaces ST(0) with its square root
// Format: FSQRT (no operands)
inline void handle_FSQRT(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, bool& running) {
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[PC=0x{:04X}] [FSQRT] Executing floating point square root", cpu.get_pc()), Logging::DebugLevel::DETAIL);
    
    // Get current value from ST(0) without popping
    double value = cpu.fpu_peek(0);
    
    // Check for negative value (domain error)
    if (value < 0.0) {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[PC=0x{:04X}] [FSQRT] Domain error: cannot compute square root of negative number ({})", cpu.get_pc(), value), Logging::DebugLevel::CRITICAL);
        running = false;
        return;
    }
    
    // Calculate square root
    double result = std::sqrt(value);
    
    // Replace ST(0) with the result
    cpu.fpu_pop(); // Remove old value
    cpu.fpu_push(result); // Push result
    
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[PC=0x{:04X}] [FSQRT] ST(0): {} -> {}", cpu.get_pc(), value, result), Logging::DebugLevel::DETAIL);
    
    // Move to next instruction
    cpu.set_pc(cpu.get_pc() + 1);
}
