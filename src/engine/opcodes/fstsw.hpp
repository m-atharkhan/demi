#pragma once
#include "../cpu.hpp"
#include "../cpu_registers.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/logger.hpp"
#include <fmt/format.h>

// FSTSW: Store FPU status word to memory or register
// Format: FSTSW <memory_address> or FSTSW AX
// Stores the 16-bit FPU status word to the specified destination
void handle_FSTSW(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 >= program.size()) {
        running = false;
        return;
    }
    
    // Read operand type
    uint8_t operand_type = program[cpu.get_pc() + 1];
    
    // Get status word
    uint16_t status_word = cpu.get_fpu_status_word();
    
    if (operand_type == 0x00) {
        // Store to memory address
        if (cpu.get_pc() + 6 > program.size()) {
            running = false;
            return;
        }
        
        uint32_t addr = program[cpu.get_pc() + 2] |
                       (program[cpu.get_pc() + 3] << 8) |
                       (program[cpu.get_pc() + 4] << 16) |
                       (program[cpu.get_pc() + 5] << 24);
        
        // Store status word to memory (16-bit)
        cpu.get_memory()[addr] = static_cast<uint8_t>(status_word & 0xFF);
        cpu.get_memory()[addr + 1] = static_cast<uint8_t>((status_word >> 8) & 0xFF);
        
        Logger::instance().debug() << fmt::format(
            "[PC={:#06x}] [FSTSW] stored status word {:#06x} to address {:#010x}", 
            cpu.get_pc(), status_word, addr
        ) << std::endl;
        
        // Increment program counter (opcode + operand_type + 4 bytes address)
        cpu.set_pc(cpu.get_pc() + 6);
    } else if (operand_type == 0x01) {
        // Store to AX register (RAX in our case)
        cpu.set_register(Register::RAX, static_cast<uint32_t>(status_word));
        
        Logger::instance().debug() << fmt::format(
            "[PC={:#06x}] [FSTSW] stored status word {:#06x} to AX (R0)", 
            cpu.get_pc(), status_word
        ) << std::endl;
        
        // Increment program counter (opcode + operand_type)
        cpu.set_pc(cpu.get_pc() + 2);
    } else {
        Logger::instance().error() << fmt::format(
            "[PC={:#06x}] [FSTSW] invalid operand type {:#04x}",
            cpu.get_pc(), operand_type
        ) << std::endl;
        running = false;
    }
}
