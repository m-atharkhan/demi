#include "instruction_fusion.hpp"
#include "../cpu.hpp"
#include "../cpu_flags.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/logger.hpp"
#include "../../config.hpp"
#include <fmt/core.h>

namespace InstructionFusion {

// Helper to check if register is valid (R0-R7)
static inline bool is_valid_legacy_register(uint8_t reg) {
    return reg < 8;
}

// FusionEngine::try_fuse_and_execute
bool FusionEngine::try_fuse_and_execute(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (!enabled) {
        if (Config::debug && stats.total_attempts == 0) {
            Logger::instance().debug() << "[FUSION] Engine disabled!" << std::endl;
        }
        return false;
    }
    if (cpu.get_pc() >= program.size()) return false;
    
    stats.total_attempts++;
    
    // Get current opcode
    Opcode current_op = static_cast<Opcode>(program[cpu.get_pc()]);
    
    // Try fusion patterns
    switch (current_op) {
        case Opcode::CMP:
            if (Config::debug) {
                Logger::instance().debug() << fmt::format("[FUSION] Attempting CMP fusion at PC=0x{:04X}", cpu.get_pc()) << std::endl;
            }
            return fuse_cmp_branch(cpu, program, running);
            
        case Opcode::LOAD_IMM:
            return fuse_load_imm_arithmetic(cpu, program, running);
            
        case Opcode::ADD:
        case Opcode::SUB:
        case Opcode::MUL:
        case Opcode::DIV:
            return fuse_arithmetic_store(cpu, program, running);
            
        default:
            return false;
    }
}

// FusionEngine::fuse_cmp_branch - CMP + conditional jump
bool FusionEngine::fuse_cmp_branch(CPU& cpu, const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    
    // CMP format: opcode (1) + reg1 (1) + reg2 (1) = 3 bytes
    // Jump format: opcode (1) + addr (1) = 2 bytes
    if (!can_lookahead(cpu, program, 5)) return false;
    
    // Read CMP operands
    uint8_t reg1 = program[pc + 1];
    uint8_t reg2 = program[pc + 2];
    
    if (!is_valid_legacy_register(reg1) || !is_valid_legacy_register(reg2)) {
        return false;
    }
    
    // Check next opcode
    Opcode branch_op = peek_opcode(program, pc, 3);
    
    // Only fuse with conditional jumps
    bool is_conditional_jump = false;
    FusionPattern pattern = FusionPattern::NONE;
    
    switch (branch_op) {
        case Opcode::JZ:
            is_conditional_jump = true;
            pattern = FusionPattern::CMP_JZ;
            break;
        case Opcode::JNZ:
            is_conditional_jump = true;
            pattern = FusionPattern::CMP_JNZ;
            break;
        case Opcode::JG:
            is_conditional_jump = true;
            pattern = FusionPattern::CMP_JG;
            break;
        case Opcode::JL:
            is_conditional_jump = true;
            pattern = FusionPattern::CMP_JL;
            break;
        case Opcode::JGE:
            is_conditional_jump = true;
            pattern = FusionPattern::CMP_JGE;
            break;
        case Opcode::JLE:
            is_conditional_jump = true;
            pattern = FusionPattern::CMP_JLE;
            break;
        default:
            return false;
    }
    
    if (!is_conditional_jump) return false;
    
    // Get register values
    uint32_t val1 = cpu.get_registers()[reg1];
    uint32_t val2 = cpu.get_registers()[reg2];
    
    // Perform comparison
    uint32_t result = val1 - val2;
    bool zero = (result == 0);
    bool sign = (static_cast<int32_t>(result) < 0);
    
    // Get jump target address
    uint8_t target_addr = program[pc + 4];
    
    // Determine if branch should be taken
    bool take_branch = false;
    switch (branch_op) {
        case Opcode::JZ:   take_branch = zero; break;
        case Opcode::JNZ:  take_branch = !zero; break;
        case Opcode::JG:   take_branch = !zero && !sign; break;
        case Opcode::JL:   take_branch = sign; break;
        case Opcode::JGE:  take_branch = !sign || zero; break;
        case Opcode::JLE:  take_branch = sign || zero; break;
        default: return false;
    }
    
    // Debug output
    if (Config::debug) {
        const char* branch_name = 
            branch_op == Opcode::JZ ? "JZ" :
            branch_op == Opcode::JNZ ? "JNZ" :
            branch_op == Opcode::JG ? "JG" :
            branch_op == Opcode::JL ? "JL" :
            branch_op == Opcode::JGE ? "JGE" : "JLE";
            
        std::string decision = take_branch ? 
            fmt::format("JUMP to 0x{:04X}", target_addr) : "CONT";
            
        Logger::instance().debug() << fmt::format(
            "[FUSION] CMP+{} at PC=0x{:04X}: R{}({}) vs R{}({}) -> {}",
            branch_name, pc, reg1, val1, reg2, val2, decision
        ) << std::endl;
    }
    
    // Execute fused operation
    if (take_branch) {
        cpu.set_pc(target_addr);
    } else {
        cpu.set_pc(pc + 5);  // Skip both CMP and jump
    }
    
    // Set CPU flags based on comparison result
    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN);  // Clear zero and sign flags
    if (zero) flags |= FLAG_ZERO;
    if (sign) flags |= FLAG_SIGN;
    cpu.set_flags(flags);
    
    // Record fusion success
    stats.record_fusion(pattern);
    
    return true;
}

// FusionEngine::fuse_load_imm_arithmetic - LOAD_IMM + arithmetic (STUBBED)
bool FusionEngine::fuse_load_imm_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    (void)cpu;
    (void)program;
    (void)running;
    return false;
}

// FusionEngine::fuse_arithmetic_store - arithmetic + STORE (STUBBED)
bool FusionEngine::fuse_arithmetic_store(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    (void)cpu;
    (void)program;
    (void)running;
    return false;
}

// FusionEngine::fuse_inc_dec_cmp - INC/DEC + CMP (STUBBED)
bool FusionEngine::fuse_inc_dec_cmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    (void)cpu;
    (void)program;
    (void)running;
    return false;
}

// FusionEngine::fuse_load_arithmetic - LOAD + arithmetic (STUBBED)
bool FusionEngine::fuse_load_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    (void)cpu;
    (void)program;
    (void)running;
    return false;
}

// FusionEngine::fuse_mov_arithmetic - MOV + arithmetic (STUBBED)
bool FusionEngine::fuse_mov_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    (void)cpu;
    (void)program;
    (void)running;
    return false;
}

// Global fusion engine instance
FusionEngine g_fusion_engine;

} // namespace InstructionFusion
