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
    
    // Mode-aware address size
    size_t addr_size = cpu.get_address_size();
    
    if (operand_type == 0x00) {
        // Store to memory address
        size_t instr_size = 2 + addr_size;  // opcode + operand_type + address
        if (cpu.get_pc() + instr_size > program.size()) {
            running = false;
            return;
        }
        
        uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc() + 2);
        
        // Check bounds - we need to write 2 bytes
        if (addr + 1 >= cpu.get_memory_size()) {
            Logger::instance().error() << fmt::format(
                "[PC={:#06x}] [FSTSW] Memory access violation: address {:#010x} out of bounds (memory size: {})",
                cpu.get_pc(), addr + 1, cpu.get_memory_size()
            ) << std::endl;
            running = false;
            return;
        }
        
        // Store status word to memory (16-bit)
        cpu.get_memory()[addr] = static_cast<uint8_t>(status_word & 0xFF);
        cpu.get_memory()[addr + 1] = static_cast<uint8_t>((status_word >> 8) & 0xFF);
        
        Logger::instance().debug() << fmt::format(
            "[PC={:#06x}] [FSTSW] stored status word {:#06x} to address {:#010x}", 
            cpu.get_pc(), status_word, addr
        ) << std::endl;
        
        // Increment program counter (opcode + operand_type + mode-aware address)
        cpu.set_pc(cpu.get_pc() + instr_size);
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
