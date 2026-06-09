#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/debug_handler.hpp"
#include "../safe_memcpy.hpp"

void handle_FSTP(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // FSTP - Store floating point value from ST(0) to destination and pop stack
    // Format: FSTP <destination>
    // DOES pop the stack (ST(0) is removed)
    
    if (cpu.get_pc() + 1 >= program.size()) {
        running = false;
        return;
    }
    
    uint8_t operand_type = program[cpu.get_pc() + 1];
    cpu.set_pc(cpu.get_pc() + 2); // Skip opcode and operand type
    
    DEBUG_INFO(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [FSTP] operand_type=0x{:02X}", cpu.get_pc() - 2, operand_type);
    
    // Get and pop value from ST(0)
    double value = cpu.fpu_pop();
    
    // Mode-aware address size
    size_t addr_size = cpu.get_address_size();
    
    switch (operand_type) {
        case 0x00: {
            // FSTP to memory (32-bit float)
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Convert double to 32-bit float
            float float_val = static_cast<float>(value);
            uint32_t raw_float = safe_bitcast<uint32_t>(float_val);
            
            // Write to memory
            cpu.write_mem32(addr, raw_float);
            DEBUG_INFO(Logging::DebugCategory::CPU_EXECUTION, "[FSTP] Stored and popped float {} to addr 0x{:04X}", float_val, addr);
            break;
        }
        
        case 0x01: {
            // FSTP to memory (64-bit double)
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Convert double to raw bytes
            uint64_t raw_double = safe_bitcast<uint64_t>(value);
            
            // Write to memory (little-endian)
            cpu.write_mem32(addr, static_cast<uint32_t>(raw_double & 0xFFFFFFFF));
            cpu.write_mem32(addr + 4, static_cast<uint32_t>(raw_double >> 32));
            DEBUG_INFO(Logging::DebugCategory::CPU_EXECUTION, "[FSTP] Stored and popped double {} to addr 0x{:04X}", value, addr);
            break;
        }
        
        case 0x02: {
            // FSTP ST(i) - move ST(0) to ST(i) and pop
            if (cpu.get_pc() + 1 > program.size()) {
                running = false;
                return;
            }
            
            uint8_t st_index = program[cpu.get_pc()];
            cpu.set_pc(cpu.get_pc() + 1);
            
            if (st_index > 7) {
                Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[FSTP] Invalid ST register index: {}", st_index), Logging::DebugLevel::CRITICAL);
                running = false;
                return;
            }
            
            // Special case: FSTP ST(0) just pops the stack
            if (st_index != 0) {
                cpu.fpu_store(st_index, value);
            }
            DEBUG_INFO(Logging::DebugCategory::CPU_EXECUTION, "[FSTP] Moved ST(0) = {} to ST({}) and popped", value, st_index);
            // Stack already popped above
            break;
        }
        
        default:
            Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[FSTP] Unknown operand type: 0x{:02X}", operand_type), Logging::DebugLevel::CRITICAL);
            running = false;
            break;
    }
    
    cpu.print_state("FSTP");
}