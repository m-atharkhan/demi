#include "loadex.hpp"
#include "../cpu_flags.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

using Logging::Logger;

void handle_loadex(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 9 >= program.size()) { // 1 + 1 + 8 bytes for opcode + reg + address
        Logger::instance().error() << fmt::format(
            "[PC=0x{:04X}] LOADEX: Not enough bytes for instruction", pc) << std::endl;
        running = false;
        return;
    }

    uint8_t reg_id = program[pc + 1];

    // Extract 64-bit address from the instruction (little-endian)
    uint64_t address = 0;
    for (int i = 0; i < 8; i++) {
        address |= static_cast<uint64_t>(program[pc + 2 + i]) << (i * 8);
    }

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}] [LOADEX] Loading from address 0x{:016X} into extended register {}", 
        pc, address, reg_id) << std::endl;

    // Validate memory address bounds
    if (address >= cpu.get_memory_size()) {
        Logger::instance().error() << fmt::format(
            "[PC=0x{:04X}] LOADEX: Memory address 0x{:016X} out of bounds (memory size: 0x{:016X})", 
            pc, address, cpu.get_memory_size()) << std::endl;
        running = false;
        return;
    }

    // Load 64-bit value from memory (little-endian)
    uint64_t value = 0;
    for (int i = 0; i < 8; i++) {
        if (address + i < cpu.get_memory_size()) {
            value |= static_cast<uint64_t>(cpu.get_memory()[address + i]) << (i * 8);
        }
    }

    // Set value in the 64-bit register array (not the legacy 32-bit array)
    cpu.get_registers_64()[reg_id] = value;

    // Also update the legacy register if it's a legacy register (R0-R7)
    if (reg_id < 8) {
        cpu.get_registers()[reg_id] = static_cast<uint32_t>(value);
    }

    // Set flags based on the loaded value
    bool zero = (value == 0);
    bool sign = (static_cast<int64_t>(value) < 0);

    // Update CPU flags
    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN);
    
    if (zero) flags |= FLAG_ZERO;
    if (sign) flags |= FLAG_SIGN;

    cpu.set_flags(flags);

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}] [LOADEX] Loaded 0x{:016X} from address 0x{:016X} into register {} (Flags: S={} Z={})",
        pc, value, address, reg_id,
        (flags & FLAG_SIGN) ? 1 : 0,
        (flags & FLAG_ZERO) ? 1 : 0) << std::endl;

    // Move to next instruction (1 opcode + 1 reg + 8 address bytes)
    cpu.set_pc(pc + 10);

    cpu.print_state("LOADEX");
}