#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/format.h>

// FSTCW: Store FPU control word to memory
// Format: FSTCW <memory_address>
// Stores the 16-bit FPU control word to the specified memory address
void handle_FSTCW(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 >= program.size()) {
        running = false;
        return;
    }
    
    // Mode-aware address reading
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address
    
    if (cpu.get_pc() + instr_size > program.size()) {
        running = false;
        return;
    }
    
    uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc() + 1);
    
    // Get control word
    uint16_t control_word = cpu.get_fpu_control_word();
    
    // Check bounds - we need to write 2 bytes
        if (cpu.get_memory_size() < addr + 2) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::MEM_ACCESS,
                fmt::format("FSTCW: Memory access violation at address 0x{:08X}", addr), Logging::DebugLevel::CRITICAL);
            return;
        }    // Store control word to memory (16-bit)
    cpu.get_memory()[addr] = static_cast<uint8_t>(control_word & 0xFF);
    cpu.get_memory()[addr + 1] = static_cast<uint8_t>((control_word >> 8) & 0xFF);
    
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC={:#06x}] [FSTCW] stored control word {:#06x} to address {:#010x}", 
        cpu.get_pc(), control_word, addr), Logging::DebugLevel::DETAIL);
    
    // Increment program counter (opcode + mode-aware address)
    cpu.set_pc(cpu.get_pc() + instr_size);
}
