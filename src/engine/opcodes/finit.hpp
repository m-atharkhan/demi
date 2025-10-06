#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/logger.hpp"

// FINIT - Initialize FPU
void handle_FINIT(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    Logger::instance().debug() << "[PC=0x" << std::hex << cpu.get_pc() << "] [FINIT] Starting FPU initialization" << std::endl;
    
    // Initialize FPU to its default state
    cpu.fpu_init();
    
    Logger::instance().debug() << "[PC=0x" << std::hex << cpu.get_pc() << "] [FINIT] FPU initialization completed" << std::endl;
    
    // Advance PC by 1 (for the opcode itself)
    cpu.set_pc(cpu.get_pc() + 1);
}