// Consolidated opcode implementations for standalone builds
// This file includes all opcode implementations in a single compilation unit
// to reduce compilation time for standalone executables

#include "opcode_dispatcher.hpp"
#include "../cpu.hpp"
#include "../cpu_flags.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/logger.hpp"
#include "../../debug/debug_handler.hpp"
#include "../../debug/error_handler.hpp"
#include <fmt/core.h>
#include <iomanip>

using Logging::Logger;
using Logging::DebugHandler;
using Logging::DebugCategory;
using Logging::DebugLevel;
using Logging::ErrorHandler;
using Logging::ErrorCode;

// Include all opcode header files
#include "add.hpp"
#include "and.hpp"
#include "call.hpp"
#include "cmp.hpp"
#include "db.hpp"
#include "dec.hpp"
#include "div.hpp"
#include "fadd.hpp"
#include "fsub.hpp"
#include "fmul.hpp"
#include "fdiv.hpp"
#include "fild.hpp"
#include "fist.hpp"
#include "finit.hpp"
#include "fistp.hpp"
#include "fld.hpp"
#include "fst.hpp"
#include "fstp.hpp"
#include "fabs.hpp"
#include "fchs.hpp"
#include "fsqrt.hpp"
#include "fsin.hpp"
#include "fcos.hpp"
#include "ftan.hpp"
#include "fcompp.hpp"
#include "fucompp.hpp"
#include "fclex.hpp"
#include "fstcw.hpp"
#include "fldcw.hpp"
#include "fstsw.hpp"
#include "halt.hpp"
#include "cli.hpp"
#include "sti.hpp"
#include "int.hpp"
#include "iret.hpp"
#include "inb.hpp"
#include "inc.hpp"
#include "in.hpp"
#include "inl.hpp"
#include "instr.hpp"
#include "inw.hpp"
#include "jmp.hpp"
#include "jns.hpp"
#include "jnz.hpp"
#include "js.hpp"
#include "jz.hpp"
#include "jc.hpp"
#include "jnc.hpp"
#include "jo.hpp"
#include "jno.hpp"
#include "lea.hpp"
#include "load.hpp"
#include "load_imm.hpp"
#include "loadr.hpp"
#include "mov.hpp"
#include "mul.hpp"
#include "nop.hpp"
#include "not.hpp"
#include "opcode_handler.hpp"
#include "or.hpp"
#include "outb.hpp"
#include "out.hpp"
#include "outl.hpp"
#include "outstr.hpp"
#include "outw.hpp"
#include "pop_arg.hpp"
#include "pop_flag.hpp"
#include "pop.hpp"
#include "push_arg.hpp"
#include "push_flag.hpp"
#include "push.hpp"
#include "ret.hpp"
#include "shl.hpp"
#include "shr.hpp"
#include "store.hpp"
#include "sub.hpp"
#include "swap.hpp"
#include "xor.hpp"

// Extended 64-bit register operation headers
#include "add64.hpp"
#include "sub64.hpp"
#include "mov64.hpp"
#include "load_imm64.hpp"
#include "mul64.hpp"
#include "div64.hpp"
#include "and64.hpp"
#include "cmp64.hpp"
#include "movex.hpp"
#include "addex.hpp"
#include "subex.hpp"
#include "loadex.hpp"
#include "storex.hpp"

// CPU mode control headers
#include "mode32.hpp"
#include "mode64.hpp"
#include "modecmp.hpp"

// SIMD instruction headers
#include "vadd.hpp"
#include "vmul.hpp"
#include "vdot.hpp"
#include "vmax.hpp"
#include "vbroadcast.hpp"
#include "vcmpgt.hpp"
#include "packb.hpp"
#include "unpackb.hpp"

// Consolidated implementations of all opcodes

// Implementation from add.cpp - MODE-AWARE
void handle_add(CPU& cpu, const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        
        // Validate register bounds
        if (reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} or R{} out of range (max: R{})", reg1, reg2, cpu.get_registers_64().size() - 1);
            std::string message = "Invalid register access in ADD instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 3);
            cpu.print_state("ADD");
            return;
        }
        
        DEBUG_INSTRUCTION("ADD", cpu.get_pc(), fmt::format("R{} += R{}", reg1, reg2), "");

        // Mode-aware arithmetic
        uint64_t value1 = cpu.read_register_mode_aware(reg1);
        uint64_t value2 = cpu.read_register_mode_aware(reg2);
        uint64_t mask = cpu.get_operand_mask();
        uint64_t result = (value1 + value2) & mask;

        // Update flags based on mode
        uint32_t flags = cpu.get_flags();
        
        // Carry flag - check for overflow beyond operand size
        if (cpu.is_64bit_mode()) {
            if (result < value1) flags |= FLAG_CARRY;
            else flags &= ~FLAG_CARRY;
        } else {
            uint64_t sum = value1 + value2;
            if (sum > 0xFFFFFFFF) flags |= FLAG_CARRY;
            else flags &= ~FLAG_CARRY;
        }

        // Overflow flag for signed arithmetic
        bool sign1 = cpu.is_64bit_mode() ? ((value1 >> 63) & 1) : ((value1 >> 31) & 1);
        bool sign2 = cpu.is_64bit_mode() ? ((value2 >> 63) & 1) : ((value2 >> 31) & 1);
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        
        if ((sign1 == sign2) && (sign1 != signr)) {
            flags |= FLAG_OVERFLOW;
        } else {
            flags &= ~FLAG_OVERFLOW;
        }

        // Zero flag
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;

        // Sign flag
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;

        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg1, result);
        
        DEBUG_DETAIL(Logging::DebugCategory::CPU_REGISTERS, "R{}: 0x{:X} + 0x{:X} = 0x{:X}", 
                    reg1, value1, value2, result);
    }
    cpu.set_pc(cpu.get_pc() + 3);
    cpu.print_state("ADD");
}

// Implementation from and.cpp - MODE-AWARE
void handle_and(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];

        // Validate register bounds
        if (reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} or R{} out of range", reg1, reg2);
            std::string message = "Invalid register access in AND instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("AND");
            return;
        }

        // Mode-aware register operations
        uint64_t value1 = cpu.read_register_mode_aware(reg1);
        uint64_t value2 = cpu.read_register_mode_aware(reg2);
        uint64_t result = value1 & value2;
        
        // Update flags
        uint32_t flags = cpu.get_flags();
        flags &= ~(FLAG_CARRY | FLAG_OVERFLOW);  // AND clears these
        
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;
        
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;
        
        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg1, result);

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("AND");
}

// Implementation from call.cpp
void handle_call(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    // Check for call stack overflow (too many nested calls)
    size_t max_depth = cpu.get_effective_max_call_depth();
    if (cpu.get_call_depth() >= max_depth) {
        std::string context = fmt::format("Call depth: {} (max: {})", cpu.get_call_depth(), max_depth);
        std::string message = fmt::format("Call stack overflow: maximum nesting depth exceeded");
        ErrorHandler::instance().report_runtime(ErrorCode::CPU_CALL_STACK_OVERFLOW, message, pc, context);
        running = false;
        throw CPUException(message);
    }

    // Reset offset at each call
    cpu.set_arg_offset(8);

    // Read mode-aware address
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address
    
    if (pc + addr_size >= program.size()) {
        running = false;
        return;
    }
    
    uint64_t addr = cpu.read_address_from_program(program, pc + 1);

    // Push old FP
    uint32_t sp = cpu.get_sp() - 4;
    cpu.set_sp(sp);
    cpu.get_registers()[4] = sp;
    cpu.write_mem32(sp, cpu.get_fp());

    // Push return address (pc + instr_size for mode-aware CALL instruction)
    sp -= 4;
    cpu.set_sp(sp);
    cpu.get_registers()[4] = sp;
    cpu.write_mem32(sp, pc + instr_size);

    // Set new FP
    cpu.set_fp(sp);
    
    // Increment call depth
    cpu.increment_call_depth();
    
    cpu.print_stack_frame("CALL");
    cpu.set_pc(static_cast<uint32_t>(addr));
    cpu.print_state("CALL");
}

// Implementation from cmp.cpp - MODE-AWARE
void handle_cmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        
        // Validate register bounds
        if (reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} or R{} out of range", reg1, reg2);
            std::string message = "Invalid register access in CMP instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 3);
            cpu.print_state("CMP");
            return;
        }
        
        // Mode-aware register operations
        uint64_t value1 = cpu.read_register_mode_aware(reg1);
        uint64_t value2 = cpu.read_register_mode_aware(reg2);
        
        uint32_t flags = cpu.get_flags();
        
        if (cpu.is_64bit_mode()) {
            int64_t result = static_cast<int64_t>(value1) - static_cast<int64_t>(value2);
            
            // Clear and update zero flag
            flags &= ~FLAG_ZERO;
            if (result == 0) flags |= FLAG_ZERO;
            
            // Clear and update sign flag
            flags &= ~FLAG_SIGN;
            if (result < 0) flags |= FLAG_SIGN;
            
            // Update carry flag (unsigned comparison)
            flags &= ~FLAG_CARRY;
            if (value1 < value2) flags |= FLAG_CARRY;
        } else {
            int32_t result = static_cast<int32_t>(value1) - static_cast<int32_t>(value2);
            
            // Clear and update zero flag
            flags &= ~FLAG_ZERO;
            if (result == 0) flags |= FLAG_ZERO;
            
            // Clear and update sign flag
            flags &= ~FLAG_SIGN;
            if (result < 0) flags |= FLAG_SIGN;
            
            // Update carry flag (unsigned comparison)
            flags &= ~FLAG_CARRY;
            if (static_cast<uint32_t>(value1) < static_cast<uint32_t>(value2)) flags |= FLAG_CARRY;
        }
        
        cpu.set_flags(flags);
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("CMP");
}

// Implementation from db.cpp
void handle_db(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    // DB opcode: Define bytes - copy data to specified address
    if (pc + 2 < program.size()) {
        uint8_t target_addr = program[pc + 1]; // Target memory address
        uint8_t length = program[pc + 2]; // Number of data bytes

        DebugHandler::instance().report(DebugCategory::MEM_ACCESS, fmt::format(
            "[PC=0x{:04X}] [DB] Copying {} data bytes to address 0x{:02X}",
            pc, length, target_addr
        ), DebugLevel::DETAIL);

        // Copy data bytes to memory starting at target_addr
        for (uint8_t i = 0; i < length && (pc + 3 + i) < program.size() && (target_addr + i) < cpu.get_memory().size(); ++i) {
            cpu.get_memory()[target_addr + i] = program[pc + 3 + i];

            DebugHandler::instance().report(DebugCategory::MEM_ACCESS, fmt::format(
                "[PC=0x{:04X}] [DB] memory[0x{:02X}] = 0x{:02X} ('{}')",
                pc, target_addr + i, program[pc + 3 + i],
                (program[pc + 3 + i] >= 32 && program[pc + 3 + i] <= 126) ? static_cast<char>(program[pc + 3 + i]) : '.'
            ), DebugLevel::DETAIL);
        }

        cpu.set_pc(pc + 3 + length); // Skip opcode + target_addr + length + data bytes
    } else {
        running = false;
    }

    cpu.print_state("DB");
}

