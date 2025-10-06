#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include <cstring>

void handle_FST(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    // FST - Store floating point value from ST(0) to destination
    // Format: FST <destination>
    // Does NOT pop the stack (ST(0) remains)
    
    uint8_t operand_type = cpu.fetch_operand();
    
    // Get value from ST(0)
    double value = cpu.fpu_peek(0);
    
    switch (operand_type) {
        case 0x00: {
            // FST to memory (32-bit float)
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Convert double to 32-bit float
            float float_val = static_cast<float>(value);
            uint32_t raw_float;
            std::memcpy(&raw_float, &float_val, sizeof(float));
            
            // Write to memory
            cpu.write_mem32(addr, raw_float);
            break;
        }
        
        case 0x01: {
            // FST to memory (64-bit double)
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Convert double to raw bytes
            uint64_t raw_double;
            std::memcpy(&raw_double, &value, sizeof(double));
            
            // Write to memory (little-endian)
            cpu.write_mem32(addr, static_cast<uint32_t>(raw_double & 0xFFFFFFFF));
            cpu.write_mem32(addr + 4, static_cast<uint32_t>(raw_double >> 32));
            break;
        }
        
        case 0x02: {
            // FST ST(i) - copy ST(0) to ST(i)
            uint8_t st_index = cpu.fetch_operand();
            if (st_index > 7) {
                // Invalid ST register index
                break;
            }
            
            cpu.fpu_store(st_index, value);
            break;
        }
        
        default:
            // Unknown operand type
            break;
    }
}