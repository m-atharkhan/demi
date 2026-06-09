#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/debug_handler.hpp"
#include "../safe_memcpy.hpp"
#include <cmath>
#include <fmt/format.h>

void handle_FDIV(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // FDIV - Floating point division
    // Format: FDIV [source] - divides ST(0) by source
    // Operand types:
    //   0x00: 32-bit float from memory
    //   0x01: 64-bit double from memory
    //   0x02: Immediate 64-bit double value
    
    if (cpu.get_pc() + 1 >= program.size()) {
        running = false;
        return;
    }
    
    uint8_t operand_type = program[cpu.get_pc() + 1];
    cpu.set_pc(cpu.get_pc() + 2); // Skip opcode and operand_type
    
    // Mode-aware address size
    size_t addr_size = cpu.get_address_size();
    
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[PC=0x{:04X}] [FDIV] operand_type=0x{:02X}", cpu.get_pc() - 2, operand_type), Logging::DebugLevel::DETAIL);
    
    switch (operand_type) {
        case 0x00: {
            // FDIV from memory (32-bit float)
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Read 32-bit float from memory
            uint32_t raw_float = cpu.read_mem32(addr);
            float float_val = safe_bitcast<float>(raw_float);
            
            double divisor = static_cast<double>(float_val);
            
            // Check for divide by zero
            if (std::abs(divisor) < 1e-15) {
                Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, "[FDIV] Division by zero", Logging::DebugLevel::CRITICAL);
                running = false;
                return;
            }
            
            // Divide ST(0)
            double st0_val = cpu.fpu_peek(0);
            double result = st0_val / divisor;
            cpu.fpu_store(0, result);
            
            Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[FDIV] {} / {} = {}", st0_val, divisor, result), Logging::DebugLevel::DETAIL);
            break;
        }
        
        case 0x01: {
            // FDIV from memory (64-bit double)
            if (cpu.get_pc() + addr_size > program.size()) {
                running = false;
                return;
            }
            
            uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc());
            cpu.set_pc(cpu.get_pc() + addr_size);
            
            // Read 64-bit double from memory
            uint64_t raw_double = static_cast<uint64_t>(cpu.read_mem32(addr)) |
                                  (static_cast<uint64_t>(cpu.read_mem32(addr + 4)) << 32);
            double double_val = safe_bitcast<double>(raw_double);
            
            // Check for divide by zero
            if (std::abs(double_val) < 1e-15) {
                Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, "[FDIV] Division by zero", Logging::DebugLevel::CRITICAL);
                running = false;
                return;
            }
            
            // Divide ST(0)
            double st0_val = cpu.fpu_peek(0);
            double result = st0_val / double_val;
            cpu.fpu_store(0, result);
            
            Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[FDIV] {} / {} = {}", st0_val, double_val, result), Logging::DebugLevel::DETAIL);
            break;
        }
        
        case 0x02: {
            // FDIV immediate 64-bit double
            if (cpu.get_pc() + 8 > program.size()) {
                running = false;
                return;
            }
            
            uint64_t raw_double = static_cast<uint64_t>(program[cpu.get_pc()]) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+1]) << 8) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+2]) << 16) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+3]) << 24) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+4]) << 32) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+5]) << 40) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+6]) << 48) |
                                 (static_cast<uint64_t>(program[cpu.get_pc()+7]) << 56);
            cpu.set_pc(cpu.get_pc() + 8);
            
            double immediate_val = safe_bitcast<double>(raw_double);
            
            // Check for divide by zero
            if (std::abs(immediate_val) < 1e-15) {
                Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, "[FDIV] Division by zero", Logging::DebugLevel::CRITICAL);
                running = false;
                return;
            }
            
            // Divide ST(0)
            double st0_val = cpu.fpu_peek(0);
            double result = st0_val / immediate_val;
            cpu.fpu_store(0, result);
            
            Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[FDIV] {} / {} = {}", st0_val, immediate_val, result), Logging::DebugLevel::DETAIL);
            break;
        }
        
        default:
            Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[FDIV] Invalid operand type 0x{:02X}", operand_type), Logging::DebugLevel::CRITICAL);
            running = false;
            break;
    }
    
    cpu.print_state("FDIV");
}