// Implementation from dec.cpp - MODE-AWARE
void handle_dec(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        DEBUG_INSTRUCTION("DEC", cpu.get_pc(), fmt::format("R{}", static_cast<int>(reg)), "");
        
        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in DEC instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 2);
            cpu.print_state("DEC");
            return;
        }
        
        // Mode-aware register operations
        uint64_t value = cpu.read_register_mode_aware(reg);
        uint64_t result = value - 1;
        uint64_t mask = cpu.get_operand_mask();
        result &= mask;
        
        // Update flags
        uint32_t flags = cpu.get_flags();
        
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;
        
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;
        
        // Overflow when decrementing from MIN_VALUE (for signed arithmetic)
        if (cpu.is_64bit_mode()) {
            if (value == 0x8000000000000000ULL) flags |= FLAG_OVERFLOW;
            else flags &= ~FLAG_OVERFLOW;
        } else {
            if (static_cast<uint32_t>(value) == 0x80000000U) flags |= FLAG_OVERFLOW;
            else flags &= ~FLAG_OVERFLOW;
        }
        
        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg, result);
        
        DEBUG_DETAIL(Logging::DebugCategory::CPU_REGISTERS, "R{}: 0x{:X} - 1 = 0x{:X}", static_cast<int>(reg), value, result);
        cpu.set_pc(cpu.get_pc() + 2);
    } else {
        running = false;
    }
    cpu.print_state("DEC");
}

// Implementation from div.cpp - MODE-AWARE
void handle_div(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        
        // Validate register bounds
        if (reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} or R{} out of range", reg1, reg2);
            std::string message = "Invalid register access in DIV instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 3);
            cpu.print_state("DIV");
            return;
        }
        
        DEBUG_INSTRUCTION("DIV", cpu.get_pc(), fmt::format("R{} /= R{}", reg1, reg2), "");
        
        // Mode-aware register operations
        uint64_t value1 = cpu.read_register_mode_aware(reg1);
        uint64_t value2 = cpu.read_register_mode_aware(reg2);
        
        if (value2 == 0) {
            std::string context = fmt::format("R{} = 0x{:X}, R{} = 0", reg1, value1, reg2);
            std::string message = "Division by zero: attempted to divide by register with value 0";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_DIVISION_BY_ZERO, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        uint64_t result = value1 / value2;
        
        // Update flags
        uint32_t flags = cpu.get_flags();
        flags &= ~(FLAG_CARRY | FLAG_OVERFLOW);  // DIV typically clears these
        
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;
        
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;
        
        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg1, result);
        
        DEBUG_DETAIL(Logging::DebugCategory::CPU_REGISTERS, "R{}: 0x{:X} / 0x{:X} = 0x{:X}", reg1, value1, value2, result);
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("DIV");
}

// Implementation for MOD - MODE-AWARE
void handle_mod(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        
        // Validate register bounds
        if (reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} or R{} out of range", reg1, reg2);
            std::string message = "Invalid register access in MOD instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 3);
            cpu.print_state("MOD");
            return;
        }
        
        DEBUG_INSTRUCTION("MOD", cpu.get_pc(), fmt::format("R{} %= R{}", reg1, reg2), "");
        
        // Mode-aware register operations
        uint64_t value1 = cpu.read_register_mode_aware(reg1);
        uint64_t value2 = cpu.read_register_mode_aware(reg2);
        
        if (value2 == 0) {
            std::string context = fmt::format("R{} = 0x{:X}, R{} = 0", reg1, value1, reg2);
            std::string message = "Modulo by zero: attempted to compute modulo with divisor of 0";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_MODULO_BY_ZERO, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        uint64_t result = value1 % value2;
        
        // Update flags
        uint32_t flags = cpu.get_flags();
        flags &= ~(FLAG_CARRY | FLAG_OVERFLOW);  // MOD typically clears these
        
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;
        
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;
        
        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg1, result);
        
        DEBUG_DETAIL(Logging::DebugCategory::CPU_REGISTERS, "R{}: 0x{:X} % 0x{:X} = 0x{:X}", reg1, value1, value2, result);
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("MOD");
}

// Implementation from halt.cpp
void handle_halt(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    // FIXED: Remove Logger call to prevent deadlock
    // Logger::instance().debug() << fmt::format(
    //     "[PC=0x{:04X}] [HALT] PC={}",
    //     cpu.get_pc(), cpu.get_pc()
    // ) << std::endl;
    running = false;
    cpu.set_pc(cpu.get_pc() + 1);
    cpu.print_state("HALT");
}

// Implementation for INT opcode - Software interrupt
void handle_int(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 1 < program.size()) {
        uint8_t vector = program[pc + 1];
        
        DebugHandler::instance().report(DebugCategory::IO_INTERRUPT, fmt::format(
            "[PC=0x{:04X}] [INT] Triggering software interrupt vector 0x{:02X}",
            pc, vector
        ), DebugLevel::INFO);
        
        cpu.trigger_interrupt(vector);
        cpu.set_pc(pc + 2);
    } else {
        running = false;
    }
    
    cpu.print_state("INT");
}

// Implementation for IRET opcode - Return from interrupt
void handle_iret(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    DebugHandler::instance().report(DebugCategory::IO_INTERRUPT, fmt::format(
        "[PC=0x{:04X}] [IRET] Returning from interrupt",
        pc
    ), DebugLevel::INFO);
    
    // Check for stack underflow
    if (cpu.get_sp() + 8 > cpu.get_memory().size()) {
        std::string context = fmt::format("Stack pointer: 0x{:X}, memory size: 0x{:X}", 
            cpu.get_sp(), cpu.get_memory().size());
        std::string message = "Stack underflow during IRET: insufficient data to pop";
        ErrorHandler::instance().report_runtime(ErrorCode::CPU_STACK_UNDERFLOW, message, pc, context);
        running = false;
        throw CPUException(message);
    }
    
    // Pop flags and return address from stack
    uint32_t flags = cpu.read_mem32(cpu.get_sp());
    cpu.set_sp(cpu.get_sp() + 4);
    uint32_t ret_addr = cpu.read_mem32(cpu.get_sp());
    cpu.set_sp(cpu.get_sp() + 4);
    
    cpu.set_flags(flags);
    cpu.set_pc(ret_addr);
    
    cpu.print_state("IRET");
}

// Implementation for CLI opcode - Clear interrupt flag (disable interrupts)
void handle_cli(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    
    DebugHandler::instance().report(DebugCategory::IO_INTERRUPT, 
        "[CLI] Disabling interrupts (placeholder - interrupt system not fully implemented)",
        DebugLevel::INFO);
    
    // TODO: Implement interrupt flag when interrupt system is complete
    // For now, this is a no-op
    cpu.set_pc(pc + 1);
    
    cpu.print_state("CLI");
}

// Implementation for STI opcode - Set interrupt flag (enable interrupts)
void handle_sti(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    
    DebugHandler::instance().report(DebugCategory::IO_INTERRUPT, 
        "[STI] Enabling interrupts (placeholder - interrupt system not fully implemented)",
        DebugLevel::INFO);
    
    // TODO: Implement interrupt flag when interrupt system is complete
    // For now, this is a no-op
    cpu.set_pc(pc + 1);
    
    cpu.print_state("STI");
}

// Implementation from inb.cpp
void handle_inb(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in INB instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("INB");
            return;
        }

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [INB] PC={} R{} <- port {}",
            pc, pc, reg, port
        ), DebugLevel::DETAIL);

        uint8_t value = cpu.read_port(port);
        cpu.write_register_mode_aware(reg, value);

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [INB] R{} = {}",
            pc, reg, value
        ), DebugLevel::DETAIL);

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("INB");
}

// Implementation from inc.cpp - MODE-AWARE
void handle_inc(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        DEBUG_INSTRUCTION("INC", cpu.get_pc(), fmt::format("R{}", static_cast<int>(reg)), "");
        
        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in INC instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 2);
            cpu.print_state("INC");
            return;
        }
        
        // Mode-aware register operations
        uint64_t value = cpu.read_register_mode_aware(reg);
        uint64_t result = value + 1;
        uint64_t mask = cpu.get_operand_mask();
        result &= mask;
        
        // Update flags
        uint32_t flags = cpu.get_flags();
        
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;
        
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;
        
        // Overflow when incrementing from MAX_VALUE (for signed arithmetic)
        if (cpu.is_64bit_mode()) {
            if (value == 0x7FFFFFFFFFFFFFFFULL) flags |= FLAG_OVERFLOW;
            else flags &= ~FLAG_OVERFLOW;
        } else {
            if (static_cast<uint32_t>(value) == 0x7FFFFFFFU) flags |= FLAG_OVERFLOW;
            else flags &= ~FLAG_OVERFLOW;
        }
        
        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg, result);
        
        DEBUG_DETAIL(Logging::DebugCategory::CPU_REGISTERS, "R{}: 0x{:X} + 1 = 0x{:X}", static_cast<int>(reg), value, result);
        cpu.set_pc(cpu.get_pc() + 2);
    } else {
        running = false;
    }
    cpu.print_state("INC");
}

// Implementation from in.cpp - MODE-AWARE
void handle_in(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in IN instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("IN");
            return;
        }

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [IN] PC={} R{} <- port {}",
            pc, pc, reg, port
        ), DebugLevel::DETAIL);

        uint8_t value = cpu.read_port(port);
        cpu.write_register_mode_aware(reg, value);

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [IN] R{} = {}",
            pc, reg, value
        ), DebugLevel::DETAIL);

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("IN");
}

// Implementation from inl.cpp - MODE-AWARE
void handle_inl(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in INL instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("INL");
            return;
        }

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [INL] PC={} R{} <- port {} (dword)",
            pc, pc, reg, port
        ), DebugLevel::DETAIL);

        uint32_t value = cpu.read_port_dword(port);
        // In mode-aware fashion, store the full dword in the register
        cpu.write_register_mode_aware(reg, value);

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("INL");
}

// Implementation from instr.cpp - MODE-AWARE
void handle_instr(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in INSTR instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("INSTR");
            return;
        }

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [INSTR] PC={} R{} <- port {} (string)",
            pc, pc, reg, port
        ), DebugLevel::DETAIL);

        uint64_t maxLength = cpu.read_register_mode_aware(reg); // Use register value as max length
        std::string value = cpu.read_port_string(port, static_cast<uint8_t>(maxLength & 0xFF));

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [INSTR] Read string: '{}'",
            pc, value
        ), DebugLevel::DETAIL);

        // Store string length in register
        cpu.write_register_mode_aware(reg, value.length());

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("INSTR");
}

// Implementation from inw.cpp - MODE-AWARE
void handle_inw(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in INW instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("INW");
            return;
        }

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [INW] PC={} R{} <- port {} (word)",
            pc, pc, reg, port
        ), DebugLevel::DETAIL);

        uint16_t value = cpu.read_port_word(port);
        // In mode-aware fashion, store the full word in the register
        cpu.write_register_mode_aware(reg, value);

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [INW] R{} = 0x{:04X}",
            pc, reg, value
        ), DebugLevel::DETAIL);

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("INW");
}

// Implementation from jmp.cpp
void handle_jmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address
    
    if (cpu.get_pc() + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc() + 1);
        
        // Simple validation - check if address is within program bounds
        if (addr >= program.size()) {
            std::string context = fmt::format("Jump target: 0x{:X}, program size: 0x{:X}", addr, program.size());
            std::string message = "Invalid jump address: target exceeds program bounds";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_JUMP, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        cpu.set_pc(static_cast<uint32_t>(addr));
    } else {
        running = false;
    }
    cpu.print_state("JMP");
}

