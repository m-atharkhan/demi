#include "storex.hpp"
#include "../cpu_flags.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/core.h>

void handle_storex(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 9 >= program.size()) { // 1 + 1 + 8 bytes for opcode + reg + address
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, fmt::format("[PC=0x{:04X}] STOREX: Not enough bytes for instruction", pc), Logging::DebugLevel::CRITICAL);
        running = false;
        return;
    }

    uint8_t reg_id = program[pc + 1];

    // Extract 64-bit address from the instruction (little-endian)
    uint64_t address = 0;
    for (int i = 0; i < 8; i++) {
        address |= static_cast<uint64_t>(program[pc + 2 + i]) << (i * 8);
    }

    // Get value from the 64-bit register array (not the legacy 32-bit array)
    uint64_t value = cpu.get_registers_64()[reg_id];

    DEBUG_INFO(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [STOREX] Storing 0x{:016X} from register {} to address 0x{:016X}", pc, value, reg_id, address);

    // Validate memory address bounds
    if (address + 7 >= cpu.get_memory_size()) {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::MEM_ACCESS, fmt::format("[PC=0x{:04X}] STOREX: Memory address 0x{:016X} out of bounds (memory size: 0x{:016X})", pc, address, cpu.get_memory_size()), Logging::DebugLevel::CRITICAL);
        running = false;
        return;
    }

    // Store 64-bit value to memory (little-endian)
    for (int i = 0; i < 8; i++) {
        if (address + i < cpu.get_memory_size()) {
            uint8_t byte_value = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
            cpu.get_memory()[address + i] = byte_value;
        }
    }

    // Set flags based on the stored value
    bool zero = (value == 0);
    bool sign = (static_cast<int64_t>(value) < 0);

    // Update CPU flags
    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN);
    
    if (zero) flags |= FLAG_ZERO;
    if (sign) flags |= FLAG_SIGN;

    cpu.set_flags(flags);

    DEBUG_INFO(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [STOREX] Stored 0x{:016X} from register {} to address 0x{:016X} (Flags: S={} Z={})",
        pc, value, reg_id, address,
        (flags & FLAG_SIGN) ? 1 : 0,
        (flags & FLAG_ZERO) ? 1 : 0);

    // Move to next instruction (1 opcode + 1 reg + 8 address bytes)
    cpu.set_pc(pc + 10);

    cpu.print_state("STOREX");
}