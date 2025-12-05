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
    
    // Mode-aware address reading
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address
    
    if (cpu.get_pc() + instr_size > program.size()) {
        running = false;
        return;
    }
    
    uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc() + 1);
    
    // Check bounds - we need to read 2 bytes
    if (addr + 1 >= cpu.get_memory_size()) {
        Logger::instance().error() << fmt::format(
            "[PC={:#06x}] [FLDCW] Memory access violation: address {:#010x} out of bounds (memory size: {})",
            cpu.get_pc(), addr + 1, cpu.get_memory_size()
        ) << std::endl;
        running = false;
        return;
    }
    
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
    
    // Increment program counter (opcode + mode-aware address)
    cpu.set_pc(cpu.get_pc() + instr_size);
}