// Implementation from jns.cpp
void handle_jns(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address

    if (pc + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, pc + 1);

        if (!(cpu.get_flags() & FLAG_SIGN)) {
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            cpu.set_pc(pc + instr_size);
        }
    } else {
        running = false;
    }

    cpu.print_state("JNS");
}

// Implementation from jnz.cpp
void handle_jnz(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address

    if (pc + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, pc + 1);

        if (!(cpu.get_flags() & FLAG_ZERO)) {
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            cpu.set_pc(pc + instr_size);
        }
    } else {
        running = false;
    }

    cpu.print_state("JNZ");
}

// Implementation from js.cpp
void handle_js(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address

    if (pc + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, pc + 1);

        if (cpu.get_flags() & FLAG_SIGN) {
            // Simple validation - check if address is within program bounds
            if (addr >= program.size()) {
                std::string context = fmt::format("Jump target: 0x{:X}, program size: 0x{:X}", addr, program.size());
                std::string message = "Invalid jump address in JS instruction: target exceeds program bounds";
                ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_JUMP, message, cpu.get_pc(), context);
                running = false;
                throw CPUException(message);
            }
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            cpu.set_pc(pc + instr_size);
        }
    } else {
        running = false;
    }

    cpu.print_state("JS");
}

// Implementation from jz.cpp
void handle_jz(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address
    
    if (cpu.get_pc() + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc() + 1);
        
        if (cpu.get_flags() & FLAG_ZERO) {
            // Simple validation - check if address is within program bounds
            if (addr >= program.size()) {
                std::string context = fmt::format("Jump target: 0x{:X}, program size: 0x{:X}", addr, program.size());
                std::string message = "Invalid jump address in JZ instruction: target exceeds program bounds";
                ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_JUMP, message, cpu.get_pc(), context);
                running = false;
                throw CPUException(message);
            }
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            cpu.set_pc(cpu.get_pc() + instr_size);
        }
    } else {
        running = false;
    }
    cpu.print_state("JZ");
}

// Implementation from load.cpp
void handle_load(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // LOAD reg, addr (mode-aware address size)
    // Format: OPCODE(1) REG(1) ADDR(4 or 8 bytes depending on mode)
    // 32-bit mode: 6 bytes total, 64-bit mode: 10 bytes total
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 2 + addr_size;  // opcode + reg + address
    
    if (cpu.get_pc() + instr_size - 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        // Read address based on current CPU mode
        uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc() + 2);
        
        // Check bounds - use mode-aware register count
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range (0-{})", reg, cpu.get_registers_64().size() - 1);
            std::string message = "Invalid register in LOAD instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        if (addr >= cpu.get_memory().size()) {
            std::string context = fmt::format("Attempted access at 0x{:X}, memory range: 0x0000-0x{:X}", addr, cpu.get_memory().size() - 1);
            std::string message = "Memory read out of bounds in LOAD instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_MEMORY_OUT_OF_BOUNDS, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        // Mode-aware register write
        cpu.write_register_mode_aware(reg, cpu.get_memory()[addr]);
        cpu.set_pc(cpu.get_pc() + instr_size);
    } else {
        running = false;
    }
    cpu.print_state("LOAD");
}

// Implementation of LOADR - Load value from memory to register (indirect addressing) - MODE-AWARE
void handle_loadr(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // LOADR dest_reg, addr_reg
    // Load value from memory address stored in addr_reg into dest_reg
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t dest_reg = program[cpu.get_pc() + 1];
        uint8_t addr_reg = program[cpu.get_pc() + 2];
        
        // Check destination register bounds - mode-aware
        if (dest_reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range (0-{})", dest_reg, cpu.get_registers_64().size() - 1);
            std::string message = "Invalid destination register in LOADR instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        // Check address register bounds - mode-aware
        if (addr_reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range (0-{})", addr_reg, cpu.get_registers_64().size() - 1);
            std::string message = "Invalid address register in LOADR instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        // Get the address from the address register - mode-aware
        uint64_t addr = cpu.read_register_mode_aware(addr_reg);
        
        // Check memory bounds
        if (addr >= cpu.get_memory().size()) {
            std::string context = fmt::format("Attempted read at 0x{:X}, memory range: 0x0000-0x{:X}", addr, cpu.get_memory().size() - 1);
            std::string message = "Memory read out of bounds in LOADR instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_MEMORY_OUT_OF_BOUNDS, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        // Load value from memory address into destination register - mode-aware
        cpu.write_register_mode_aware(dest_reg, cpu.get_memory()[addr]);
        cpu.set_pc(cpu.get_pc() + 3);
        
        DebugHandler::instance().report(DebugCategory::MEM_ACCESS, fmt::format(
            "[PC=0x{:04X}] [LOADR] R{} = memory[R{}] = memory[0x{:X}] = 0x{:02X}", 
            cpu.get_pc() - 3, dest_reg, addr_reg, addr, cpu.get_memory()[addr]
        ), DebugLevel::DETAIL);
    } else {
        running = false;
    }
    cpu.print_state("LOADR");
}

// Implementation of STORER - Store value from register to memory (indirect addressing) - MODE-AWARE
void handle_storer(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // STORER value_reg, addr_reg
    // Store value from value_reg into memory address stored in addr_reg
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t value_reg = program[cpu.get_pc() + 1];
        uint8_t addr_reg = program[cpu.get_pc() + 2];
        
        // Check value register bounds - mode-aware
        if (value_reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range (0-{})", value_reg, cpu.get_registers_64().size() - 1);
            std::string message = "Invalid value register in STORER instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        // Check address register bounds - mode-aware
        if (addr_reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range (0-{})", addr_reg, cpu.get_registers_64().size() - 1);
            std::string message = "Invalid address register in STORER instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        // Get the address from the address register - mode-aware
        uint64_t addr = cpu.read_register_mode_aware(addr_reg);
        
        // Check memory bounds
        if (addr >= cpu.get_memory().size()) {
            std::string context = fmt::format("Attempted write at 0x{:X}, memory range: 0x0000-0x{:X}", addr, cpu.get_memory().size() - 1);
            std::string message = "Memory write out of bounds in STORER instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_MEMORY_OUT_OF_BOUNDS, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        // Store value from value_reg into memory address - mode-aware read
        uint64_t value = cpu.read_register_mode_aware(value_reg);
        cpu.get_memory()[addr] = static_cast<uint8_t>(value & 0xFF);
        cpu.set_pc(cpu.get_pc() + 3);
        
        DebugHandler::instance().report(DebugCategory::MEM_ACCESS, fmt::format(
            "[PC=0x{:04X}] [STORER] memory[R{}] = memory[0x{:X}] = R{} = 0x{:02X}", 
            cpu.get_pc() - 3, addr_reg, addr, value_reg, cpu.get_memory()[addr]
        ), DebugLevel::DETAIL);
    } else {
        running = false;
    }
    cpu.print_state("STORER");
}

// Implementation from load_imm.cpp - MODE-AWARE
void handle_load_imm(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // LOAD_IMM is mode-aware:
    // - 32-bit mode: opcode + reg + imm32 (6 bytes total)
    // - 64-bit mode: opcode + reg + imm64 (10 bytes total)
    // For backward compatibility, 8-bit immediates still supported
    
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        
        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in LOAD_IMM instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 3);
            cpu.print_state("LOAD_IMM");
            return;
        }
        
        uint64_t value;
        size_t pc_increment;
        
        if (cpu.is_64bit_mode()) {
            // 64-bit mode: read 8-byte immediate
            if (cpu.get_pc() + 9 < program.size()) {
                value = static_cast<uint64_t>(program[cpu.get_pc() + 2]) |
                       (static_cast<uint64_t>(program[cpu.get_pc() + 3]) << 8) |
                       (static_cast<uint64_t>(program[cpu.get_pc() + 4]) << 16) |
                       (static_cast<uint64_t>(program[cpu.get_pc() + 5]) << 24) |
                       (static_cast<uint64_t>(program[cpu.get_pc() + 6]) << 32) |
                       (static_cast<uint64_t>(program[cpu.get_pc() + 7]) << 40) |
                       (static_cast<uint64_t>(program[cpu.get_pc() + 8]) << 48) |
                       (static_cast<uint64_t>(program[cpu.get_pc() + 9]) << 56);
                pc_increment = 10;
                DEBUG_INSTRUCTION("LOAD_IMM", cpu.get_pc(), fmt::format("R{}, 0x{:016X} (64-bit)", reg, value), "");
            } else if (cpu.get_pc() + 5 < program.size()) {
                // Fallback to 32-bit if not enough bytes for 64-bit
                value = static_cast<uint32_t>(program[cpu.get_pc() + 2]) |
                       (static_cast<uint32_t>(program[cpu.get_pc() + 3]) << 8) |
                       (static_cast<uint32_t>(program[cpu.get_pc() + 4]) << 16) |
                       (static_cast<uint32_t>(program[cpu.get_pc() + 5]) << 24);
                pc_increment = 6;
                DEBUG_INSTRUCTION("LOAD_IMM", cpu.get_pc(), fmt::format("R{}, 0x{:08X} (32-bit fallback)", reg, value), "");
            } else {
                // Minimal fallback: 8-bit
                value = program[cpu.get_pc() + 2];
                pc_increment = 3;
                DEBUG_INSTRUCTION("LOAD_IMM", cpu.get_pc(), fmt::format("R{}, 0x{:02X} (8-bit)", reg, value), "");
            }
        } else {
            // 32-bit mode
            if (cpu.get_pc() + 5 < program.size()) {
                // Read 4-byte little-endian immediate
                value = static_cast<uint32_t>(program[cpu.get_pc() + 2]) |
                       (static_cast<uint32_t>(program[cpu.get_pc() + 3]) << 8) |
                       (static_cast<uint32_t>(program[cpu.get_pc() + 4]) << 16) |
                       (static_cast<uint32_t>(program[cpu.get_pc() + 5]) << 24);
                pc_increment = 6;
                DEBUG_INSTRUCTION("LOAD_IMM", cpu.get_pc(), fmt::format("R{}, 0x{:08X} (32-bit)", reg, value), "");
            } else {
                // Read 1-byte immediate
                value = program[cpu.get_pc() + 2];
                pc_increment = 3;
                DEBUG_INSTRUCTION("LOAD_IMM", cpu.get_pc(), fmt::format("R{}, 0x{:02X} (8-bit)", reg, value), "");
            }
        }
        
        cpu.write_register_mode_aware(reg, value);
        DEBUG_DETAIL(Logging::DebugCategory::CPU_REGISTERS, "Set R{} = 0x{:X}", reg, value);
        cpu.set_pc(cpu.get_pc() + pc_increment);
    } else {
        running = false;
    }
    cpu.print_state("LOAD_IMM");
}

// Implementation for LEA (Load Effective Address) - MODE-AWARE
void handle_lea(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // Mode-aware: opcode + reg + address (4 or 8 bytes)
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + 1 + addr_size;  // opcode + register + address
    
    if (cpu.get_pc() + instr_size - 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc() + 2);
        
        // Validate register bounds - mode-aware
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in LEA instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + instr_size);
            cpu.print_state("LEA");
            return;
        }
        
        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [LEA] PC={} Loading address 0x{:X} into R{}",
            cpu.get_pc(), cpu.get_pc(), addr, reg
        ), DebugLevel::DETAIL);
        
        // Mode-aware register write - load the address itself, not the value at the address
        cpu.write_register_mode_aware(reg, addr);
        
        DebugHandler::instance().report(DebugCategory::CPU_REGISTERS, fmt::format(
            "[PC=0x{:04X}] [LEA] R{} = 0x{:X} (address)",
            cpu.get_pc(), reg, addr
        ), DebugLevel::DETAIL);
        
        cpu.set_pc(cpu.get_pc() + instr_size);
    } else {
        running = false;
    }
    cpu.print_state("LEA");
}

