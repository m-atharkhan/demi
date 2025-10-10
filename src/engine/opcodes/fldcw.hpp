#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/logger.hpp"
#include <fmt/format.h>

// FLDCW: Load FPU control word from memory
// Format: FLDCW <memory_address>
// Loads the 16-bit FPU control word from the specified memory address
void handle_FLDCW(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 >= program.size()) {
        running = false;
        return;
    }
    
    // Read memory address (4 bytes)
    if (cpu.get_pc() + 5 > program.size()) {
        running = false;
        return;
    }
    
    uint32_t addr = program[cpu.get_pc() + 1] |
                   (program[cpu.get_pc() + 2] << 8) |
                   (program[cpu.get_pc() + 3] << 16) |
                   (program[cpu.get_pc() + 4] << 24);
    
    // Load control word from memory (16-bit)
    uint8_t low_byte = cpu.get_memory()[addr];
    uint8_t high_byte = cpu.get_memory()[addr + 1];
    uint16_t control_word = low_byte | (high_byte << 8);
    
    // Set control word
    cpu.set_fpu_control_word(control_word);
    
    Logger::instance().debug() << fmt::format(
        "[PC={:#06x}] [FLDCW] loaded control word {:#06x} from address {:#010x}", 
        cpu.get_pc(), control_word, addr
    ) << std::endl;
    
    // Increment program counter (opcode + 4 bytes address)
    cpu.set_pc(cpu.get_pc() + 5);
}
