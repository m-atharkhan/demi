#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include <cstring>

void handle_FLD(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    // FLD - Load floating point value onto FPU stack
    // Format: FLD <source>
    // Source can be: memory address, immediate value, or ST(i) register
    
    uint8_t operand_type = cpu.fetch_operand();
    
    switch (operand_type) {
        case 0x00: {
            // FLD from memory (32-bit float)
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Read 32-bit float from memory
            uint32_t raw_float = cpu.read_mem32(addr);
            float float_val;
            std::memcpy(&float_val, &raw_float, sizeof(float));
            
            // Push onto FPU stack as double
            cpu.fpu_push(static_cast<double>(float_val));
            break;
        }
        
        case 0x01: {
            // FLD from memory (64-bit double)
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Read 64-bit double from memory
            uint64_t raw_double = static_cast<uint64_t>(cpu.read_mem32(addr)) |
                                  (static_cast<uint64_t>(cpu.read_mem32(addr + 4)) << 32);
            double double_val;
            std::memcpy(&double_val, &raw_double, sizeof(double));
            
            cpu.fpu_push(double_val);
            break;
        }
        
        case 0x02: {
            // FLD immediate value (64-bit double encoded in instruction)
            uint64_t raw_double = static_cast<uint64_t>(cpu.read_mem32(cpu.get_pc())) |
                                  (static_cast<uint64_t>(cpu.read_mem32(cpu.get_pc() + 4)) << 32);
            cpu.set_pc(cpu.get_pc() + 8);
            
            double double_val;
            std::memcpy(&double_val, &raw_double, sizeof(double));
            
            cpu.fpu_push(double_val);
            break;
        }
        
        case 0x03: {
            // FLD ST(i) - duplicate ST(i) register onto stack top
            uint8_t st_index = cpu.fetch_operand();
            if (st_index > 7) {
                // Invalid ST register index
                break;
            }
            
            double value = cpu.fpu_peek(st_index);
            cpu.fpu_push(value);
            break;
        }
        
        default:
            // Unknown operand type - could log warning
            break;
    }
}