// Implementation from mov.cpp - MODE-AWARE
void handle_mov(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        
        // Validate register bounds
        if (reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} or R{} out of range", reg1, reg2);
            std::string message = "Invalid register access in MOV instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 3);
            cpu.print_state("MOV");
            return;
        }
        
        // Mode-aware register move
        uint64_t value = cpu.read_register_mode_aware(reg2);
        cpu.write_register_mode_aware(reg1, value);
        
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("MOV");
}

// Implementation from mul.cpp - MODE-AWARE
void handle_mul(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        
        // Validate register bounds
        if (reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} or R{} out of range", reg1, reg2);
            std::string message = "Invalid register access in MUL instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 3);
            cpu.print_state("MUL");
            return;
        }
        
        DEBUG_INSTRUCTION("MUL", cpu.get_pc(), fmt::format("R{} *= R{}", reg1, reg2), "");
        
        // Mode-aware arithmetic
        uint64_t value1 = cpu.read_register_mode_aware(reg1);
        uint64_t value2 = cpu.read_register_mode_aware(reg2);
        uint64_t mask = cpu.get_operand_mask();
        
        uint32_t flags = cpu.get_flags();
        uint64_t result;
        
        if (cpu.is_64bit_mode()) {
            // 64-bit multiplication - use __uint128_t for overflow detection if available
            result = value1 * value2;
            // Simplified overflow check for 64-bit
            if (value2 != 0 && result / value2 != value1) {
                flags |= FLAG_CARRY | FLAG_OVERFLOW;
            } else {
                flags &= ~(FLAG_CARRY | FLAG_OVERFLOW);
            }
        } else {
            // 32-bit multiplication
            uint64_t result64 = value1 * value2;
            result = result64 & mask;
            
            if (result64 > 0xFFFFFFFF) {
                flags |= FLAG_CARRY;
            } else {
                flags &= ~FLAG_CARRY;
            }
            
            // Signed overflow check
            int64_t signed_result = static_cast<int64_t>(static_cast<int32_t>(value1)) * 
                                    static_cast<int64_t>(static_cast<int32_t>(value2));
            if (signed_result < INT32_MIN || signed_result > INT32_MAX) {
                flags |= FLAG_OVERFLOW;
            } else {
                flags &= ~FLAG_OVERFLOW;
            }
        }

        // Zero and sign flags
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;
        
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;

        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg1, result);
        
        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [MUL] R{}: 0x{:X} * 0x{:X} = 0x{:X}",
            cpu.get_pc(), reg1, value1, value2, result
        ), DebugLevel::DETAIL);
        
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("MUL");
}

// Implementation from nop.cpp
void handle_nop(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
        "[PC=0x{:04X}] [NOP] PC={}",
        cpu.get_pc(), cpu.get_pc()
    ), DebugLevel::DETAIL);
    cpu.set_pc(cpu.get_pc() + 1);
    cpu.print_state("NOP");
}

// Implementation from not.cpp - MODE-AWARE
void handle_not(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 1 < program.size()) {
        uint8_t reg = program[pc + 1];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in NOT instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 2);
            cpu.print_state("NOT");
            return;
        }

        // Mode-aware register operations
        uint64_t value = cpu.read_register_mode_aware(reg);
        uint64_t result = ~value;
        uint64_t mask = cpu.get_operand_mask();
        result &= mask;
        
        // Update flags (NOT doesn't affect carry/overflow)
        uint32_t flags = cpu.get_flags();
        
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;
        
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;
        
        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg, result);

        cpu.set_pc(pc + 2);
    } else {
        running = false;
    }

    cpu.print_state("NOT");
}

// Implementation from or.cpp - MODE-AWARE
void handle_or(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];

        // Validate register bounds
        if (reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} or R{} out of range", reg1, reg2);
            std::string message = "Invalid register access in OR instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("OR");
            return;
        }

        // Mode-aware register operations
        uint64_t value1 = cpu.read_register_mode_aware(reg1);
        uint64_t value2 = cpu.read_register_mode_aware(reg2);
        uint64_t result = value1 | value2;
        
        // Update flags
        uint32_t flags = cpu.get_flags();
        flags &= ~(FLAG_CARRY | FLAG_OVERFLOW);  // OR clears these
        
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;
        
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;
        
        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg1, result);

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OR");
}

// Implementation from outb.cpp
// Implementation from outb.cpp - MODE-AWARE
void handle_outb(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in OUTB instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("OUTB");
            return;
        }

        uint64_t value = cpu.read_register_mode_aware(reg);
        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [OUTB] PC={} port {} <- R{}=0x{:X}",
            pc, pc, port, reg, value
        ), DebugLevel::DETAIL);

        cpu.write_port(port, static_cast<uint8_t>(value & 0xFF));

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OUTB");
}

// Implementation from out.cpp - MODE-AWARE
void handle_out(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in OUT instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("OUT");
            return;
        }

        uint64_t value = cpu.read_register_mode_aware(reg);
        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [OUT] PC={} port {} <- R{}=0x{:X}",
            pc, pc, port, reg, value
        ), DebugLevel::DETAIL);

        cpu.write_port(port, static_cast<uint8_t>(value & 0xFF));

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OUT");
}

// Implementation from outl.cpp - MODE-AWARE
void handle_outl(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in OUTL instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("OUTL");
            return;
        }

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [OUTL] PC={} port {} <- R{} (dword)",
            pc, pc, port, reg
        ), DebugLevel::DETAIL);

        // Mode-aware: read full register value and output as dword
        uint64_t value = cpu.read_register_mode_aware(reg);
        cpu.write_port_dword(port, static_cast<uint32_t>(value & 0xFFFFFFFF));

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OUTL");
}

// Implementation from outstr.cpp
void handle_outstr(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [OUTSTR] PC={} port {} <- string from memory",
            pc, pc, port
        ), DebugLevel::DETAIL);

        if (reg < cpu.get_registers().size()) {
            // Build string from memory starting at address in register
            uint32_t addr = cpu.get_register_32(static_cast<Register>(reg));
            std::string str;
            for (size_t i = addr; i < cpu.get_memory().size() && cpu.get_memory()[i] != 0; ++i) {
                str += static_cast<char>(cpu.get_memory()[i]);
            }

            DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
                "[PC=0x{:04X}] [OUTSTR] Writing string: '{}'",
                pc, str
            ), DebugLevel::DETAIL);

            cpu.write_port_string(port, str);
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OUTSTR");
}

// Implementation from outw.cpp - MODE-AWARE
void handle_outw(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in OUTW instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("OUTW");
            return;
        }

        DebugHandler::instance().report(DebugCategory::IO_DEVICE, fmt::format(
            "[PC=0x{:04X}] [OUTW] PC={} port {} <- R{} (word)",
            pc, pc, port, reg
        ), DebugLevel::DETAIL);

        // Mode-aware: read full register value and output as word
        uint64_t value = cpu.read_register_mode_aware(reg);
        cpu.write_port_word(port, static_cast<uint16_t>(value & 0xFFFF));

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OUTW");
}

// Implementation from pop_arg.cpp - MODE-AWARE
void handle_pop_arg(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    uint8_t reg = cpu.fetch_operand();

    // Validate register bounds
    if (reg >= cpu.get_registers_64().size()) {
        std::string context = fmt::format("Register R{} out of range", reg);
        std::string message = "Invalid register access in POP_ARG instruction";
        ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
        cpu.print_state("POP_ARG");
        return;
    }

    // Check if we're in a function call context by checking if arg_offset has been set
    // In function context, arg_offset is set to 8 by CALL
    // In standalone context, arg_offset remains 0 (initialized value)
    if (cpu.get_arg_offset() > 0) {
        // Function context: use frame pointer + offset
        uint32_t value = cpu.read_mem32(cpu.get_fp() + cpu.get_arg_offset());
        cpu.write_register_mode_aware(reg, value);

        DebugHandler::instance().report(DebugCategory::CPU_STACK, fmt::format(
            "[PC=0x{:04X}] [POP_ARG] Function context: FP={} arg_offset={} addr={} value={}",
            pc, cpu.get_fp(), cpu.get_arg_offset(),
            (cpu.get_fp() + cpu.get_arg_offset()), value
        ), DebugLevel::DETAIL);

        cpu.set_arg_offset(cpu.get_arg_offset() + 4);
    } else {
        // Standalone context: pop from stack like regular POP
        // Check for stack underflow
        if (cpu.get_sp() >= cpu.get_memory().size()) {
            std::string context = fmt::format("Stack pointer: 0x{:X}, memory size: 0x{:X}", cpu.get_sp(), cpu.get_memory().size());
            std::string message = "Stack underflow in POP_ARG instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_STACK_UNDERFLOW, message, pc, context);
            running = false;
            throw CPUException(message);
        }
        
        uint32_t value = cpu.read_mem32(cpu.get_sp());
        cpu.write_register_mode_aware(reg, value);
        cpu.set_sp(cpu.get_sp() + 4);

        DebugHandler::instance().report(DebugCategory::CPU_STACK, fmt::format(
            "[PC=0x{:04X}] [POP_ARG] Standalone context: popped from SP={} value={}",
            pc, cpu.get_sp() - 4, value
        ), DebugLevel::DETAIL);
    }

    // Don't set PC - fetch_operand already advanced it
    cpu.print_state("POP_ARG");
}

// Implementation from pop.cpp - MODE-AWARE
void handle_pop(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        
        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in POP instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 2);
            cpu.print_state("POP");
            return;
        }
        
        // Check for stack underflow (SP at or beyond initial position)
        // Stack starts at memory.size() - 4, so if SP >= memory.size(), stack is empty
        if (cpu.get_sp() >= cpu.get_memory().size()) {
            std::string context = fmt::format("Stack pointer: 0x{:X}, memory size: 0x{:X}", cpu.get_sp(), cpu.get_memory().size());
            std::string message = "Stack underflow: not enough data to POP";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_STACK_UNDERFLOW, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        uint32_t value = cpu.read_mem32(cpu.get_sp());
        cpu.write_register_mode_aware(reg, value);
        DebugHandler::instance().report(DebugCategory::CPU_STACK, fmt::format(
            "[PC=0x{:04X}] [POP] PC={} Popping to R{}={}",
            cpu.get_pc(), cpu.get_pc(), static_cast<int>(reg), value
        ), DebugLevel::DETAIL);
        cpu.set_sp(cpu.get_sp() + 4);
        // Sync SP to legacy R4 for compatibility
        cpu.get_registers()[4] = cpu.get_sp();
        cpu.set_pc(cpu.get_pc() + 2);
    } else {
        running = false;
    }
    cpu.print_state("POP");
}

