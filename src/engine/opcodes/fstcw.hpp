#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/logger.hpp"
#include <fmt/format.h>

// FSTCW: Store FPU control word to memory
// Format: FSTCW <memory_address>
// Stores the 16-bit FPU control word to the specified memory address
void handle_FSTCW(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
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
    
    // Get control word
    uint16_t control_word = cpu.get_fpu_control_word();
    
    // Check bounds - we need to write 2 bytes
    if (addr + 1 >= cpu.get_memory_size()) {
        Logger::instance().error() << fmt::format(
            "[PC={:#06x}] [FSTCW] Memory access violation: address {:#010x} out of bounds (memory size: {})",
            cpu.get_pc(), addr + 1, cpu.get_memory_size()
        ) << std::endl;
        running = false;
        return;
    }
    
    // Store control word to memory (16-bit)
    cpu.get_memory()[addr] = static_cast<uint8_t>(control_word & 0xFF);
    cpu.get_memory()[addr + 1] = static_cast<uint8_t>((control_word >> 8) & 0xFF);
    
    Logger::instance().debug() << fmt::format(
        "[PC={:#06x}] [FSTCW] stored control word {:#06x} to address {:#010x}", 
        cpu.get_pc(), control_word, addr
    ) << std::endl;
    
    // Increment program counter (opcode + 4 bytes address)
    cpu.set_pc(cpu.get_pc() + 5);
}
