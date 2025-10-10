#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/logger.hpp"
#include <cstring>

void handle_FLD(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // FLD - Load floating point value onto FPU stack
    // Format: FLD <source>
    // Source can be: memory address, immediate value, or ST(i) register
    
    if (cpu.get_pc() + 1 >= program.size()) {
        running = false;
        return;
    }
    
    uint8_t operand_type = program[cpu.get_pc() + 1];
    cpu.set_pc(cpu.get_pc() + 2); // Skip opcode and operand type
    
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [FLD] operand_type=0x{:02X}", cpu.get_pc() - 2, operand_type) << std::endl;
    
    switch (operand_type) {
        case 0x00: {
            // FLD from memory (32-bit float)
            if (cpu.get_pc() + 4 > program.size()) {
                running = false;
                return;
            }
            
            uint32_t addr = program[cpu.get_pc()] |
                           (program[cpu.get_pc() + 1] << 8) |
                           (program[cpu.get_pc() + 2] << 16) |
                           (program[cpu.get_pc() + 3] << 24);
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Read 32-bit float from memory
            uint32_t raw_float = cpu.read_mem32(addr);
            float float_val;
            std::memcpy(&float_val, &raw_float, sizeof(float));
            
            // Push onto FPU stack as double
            cpu.fpu_push(static_cast<double>(float_val));
            Logger::instance().debug() << fmt::format("[FLD] Loaded float {} from addr 0x{:04X}", float_val, addr) << std::endl;
            break;
        }
        
        case 0x01: {
            // FLD from memory (64-bit double)
            if (cpu.get_pc() + 4 > program.size()) {
                running = false;
                return;
            }
            
            uint32_t addr = program[cpu.get_pc()] |
                           (program[cpu.get_pc() + 1] << 8) |
                           (program[cpu.get_pc() + 2] << 16) |
                           (program[cpu.get_pc() + 3] << 24);
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Read 64-bit double from memory
            uint64_t raw_double = static_cast<uint64_t>(cpu.read_mem32(addr)) |
                                  (static_cast<uint64_t>(cpu.read_mem32(addr + 4)) << 32);
            double double_val;
            std::memcpy(&double_val, &raw_double, sizeof(double));
            
            cpu.fpu_push(double_val);
            Logger::instance().debug() << fmt::format("[FLD] Loaded double {} from addr 0x{:04X}", double_val, addr) << std::endl;
            break;
        }
        
        case 0x02: {
            // FLD immediate value (64-bit double encoded in instruction)
            if (cpu.get_pc() + 8 > program.size()) {
                running = false;
                return;
            }
            
            uint64_t raw_double = static_cast<uint64_t>(program[cpu.get_pc()]) |
                                  (static_cast<uint64_t>(program[cpu.get_pc() + 1]) << 8) |
                                  (static_cast<uint64_t>(program[cpu.get_pc() + 2]) << 16) |
                                  (static_cast<uint64_t>(program[cpu.get_pc() + 3]) << 24) |
                                  (static_cast<uint64_t>(program[cpu.get_pc() + 4]) << 32) |
                                  (static_cast<uint64_t>(program[cpu.get_pc() + 5]) << 40) |
                                  (static_cast<uint64_t>(program[cpu.get_pc() + 6]) << 48) |
                                  (static_cast<uint64_t>(program[cpu.get_pc() + 7]) << 56);
            cpu.set_pc(cpu.get_pc() + 8);
            
            double double_val;
            std::memcpy(&double_val, &raw_double, sizeof(double));
            
            cpu.fpu_push(double_val);
            Logger::instance().debug() << fmt::format("[FLD] Loaded immediate double {}", double_val) << std::endl;
            break;
        }
        
        case 0x03: {
            // FLD ST(i) - duplicate ST(i) register onto stack top
            if (cpu.get_pc() + 1 > program.size()) {
                running = false;
                return;
            }
            
            uint8_t st_index = program[cpu.get_pc()];
            cpu.set_pc(cpu.get_pc() + 1);
            
            if (st_index > 7) {
                Logger::instance().error() << fmt::format("[FLD] Invalid ST register index: {}", st_index) << std::endl;
                running = false;
                return;
            }
            
            double value = cpu.fpu_peek(st_index);
            cpu.fpu_push(value);
            Logger::instance().debug() << fmt::format("[FLD] Duplicated ST({}) = {}", st_index, value) << std::endl;
            break;
        }
        
        default:
            Logger::instance().error() << fmt::format("[FLD] Unknown operand type: 0x{:02X}", operand_type) << std::endl;
            running = false;
            break;
    }
    
    cpu.print_state("FLD");
}