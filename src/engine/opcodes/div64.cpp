#include "div64.hpp"
#include "../cpu_flags.hpp"
#include "../cpu.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/core.h>

void handle_div64(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    // Check bounds for 3-operand instruction (opcode + 3 registers)
    if (pc + 3 >= program.size()) {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
            fmt::format("[PC=0x{:04X}] DIV64: Not enough bytes for instruction", pc), Logging::DebugLevel::CRITICAL);
        running = false;
        return;
    }

    uint8_t dest_reg = program[pc + 1];   // Destination register
    uint8_t src1_reg = program[pc + 2];   // Source register 1 (dividend)
    uint8_t src2_reg = program[pc + 3];   // Source register 2 (divisor)

    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC=0x{:04X}] [DIV64] R{} = R{} / R{}", 
        pc, dest_reg, src1_reg, src2_reg), Logging::DebugLevel::DETAIL);

    // Check register bounds before accessing
    if (dest_reg >= cpu.get_registers_64().size() || 
        src1_reg >= cpu.get_registers_64().size() || 
        src2_reg >= cpu.get_registers_64().size()) {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
            fmt::format("[PC=0x{:04X}] DIV64: Register index out of bounds", pc), Logging::DebugLevel::CRITICAL);
        running = false;
        return;
    }

    // Get values from 64-bit register array
    uint64_t dividend = cpu.get_registers_64()[src1_reg];
    uint64_t divisor = cpu.get_registers_64()[src2_reg];

    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC=0x{:04X}] [DIV64] Values: {} / {}", 
        pc, dividend, divisor), Logging::DebugLevel::DETAIL);

    // Check for division by zero
    if (divisor == 0) {
        std::string error_msg = fmt::format("[PC=0x{:04X}] DIV64: Division by zero", pc);
        Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION, error_msg, Logging::DebugLevel::CRITICAL);
        
        // Set error flags 
        uint32_t flags = cpu.get_flags();
        flags |= FLAG_OVERFLOW; // Use overflow flag to indicate division by zero
        cpu.set_flags(flags);
        
        // Throw exception to halt execution and trigger error handling
        throw CPUException(error_msg);
    }

    // Perform 64-bit division
    uint64_t result = dividend / divisor;
    uint64_t remainder = dividend % divisor;

    // Set result in 64-bit register array
    cpu.get_registers_64()[dest_reg] = result;

    // Store remainder in RDX (register 2), following x86 convention
    cpu.get_registers_64()[2] = remainder;

    // Also update legacy register if it's a legacy register (R0-R7)
    if (dest_reg < 8) {
        cpu.get_registers()[dest_reg] = static_cast<uint32_t>(result);
    }
    
    // Update legacy RDX as well
    cpu.get_registers()[2] = static_cast<uint32_t>(remainder);

    // Set flags based on the result
    bool zero = (result == 0);
    bool sign = (static_cast<int64_t>(result) < 0);

    // Update CPU flags
    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN | FLAG_OVERFLOW | FLAG_CARRY);
    
    if (zero) flags |= FLAG_ZERO;
    if (sign) flags |= FLAG_SIGN;
    // Clear overflow and carry flags for successful division

    cpu.set_flags(flags);

    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PC=0x{:04X}] [DIV64] Result: R{} = 0x{:016X}, RDX (remainder) = 0x{:016X} (Flags: C={} O={} S={} Z={})",
        pc, dest_reg, result, remainder,
        (flags & FLAG_CARRY) ? 1 : 0,
        (flags & FLAG_OVERFLOW) ? 1 : 0,
        (flags & FLAG_SIGN) ? 1 : 0,
        (flags & FLAG_ZERO) ? 1 : 0), Logging::DebugLevel::DETAIL);

    // Move to next instruction (opcode + 3 operands = 4 bytes)
    cpu.set_pc(pc + 4);

    cpu.print_state("DIV64");
}