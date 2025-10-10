#pragma once
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <cmath>

// FSQRT - Floating Point Square Root
// Replaces ST(0) with its square root
// Format: FSQRT (no operands)
inline void handle_FSQRT(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, bool& running) {
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [FSQRT] Executing floating point square root", cpu.get_pc()) << std::endl;
    
    // Get current value from ST(0) without popping
    double value = cpu.fpu_peek(0);
    
    // Check for negative value (domain error)
    if (value < 0.0) {
        Logger::instance().error() << fmt::format("[PC=0x{:04X}] [FSQRT] Domain error: cannot compute square root of negative number ({})", cpu.get_pc(), value) << std::endl;
        running = false;
        return;
    }
    
    // Calculate square root
    double result = std::sqrt(value);
    
    // Replace ST(0) with the result
    cpu.fpu_pop(); // Remove old value
    cpu.fpu_push(result); // Push result
    
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [FSQRT] ST(0): {} -> {}", cpu.get_pc(), value, result) << std::endl;
    
    // Move to next instruction
    cpu.set_pc(cpu.get_pc() + 1);
}
