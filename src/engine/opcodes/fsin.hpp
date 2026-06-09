#pragma once
#include "../cpu.hpp"
#include "../../debug/debug_handler.hpp"
#include <cmath>
#include <fmt/format.h>

// FSIN: Floating Point Sine
// Computes sine of ST(0) and replaces it with the result
// ST(0) = sin(ST(0))
inline void handle_FSIN(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    // Get value from top of FPU stack
    double value = cpu.fpu_peek(0);
    
    // Compute sine
    double result = std::sin(value);
    
    // Replace ST(0) with result
    cpu.fpu_store(0, result);
    
    DEBUG_INFO(Logging::DebugCategory::CPU_EXECUTION, "[PC={:#06x}] [FSIN] sin({}) = {}", cpu.get_pc(), value, result);
    
    // Increment program counter
    cpu.set_pc(cpu.get_pc() + 1);
}
