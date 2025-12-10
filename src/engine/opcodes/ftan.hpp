#pragma once
#include "../cpu.hpp"
#include "../../debug/debug_handler.hpp"
#include <cmath>
#include <fmt/format.h>

// FTAN: Floating Point Tangent
// Computes tangent of ST(0) and replaces it with the result
// ST(0) = tan(ST(0))
inline void handle_FTAN(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    // Get value from top of FPU stack
    double value = cpu.fpu_peek(0);
    
    // Compute tangent
    double result = std::tan(value);
    
    // Replace ST(0) with result
    cpu.fpu_pop();
    cpu.fpu_push(result);
    
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format(
        "[PC={:#06x}] [FTAN] tan({}) = {}", 
        cpu.get_pc(), value, result
    ), Logging::DebugLevel::DETAIL);
    
    // Increment program counter
    cpu.set_pc(cpu.get_pc() + 1);
}
