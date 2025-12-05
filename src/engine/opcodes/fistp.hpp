#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/logger.hpp"

void handle_FISTP(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // FISTP - Store floating point as integer and pop
    // Format: FISTP <dest> - converts ST(0) to integer, stores, and pops from FPU stack
    // Operand types:
    //   0x01: Memory address (32-bit integer)
    //   0x02: Memory address (16-bit integer)
    //   0x03: Memory address (64-bit integer)
    
    if (cpu.get_pc() + 1 >= program.size()) {
        running = false;
        return;
    }
    
    uint8_t operand_type = program[cpu.get_pc() + 1];
    cpu.set_pc(cpu.get_pc() + 2); // Skip opcode and operand_type
    
    // Mode-aware address size
    size_t addr_size = cpu.get_address_size();
    
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [FISTP] operand_type=0x{:02X}", cpu.get_pc() - 2, operand_type) << std::endl;
    
    switch (operand_type) {
        case 0x01: { // Memory address - store as 32-bit integer
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Pop value from FPU stack and convert to integer
            double float_value = cpu.fpu_pop();
            int32_t int_value = static_cast<int32_t>(float_value);
            
            // Store 32-bit integer to memory
            cpu.write_mem32(addr, static_cast<uint32_t>(int_value));
            
            Logger::instance().debug() << fmt::format("[FISTP] Popped ST(0)={} as int32 {} to addr 0x{:04X}", float_value, int_value, addr) << std::endl;
            break;
        }
        
        case 0x02: { // Memory address - store as 16-bit integer
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Pop value from FPU stack and convert to integer
            double float_value = cpu.fpu_pop();
            int16_t int_value = static_cast<int16_t>(float_value);
            
            // Store 16-bit integer to memory (store as lower 16 bits of 32-bit value)
            uint32_t existing = cpu.read_mem32(addr);
            uint32_t new_value = (existing & 0xFFFF0000) | (static_cast<uint16_t>(int_value) & 0xFFFF);
            cpu.write_mem32(addr, new_value);
            
            Logger::instance().debug() << fmt::format("[FISTP] Popped ST(0)={} as int16 {} to addr 0x{:04X}", float_value, int_value, addr) << std::endl;
            break;
        }
        
        case 0x03: { // Memory address - store as 64-bit integer
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Pop value from FPU stack and convert to integer
            double float_value = cpu.fpu_pop();
            int64_t int_value = static_cast<int64_t>(float_value);
            
            // Store 64-bit integer to memory (as two 32-bit writes)
            uint32_t low = static_cast<uint32_t>(int_value & 0xFFFFFFFF);
            uint32_t high = static_cast<uint32_t>((int_value >> 32) & 0xFFFFFFFF);
            cpu.write_mem32(addr, low);
            cpu.write_mem32(addr + 4, high);
            
            Logger::instance().debug() << fmt::format("[FISTP] Popped ST(0)={} as int64 {} to addr 0x{:04X}", float_value, int_value, addr) << std::endl;
            break;
        }
        
        default:
            Logger::instance().error() << fmt::format("[FISTP] Invalid operand type 0x{:02X}", operand_type) << std::endl;
            running = false;
            break;
    }
    
    cpu.print_state("FISTP");
}