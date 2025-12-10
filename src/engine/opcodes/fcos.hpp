#pragma once
#include "../cpu.hpp"
#include "../../debug/debug_handler.hpp"
#include <cmath>
#include <fmt/format.h>

// FCOS: Floating Point Cosine
// Computes cosine of ST(0) and replaces it with the result
// ST(0) = cos(ST(0))
inline void handle_FCOS(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    // Get value from top of FPU stack
    double value = cpu.fpu_peek(0);
    
    // Compute cosine
    double result = std::cos(value);
    
    // Replace ST(0) with result
    cpu.fpu_pop();
    cpu.fpu_push(result);
    
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC={:#06x}] [FCOS] cos({}) = {}", 
        cpu.get_pc(), value, result), Logging::DebugLevel::DETAIL);
    
    // Increment program counter
    cpu.set_pc(cpu.get_pc() + 1);
}
