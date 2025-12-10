#pragma once
#include "../cpu.hpp"
#include "../../debug/debug_handler.hpp"
#include <cmath>
#include <fmt/format.h>

// FABS - Floating Point Absolute Value
// Replaces ST(0) with its absolute value
// Format: FABS (no operands)
inline void handle_FABS(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC=0x{:04X}] [FABS] Executing floating point absolute value", cpu.get_pc()), Logging::DebugLevel::DETAIL);
    
    // Get current value from ST(0) without popping
    double value = cpu.fpu_peek(0);
    
    // Calculate absolute value
    double result = std::fabs(value);
    
    // Replace ST(0) with the result
    cpu.fpu_pop(); // Remove old value
    cpu.fpu_push(result); // Push result
    
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC=0x{:04X}] [FABS] ST(0): {} -> {}", cpu.get_pc(), value, result), Logging::DebugLevel::DETAIL);
    
    // Move to next instruction
    cpu.set_pc(cpu.get_pc() + 1);
}
