#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"

// FISTP - Store floating point as integer and pop
void handle_FISTP(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint8_t operand_type = cpu.fetch_operand();
    
    switch (operand_type) {
        case 0x01: { // Memory address - store as 32-bit integer
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Pop value from FPU stack and convert to integer
            double float_value = cpu.fpu_pop();
            int32_t int_value = static_cast<int32_t>(float_value);
            
            // Store 32-bit integer to memory
            cpu.write_mem32(addr, static_cast<uint32_t>(int_value));
            break;
        }
        case 0x02: { // Memory address - store as 16-bit integer
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Pop value from FPU stack and convert to integer
            double float_value = cpu.fpu_pop();
            int16_t int_value = static_cast<int16_t>(float_value);
            
            // Store 16-bit integer to memory (store as lower 16 bits of 32-bit value)
            uint32_t existing = cpu.read_mem32(addr);
            uint32_t new_value = (existing & 0xFFFF0000) | (static_cast<uint16_t>(int_value) & 0xFFFF);
            cpu.write_mem32(addr, new_value);
            break;
        }
        case 0x03: { // Memory address - store as 64-bit integer
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Pop value from FPU stack and convert to integer
            double float_value = cpu.fpu_pop();
            int64_t int_value = static_cast<int64_t>(float_value);
            
            // Store 64-bit integer to memory (as two 32-bit writes)
            uint32_t low = static_cast<uint32_t>(int_value & 0xFFFFFFFF);
            uint32_t high = static_cast<uint32_t>((int_value >> 32) & 0xFFFFFFFF);
            cpu.write_mem32(addr, low);
            cpu.write_mem32(addr + 4, high);
            break;
        }
        default:
            // Invalid operand type
            break;
    }
}