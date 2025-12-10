#pragma once
#include "../cpu.hpp"
#include "../cpu_flags.hpp"
#include "../../debug/debug_handler.hpp"
#include <cmath>
#include <fmt/format.h>

// FUCOMPP: Unordered compare ST(0) with ST(1) and pop twice
// Like FCOMPP but handles NaN (Not a Number) values without raising exceptions
// Compares ST(0) with ST(1), sets CPU flags accordingly, then pops both values
// Flags set:
//   ZF=1, CF=0: ST(0) == ST(1)
//   ZF=0, CF=1: ST(0) < ST(1)
//   ZF=0, CF=0: ST(0) > ST(1)
//   ZF=1, CF=1: Unordered (one or both are NaN)
inline void handle_FUCOMPP(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    // Get values from FPU stack
    double st0 = cpu.fpu_peek(0);
    double st1 = cpu.fpu_peek(1);
    
    // Get current flags
    uint32_t flags = cpu.get_flags();
    
    // Clear ZF and CF flags
    flags &= ~(FLAG_ZERO | FLAG_CARRY);
    
    // Check for NaN (unordered comparison)
    if (std::isnan(st0) || std::isnan(st1)) {
        // Unordered: ZF=1, CF=1
        flags |= (FLAG_ZERO | FLAG_CARRY);
    } else {
        // Compare and set flags (ordered comparison)
        if (st0 == st1) {
            // Equal: ZF=1, CF=0
            flags |= FLAG_ZERO;
        } else if (st0 < st1) {
            // Less than: ZF=0, CF=1
            flags |= FLAG_CARRY;
        }
        // Greater than: ZF=0, CF=0 (both already cleared)
    }
    
    // Update flags
    cpu.set_flags(flags);
    
    // Pop both values
    cpu.fpu_pop();
    cpu.fpu_pop();
    
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format(
        "[PC={:#06x}] [FUCOMPP] unordered compare {} vs {} -> ZF={} CF={}", 
        cpu.get_pc(), st0, st1,
        (flags & FLAG_ZERO) ? 1 : 0,
        (flags & FLAG_CARRY) ? 1 : 0
    ), Logging::DebugLevel::DETAIL);
    
    // Increment program counter
    cpu.set_pc(cpu.get_pc() + 1);
}