// Implementation from pop_flag.cpp
void handle_pop_flag(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    // Check for stack underflow
    if (cpu.get_sp() >= cpu.get_memory().size()) {
        std::string context = fmt::format("Stack pointer: 0x{:X}, memory size: 0x{:X}", cpu.get_sp(), cpu.get_memory().size());
        std::string message = "Stack underflow: cannot pop flags";
        ErrorHandler::instance().report_runtime(ErrorCode::CPU_STACK_UNDERFLOW, message, cpu.get_pc(), context);
        running = false;
        throw CPUException(message);
    }
    
    DebugHandler::instance().report(DebugCategory::CPU_STACK, fmt::format(
        "[PC=0x{:04X}] [POPF] PC={} Popping FLAGS={:08X}",
        cpu.get_pc(), cpu.get_pc(), cpu.get_flags()
    ), DebugLevel::DETAIL);
    cpu.set_flags(cpu.read_mem32(cpu.get_sp()));
    cpu.set_sp(cpu.get_sp() + 4);
    cpu.set_pc(cpu.get_pc() + 1);  // POP_FLAG is a single-byte instruction
    cpu.print_state("POPF");
}

// Implementation from push_arg.cpp - MODE-AWARE
void handle_push_arg(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    uint8_t reg = cpu.fetch_operand();

    // Validate register bounds
    if (reg >= cpu.get_registers_64().size()) {
        std::string context = fmt::format("Register R{} out of range", reg);
        std::string message = "Invalid register access in PUSH_ARG instruction";
        ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
        cpu.print_state("PUSH_ARG");
        return;
    }

    uint64_t value = cpu.read_register_mode_aware(reg);
    DebugHandler::instance().report(DebugCategory::CPU_STACK, fmt::format(
        "[PC=0x{:04X}] [PUSH_ARG] SP={} Pushing R{}=0x{:X}",
        pc, cpu.get_sp(), static_cast<int>(reg), value
    ), DebugLevel::DETAIL);

    uint32_t sp = cpu.get_sp() - 4;
    cpu.set_sp(sp);
    // Sync SP to legacy R4 for compatibility
    cpu.get_registers()[4] = sp;
    cpu.write_mem32(sp, static_cast<uint32_t>(value & 0xFFFFFFFF));

    // Don't set PC - fetch_operand already advanced it
    cpu.print_state("PUSH_ARG");
}

// Implementation from push.cpp - MODE-AWARE
void handle_push(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        
        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in PUSH instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 2);
            cpu.print_state("PUSH");
            return;
        }
        
        uint64_t value = cpu.read_register_mode_aware(reg);
        DebugHandler::instance().report(DebugCategory::CPU_STACK, fmt::format(
            "[PC=0x{:04X}] [PUSH] PC={} Pushing R{}=0x{:X}",
            cpu.get_pc(), cpu.get_pc(), static_cast<int>(reg), value
        ), DebugLevel::DETAIL);
        
        // Check for stack overflow (SP going below reasonable minimum)
        if (cpu.get_sp() < 8) {
            std::string context = fmt::format("Stack pointer: 0x{:X}, minimum safe SP: 0x0008", cpu.get_sp());
            std::string message = "Stack overflow during PUSH: insufficient space";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_STACK_OVERFLOW, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        cpu.set_sp(cpu.get_sp() - 4);
        // Sync only SP to legacy R4 (don't overwrite other legacy registers)
        cpu.get_registers()[4] = cpu.get_sp();
        cpu.write_mem32(cpu.get_sp(), static_cast<uint32_t>(value & 0xFFFFFFFF));
        cpu.set_pc(cpu.get_pc() + 2);
    } else {
        running = false;
    }
    cpu.print_state("PUSH");
}

// Implementation from push_flag.cpp
void handle_push_flag(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    DebugHandler::instance().report(DebugCategory::CPU_STACK, fmt::format(
        "[PC=0x{:04X}] [PUSHF] PC={} Pushing FLAGS={:08X}",
        cpu.get_pc(), cpu.get_pc(), cpu.get_flags()
    ), DebugLevel::DETAIL);
    cpu.set_sp(cpu.get_sp() - 4);
    cpu.write_mem32(cpu.get_sp(), cpu.get_flags());
    cpu.set_pc(cpu.get_pc() + 1);  // PUSH_FLAG is a single-byte instruction
    cpu.print_state("PUSHF");
}

// Implementation from ret.cpp
void handle_ret(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    uint32_t sp = cpu.get_sp();

    DebugHandler::instance().report(DebugCategory::CPU_STACK, fmt::format(
        "[PC=0x{:04X}] [RET] SP={} Restoring FP and popping return address",
        pc, sp
    ), DebugLevel::DETAIL);

    // Check if RET is being called without a matching CALL
    // SP should be well below memory size if there's a valid call frame
    if (sp + 8 > cpu.get_memory().size()) {
        std::string context = fmt::format("Stack pointer: 0x{:X}, memory size: 0x{:X}", sp, cpu.get_memory().size());
        std::string message = "Invalid RET: stack underflow or no matching CALL";
        ErrorHandler::instance().report_runtime(ErrorCode::CPU_STACK_UNDERFLOW, message, pc, context);
        running = false;
        throw CPUException(message);
    }

    // Stack layout from CALL:
    // SP: return address
    // SP+4: old frame pointer
    uint32_t ret_addr = cpu.read_mem32(sp);      // return address at sp
    uint32_t old_fp = cpu.read_mem32(sp + 4);    // old frame pointer at sp+4

    // Unwind stack (pop 8 bytes: return address + old FP)
    sp += 8;
    cpu.set_sp(sp);
    cpu.get_registers()[4] = sp;
    cpu.set_fp(old_fp);
    
    // Decrement call depth
    cpu.decrement_call_depth();

    cpu.print_stack_frame("RET");
    cpu.set_pc(ret_addr);

    // Reset offset at each return
    cpu.set_arg_offset(0);

    cpu.print_state("RET");
}

// Implementation from shl.cpp - MODE-AWARE
void handle_shl(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t shift_amount = program[pc + 2];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in SHL instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("SHL");
            return;
        }

        // Mode-aware shift
        uint64_t value = cpu.read_register_mode_aware(reg);
        uint64_t result;
        uint32_t flags = cpu.get_flags();
        
        if (cpu.is_64bit_mode()) {
            // Limit shift to 63 bits
            shift_amount &= 0x3F;
            // Set carry flag to last bit shifted out (if shifting > 0)
            if (shift_amount > 0) {
                bool last_bit_out = (value >> (64 - shift_amount)) & 1;
                if (last_bit_out) flags |= FLAG_CARRY;
                else flags &= ~FLAG_CARRY;
            }
            result = value << shift_amount;
        } else {
            // Limit shift to 31 bits
            shift_amount &= 0x1F;
            // Set carry flag to last bit shifted out (if shifting > 0)
            if (shift_amount > 0) {
                bool last_bit_out = (value >> (32 - shift_amount)) & 1;
                if (last_bit_out) flags |= FLAG_CARRY;
                else flags &= ~FLAG_CARRY;
            }
            result = (static_cast<uint32_t>(value) << shift_amount);
        }
        
        // Update flags
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;
        
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;
        
        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg, result);

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("SHL");
}

// Implementation from shr.cpp - MODE-AWARE
void handle_shr(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t shift_amount = program[pc + 2];

        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in SHR instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("SHR");
            return;
        }

        // Mode-aware shift
        uint64_t value = cpu.read_register_mode_aware(reg);
        uint64_t result;
        uint32_t flags = cpu.get_flags();
        
        if (cpu.is_64bit_mode()) {
            // Limit shift to 63 bits
            shift_amount &= 0x3F;
            // Set carry flag to last bit shifted out (if shifting > 0)
            if (shift_amount > 0) {
                bool last_bit_out = (value >> (shift_amount - 1)) & 1;
                if (last_bit_out) flags |= FLAG_CARRY;
                else flags &= ~FLAG_CARRY;
            }
            result = value >> shift_amount;
        } else {
            // Limit shift to 31 bits
            shift_amount &= 0x1F;
            // Set carry flag to last bit shifted out (if shifting > 0)
            if (shift_amount > 0) {
                bool last_bit_out = (value >> (shift_amount - 1)) & 1;
                if (last_bit_out) flags |= FLAG_CARRY;
                else flags &= ~FLAG_CARRY;
            }
            result = static_cast<uint32_t>(value) >> shift_amount;
        }
        
        // Update flags
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;
        
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;
        
        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg, result);

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("SHR");
}

// Implementation from store.cpp
void handle_store(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // STORE reg, addr (mode-aware address size)
    // Format: OPCODE(1) REG(1) ADDR(4 or 8 bytes depending on mode)
    // 32-bit mode: 6 bytes total, 64-bit mode: 10 bytes total
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 2 + addr_size;  // opcode + reg + address
    
    if (cpu.get_pc() + instr_size - 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        // Read address based on current CPU mode
        uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc() + 2);
        
        // Check bounds - mode-aware register count
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range (0-{})", reg, cpu.get_registers_64().size() - 1);
            std::string message = "Invalid register in STORE instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        if (addr >= cpu.get_memory().size()) {
            std::string context = fmt::format("Attempted write at 0x{:X}, memory range: 0x0000-0x{:X}", addr, cpu.get_memory().size() - 1);
            std::string message = "Memory write out of bounds in STORE instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_MEMORY_OUT_OF_BOUNDS, message, cpu.get_pc(), context);
            running = false;
            throw CPUException(message);
        }
        
        // Mode-aware register read
        uint64_t value = cpu.read_register_mode_aware(reg);
        cpu.get_memory()[addr] = static_cast<uint8_t>(value & 0xFF);
        cpu.set_pc(cpu.get_pc() + instr_size);
    } else {
        running = false;
    }
    cpu.print_state("STORE");
}

// Implementation from sub.cpp - MODE-AWARE
void handle_sub(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        
        // Validate register bounds
        if (reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} or R{} out of range", reg1, reg2);
            std::string message = "Invalid register access in SUB instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + 3);
            cpu.print_state("SUB");
            return;
        }

        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [SUB] R{} -= R{}",
            cpu.get_pc(), reg1, reg2
        ), DebugLevel::DETAIL);

        // Mode-aware arithmetic
        uint64_t value1 = cpu.read_register_mode_aware(reg1);
        uint64_t value2 = cpu.read_register_mode_aware(reg2);
        uint64_t mask = cpu.get_operand_mask();
        uint64_t result = (value1 - value2) & mask;

        // Update flags based on mode
        uint32_t flags = cpu.get_flags();
        
        // Carry flag - set if underflow (borrow needed)
        if (value1 < value2) flags |= FLAG_CARRY;
        else flags &= ~FLAG_CARRY;

        // Overflow flag for signed arithmetic
        bool sign1 = cpu.is_64bit_mode() ? ((value1 >> 63) & 1) : ((value1 >> 31) & 1);
        bool sign2 = cpu.is_64bit_mode() ? ((value2 >> 63) & 1) : ((value2 >> 31) & 1);
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        
        // Overflow: pos - neg = neg, or neg - pos = pos
        if ((sign1 != sign2) && (sign1 != signr)) {
            flags |= FLAG_OVERFLOW;
        } else {
            flags &= ~FLAG_OVERFLOW;
        }

        // Zero flag
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;

        // Sign flag
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;

        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg1, result);
        
        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [SUB] R{}: 0x{:X} - 0x{:X} = 0x{:X}",
            cpu.get_pc(), reg1, value1, value2, result
        ), DebugLevel::DETAIL);
        
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("SUB");
}

