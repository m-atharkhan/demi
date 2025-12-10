#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/format.h>

void handle_FILD(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // FILD - Load integer as floating point
    // Format: FILD <source> - loads integer from source, converts to double, pushes onto FPU stack
    // Operand types:
    //   0x00: Immediate 32-bit integer
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
    
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[PC=0x{:04X}] [FILD] operand_type=0x{:02X}", cpu.get_pc() - 2, operand_type), Logging::DebugLevel::DETAIL);
    
    switch (operand_type) {
        case 0x00: { // Immediate 32-bit integer
            if (cpu.get_pc() + 4 > program.size()) {
                running = false;
                return;
            }
            
            uint32_t raw_value = program[cpu.get_pc()] | (program[cpu.get_pc()+1] << 8) | 
                                 (program[cpu.get_pc()+2] << 16) | (program[cpu.get_pc()+3] << 24);
            cpu.set_pc(cpu.get_pc() + 4);
            
            int32_t int_value = static_cast<int32_t>(raw_value);
            double float_value = static_cast<double>(int_value);
            cpu.fpu_push(float_value);
            
            Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[FILD] Loaded immediate int32 {} as double {}", int_value, float_value), Logging::DebugLevel::DETAIL);
            break;
        }
        
        case 0x01: { // Memory address (32-bit integer)
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Load 32-bit integer from memory
            int32_t int_value = static_cast<int32_t>(cpu.read_mem32(addr));
            double float_value = static_cast<double>(int_value);
            cpu.fpu_push(float_value);
            
            Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[FILD] Loaded int32 {} from addr 0x{:04X} as double {}", int_value, addr, float_value), Logging::DebugLevel::DETAIL);
            break;
        }
        
        case 0x02: { // Memory address (16-bit integer)
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Load 16-bit integer from memory
            uint32_t raw_value = cpu.read_mem32(addr);
            int16_t int_value = static_cast<int16_t>(raw_value & 0xFFFF);
            double float_value = static_cast<double>(int_value);
            cpu.fpu_push(float_value);
            
            Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[FILD] Loaded int16 {} from addr 0x{:04X} as double {}", int_value, addr, float_value), Logging::DebugLevel::DETAIL);
            break;
        }
        
        case 0x03: { // Memory address (64-bit integer)
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Load 64-bit integer from memory (as two 32-bit reads)
            uint32_t low = cpu.read_mem32(addr);
            uint32_t high = cpu.read_mem32(addr + 4);
            int64_t int_value = (static_cast<int64_t>(high) << 32) | low;
            double float_value = static_cast<double>(int_value);
            cpu.fpu_push(float_value);
            
            Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[FILD] Loaded int64 {} from addr 0x{:04X} as double {}", int_value, addr, float_value), Logging::DebugLevel::DETAIL);
            break;
        }
        
        default:
            Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[FILD] Invalid operand type 0x{:02X}", operand_type), Logging::DebugLevel::CRITICAL);
            running = false;
            break;
    }
    
    cpu.print_state("FILD");
}