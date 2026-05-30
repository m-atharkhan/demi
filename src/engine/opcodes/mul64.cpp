#include "mul64.hpp"
#include "../cpu_flags.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/core.h>

void handle_mul64(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    // Check bounds for 3-operand instruction (opcode + 3 registers)
    if (pc + 3 >= program.size()) {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
            fmt::format("[PC=0x{:04X}] MUL64: Not enough bytes for instruction", pc), Logging::DebugLevel::CRITICAL);
        running = false;
        return;
    }

    uint8_t dest_reg = program[pc + 1];   // Destination register
    uint8_t src1_reg = program[pc + 2];   // Source register 1
    uint8_t src2_reg = program[pc + 3];   // Source register 2

    // Debug output for operation
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC=0x{:04X}] [MUL64] R{} = R{} * R{}", 
        pc, dest_reg, src1_reg, src2_reg), Logging::DebugLevel::DETAIL);

    // Check register bounds before accessing
    if (dest_reg >= cpu.get_registers_64().size() || 
        src1_reg >= cpu.get_registers_64().size() || 
        src2_reg >= cpu.get_registers_64().size()) {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
            fmt::format("[PC=0x{:04X}] MUL64: Invalid register numbers - dest:{} src1:{} src2:{}", 
            pc, dest_reg, src1_reg, src2_reg), Logging::DebugLevel::CRITICAL);
        running = false;
        return;
    }

    // Get values from 64-bit register array
    int64_t value1 = cpu.get_registers_64()[src1_reg];
    int64_t value2 = cpu.get_registers_64()[src2_reg];

    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC=0x{:04X}] [MUL64] Values: {} * {}", 
        pc, value1, value2), Logging::DebugLevel::DETAIL);

    // Simple multiplication for now
    int64_t result = value1 * value2;

    // Set result in 64-bit register array
    cpu.get_registers_64()[dest_reg] = result;

    // Set flags based on the result
    bool zero = (result == 0);
    bool sign = (result < 0);
    __int128 full_product = static_cast<__int128>(value1) * static_cast<__int128>(value2);
    bool overflow = (full_product != static_cast<__int128>(result));

    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN | FLAG_CARRY | FLAG_OVERFLOW);
    if (zero) flags |= FLAG_ZERO;
    if (sign) flags |= FLAG_SIGN;
    if (overflow) flags |= FLAG_CARRY | FLAG_OVERFLOW;
    cpu.set_flags(flags);

    // Also update legacy register if it's a legacy register (R0-R7)
    if (dest_reg < 8) {
        cpu.get_registers()[dest_reg] = static_cast<uint32_t>(result);
    }

    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC=0x{:04X}] [MUL64] Result: R{} = {}", 
        pc, dest_reg, result), Logging::DebugLevel::DETAIL);

    // Move to next instruction (opcode + 3 register bytes)
    cpu.set_pc(pc + 4);
}