// Implementation for SWAP - MODE-AWARE
void handle_swap(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // Mode-aware: opcode + reg + address (4 or 8 bytes)
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + 1 + addr_size;  // opcode + register + address
    
    if (cpu.get_pc() + instr_size - 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        uint64_t addr = cpu.read_address_from_program(program, cpu.get_pc() + 2);
        
        // Validate register bounds
        if (reg >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} out of range", reg);
            std::string message = "Invalid register access in SWAP instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + instr_size);
            cpu.print_state("SWAP");
            return;
        }
        
        // Validate memory bounds
        if (addr >= cpu.get_memory().size()) {
            std::string context = fmt::format("Attempted access at 0x{:X}, memory range: 0x0000-0x{:X}", addr, cpu.get_memory().size() - 1);
            std::string message = "Memory access out of bounds in SWAP instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_MEMORY_OUT_OF_BOUNDS, message, cpu.get_pc(), context);
            cpu.set_pc(cpu.get_pc() + instr_size);
            cpu.print_state("SWAP");
            return;
        }
        
        DebugHandler::instance().report(DebugCategory::MEM_ACCESS, fmt::format(
            "[PC=0x{:04X}] [SWAP] PC={} Swapping R{} with memory[0x{:X}]",
            cpu.get_pc(), cpu.get_pc(), reg, addr
        ), DebugLevel::DETAIL);

        // Mode-aware swap
        uint64_t reg_value = cpu.read_register_mode_aware(reg);
        uint8_t mem_value = cpu.get_memory()[addr];
        cpu.write_register_mode_aware(reg, mem_value);
        cpu.get_memory()[addr] = static_cast<uint8_t>(reg_value & 0xFF);
        
        DebugHandler::instance().report(DebugCategory::MEM_ACCESS, fmt::format(
            "[PC=0x{:04X}] [SWAP] R{} = 0x{:X}, memory[0x{:X}] = 0x{:X}",
            cpu.get_pc(), reg, mem_value, addr, static_cast<uint8_t>(reg_value & 0xFF)
        ), DebugLevel::DETAIL);
        
        cpu.set_pc(cpu.get_pc() + instr_size);
    } else {
        running = false;
    }
    cpu.print_state("SWAP");
}

// Implementation from xor.cpp - MODE-AWARE
void handle_xor(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];

        // Validate register bounds
        if (reg1 >= cpu.get_registers_64().size() || reg2 >= cpu.get_registers_64().size()) {
            std::string context = fmt::format("Register R{} or R{} out of range", reg1, reg2);
            std::string message = "Invalid register access in XOR instruction";
            ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_REGISTER, message, pc, context);
            cpu.set_pc(pc + 3);
            cpu.print_state("XOR");
            return;
        }

        // Mode-aware register operations
        uint64_t value1 = cpu.read_register_mode_aware(reg1);
        uint64_t value2 = cpu.read_register_mode_aware(reg2);
        uint64_t result = value1 ^ value2;
        
        // Update flags
        uint32_t flags = cpu.get_flags();
        flags &= ~(FLAG_CARRY | FLAG_OVERFLOW);  // XOR clears these
        
        if (result == 0) flags |= FLAG_ZERO;
        else flags &= ~FLAG_ZERO;
        
        bool signr = cpu.is_64bit_mode() ? ((result >> 63) & 1) : ((result >> 31) & 1);
        if (signr) flags |= FLAG_SIGN;
        else flags &= ~FLAG_SIGN;
        
        cpu.set_flags(flags);
        cpu.write_register_mode_aware(reg1, result);

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("XOR");
}

// Implementation for JC (Jump if Carry)
void handle_jc(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address

    if (pc + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, pc + 1);
        DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
            "[PC=0x{:04X}] [JC] PC={} Checking carry flag",
            pc, pc
        ), DebugLevel::DETAIL);

        if (cpu.get_flags() & FLAG_CARRY) {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JC] Carry flag set, jumping to address {}",
                pc, addr
            ), DebugLevel::DETAIL);
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JC] Carry flag clear, continuing",
                pc, pc
            ), DebugLevel::DETAIL);
            cpu.set_pc(pc + instr_size);
        }
    } else {
        running = false;
    }

    cpu.print_state("JC");
}

// Implementation for JNC (Jump if No Carry)
void handle_jnc(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address

    if (pc + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, pc + 1);
        DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
            "[PC=0x{:04X}] [JNC] PC={} Checking carry flag",
            pc, pc
        ), DebugLevel::DETAIL);

        if (!(cpu.get_flags() & FLAG_CARRY)) {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JNC] Carry flag clear, jumping to address {}",
                pc, addr
            ), DebugLevel::DETAIL);
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JNC] Carry flag set, continuing",
                pc, pc
            ), DebugLevel::DETAIL);
            cpu.set_pc(pc + instr_size);
        }
    } else {
        running = false;
    }

    cpu.print_state("JNC");
}

// Implementation from jo.cpp
void handle_jo(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address

    if (pc + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, pc + 1);
        DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
            "[PC=0x{:04X}] [JO] PC={} Checking overflow flag",
            pc, pc
        ), DebugLevel::DETAIL);

        if (cpu.get_flags() & FLAG_OVERFLOW) {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JO] Overflow flag set, jumping to address {}",
                pc, addr
            ), DebugLevel::DETAIL);
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JO] Overflow flag clear, continuing",
                pc, pc
            ), DebugLevel::DETAIL);
            cpu.set_pc(pc + instr_size);
        }
    } else {
        running = false;
    }

    cpu.print_state("JO");
}

// Implementation from jno.cpp
void handle_jno(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address

    if (pc + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, pc + 1);
        DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
            "[PC=0x{:04X}] [JNO] PC={} Checking overflow flag",
            pc, pc
        ), DebugLevel::DETAIL);

        if (!(cpu.get_flags() & FLAG_OVERFLOW)) {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JNO] Overflow flag clear, jumping to address {}",
                pc, addr
            ), DebugLevel::DETAIL);
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JNO] Overflow flag set, continuing",
                pc, pc
            ), DebugLevel::DETAIL);
            cpu.set_pc(pc + instr_size);
        }
    } else {
        running = false;
    }

    cpu.print_state("JNO");
}

// Implementation for JG (Jump if Greater)
void handle_jg(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address

    if (pc + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, pc + 1);
        DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
            "[PC=0x{:04X}] [JG] PC={} Checking flags for greater",
            pc, pc
        ), DebugLevel::DETAIL);

        // JG: Jump if greater (not sign and not zero)
        if (!(cpu.get_flags() & FLAG_SIGN) && !(cpu.get_flags() & FLAG_ZERO)) {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JG] Greater condition met, jumping to address {}",
                pc, addr
            ), DebugLevel::DETAIL);
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JG] Greater condition not met, continuing",
                pc, pc
            ), DebugLevel::DETAIL);
            cpu.set_pc(pc + instr_size);
        }
    } else {
        running = false;
    }

    cpu.print_state("JG");
}

// Implementation for JL (Jump if Less)
void handle_jl(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address

    if (pc + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, pc + 1);
        DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
            "[PC=0x{:04X}] [JL] PC={} Checking flags for less",
            pc, pc
        ), DebugLevel::DETAIL);

        // JL: Jump if less (sign set and not zero)
        if ((cpu.get_flags() & FLAG_SIGN) && !(cpu.get_flags() & FLAG_ZERO)) {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JL] Less condition met, jumping to address {}",
                pc, addr
            ), DebugLevel::DETAIL);
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JL] Less condition not met, continuing",
                pc, pc
            ), DebugLevel::DETAIL);
            cpu.set_pc(pc + instr_size);
        }
    } else {
        running = false;
    }

    cpu.print_state("JL");
}

// Implementation for JGE (Jump if Greater or Equal)
void handle_jge(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address

    if (pc + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, pc + 1);
        DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
            "[PC=0x{:04X}] [JGE] PC={} Checking flags for greater or equal",
            pc, pc
        ), DebugLevel::DETAIL);

        // JGE: Jump if greater or equal (not sign or zero)
        if (!(cpu.get_flags() & FLAG_SIGN) || (cpu.get_flags() & FLAG_ZERO)) {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JGE] Greater or equal condition met, jumping to address {}",
                pc, addr
            ), DebugLevel::DETAIL);
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JGE] Greater or equal condition not met, continuing",
                pc, pc
            ), DebugLevel::DETAIL);
            cpu.set_pc(pc + instr_size);
        }
    } else {
        running = false;
    }

    cpu.print_state("JGE");
}

// Implementation for JLE (Jump if Less or Equal)
void handle_jle(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    size_t addr_size = cpu.get_address_size();
    size_t instr_size = 1 + addr_size;  // opcode + address

    if (pc + addr_size < program.size()) {
        uint64_t addr = cpu.read_address_from_program(program, pc + 1);
        DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
            "[PC=0x{:04X}] [JLE] PC={} Checking flags for less or equal",
            pc, pc
        ), DebugLevel::DETAIL);

        // JLE: Jump if less or equal (sign set or zero)
        if ((cpu.get_flags() & FLAG_SIGN) || (cpu.get_flags() & FLAG_ZERO)) {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JLE] Less or equal condition met, jumping to address {}",
                pc, addr
            ), DebugLevel::DETAIL);
            cpu.set_pc(static_cast<uint32_t>(addr));
        } else {
            DebugHandler::instance().report(DebugCategory::CPU_JUMP, fmt::format(
                "[PC=0x{:04X}] [JLE] Less or equal condition not met, continuing",
                pc, pc
            ), DebugLevel::DETAIL);
            cpu.set_pc(pc + instr_size);
        }
    } else {
        running = false;
    }

    cpu.print_state("JLE");
}

