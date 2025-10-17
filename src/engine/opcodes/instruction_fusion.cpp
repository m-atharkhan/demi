#include "instruction_fusion.hpp"
#include "../cpu_flags.hpp"
#include "../../assembler/opcodes.hpp"
#include <fmt/core.h>

namespace InstructionFusion {

// Global fusion engine instance
FusionEngine g_fusion_engine;

// Helper: Get legacy register value (registers are stored as uint32_t, but only lower 8 bits used in legacy mode)
inline uint8_t get_legacy_register_value(const CPU& cpu, uint8_t reg_idx) {
    return static_cast<uint8_t>(cpu.get_registers()[reg_idx] & 0xFF);
}

inline void set_legacy_register_value(CPU& cpu, uint8_t reg_idx, uint8_t value) {
    cpu.get_registers()[reg_idx] = value;
}

// Helper: Check if register index is valid (0-7 for legacy)
inline bool is_valid_legacy_register(uint8_t reg_idx) {
    return reg_idx <= 7;
}

// Helper: Update flags after arithmetic operation
inline void update_add_flags(CPU& cpu, uint8_t before, uint8_t operand, uint16_t sum) {
    uint8_t result = static_cast<uint8_t>(sum & 0xFF);
    uint32_t flags = cpu.get_flags();
    
    // Carry flag
    if (sum > 0xFF) {
        flags |= FLAG_CARRY;
    } else {
        flags &= ~FLAG_CARRY;
    }
    
    // Overflow flag
    bool sign_before = (before & 0x80) != 0;
    bool sign_operand = (operand & 0x80) != 0;
    bool sign_result = (result & 0x80) != 0;
    if ((sign_before == sign_operand) && (sign_before != sign_result)) {
        flags |= FLAG_OVERFLOW;
    } else {
        flags &= ~FLAG_OVERFLOW;
    }
    
    // Zero flag
    if (result == 0) {
        flags |= FLAG_ZERO;
    } else {
        flags &= ~FLAG_ZERO;
    }
    
    // Sign flag
    if (sign_result) {
        flags |= FLAG_SIGN;
    } else {
        flags &= ~FLAG_SIGN;
    }
    
    cpu.set_flags(flags);
}

inline void update_sub_flags(CPU& cpu, uint8_t before, uint8_t operand) {
    int16_t signed_result = static_cast<int16_t>(before) - static_cast<int16_t>(operand);
    uint8_t result = static_cast<uint8_t>(signed_result & 0xFF);
    uint32_t flags = cpu.get_flags();
    
    // Zero flag
    if (result == 0) {
        flags |= FLAG_ZERO;
    } else {
        flags &= ~FLAG_ZERO;
    }
    
    // Sign flag
    if (result & 0x80) {
        flags |= FLAG_SIGN;
    } else {
        flags &= ~FLAG_SIGN;
    }
    
    // Carry flag (borrow)
    if (before < operand) {
        flags |= FLAG_CARRY;
    } else {
        flags &= ~FLAG_CARRY;
    }
    
    // Overflow flag
    bool sign_before = (before & 0x80) != 0;
    bool sign_operand = (operand & 0x80) != 0;
    bool sign_result = (result & 0x80) != 0;
    if ((sign_before != sign_operand) && (sign_before != sign_result)) {
        flags |= FLAG_OVERFLOW;
    } else {
        flags &= ~FLAG_OVERFLOW;
    }
    
    cpu.set_flags(flags);
}

bool InstructionFusion::try_instruction_fusion(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() >= program.size()) {
        return false;
    }

    // Get current opcode
    Opcode current_op = static_cast<Opcode>(program[cpu.get_pc()]);
    
    // Log fusion attempt in debug mode
    if (Config::debug) {
        Logger::instance().debug() << fmt::format("[FUSION] Checking fusion for opcode 0x{:02X} at PC=0x{:04X}", 
                                                   static_cast<uint8_t>(current_op), cpu.get_pc()) << std::endl;
    }

