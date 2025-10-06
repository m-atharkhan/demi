#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include <cstring>

void handle_FSTP(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    // FSTP - Store floating point value from ST(0) to destination and pop stack
    // Format: FSTP <destination>
    // DOES pop the stack (ST(0) is removed)
    
    uint8_t operand_type = cpu.fetch_operand();
    
    // Get and pop value from ST(0)
    double value = cpu.fpu_pop();
    
    switch (operand_type) {
        case 0x00: {
            // FSTP to memory (32-bit float)
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
            // FSTP to memory (64-bit double)
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
            // FSTP ST(i) - move ST(0) to ST(i) and pop
            uint8_t st_index = cpu.fetch_operand();
            if (st_index > 7) {
                // Invalid ST register index
                break;
            }
            
            // Special case: FSTP ST(0) just pops the stack
            if (st_index != 0) {
                cpu.fpu_store(st_index, value);
            }
            // Stack already popped above
            break;
        }
        
        default:
            // Unknown operand type
            break;
    }
}