// Forward declarations for 64-bit operations implemented later in this file
void handle_inc64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
void handle_dec64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// Dispatcher function (copied from opcode_dispatcher.cpp)
void dispatch_opcode(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    DebugHandler::instance().report(DebugCategory::CPU_DISPATCHER, fmt::format(
        "[DISPATCH_OPCODE] ENTRY: PC=0x{:04X}",
        cpu.get_pc()
    ), DebugLevel::TRACE);
    
    if (cpu.get_pc() >= program.size()) {
        DebugHandler::instance().report(DebugCategory::CPU_DISPATCHER, 
            "[DISPATCH_OPCODE] PC out of bounds, stopping",
            DebugLevel::IMPORTANT);
        running = false;
        return;
    }

    Opcode opcode = static_cast<Opcode>(program[cpu.get_pc()]);
    DebugHandler::instance().report(DebugCategory::CPU_DISPATCHER, fmt::format(
        "[DISPATCH_OPCODE] Processing opcode 0x{:02X}",
        static_cast<uint8_t>(opcode)
    ), DebugLevel::TRACE);

    switch (opcode) {
        case Opcode::NOP:
            handle_nop(cpu, program, running);
            break;
        case Opcode::LOAD_IMM:
            handle_load_imm(cpu, program, running);
            break;
        case Opcode::ADD:
            handle_add(cpu, program, running);
            break;
        case Opcode::SUB:
            handle_sub(cpu, program, running);
            break;
        case Opcode::MUL:
            handle_mul(cpu, program, running);
            break;
        case Opcode::DIV:
            handle_div(cpu, program, running);
            break;
        case Opcode::MOD:
            handle_mod(cpu, program, running);
            break;
        case Opcode::INC:
            handle_inc(cpu, program, running);
            break;
        case Opcode::DEC:
            handle_dec(cpu, program, running);
            break;
        case Opcode::MOV:
            handle_mov(cpu, program, running);
            break;
        case Opcode::JMP:
            handle_jmp(cpu, program, running);
            break;
        case Opcode::JZ:
            handle_jz(cpu, program, running);
            break;
        case Opcode::JNZ:
            handle_jnz(cpu, program, running);
            break;
        case Opcode::JS:
            handle_js(cpu, program, running);
            break;
        case Opcode::JNS:
            handle_jns(cpu, program, running);
            break;
        case Opcode::JC:
            handle_jc(cpu, program, running);
            break;
        case Opcode::JNC:
            handle_jnc(cpu, program, running);
            break;
        case Opcode::JO:
            handle_jo(cpu, program, running);
            break;
        case Opcode::JNO:
            handle_jno(cpu, program, running);
            break;
        case Opcode::JG:
            handle_jg(cpu, program, running);
            break;
        case Opcode::JL:
            handle_jl(cpu, program, running);
            break;
        case Opcode::JGE:
            handle_jge(cpu, program, running);
            break;
        case Opcode::JLE:
            handle_jle(cpu, program, running);
            break;
        case Opcode::LOAD:
            handle_load(cpu, program, running);
            break;
        case Opcode::LOADR:
            handle_loadr(cpu, program, running);
            break;
        case Opcode::LEA:
            handle_lea(cpu, program, running);
            break;
        case Opcode::STORE:
            handle_store(cpu, program, running);
            break;
        case Opcode::SWAP:
            handle_swap(cpu, program, running);
            break;
        case Opcode::PUSH:
            handle_push(cpu, program, running);
            break;
        case Opcode::POP:
            handle_pop(cpu, program, running);
            break;
        case Opcode::CMP:
            handle_cmp(cpu, program, running);
            break;
        case Opcode::PUSH_FLAG:
            handle_push_flag(cpu, program, running);
            break;
        case Opcode::POP_FLAG:
            handle_pop_flag(cpu, program, running);
            break;
        case Opcode::HALT:
            handle_halt(cpu, program, running);
            break;
        case Opcode::AND:
            handle_and(cpu, program, running);
            break;
        case Opcode::OR:
            handle_or(cpu, program, running);
            break;
        case Opcode::XOR:
            handle_xor(cpu, program, running);
            break;
        case Opcode::NOT:
            handle_not(cpu, program, running);
            break;
        case Opcode::SHL:
            handle_shl(cpu, program, running);
            break;
        case Opcode::SHR:
            handle_shr(cpu, program, running);
            break;
        case Opcode::CALL:
            handle_call(cpu, program, running);
            break;
        case Opcode::RET:
            handle_ret(cpu, program, running);
            break;
        case Opcode::PUSH_ARG:
            handle_push_arg(cpu, program, running);
            break;
        case Opcode::POP_ARG:
            handle_pop_arg(cpu, program, running);
            break;
        case Opcode::IN:
            handle_in(cpu, program, running);
            break;
        case Opcode::OUT:
            handle_out(cpu, program, running);
            break;
        case Opcode::INB:
            handle_inb(cpu, program, running);
            break;
        case Opcode::OUTB:
            handle_outb(cpu, program, running);
            break;
        case Opcode::INW:
            handle_inw(cpu, program, running);
            break;
        case Opcode::OUTW:
            handle_outw(cpu, program, running);
            break;
        case Opcode::INL:
            handle_inl(cpu, program, running);
            break;
        case Opcode::OUTL:
            handle_outl(cpu, program, running);
            break;
        case Opcode::INSTR:
            handle_instr(cpu, program, running);
            break;
        case Opcode::OUTSTR:
            handle_outstr(cpu, program, running);
            break;
        case Opcode::DB:
            handle_db(cpu, program, running);
            break;

        // Extended 64-bit register operations
        case Opcode::ADD64:
            handle_add64(cpu, program, running);
            break;
        case Opcode::SUB64:
            handle_sub64(cpu, program, running);
            break;
        case Opcode::MOV64:
            handle_mov64(cpu, program, running);
            break;
        case Opcode::INC64:
            handle_inc64(cpu, program, running);
            break;
        case Opcode::DEC64:
            handle_dec64(cpu, program, running);
            break;
        case Opcode::LOAD_IMM64:
            handle_load_imm64(cpu, program, running);
            break;
        case Opcode::MUL64:
            handle_mul64(cpu, program, running);
            break;
        case Opcode::DIV64:
            handle_div64(cpu, program, running);
            break;
        case Opcode::AND64:
            handle_and64(cpu, program, running);
            break;
        case Opcode::CMP64:
            handle_cmp64(cpu, program, running);
            break;
        case Opcode::MOVEX:
            handle_movex(cpu, program, running);
            break;
        case Opcode::ADDEX:
            handle_addex(cpu, program, running);
            break;
        case Opcode::SUBEX:
            handle_subex(cpu, program, running);
            break;
        case Opcode::LOADEX:
            handle_loadex(cpu, program, running);
            break;
        case Opcode::STOREX:
            handle_storex(cpu, program, running);
            break;

        // CPU Mode Control Operations
        case Opcode::MODE32:
            handle_mode32(cpu, program, running);
            break;
        case Opcode::MODE64:
            handle_mode64(cpu, program, running);
            break;
        case Opcode::MODECMP:
            handle_modecmp(cpu, program, running);
            break;

        // FPU Operations
        case Opcode::FLD:
            handle_FLD(cpu, program, running);
            break;
        case Opcode::FST:
            handle_FST(cpu, program, running);
            break;
        case Opcode::FSTP:
            handle_FSTP(cpu, program, running);
            break;
        case Opcode::FILD:
            handle_FILD(cpu, program, running);
            break;
        case Opcode::FIST:
            handle_FIST(cpu, program, running);
            break;
        case Opcode::FISTP:
            handle_FISTP(cpu, program, running);
            break;
        case Opcode::FADD:
            handle_FADD(cpu, program, running);
            break;
        case Opcode::FSUB:
            handle_FSUB(cpu, program, running);
            break;
        case Opcode::FMUL:
            handle_FMUL(cpu, program, running);
            break;
        case Opcode::FDIV:
            handle_FDIV(cpu, program, running);
            break;
        case Opcode::FINIT:
            handle_FINIT(cpu, program, running);
            break;
        case Opcode::FABS:
            handle_FABS(cpu, program, running);
            break;
        case Opcode::FCHS:
            handle_FCHS(cpu, program, running);
            break;
        case Opcode::FSQRT:
            handle_FSQRT(cpu, program, running);
            break;
        case Opcode::FSIN:
            handle_FSIN(cpu, program, running);
            break;
        case Opcode::FCOS:
            handle_FCOS(cpu, program, running);
            break;
        case Opcode::FTAN:
            handle_FTAN(cpu, program, running);
            break;
        case Opcode::FCOMPP:
            handle_FCOMPP(cpu, program, running);
            break;
        case Opcode::FUCOMPP:
            handle_FUCOMPP(cpu, program, running);
            break;
        case Opcode::FCLEX:
            handle_FCLEX(cpu, program, running);
            break;
        case Opcode::FSTCW:
            handle_FSTCW(cpu, program, running);
            break;
        case Opcode::FLDCW:
            handle_FLDCW(cpu, program, running);
            break;
        case Opcode::FSTSW:
            handle_FSTSW(cpu, program, running);
            break;

        // SIMD Vector Operations
        case Opcode::VADD:
            handle_VADD(cpu, program, running);
            break;
        case Opcode::VMUL:
            handle_VMUL(cpu, program, running);
            break;
        case Opcode::VDOT:
            handle_VDOT(cpu, program, running);
            break;
        case Opcode::VMAX:
            handle_VMAX(cpu, program, running);
            break;
        case Opcode::VBROADCAST:
            handle_VBROADCAST(cpu, program, running);
            break;
        case Opcode::VCMPGT:
            handle_VCMPGT(cpu, program, running);
            break;
        case Opcode::PACKB:
            handle_PACKB(cpu, program, running);
            break;
        case Opcode::UNPACKB:
            handle_UNPACKB(cpu, program, running);
            break;

        default:
            {
                std::string opcode_hex = fmt::format("{:02X}", static_cast<int>(opcode));
                std::string context = fmt::format("Invalid opcode: 0x{}", opcode_hex);
                std::string message = "Invalid or unrecognized opcode encountered";
                ErrorHandler::instance().report_runtime(ErrorCode::CPU_INVALID_OPCODE, message, cpu.get_pc(), context);
                running = false;
                throw CPUException("Invalid opcode: 0x" + opcode_hex);
            }
            break;
    }
    DebugHandler::instance().report(DebugCategory::CPU_DISPATCHER, fmt::format(
        "[DISPATCH_OPCODE] EXIT: PC=0x{:04X}",
        cpu.get_pc()
    ), DebugLevel::TRACE);
}

// 64-bit and Extended Register Operations Implementation
// These are placeholder implementations for compatibility

// Implementation for ADD64 opcode - 64-bit addition
void handle_add64(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];

        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [ADD64] Adding 64-bit registers R{} and R{}",
            pc, reg1, reg2
        ), DebugLevel::DETAIL);

        // Use 64-bit register access for extended registers
        uint64_t value1 = cpu.get_register_64(static_cast<Register>(reg1));
        uint64_t value2 = cpu.get_register_64(static_cast<Register>(reg2));
        uint64_t result = value1 + value2;

        // Update flags for 64-bit arithmetic
        uint32_t flags = cpu.get_flags();

        // Check for carry
        if (result < value1) { // Overflow in unsigned addition
            flags |= FLAG_CARRY;
        } else {
            flags &= ~FLAG_CARRY;
        }

        // Check for signed overflow
        bool sign1 = (value1 >> 63) & 1;
        bool sign2 = (value2 >> 63) & 1;
        bool signr = (result >> 63) & 1;
        
        if ((sign1 == sign2) && (sign1 != signr)) {
            flags |= FLAG_OVERFLOW;
        } else {
            flags &= ~FLAG_OVERFLOW;
        }

        // Check for zero
        if (result == 0) {
            flags |= FLAG_ZERO;
        } else {
            flags &= ~FLAG_ZERO;
        }

        // Check for negative (using sign bit)
        if (signr) {
            flags |= FLAG_SIGN;
        } else {
            flags &= ~FLAG_SIGN;
        }

        // Update flags and register
        cpu.set_flags(flags);
        cpu.set_register_64(static_cast<Register>(reg1), result);

        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [ADD64] Result: R{} = 0x{:016X} + 0x{:016X} = 0x{:016X}",
            pc, reg1, value1, value2, result
        ), DebugLevel::DETAIL);

        cpu.set_pc(pc + 3); // Advance past opcode and two register operands
    } else {
        running = false;
    }

    cpu.print_state("ADD64");
}