    // Try fusion patterns in order of likelihood
    switch (current_op) {

bool FusionEngine::fuse_load_imm_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, bool& /*running*/) {
    // Pattern: LOAD_IMM Rx, imm + ADD/SUB Ry, Rx
    // Fuse into: ADD/SUB Ry, immediate value
    
    uint32_t pc = cpu.get_pc();
    
    // LOAD_IMM format varies - for legacy it's 3 bytes: opcode + reg + imm8
    if (!can_lookahead(cpu, program, 3 + 3)) return false;
    
    uint8_t load_dst = program[pc + 1];
    uint8_t imm_value = program[pc + 2];  // 8-bit immediate
    
    // Validate legacy register
    if (!is_valid_legacy_register(load_dst)) return false;
    
    Opcode next_op = peek_opcode(program, pc, 3);
    
    // Check if next operation uses the loaded immediate
    if (pc + 3 + 3 > program.size()) return false;
    
    uint8_t arith_dst = program[pc + 4];
    uint8_t arith_src = program[pc + 5];
    
    // Validate registers
    if (!is_valid_legacy_register(arith_dst) || !is_valid_legacy_register(arith_src)) return false;
    
    // Verify the arithmetic op uses the register we just loaded as source
    if (arith_src != load_dst) return false;
    
    FusionPattern pattern = FusionPattern::NONE;
    switch (next_op) {
        case Opcode::ADD:
            pattern = FusionPattern::LOAD_IMM_ADD;
            break;
        case Opcode::SUB:
            pattern = FusionPattern::LOAD_IMM_SUB;
            break;
        default:
            return false;
    }
    
    // Execute fused operation
    uint8_t dst_val = get_legacy_register_value(cpu, arith_dst);
    uint16_t sum = 0;
    uint8_t result = 0;
    
    switch (pattern) {
        case FusionPattern::LOAD_IMM_ADD:
            sum = static_cast<uint16_t>(dst_val) + static_cast<uint16_t>(imm_value);
            result = static_cast<uint8_t>(sum & 0xFF);
            set_legacy_register_value(cpu, arith_dst, result);
            update_add_flags(cpu, dst_val, imm_value, sum);
            break;
        case FusionPattern::LOAD_IMM_SUB:
            result = dst_val - imm_value;
            set_legacy_register_value(cpu, arith_dst, result);
            update_sub_flags(cpu, dst_val, imm_value);
            break;
        default:
            return false;
    }
    
    // Skip both instructions
    cpu.set_pc(pc + 3 + 3);
    
    stats.record_fusion(pattern);
    
    Logger::instance().debug() << fmt::format(
        "[FUSION] LOAD_IMM+{} at PC=0x{:04X}: R{}={} + imm={} -> {}",
        (pattern == FusionPattern::LOAD_IMM_ADD) ? "ADD" : "SUB",
        pc, arith_dst, dst_val, imm_value, result
    ) << std::endl;
    
    return true;
}

bool FusionEngine::fuse_cmp_branch(CPU& cpu, const std::vector<uint8_t>& program, bool& /*running*/) {
    // Pattern: CMP Rx, Ry + conditional branch
    // Fuse into: compare-and-branch operation
    
    uint32_t pc = cpu.get_pc();
    
    // CMP is 3 bytes, branch is 2 bytes
    if (!can_lookahead(cpu, program, 3 + 2)) return false;
    
    uint8_t cmp_r1 = program[pc + 1];
    uint8_t cmp_r2 = program[pc + 2];
    
    // Validate legacy registers
    if (!is_valid_legacy_register(cmp_r1) || !is_valid_legacy_register(cmp_r2)) return false;
    
    Opcode branch_op = peek_opcode(program, pc, 3);
    
    FusionPattern pattern = FusionPattern::NONE;
    switch (branch_op) {
        case Opcode::JZ:  pattern = FusionPattern::CMP_JZ; break;
        case Opcode::JNZ: pattern = FusionPattern::CMP_JNZ; break;
        default:
            return false;  // Only support JZ/JNZ for now (most common)
    }
    
    // Read jump address (1 byte for short jumps)
    uint8_t jump_addr = program[pc + 4];
    
    // Execute fused compare-and-branch
    uint8_t val1 = get_legacy_register_value(cpu, cmp_r1);
    uint8_t val2 = get_legacy_register_value(cpu, cmp_r2);
    
    // Update flags
    update_sub_flags(cpu, val1, val2);
    
    // Check branch condition
    bool take_branch = false;
    uint32_t flags = cpu.get_flags();
    
    switch (pattern) {
        case FusionPattern::CMP_JZ:
            take_branch = (flags & FLAG_ZERO) != 0;
            break;
        case FusionPattern::CMP_JNZ:
            take_branch = (flags & FLAG_ZERO) == 0;
            break;
        default:
            return false;
    }
    
    if (take_branch) {
        cpu.set_pc(jump_addr);
    } else {
        cpu.set_pc(pc + 3 + 2); // Skip both instructions
    }
    
    stats.record_fusion(pattern);
    
    Logger::instance().debug() << fmt::format(
        "[FUSION] CMP+{} at PC=0x{:04X}: R{}({}) vs R{}({}) -> {}",
        (pattern == FusionPattern::CMP_JZ) ? "JZ" : "JNZ",
        pc, cmp_r1, val1, cmp_r2, val2,
        take_branch ? "JUMP" : "CONT"
    ) << std::endl;
    
    return true;
}

bool FusionEngine::fuse_arithmetic_store(CPU& cpu, const std::vector<uint8_t>& program, bool& /*running*/) {
    // Pattern: arithmetic Rx, Ry + STORE [addr], Rx
    // TODO: Implement for memory-intensive operations
    // Complexity: Need to handle memory bounds checking
    
    uint32_t pc = cpu.get_pc();
    (void)pc;  // Suppress unused warning
    (void)program;
    return false;  // Not implemented yet
}

bool FusionEngine::fuse_inc_dec_cmp(CPU& cpu, const std::vector<uint8_t>& program, bool& /*running*/) {
    // Pattern: INC/DEC Rx + CMP Rx, Ry
    // Common in loop counters
    
    uint32_t pc = cpu.get_pc();
    
    // INC/DEC is 2 bytes, CMP is 3 bytes
    if (!can_lookahead(cpu, program, 2 + 3)) return false;
    
    Opcode first_op = static_cast<Opcode>(program[pc]);
    uint8_t inc_dec_reg = program[pc + 1];
    
    // Validate register
    if (!is_valid_legacy_register(inc_dec_reg)) return false;
    
    Opcode next_op = peek_opcode(program, pc, 2);
    if (next_op != Opcode::CMP) return false;
    
    uint8_t cmp_r1 = program[pc + 3];
    uint8_t cmp_r2 = program[pc + 4];
    
    // Validate registers
    if (!is_valid_legacy_register(cmp_r1) || !is_valid_legacy_register(cmp_r2)) return false;
    
    // Verify CMP uses the modified register
    if (cmp_r1 != inc_dec_reg) return false;  // Only support INC/DEC R0 + CMP R0, R1 pattern
    
    FusionPattern pattern = (first_op == Opcode::INC) ? 
        FusionPattern::INC_CMP : FusionPattern::DEC_CMP;
    
    // Execute fused operation
    uint8_t val = get_legacy_register_value(cpu, inc_dec_reg);
    uint8_t result = (first_op == Opcode::INC) ? (val + 1) : (val - 1);
    
    set_legacy_register_value(cpu, inc_dec_reg, result);
    
    // Update flags for inc/dec (simple - just set zero/sign)
    uint32_t flags = cpu.get_flags();
    if (result == 0) flags |= FLAG_ZERO; else flags &= ~FLAG_ZERO;
    if (result & 0x80) flags |= FLAG_SIGN; else flags &= ~FLAG_SIGN;
    
    // Now perform compare
    uint8_t cmp_val2 = get_legacy_register_value(cpu, cmp_r2);
    update_sub_flags(cpu, result, cmp_val2);  // Compare new value with second operand
    
    // Skip both instructions
    cpu.set_pc(pc + 2 + 3);
    
    stats.record_fusion(pattern);
    
    Logger::instance().debug() << fmt::format(
        "[FUSION] {}{} +CMP at PC=0x{:04X}: R{}:{}->{}  cmp R{}({})",
        (first_op == Opcode::INC) ? "INC" : "DEC",
        inc_dec_reg, pc, inc_dec_reg, val, result, cmp_r2, cmp_val2
    ) << std::endl;
    
    return true;
}

bool FusionEngine::fuse_load_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, bool& /*running*/) {
    // Pattern: LOAD Rx, [addr] + arithmetic Rx, Ry
    // TODO: Implement for memory-heavy operations
    
    uint32_t pc = cpu.get_pc();
    (void)pc;  // Suppress unused warning
    (void)program;
    return false;  // Not implemented yet
}

bool FusionEngine::fuse_mov_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, bool& /*running*/) {
    // Pattern: MOV Rx, Ry + arithmetic Rx, Rz
    // TODO: Implement for register-heavy operations
    
    uint32_t pc = cpu.get_pc();
    (void)pc;  // Suppress unused warning
    (void)program;
    return false;  // Not implemented yet
}

} // namespace InstructionFusion
