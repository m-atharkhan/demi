#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/format.h>

// FINIT - Initialize FPU
void handle_FINIT(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[PC=0x{:04X}] [FINIT] Starting FPU initialization", cpu.get_pc()), Logging::DebugLevel::DETAIL);
    
    // Initialize FPU to its default state
    cpu.fpu_init();
    
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[PC=0x{:04X}] [FINIT] FPU initialization completed", cpu.get_pc()), Logging::DebugLevel::DETAIL);
    
    // Advance PC by 1 (for the opcode itself)
    cpu.set_pc(cpu.get_pc() + 1);
}