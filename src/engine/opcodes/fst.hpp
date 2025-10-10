#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/logger.hpp"
#include <cstring>

void handle_FST(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // FST - Store floating point value from ST(0) to destination
    // Format: FST <destination>
    // Does NOT pop the stack (ST(0) remains)
    
    if (cpu.get_pc() + 1 >= program.size()) {
        running = false;
        return;
    }
    
    uint8_t operand_type = program[cpu.get_pc() + 1];
    cpu.set_pc(cpu.get_pc() + 2); // Skip opcode and operand type
    
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [FST] operand_type=0x{:02X}", cpu.get_pc() - 2, operand_type) << std::endl;
    
    // Get value from ST(0)
    double value = cpu.fpu_peek(0);
    
    switch (operand_type) {
        case 0x00: {
            // FST to memory (32-bit float)
            if (cpu.get_pc() + 4 > program.size()) {
                running = false;
                return;
            }
            
            uint32_t addr = program[cpu.get_pc()] |
                           (program[cpu.get_pc() + 1] << 8) |
                           (program[cpu.get_pc() + 2] << 16) |
                           (program[cpu.get_pc() + 3] << 24);
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Convert double to 32-bit float
            float float_val = static_cast<float>(value);
            uint32_t raw_float;
            std::memcpy(&raw_float, &float_val, sizeof(float));
            
            // Write to memory
            cpu.write_mem32(addr, raw_float);
            Logger::instance().debug() << fmt::format("[FST] Stored float {} to addr 0x{:04X}", float_val, addr) << std::endl;
            break;
        }
        
        case 0x01: {
            // FST to memory (64-bit double)
            if (cpu.get_pc() + 4 > program.size()) {
                running = false;
                return;
            }
            
            uint32_t addr = program[cpu.get_pc()] |
                           (program[cpu.get_pc() + 1] << 8) |
                           (program[cpu.get_pc() + 2] << 16) |
                           (program[cpu.get_pc() + 3] << 24);
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Convert double to raw bytes
            uint64_t raw_double;
            std::memcpy(&raw_double, &value, sizeof(double));
            
            // Write to memory (little-endian)
            cpu.write_mem32(addr, static_cast<uint32_t>(raw_double & 0xFFFFFFFF));
            cpu.write_mem32(addr + 4, static_cast<uint32_t>(raw_double >> 32));
            Logger::instance().debug() << fmt::format("[FST] Stored double {} to addr 0x{:04X}", value, addr) << std::endl;
            break;
        }
        
        case 0x02: {
            // FST ST(i) - copy ST(0) to ST(i)
            if (cpu.get_pc() + 1 > program.size()) {
                running = false;
                return;
            }
            
            uint8_t st_index = program[cpu.get_pc()];
            cpu.set_pc(cpu.get_pc() + 1);
            
            if (st_index > 7) {
                Logger::instance().error() << fmt::format("[FST] Invalid ST register index: {}", st_index) << std::endl;
                running = false;
                return;
            }
            
            cpu.fpu_store(st_index, value);
            Logger::instance().debug() << fmt::format("[FST] Copied ST(0) = {} to ST({})", value, st_index) << std::endl;
            break;
        }
        
        default:
            Logger::instance().error() << fmt::format("[FST] Unknown operand type: 0x{:02X}", operand_type) << std::endl;
            running = false;
            break;
    }
    
    cpu.print_state("FST");
}