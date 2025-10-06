#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"

// FILD - Load integer as floating point
void handle_FILD(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint8_t operand_type = cpu.fetch_operand();
    
    switch (operand_type) {
        case 0x01: { // Memory address (32-bit integer)
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Load 32-bit integer from memory
            int32_t int_value = static_cast<int32_t>(cpu.read_mem32(addr));
            
            // Convert to double and push onto FPU stack
            double float_value = static_cast<double>(int_value);
            cpu.fpu_push(float_value);
            break;
        }
        case 0x02: { // Memory address (16-bit integer)
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Load 16-bit integer from memory (read as 32-bit and cast)
            uint32_t raw_value = cpu.read_mem32(addr);
            int16_t int_value = static_cast<int16_t>(raw_value & 0xFFFF);
            
            // Convert to double and push onto FPU stack
            double float_value = static_cast<double>(int_value);
            cpu.fpu_push(float_value);
            break;
        }
        case 0x03: { // Memory address (64-bit integer)
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Load 64-bit integer from memory (as two 32-bit reads)
            uint32_t low = cpu.read_mem32(addr);
            uint32_t high = cpu.read_mem32(addr + 4);
            int64_t int_value = (static_cast<int64_t>(high) << 32) | low;
            
            // Convert to double and push onto FPU stack
            double float_value = static_cast<double>(int_value);
            cpu.fpu_push(float_value);
            break;
        }
        default:
            // Invalid operand type
            break;
    }
}