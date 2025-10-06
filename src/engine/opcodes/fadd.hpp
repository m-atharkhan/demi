#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include <cstring>

void handle_FADD(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    // FADD - Floating point addition
    // Format: FADD [source] - adds source to ST(0)
    // If no operand, adds ST(1) to ST(0) and pops ST(1)
    
    uint8_t operand_type = cpu.fetch_operand();
    
    switch (operand_type) {
        case 0x00: {
            // FADD from memory (32-bit float)
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Read 32-bit float from memory
            uint32_t raw_float = cpu.read_mem32(addr);
            float float_val;
            std::memcpy(&float_val, &raw_float, sizeof(float));
            
            // Add to ST(0)
            double st0_val = cpu.fpu_peek(0);
            double result = st0_val + static_cast<double>(float_val);
            cpu.fpu_store(0, result);
            break;
        }
        
        case 0x01: {
            // FADD from memory (64-bit double)
            uint32_t addr = cpu.read_mem32(cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + 4);
            
            // Read 64-bit double from memory
            uint64_t raw_double = static_cast<uint64_t>(cpu.read_mem32(addr)) |
                                  (static_cast<uint64_t>(cpu.read_mem32(addr + 4)) << 32);
            double double_val;
            std::memcpy(&double_val, &raw_double, sizeof(double));
            
            // Add to ST(0)
            double st0_val = cpu.fpu_peek(0);
            double result = st0_val + double_val;
            cpu.fpu_store(0, result);
            break;
        }
        
        case 0x02: {
            // FADD ST(i) - add ST(i) to ST(0)
            uint8_t st_index = cpu.fetch_operand();
            if (st_index > 7) {
                // Invalid ST register index
                break;
            }
            
            double st0_val = cpu.fpu_peek(0);
            double sti_val = cpu.fpu_peek(st_index);
            double result = st0_val + sti_val;
            cpu.fpu_store(0, result);
            break;
        }
        
        case 0x03: {
            // FADD ST(0), ST(i) - add ST(0) to ST(i)
            uint8_t st_index = cpu.fetch_operand();
            if (st_index > 7) {
                // Invalid ST register index
                break;
            }
            
            double st0_val = cpu.fpu_peek(0);
            double sti_val = cpu.fpu_peek(st_index);
            double result = sti_val + st0_val;
            cpu.fpu_store(st_index, result);
            break;
        }
        
        case 0xFF: {
            // FADD with no operand - add ST(1) to ST(0), pop ST(1)
            if (cpu.get_fpu_stack_top() == 7) {
                // Stack underflow - only one element
                break;
            }
            
            double st0_val = cpu.fpu_peek(0);
            double st1_val = cpu.fpu_peek(1);
            double result = st0_val + st1_val;
            
            // Pop ST(1) first (this moves ST(1) to ST(0) position)
            cpu.fpu_pop();
            // Store result in new ST(0)
            cpu.fpu_store(0, result);
            break;
        }
        
        default:
            // Unknown operand type
            break;
    }
}