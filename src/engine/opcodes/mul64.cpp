#include "mul64.hpp"
#include "../cpu_flags.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

using Logging::Logger;

void handle_mul64(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    // Check bounds for 3-operand instruction (opcode + 3 registers)
    if (pc + 3 >= program.size()) {
        Logger::instance().error() << fmt::format(
            "[PC=0x{:04X}] MUL64: Not enough bytes for instruction", pc) << std::endl;
        running = false;
        return;
    }

    uint8_t dest_reg = program[pc + 1];   // Destination register
    uint8_t src1_reg = program[pc + 2];   // Source register 1
    uint8_t src2_reg = program[pc + 3];   // Source register 2

    // Debug output for operation
    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}] [MUL64] R{} = R{} * R{}", 
        pc, dest_reg, src1_reg, src2_reg) << std::endl;

    // Check register bounds before accessing
    if (dest_reg >= cpu.get_registers_64().size() || 
        src1_reg >= cpu.get_registers_64().size() || 
        src2_reg >= cpu.get_registers_64().size()) {
        Logger::instance().error() << fmt::format(
            "[PC=0x{:04X}] MUL64: Invalid register numbers - dest:{} src1:{} src2:{}", 
            pc, dest_reg, src1_reg, src2_reg) << std::endl;
        running = false;
        return;
    }

    // Get values from 64-bit register array
    int64_t value1 = cpu.get_registers_64()[src1_reg];
    int64_t value2 = cpu.get_registers_64()[src2_reg];

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}] [MUL64] Values: {} * {}", 
        pc, value1, value2) << std::endl;

    // Simple multiplication for now
    int64_t result = value1 * value2;

    // Set result in 64-bit register array
    cpu.get_registers_64()[dest_reg] = result;

    // Also update legacy register if it's a legacy register (R0-R7)
    if (dest_reg < 8) {
        cpu.get_registers()[dest_reg] = static_cast<uint32_t>(result);
    }

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}] [MUL64] Result: R{} = {}", 
        pc, dest_reg, result) << std::endl;

    // Move to next instruction (opcode + 3 register bytes)
    cpu.set_pc(pc + 4);
}