// Implementation for SUB64 opcode - 64-bit subtraction
void handle_sub64(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];

        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [SUB64] Subtracting 64-bit registers R{} - R{}",
            pc, reg1, reg2
        ), DebugLevel::DETAIL);

        // Use 64-bit register access for extended registers
        uint64_t value1 = cpu.get_register_64(static_cast<Register>(reg1));
        uint64_t value2 = cpu.get_register_64(static_cast<Register>(reg2));
        uint64_t result = value1 - value2;

        // Update flags for 64-bit arithmetic
        uint32_t flags = cpu.get_flags();

        // Check for borrow (unsigned underflow)
        if (value1 < value2) {
            flags |= FLAG_CARRY;  // Carry flag indicates borrow
        } else {
            flags &= ~FLAG_CARRY;
        }

        // Check for signed overflow
        bool sign1 = (value1 >> 63) & 1;
        bool sign2 = (value2 >> 63) & 1;
        bool signr = (result >> 63) & 1;
        
        // Overflow occurs when subtracting numbers of opposite signs produces wrong sign
        if ((sign1 != sign2) && (sign1 != signr)) {
            flags |= FLAG_OVERFLOW;
        } else {
            flags &= ~FLAG_OVERFLOW;
        }

        // Check for zero
        if (result == 0) {
            flags |= FLAG_ZERO;
        } else {
            flags &= ~FLAG_ZERO;
        }

        // Check for negative (using sign bit)
        if (signr) {
            flags |= FLAG_SIGN;
        } else {
            flags &= ~FLAG_SIGN;
        }

        // Update flags and register
        cpu.set_flags(flags);
        cpu.set_register_64(static_cast<Register>(reg1), result);

        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [SUB64] Result: R{} = 0x{:016X} - 0x{:016X} = 0x{:016X}",
            pc, reg1, value1, value2, result
        ), DebugLevel::DETAIL);

        cpu.set_pc(pc + 3); // Advance past opcode and two register operands
    } else {
        running = false;
    }

    cpu.print_state("SUB64");
}

// Implementation for MOV64 opcode - 64-bit move
void handle_mov64(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t dest_reg = program[pc + 1];
        uint8_t src_reg = program[pc + 2];

        DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [MOV64] Moving 64-bit value from R{} to R{}", pc, src_reg, dest_reg);

        // Use 64-bit register access for extended registers
        uint64_t value = cpu.get_register_64(static_cast<Register>(src_reg));
        cpu.set_register_64(static_cast<Register>(dest_reg), value);

        DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [MOV64] Result: R{} = 0x{:016X}", pc, dest_reg, value);

        cpu.set_pc(pc + 3); // Advance past opcode and two register operands
    } else {
        running = false;
    }

    cpu.print_state("MOV64");
}

// Implementation for INC64 opcode - 64-bit increment
void handle_inc64(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 1 < program.size()) {
        uint8_t reg = program[pc + 1];

        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [INC64] Incrementing 64-bit register R{}",
            pc, reg
        ), DebugLevel::DETAIL);

        // Use 64-bit register access
        uint64_t value = cpu.get_register_64(static_cast<Register>(reg));
        uint64_t result = value + 1;

        // Update flags for 64-bit arithmetic
        uint32_t flags = cpu.get_flags();

        // Check for overflow (signed)
        // Incrementing 0x7FFFFFFFFFFFFFFF to 0x8000000000000000 causes overflow
        if (value == 0x7FFFFFFFFFFFFFFF) {
            flags |= FLAG_OVERFLOW;
        } else {
            flags &= ~FLAG_OVERFLOW;
        }

        // Check for zero
        if (result == 0) {
            flags |= FLAG_ZERO;
        } else {
            flags &= ~FLAG_ZERO;
        }

        // Check for negative (using sign bit)
        if ((result >> 63) & 1) {
            flags |= FLAG_SIGN;
        } else {
            flags &= ~FLAG_SIGN;
        }

        // Note: INC typically does not affect the carry flag on x86, 
        // but we'll follow the pattern of other arithmetic ops if needed.
        // For now, let's preserve the carry flag (standard x86 behavior).

        // Update flags and register
        cpu.set_flags(flags);
        cpu.set_register_64(static_cast<Register>(reg), result);

        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [INC64] Result: R{} = 0x{:016X} + 1 = 0x{:016X}",
            pc, reg, value, result
        ), DebugLevel::DETAIL);

        cpu.set_pc(pc + 2); // Advance past opcode and register operand
    } else {
        running = false;
    }

    cpu.print_state("INC64");
}

// Implementation for DEC64 opcode - 64-bit decrement
void handle_dec64(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 1 < program.size()) {
        uint8_t reg = program[pc + 1];

        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [DEC64] Decrementing 64-bit register R{}",
            pc, reg
        ), DebugLevel::DETAIL);

        // Use 64-bit register access
        uint64_t value = cpu.get_register_64(static_cast<Register>(reg));
        uint64_t result = value - 1;

        // Update flags for 64-bit arithmetic
        uint32_t flags = cpu.get_flags();

        // Check for overflow (signed)
        // Decrementing 0x8000000000000000 to 0x7FFFFFFFFFFFFFFF causes overflow
        if (value == 0x8000000000000000) {
            flags |= FLAG_OVERFLOW;
        } else {
            flags &= ~FLAG_OVERFLOW;
        }

        // Check for zero
        if (result == 0) {
            flags |= FLAG_ZERO;
        } else {
            flags &= ~FLAG_ZERO;
        }

        // Check for negative (using sign bit)
        if ((result >> 63) & 1) {
            flags |= FLAG_SIGN;
        } else {
            flags &= ~FLAG_SIGN;
        }

        // Note: DEC typically does not affect the carry flag on x86.

        // Update flags and register
        cpu.set_flags(flags);
        cpu.set_register_64(static_cast<Register>(reg), result);

        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [DEC64] Result: R{} = 0x{:016X} - 1 = 0x{:016X}",
            pc, reg, value, result
        ), DebugLevel::DETAIL);

        cpu.set_pc(pc + 2); // Advance past opcode and register operand
    } else {
        running = false;
    }

    cpu.print_state("DEC64");
}

// Implementation for LOAD_IMM64 opcode - 64-bit immediate load
void handle_load_imm64(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "[PC=0x{:04X}] [LOAD_IMM64] 64-bit immediate load operation", cpu.get_pc());
    uint32_t pc = cpu.get_pc();
    if (pc + 9 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint64_t imm = 0;
        // Little-endian: lowest byte first
        for (int i = 0; i < 8; ++i) {
            imm |= static_cast<uint64_t>(program[pc + 2 + i]) << (8 * i);
        }
        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [LOAD_IMM64] reg={} imm=0x{:016X}",
            pc, reg, imm
        ), DebugLevel::DETAIL);
        if (reg < TOTAL_REGISTERS) {
            cpu.set_register_64(static_cast<Register>(reg), imm);
            DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
                "[PC=0x{:04X}] [LOAD_IMM64] Set R{} = 0x{:016X}",
                pc, reg, imm
            ), DebugLevel::DETAIL);
        }
        cpu.set_pc(pc + 10);
    } else {
        running = false;
    }
    cpu.print_state("LOAD_IMM64");
}

// Mode Control Operations Implementation

// Implementation for MODE32 opcode - Switch to 32-bit mode
void handle_mode32(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
        "[PC=0x{:04X}] [MODE32] Switching CPU to 32-bit mode",
        cpu.get_pc()
    ), DebugLevel::INFO);
    cpu.set_cpu_mode(CPUMode::MODE_32BIT);
    cpu.set_pc(cpu.get_pc() + 1);
}

// Implementation for MODE64 opcode - Switch to 64-bit mode
void handle_mode64(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
        "[PC=0x{:04X}] [MODE64] Switching CPU to 64-bit mode",
        cpu.get_pc()
    ), DebugLevel::INFO);
    cpu.set_cpu_mode(CPUMode::MODE_64BIT);
    cpu.set_pc(cpu.get_pc() + 1);
}

// Implementation for MODECMP opcode - Mode-aware comparison
void handle_modecmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];

        // Get current CPU mode
        CPUMode mode = cpu.get_cpu_mode();

        DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
            "[PC=0x{:04X}] [MODECMP] Comparing R{} and R{} in {} mode",
            pc, reg1, reg2, (mode == CPUMode::MODE_64BIT ? "64-bit" : "32-bit")
        ), DebugLevel::DETAIL);

        uint32_t flags = cpu.get_flags();

        if (mode == CPUMode::MODE_64BIT) {
            // 64-bit comparison
            uint64_t value1 = cpu.get_register_64(static_cast<Register>(reg1));
            uint64_t value2 = cpu.get_register_64(static_cast<Register>(reg2));
            
            // Perform subtraction for comparison (don't store result)
            uint64_t result = value1 - value2;

            // Zero flag
            if (result == 0) {
                flags |= FLAG_ZERO;
            } else {
                flags &= ~FLAG_ZERO;
            }

            // Sign flag (bit 63 for 64-bit)
            if ((result >> 63) & 1) {
                flags |= FLAG_SIGN;
            } else {
                flags &= ~FLAG_SIGN;
            }

            // Carry flag (unsigned comparison: value1 < value2)
            if (value1 < value2) {
                flags |= FLAG_CARRY;
            } else {
                flags &= ~FLAG_CARRY;
            }

            DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
                "[PC=0x{:04X}] [MODECMP] 64-bit compare: 0x{:016X} vs 0x{:016X}",
                pc, value1, value2
            ), DebugLevel::DETAIL);
        } else {
            // 32-bit comparison (MODE_32BIT or default)
            // Use legacy registers for R0-R7 in 32-bit mode
            uint32_t value1 = (reg1 < cpu.get_registers().size()) ? 
                cpu.get_registers()[reg1] : static_cast<uint32_t>(cpu.get_register(static_cast<Register>(reg1)));
            uint32_t value2 = (reg2 < cpu.get_registers().size()) ? 
                cpu.get_registers()[reg2] : static_cast<uint32_t>(cpu.get_register(static_cast<Register>(reg2)));
            
            // Perform subtraction for comparison (don't store result)
            uint32_t result = value1 - value2;

            // Zero flag
            if (result == 0) {
                flags |= FLAG_ZERO;
            } else {
                flags &= ~FLAG_ZERO;
            }

            // Sign flag (bit 31 for 32-bit)
            if ((result >> 31) & 1) {
                flags |= FLAG_SIGN;
            } else {
                flags &= ~FLAG_SIGN;
            }

            // Carry flag (unsigned comparison: value1 < value2)
            if (value1 < value2) {
                flags |= FLAG_CARRY;
            } else {
                flags &= ~FLAG_CARRY;
            }

            DebugHandler::instance().report(DebugCategory::CPU_EXECUTION, fmt::format(
                "[PC=0x{:04X}] [MODECMP] 32-bit compare: 0x{:08X} vs 0x{:08X}",
                pc, value1, value2
            ), DebugLevel::DETAIL);
        }

        cpu.set_flags(flags);
        cpu.set_pc(pc + 3); // Advance past opcode and two register operands
    } else {
        running = false;
    }

    cpu.print_state("MODECMP");
}
