#include "cmp64.hpp"
#include "../cpu_flags.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

using Logging::Logger;

void handle_cmp64(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 2 >= program.size()) {
        Logger::instance().error() << fmt::format(
            "[PC=0x{:04X}] CMP64: Not enough bytes for instruction", pc) << std::endl;
        running = false;
        return;
    }

    uint8_t reg1_id = program[pc + 1];
    uint8_t reg2_id = program[pc + 2];

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}] [CMP64] Comparing 64-bit registers {} and {}", 
        pc, reg1_id, reg2_id) << std::endl;

    // Get values directly from register array (consistent with other 64-bit instructions)
    uint64_t value1 = cpu.get_registers_64()[reg1_id];
    uint64_t value2 = cpu.get_registers_64()[reg2_id];

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}] [CMP64] Comparing {} with {}", 
        pc, value1, value2) << std::endl;

    // Perform 64-bit comparison using subtraction
    int64_t signed_val1 = static_cast<int64_t>(value1);
    int64_t signed_val2 = static_cast<int64_t>(value2);
    
    // Calculate the difference for flag setting
    uint64_t result = value1 - value2;
    
    // Set flags based on comparison
    bool zero = (value1 == value2);
    bool sign = (signed_val1 < signed_val2);
    bool carry = (value1 < value2);  // Unsigned comparison
    
    // Overflow occurs when signed comparison gives different result than unsigned
    bool overflow = ((signed_val1 > signed_val2) && (value1 < value2)) ||
                   ((signed_val1 < signed_val2) && (value1 > value2));

    // Update CPU flags
    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN | FLAG_OVERFLOW | FLAG_CARRY);
    
    if (zero) flags |= FLAG_ZERO;
    if (sign) flags |= FLAG_SIGN;
    if (carry) flags |= FLAG_CARRY;
    if (overflow) flags |= FLAG_OVERFLOW;

    cpu.set_flags(flags);

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}] [CMP64] Compare 0x{:016X} vs 0x{:016X} (Flags: C={} O={} S={} Z={})",
        pc, value1, value2,
        (flags & FLAG_CARRY) ? 1 : 0,
        (flags & FLAG_OVERFLOW) ? 1 : 0,
        (flags & FLAG_SIGN) ? 1 : 0,
        (flags & FLAG_ZERO) ? 1 : 0) << std::endl;

    // Move to next instruction
    cpu.set_pc(pc + 3);

    cpu.print_state("CMP64");
}