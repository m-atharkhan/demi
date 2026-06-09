#include "or64.hpp"
#include "../cpu_flags.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/core.h>

void handle_or64(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 3 >= program.size()) {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
            fmt::format("[PC=0x{:04X}] OR64: Not enough bytes for instruction", pc), Logging::DebugLevel::CRITICAL);
        running = false;
        return;
    }

    uint8_t dest_reg = program[pc + 1];
    uint8_t src1_reg = program[pc + 2];
    uint8_t src2_reg = program[pc + 3];

    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC=0x{:04X}] [OR64] R{} = R{} | R{}",
        pc, dest_reg, src1_reg, src2_reg), Logging::DebugLevel::DETAIL);

    if (dest_reg >= cpu.get_registers_64().size() ||
        src1_reg >= cpu.get_registers_64().size() ||
        src2_reg >= cpu.get_registers_64().size()) {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
            fmt::format("[PC=0x{:04X}] OR64: Register index out of bounds", pc), Logging::DebugLevel::CRITICAL);
        running = false;
        return;
    }

    uint64_t operand1 = cpu.get_registers_64()[src1_reg];
    uint64_t operand2 = cpu.get_registers_64()[src2_reg];
    uint64_t result = operand1 | operand2;

    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC=0x{:04X}] [OR64] Values: {} | {}",
        pc, operand1, operand2), Logging::DebugLevel::DETAIL);

    cpu.get_registers_64()[dest_reg] = result;

    if (dest_reg < 8) {
        cpu.get_registers()[dest_reg] = static_cast<uint32_t>(result);
    }

    bool zero = (result == 0);
    bool sign = (static_cast<int64_t>(result) < 0);

    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN | FLAG_OVERFLOW | FLAG_CARRY);

    if (zero) flags |= FLAG_ZERO;
    if (sign) flags |= FLAG_SIGN;

    cpu.set_flags(flags);

    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC=0x{:04X}] [OR64] Result: R{} = {} (Flags: C={} O={} S={} Z={})",
        pc, dest_reg, result,
        0, 0, sign ? 1 : 0, zero ? 1 : 0), Logging::DebugLevel::DETAIL);

    cpu.set_pc(pc + 4);
    cpu.print_state("OR64");
}
