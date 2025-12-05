#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/logger.hpp"
#include <cstring>

void handle_FSUB(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // FSUB - Floating point subtraction
    // Format: FSUB [source] - subtracts source from ST(0)
    // Operand types:
    //   0x00: 32-bit float from memory
    //   0x01: 64-bit double from memory
    //   0x02: Immediate 64-bit double value
    
    if (cpu.get_pc() + 1 >= program.size()) {
        running = false;
        return;
    }
    
    uint8_t operand_type = program[cpu.get_pc() + 1];
    cpu.set_pc(cpu.get_pc() + 2); // Skip opcode and operand_type
    
    // Mode-aware address size
    size_t addr_size = cpu.get_address_size();
    
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [FSUB] operand_type=0x{:02X}", cpu.get_pc() - 2, operand_type) << std::endl;
    
    switch (operand_type) {
        case 0x00: {
            // FSUB from memory (32-bit float)
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Read 32-bit float from memory
            uint32_t raw_float = cpu.read_mem32(addr);
            float float_val;
            std::memcpy(&float_val, &raw_float, sizeof(float));
            
            // Subtract from ST(0)
            double st0_val = cpu.fpu_peek(0);
            double result = st0_val - static_cast<double>(float_val);
            cpu.fpu_store(0, result);
            
            Logger::instance().debug() << fmt::format("[FSUB] {} - {} = {}", st0_val, static_cast<double>(float_val), result) << std::endl;
            break;
        }
        
        case 0x01: {
            // FSUB from memory (64-bit double)
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Read 64-bit double from memory
            uint64_t raw_double = static_cast<uint64_t>(cpu.read_mem32(addr)) |
                                  (static_cast<uint64_t>(cpu.read_mem32(addr + 4)) << 32);
            double double_val;
            std::memcpy(&double_val, &raw_double, sizeof(double));
            
            // Subtract from ST(0)
            double st0_val = cpu.fpu_peek(0);
            double result = st0_val - double_val;
            cpu.fpu_store(0, result);
            
            Logger::instance().debug() << fmt::format("[FSUB] {} - {} = {}", st0_val, double_val, result) << std::endl;
            break;
        }
        
        case 0x02: {
            // FSUB immediate 64-bit double
            if (cpu.get_pc() + 8 > program.size()) {
                running = false;
                return;
            }
            
            uint64_t raw_double = static_cast<uint64_t>(program[cpu.get_pc()]) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+1]) << 8) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+2]) << 16) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+3]) << 24) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+4]) << 32) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+5]) << 40) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+6]) << 48) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+7]) << 56);
            cpu.set_pc(cpu.get_pc() + 8);
            
            double immediate_val;
            std::memcpy(&immediate_val, &raw_double, sizeof(double));
            
            // Subtract from ST(0)
            double st0_val = cpu.fpu_peek(0);
            double result = st0_val - immediate_val;
            cpu.fpu_store(0, result);
            
            Logger::instance().debug() << fmt::format("[FSUB] {} - {} = {}", st0_val, immediate_val, result) << std::endl;
            break;
        }
        
        default:
            Logger::instance().error() << fmt::format("[FSUB] Invalid operand type 0x{:02X}", operand_type) << std::endl;
            running = false;
            break;
    }
    
    cpu.print_state("FSUB");
}
