#include "instruction_fusion.hpp"
#include "../cpu.hpp"
#include "../cpu_flags.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/debug_handler.hpp"
#include "../../debug/error_handler.hpp"
#include "../../config.hpp"
#include <fmt/core.h>

namespace InstructionFusion {

// Helper to check if register is valid (R0-R7)
static inline bool is_valid_legacy_register(uint8_t reg) {
    return reg < 8;
}

// Debug logging macro for fusion engine - only outputs when test_debug is enabled
#define FUSION_DEBUG(fmt_str, ...) \
    do { if (Config::test_debug) { \
        std::cout << "\033[38;5;208m[FUSION-DBG] \033[0m" \
                  << fmt::format(fmt_str, ##__VA_ARGS__) << std::endl; \
    } } while(0)

// Helper to convert opcode to string for debug output
static const char* opcode_name(Opcode op) {
    switch (op) {
        case Opcode::LOAD_IMM: return "LOAD_IMM";
        case Opcode::ADD:       return "ADD";
        case Opcode::SUB:       return "SUB";
        case Opcode::MUL:       return "MUL";
        case Opcode::CMP:       return "CMP";
        case Opcode::STORE:     return "STORE";
        case Opcode::LOAD:      return "LOAD";
        case Opcode::MOV:       return "MOV";
        case Opcode::INC:       return "INC";
        case Opcode::DEC:       return "DEC";
        case Opcode::PUSH:      return "PUSH";
        case Opcode::POP:       return "POP";
        case Opcode::CALL:      return "CALL";
        case Opcode::RET:       return "RET";
        case Opcode::JZ:        return "JZ";
        case Opcode::JNZ:       return "JNZ";
        case Opcode::JG:        return "JG";
        case Opcode::JL:        return "JL";
        case Opcode::JGE:       return "JGE";
        case Opcode::JLE:       return "JLE";
        case Opcode::HALT:      return "HALT";
        case Opcode::AND:       return "AND";
        case Opcode::OR:        return "OR";
        case Opcode::XOR:       return "XOR";
        default:                return "UNKNOWN";
    }
}

static const char* pattern_name(FusionPattern p) {
    switch (p) {
        case FusionPattern::NONE:            return "NONE";
        case FusionPattern::LOAD_IMM_ADD:    return "LOAD_IMM_ADD";
        case FusionPattern::LOAD_IMM_SUB:    return "LOAD_IMM_SUB";
        case FusionPattern::LOAD_IMM_MUL:    return "LOAD_IMM_MUL";
        case FusionPattern::LOAD_IMM_AND:    return "LOAD_IMM_AND";
        case FusionPattern::LOAD_IMM_OR:     return "LOAD_IMM_OR";
        case FusionPattern::LOAD_IMM_XOR:    return "LOAD_IMM_XOR";
        case FusionPattern::LOAD_IMM_CMP:    return "LOAD_IMM_CMP";
        case FusionPattern::CMP_JZ:          return "CMP_JZ";
        case FusionPattern::CMP_JNZ:         return "CMP_JNZ";
        case FusionPattern::CMP_JG:          return "CMP_JG";
        case FusionPattern::CMP_JL:          return "CMP_JL";
        case FusionPattern::CMP_JGE:         return "CMP_JGE";
        case FusionPattern::CMP_JLE:         return "CMP_JLE";
        case FusionPattern::ADD_STORE:       return "ADD_STORE";
        case FusionPattern::SUB_STORE:       return "SUB_STORE";
        case FusionPattern::INC_CMP:         return "INC_CMP";
        case FusionPattern::DEC_CMP:         return "DEC_CMP";
        case FusionPattern::LOAD_ADD:        return "LOAD_ADD";
        case FusionPattern::LOAD_SUB:        return "LOAD_SUB";
        case FusionPattern::MOV_ADD:         return "MOV_ADD";
        case FusionPattern::MOV_SUB:         return "MOV_SUB";
        case FusionPattern::PUSH_POP:        return "PUSH_POP";
        case FusionPattern::PUSH_PUSH_CALL:  return "PUSH_PUSH_CALL";
        case FusionPattern::LOAD_IMM_STORE:  return "LOAD_IMM_STORE";
    }
    return "UNKNOWN";
}

// FusionEngine::try_fuse_and_execute
bool FusionEngine::try_fuse_and_execute(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (!enabled) {
        FUSION_DEBUG("Engine DISABLED at PC=0x{:04X}", cpu.get_pc());
        return false;
    }
    if (cpu.get_pc() >= program.size()) return false;
    
    stats.total_attempts++;
    
    // Get current opcode
    uint8_t raw_op = program[cpu.get_pc()];
    Opcode current_op = static_cast<Opcode>(raw_op);
    
    FUSION_DEBUG("Attempting fusion at PC=0x{:04X} opcode=0x{:02X} ({}) program_size={}", 
                 cpu.get_pc(), raw_op, opcode_name(current_op), program.size());
    
    bool result = false;
    
    // Try fusion patterns
    switch (current_op) {
        case Opcode::CMP:
            FUSION_DEBUG("  -> Trying CMP+branch fusion", "");
            result = fuse_cmp_branch(cpu, program, running);
            if (!result) FUSION_DEBUG("  -> CMP+branch FAILED", "");
            return result;
            
        case Opcode::LOAD_IMM:
            // Try LOAD_IMM+STORE first, then LOAD_IMM+arithmetic
            FUSION_DEBUG("  -> Trying LOAD_IMM+STORE fusion", "");
            if (fuse_load_imm_store(cpu, program, running)) {
                FUSION_DEBUG("  -> LOAD_IMM+STORE SUCCESS", "");
                return true;
            }
            FUSION_DEBUG("  -> Trying LOAD_IMM+arithmetic fusion", "");
            result = fuse_load_imm_arithmetic(cpu, program, running);
            if (!result) FUSION_DEBUG("  -> LOAD_IMM+arithmetic FAILED", "");
            return result;
            
        case Opcode::ADD:
        case Opcode::SUB:
        case Opcode::MUL:
            // Try arithmetic+STORE first, then if not matched fall through
            FUSION_DEBUG("  -> Trying {}+STORE fusion", opcode_name(current_op));
            if (fuse_arithmetic_store(cpu, program, running)) {
                FUSION_DEBUG("  -> {}+STORE SUCCESS", opcode_name(current_op));
                return true;
            }
            FUSION_DEBUG("  -> {}+STORE FAILED, no fusion", opcode_name(current_op));
            return false;

        case Opcode::INC:
        case Opcode::DEC:
            FUSION_DEBUG("  -> Trying {}+CMP fusion", opcode_name(current_op));
            result = fuse_inc_dec_cmp(cpu, program, running);
            if (!result) FUSION_DEBUG("  -> {}+CMP FAILED", opcode_name(current_op));
            return result;

        case Opcode::LOAD:
            FUSION_DEBUG("  -> Trying LOAD+arithmetic fusion", "");
            result = fuse_load_arithmetic(cpu, program, running);
            if (!result) FUSION_DEBUG("  -> LOAD+arithmetic FAILED", "");
            return result;

        case Opcode::MOV:
            FUSION_DEBUG("  -> Trying MOV+arithmetic fusion", "");
            result = fuse_mov_arithmetic(cpu, program, running);
            if (!result) FUSION_DEBUG("  -> MOV+arithmetic FAILED", "");
            return result;

        case Opcode::PUSH:
            // Try PUSH+POP first, then PUSH+PUSH+CALL
            FUSION_DEBUG("  -> Trying PUSH+POP fusion", "");
            if (fuse_push_pop(cpu, program, running)) {
                FUSION_DEBUG("  -> PUSH+POP SUCCESS", "");
                return true;
            }
            FUSION_DEBUG("  -> Trying PUSH+PUSH+CALL fusion", "");
            result = fuse_push_push_call(cpu, program, running);
            if (!result) FUSION_DEBUG("  -> PUSH+PUSH+CALL FAILED", "");
            return result;
            
        default:
            return false;
    }
}

// FusionEngine::fuse_cmp_branch - CMP + conditional jump
bool FusionEngine::fuse_cmp_branch(CPU& cpu, const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    
    // CMP format: opcode (1) + reg1 (1) + reg2 (1) = 3 bytes
    // Jump format: opcode (1) + addr (4) = 5 bytes
    if (!can_lookahead(cpu, program, 8)) return false;
    
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
    
    // Get jump target address (32-bit little endian)
    uint32_t target_addr = static_cast<uint32_t>(program[pc + 4]) |
                           (static_cast<uint32_t>(program[pc + 5]) << 8) |
                           (static_cast<uint32_t>(program[pc + 6]) << 16) |
                           (static_cast<uint32_t>(program[pc + 7]) << 24);
    
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
            
        DEBUG_INFO(Logging::DebugCategory::PERF_OPTIMIZATION, "[FUSION] CMP+{} at PC=0x{:04X}: R{}({}) vs R{}({}) -> {}", branch_name, pc, reg1, val1, reg2, val2, decision);
    }
    
    // Execute fused operation
    if (take_branch) {
        cpu.set_pc(target_addr);
    } else {
        cpu.set_pc(pc + 8);  // Skip both CMP (3) and jump (5)
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

// FusionEngine::fuse_load_imm_arithmetic - LOAD_IMM + arithmetic
bool FusionEngine::fuse_load_imm_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    // LOAD_IMM: opcode(1) + reg(1) + imm(4) = 6 bytes
    // Arithmetic: opcode(1) + reg1(1) + reg2(1) = 3 bytes
    if (!can_lookahead(cpu, program, 9)) return false;

    uint8_t load_reg = program[pc + 1];
    uint32_t imm = static_cast<uint32_t>(program[pc + 2]) |
                   (static_cast<uint32_t>(program[pc + 3]) << 8) |
                   (static_cast<uint32_t>(program[pc + 4]) << 16) |
                   (static_cast<uint32_t>(program[pc + 5]) << 24);

    if (!cpu.is_valid_register(static_cast<Register>(load_reg))) return false;

    Opcode arith_op = peek_opcode(program, pc, 6);
    uint8_t arith_reg1 = program[pc + 7];
    uint8_t arith_reg2 = program[pc + 8];

    // Fusion requires the arithmetic's dest register matches the LOAD_IMM target
    if (arith_reg1 != load_reg) return false;
    if (!cpu.is_valid_register(static_cast<Register>(arith_reg2))) return false;

    uint64_t val2 = cpu.get_register_mode_aware(static_cast<Register>(arith_reg2));
    uint64_t result = 0;
    FusionPattern pattern = FusionPattern::NONE;
    uint64_t mask = cpu.get_operand_mask();

    switch (arith_op) {
        case Opcode::ADD:
            result = (static_cast<uint64_t>(imm) + val2) & mask;
            pattern = FusionPattern::LOAD_IMM_ADD;
            break;
        case Opcode::SUB:
            result = (static_cast<uint64_t>(imm) - val2) & mask;
            pattern = FusionPattern::LOAD_IMM_SUB;
            break;
        case Opcode::MUL:
            result = (static_cast<uint64_t>(imm) * val2) & mask;
            pattern = FusionPattern::LOAD_IMM_MUL;
            break;
        case Opcode::AND:
            result = (static_cast<uint64_t>(imm) & val2) & mask;
            pattern = FusionPattern::LOAD_IMM_AND;
            break;
        case Opcode::OR:
            result = (static_cast<uint64_t>(imm) | val2) & mask;
            pattern = FusionPattern::LOAD_IMM_OR;
            break;
        case Opcode::XOR:
            result = (static_cast<uint64_t>(imm) ^ val2) & mask;
            pattern = FusionPattern::LOAD_IMM_XOR;
            break;
        case Opcode::CMP: {
            // CMP with immediate - set flags based on imm vs val2
            uint64_t diff = static_cast<uint64_t>(imm) - val2;
            uint64_t masked_diff = diff & mask;
            uint32_t flags = cpu.get_flags();
            flags &= ~(FLAG_ZERO | FLAG_SIGN | FLAG_CARRY | FLAG_OVERFLOW);
            if (masked_diff == 0) flags |= FLAG_ZERO;
            if (cpu.is_32bit_mode()) {
                if (masked_diff & 0x80000000) flags |= FLAG_SIGN;
                if (static_cast<uint32_t>(imm) < static_cast<uint32_t>(val2)) flags |= FLAG_CARRY;
            } else {
                if (masked_diff & 0x8000000000000000ULL) flags |= FLAG_SIGN;
                if (static_cast<uint64_t>(imm) < val2) flags |= FLAG_CARRY;
            }
            cpu.set_flags(flags);
            cpu.set_pc(pc + 9);
            pattern = FusionPattern::LOAD_IMM_CMP;
            stats.record_fusion(pattern);
            if (Config::debug) {
                DEBUG_INFO(Logging::DebugCategory::PERF_OPTIMIZATION, "[FUSION] LOAD_IMM+CMP at PC=0x{:04X}: imm={} vs R{}={}", pc, imm, arith_reg2, val2);
            }
            return true;
        }
        default:
            return false;
    }

    cpu.set_register_mode_aware(static_cast<Register>(load_reg), result);
    cpu.set_pc(pc + 9);

    // Set flags for arithmetic
    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN);
    if ((result & mask) == 0) flags |= FLAG_ZERO;
    if (cpu.is_32bit_mode()) {
        if (result & 0x80000000) flags |= FLAG_SIGN;
    } else {
        if (result & 0x8000000000000000ULL) flags |= FLAG_SIGN;
    }
    cpu.set_flags(flags);

    stats.record_fusion(pattern);

    if (Config::debug) {
        DEBUG_INFO(Logging::DebugCategory::PERF_OPTIMIZATION, "[FUSION] LOAD_IMM+op at PC=0x{:04X}: R{} = {} op {} = {}", pc, load_reg, imm, static_cast<int>(arith_op), val2, result);
    }
    return true;
}

// FusionEngine::fuse_arithmetic_store - arithmetic + STORE
bool FusionEngine::fuse_arithmetic_store(CPU& cpu, const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    // Arithmetic: opcode(1) + reg1(1) + reg2(1) = 3 bytes
    // STORE: opcode(1) + reg(1) + addr(4 for 32-bit) = 6 bytes
    size_t addr_size = cpu.get_address_size();
    size_t total_size = 3 + 1 + 1 + addr_size;
    if (!can_lookahead(cpu, program, total_size)) return false;

    Opcode arith_op = peek_opcode(program, pc, 0);
    uint8_t arith_reg1 = program[pc + 1];
    uint8_t arith_reg2 = program[pc + 2];

    if (!cpu.is_valid_register(static_cast<Register>(arith_reg1)) ||
        !cpu.is_valid_register(static_cast<Register>(arith_reg2))) return false;

    // Check next opcode is STORE with same source register
    Opcode next_op = peek_opcode(program, pc, 3);
    if (next_op != Opcode::STORE) return false;

    uint8_t store_reg = program[pc + 4];
    if (store_reg != arith_reg1) return false;

    uint64_t addr = cpu.read_address_from_program(program, pc + 5);
    if (addr >= cpu.get_memory().size()) return false;

    // Execute arithmetic
    uint64_t val1 = cpu.get_register_mode_aware(static_cast<Register>(arith_reg1));
    uint64_t val2 = cpu.get_register_mode_aware(static_cast<Register>(arith_reg2));
    uint64_t mask = cpu.get_operand_mask();
    uint64_t result = 0;
    FusionPattern pattern = FusionPattern::NONE;

    switch (arith_op) {
        case Opcode::ADD:
            result = (val1 + val2) & mask;
            pattern = FusionPattern::ADD_STORE;
            break;
        case Opcode::SUB:
            result = (val1 - val2) & mask;
            pattern = FusionPattern::SUB_STORE;
            break;
        case Opcode::MUL:
            result = (val1 * val2) & mask;
            pattern = FusionPattern::ADD_STORE; // Use existing ADD_STORE for MUL too
            break;
        default:
            return false;
    }

    // Write result to register AND memory
    cpu.set_register_mode_aware(static_cast<Register>(arith_reg1), result);
    cpu.get_memory()[addr] = static_cast<uint8_t>(result & 0xFF);

    // Set flags
    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN);
    if ((result & mask) == 0) flags |= FLAG_ZERO;
    if (cpu.is_32bit_mode()) {
        if (result & 0x80000000) flags |= FLAG_SIGN;
    } else {
        if (result & 0x8000000000000000ULL) flags |= FLAG_SIGN;
    }
    cpu.set_flags(flags);

    cpu.set_pc(pc + 3 + 2 + addr_size);
    stats.record_fusion(pattern);

    if (Config::debug) {
        DEBUG_INFO(Logging::DebugCategory::PERF_OPTIMIZATION, "[FUSION] {}+STORE at PC=0x{:04X}: result={} -> [0x{:X}]", pc, static_cast<int>(arith_op), result, addr);
    }
    return true;
}

// FusionEngine::fuse_inc_dec_cmp - INC/DEC + CMP
bool FusionEngine::fuse_inc_dec_cmp(CPU& cpu, const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    // INC/DEC: opcode(1) + reg(1) = 2 bytes
    // CMP: opcode(1) + reg1(1) + reg2(1) = 3 bytes
    if (!can_lookahead(cpu, program, 5)) {
        FUSION_DEBUG("    INC/CMP: cannot_lookahead(5), pc={}, prog_size={}", pc, program.size());
        return false;
    }

    Opcode incdec_op = peek_opcode(program, pc, 0);
    uint8_t incdec_reg = program[pc + 1];
    FUSION_DEBUG("    INC/CMP: incdec_op={} incdec_reg=R{} raw_byte=0x{:02X}", 
                 opcode_name(incdec_op), incdec_reg, incdec_reg);
    
    if (!cpu.is_valid_register(static_cast<Register>(incdec_reg))) {
        FUSION_DEBUG("    INC/CMP: invalid register R{}", incdec_reg);
        return false;
    }

    Opcode next_op = peek_opcode(program, pc, 2);
    FUSION_DEBUG("    INC/CMP: next_op={} (at pc+2={})", opcode_name(next_op), pc + 2);
    
    if (next_op != Opcode::CMP) {
        FUSION_DEBUG("    INC/CMP: next op is {} not CMP, FAIL", opcode_name(next_op));
        return false;
    }

    uint8_t cmp_reg1 = program[pc + 3];
    uint8_t cmp_reg2 = program[pc + 4];
    FUSION_DEBUG("    INC/CMP: cmp_reg1=R{} cmp_reg2=R{}", cmp_reg1, cmp_reg2);

    // The CMP must compare the INC/DEC target register
    if (cmp_reg1 != incdec_reg) {
        FUSION_DEBUG("    INC/CMP: cmp_reg1(R{}) != incdec_reg(R{}), FAIL", cmp_reg1, incdec_reg);
        return false;
    }
    if (!cpu.is_valid_register(static_cast<Register>(cmp_reg2))) {
        FUSION_DEBUG("    INC/CMP: invalid cmp_reg2 R{}", cmp_reg2);
        return false;
    }

    uint64_t val = cpu.get_register_mode_aware(static_cast<Register>(incdec_reg));
    uint64_t val2 = cpu.get_register_mode_aware(static_cast<Register>(cmp_reg2));
    uint64_t mask = cpu.get_operand_mask();
    uint64_t new_val;
    FusionPattern pattern;

    if (incdec_op == Opcode::INC) {
        new_val = (val + 1) & mask;
        pattern = FusionPattern::INC_CMP;
    } else {
        new_val = (val - 1) & mask;
        pattern = FusionPattern::DEC_CMP;
    }

    // Compute CMP result (new_val - val2)
    uint64_t diff = new_val - val2;
    uint64_t masked_diff = diff & mask;

    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN | FLAG_CARRY | FLAG_OVERFLOW);

    if (masked_diff == 0) flags |= FLAG_ZERO;
    if (cpu.is_32bit_mode()) {
        if (masked_diff & 0x80000000) flags |= FLAG_SIGN;
        if (static_cast<uint32_t>(new_val) < static_cast<uint32_t>(val2)) flags |= FLAG_CARRY;
    } else {
        if (masked_diff & 0x8000000000000000ULL) flags |= FLAG_SIGN;
        if (new_val < val2) flags |= FLAG_CARRY;
    }

    // Update register with inc/dec result
    cpu.set_register_mode_aware(static_cast<Register>(incdec_reg), new_val);
    cpu.set_flags(flags);
    cpu.set_pc(pc + 5);
    stats.record_fusion(pattern);

    FUSION_DEBUG("    INC/CMP: SUCCESS {}+CMP at PC=0x{:04X}: R{}({}) -> R{}({}) new_val={}", 
                 pattern_name(pattern), pc, incdec_reg, val, cmp_reg2, val2, new_val);
    
    if (Config::debug) {
        DEBUG_INFO(Logging::DebugCategory::PERF_OPTIMIZATION, "[FUSION] {}+CMP at PC=0x{:04X}: R{} -> {} cmp {}", pc, incdec_reg, new_val, val2);
    }
    return true;
}

// FusionEngine::fuse_load_arithmetic - LOAD + arithmetic
bool FusionEngine::fuse_load_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    // LOAD: opcode(1) + reg(1) + addr(4) = 6 bytes
    // Arithmetic: opcode(1) + reg1(1) + reg2(1) = 3 bytes
    size_t addr_size = cpu.get_address_size();
    size_t total_size = 1 + 1 + addr_size + 3;
    if (!can_lookahead(cpu, program, total_size)) {
        FUSION_DEBUG("    LOAD+arith: cannot_lookahead({}), pc={}, prog_size={}", total_size, pc, program.size());
        return false;
    }

    uint8_t load_reg = program[pc + 1];
    uint64_t addr = cpu.read_address_from_program(program, pc + 2);
    FUSION_DEBUG("    LOAD+arith: load_reg=R{} addr=0x{:X} addr_size={}", load_reg, addr, addr_size);
    
    if (!cpu.is_valid_register(static_cast<Register>(load_reg))) {
        FUSION_DEBUG("    LOAD+arith: invalid load_reg R{}", load_reg);
        return false;
    }
    if (addr >= cpu.get_memory().size()) {
        FUSION_DEBUG("    LOAD+arith: addr 0x{:X} >= mem_size {}, OOB", addr, cpu.get_memory().size());
        return false;
    }

    Opcode arith_op = peek_opcode(program, pc, 2 + addr_size);
    uint8_t arith_reg1 = program[pc + 3 + addr_size];
    uint8_t arith_reg2 = program[pc + 4 + addr_size];
    FUSION_DEBUG("    LOAD+arith: arith_op={} arith_reg1=R{} arith_reg2=R{} (at pc+{}={})", 
                 opcode_name(arith_op), arith_reg1, arith_reg2, 3 + addr_size, pc + 3 + addr_size);

    if (arith_reg1 != load_reg) {
        FUSION_DEBUG("    LOAD+arith: arith_reg1(R{}) != load_reg(R{}), FAIL", arith_reg1, load_reg);
        return false;
    }
    if (!cpu.is_valid_register(static_cast<Register>(arith_reg2))) {
        FUSION_DEBUG("    LOAD+arith: invalid arith_reg2 R{}", arith_reg2);
        return false;
    }

    uint64_t mem_val = cpu.get_memory()[addr];
    uint64_t reg_val = cpu.get_register_mode_aware(static_cast<Register>(arith_reg2));
    uint64_t mask = cpu.get_operand_mask();
    uint64_t result = 0;
    FusionPattern pattern = FusionPattern::NONE;

    switch (arith_op) {
        case Opcode::ADD:
            result = (mem_val + reg_val) & mask;
            pattern = FusionPattern::LOAD_ADD;
            break;
        case Opcode::SUB:
            result = (mem_val - reg_val) & mask;
            pattern = FusionPattern::LOAD_SUB;
            break;
        default:
            FUSION_DEBUG("    LOAD+arith: unsupported arith_op={}, FAIL", opcode_name(arith_op));
            return false;
    }

    cpu.set_register_mode_aware(static_cast<Register>(load_reg), result);
    cpu.set_pc(pc + 2 + addr_size + 3);

    // Set flags
    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN);
    if ((result & mask) == 0) flags |= FLAG_ZERO;
    if (cpu.is_32bit_mode()) {
        if (result & 0x80000000) flags |= FLAG_SIGN;
    } else {
        if (result & 0x8000000000000000ULL) flags |= FLAG_SIGN;
    }
    cpu.set_flags(flags);

    stats.record_fusion(pattern);

    FUSION_DEBUG("    LOAD+arith: SUCCESS {} at PC=0x{:04X}: R{} = mem[0x{:X}]={} op R{}={} result={}", 
                 pattern_name(pattern), pc, load_reg, addr, mem_val, arith_reg2, reg_val, result);
    
    if (Config::debug) {
        DEBUG_INFO(Logging::DebugCategory::PERF_OPTIMIZATION, "[FUSION] LOAD+op at PC=0x{:04X}: R{} = mem[0x{:X}]={} op R{}={}", pc, load_reg, addr, mem_val, arith_reg2, reg_val);
    }
    return true;
}

// FusionEngine::fuse_mov_arithmetic - MOV + arithmetic
bool FusionEngine::fuse_mov_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    // MOV: opcode(1) + reg1(1) + reg2(1) = 3 bytes
    // Arithmetic: opcode(1) + reg1(1) + reg2(1) = 3 bytes
    if (!can_lookahead(cpu, program, 6)) {
        FUSION_DEBUG("    MOV+arith: cannot_lookahead(6), pc={}, prog_size={}", pc, program.size());
        return false;
    }

    uint8_t mov_dst = program[pc + 1];
    uint8_t mov_src = program[pc + 2];
    FUSION_DEBUG("    MOV+arith: mov_dst=R{} mov_src=R{}", mov_dst, mov_src);
    
    if (!cpu.is_valid_register(static_cast<Register>(mov_dst)) ||
        !cpu.is_valid_register(static_cast<Register>(mov_src))) {
        FUSION_DEBUG("    MOV+arith: invalid register dst=R{} src={}", mov_dst, mov_src);
        return false;
    }

    Opcode arith_op = peek_opcode(program, pc, 3);
    uint8_t arith_reg1 = program[pc + 4];
    uint8_t arith_reg2 = program[pc + 5];
    FUSION_DEBUG("    MOV+arith: arith_op={} arith_reg1=R{} arith_reg2=R{}", 
                 opcode_name(arith_op), arith_reg1, arith_reg2);

    // Arithmetic must use MOV destination as its destination
    if (arith_reg1 != mov_dst) {
        FUSION_DEBUG("    MOV+arith: arith_reg1(R{}) != mov_dst(R{}), FAIL", arith_reg1, mov_dst);
        return false;
    }
    if (!cpu.is_valid_register(static_cast<Register>(arith_reg2))) {
        FUSION_DEBUG("    MOV+arith: invalid arith_reg2 R{}", arith_reg2);
        return false;
    }

    uint64_t src_val = cpu.get_register_mode_aware(static_cast<Register>(mov_src));
    uint64_t op_val = cpu.get_register_mode_aware(static_cast<Register>(arith_reg2));
    uint64_t mask = cpu.get_operand_mask();
    uint64_t result = 0;
    FusionPattern pattern = FusionPattern::NONE;

    switch (arith_op) {
        case Opcode::ADD:
            result = (src_val + op_val) & mask;
            pattern = FusionPattern::MOV_ADD;
            break;
        case Opcode::SUB:
            result = (src_val - op_val) & mask;
            pattern = FusionPattern::MOV_SUB;
            break;
        default:
            FUSION_DEBUG("    MOV+arith: unsupported arith_op={}, FAIL", opcode_name(arith_op));
            return false;
    }

    cpu.set_register_mode_aware(static_cast<Register>(mov_dst), result);
    cpu.set_pc(pc + 6);

    // Set flags
    uint32_t flags = cpu.get_flags();
    flags &= ~(FLAG_ZERO | FLAG_SIGN);
    if ((result & mask) == 0) flags |= FLAG_ZERO;
    if (cpu.is_32bit_mode()) {
        if (result & 0x80000000) flags |= FLAG_SIGN;
    } else {
        if (result & 0x8000000000000000ULL) flags |= FLAG_SIGN;
    }
    cpu.set_flags(flags);

    stats.record_fusion(pattern);

    FUSION_DEBUG("    MOV+arith: SUCCESS {} at PC=0x{:04X}: R{} = R{}({}) op R{}({}) result={}", 
                 pattern_name(pattern), pc, mov_dst, mov_src, src_val, arith_reg2, op_val, result);
    
    if (Config::debug) {
        DEBUG_INFO(Logging::DebugCategory::PERF_OPTIMIZATION, "[FUSION] MOV+op at PC=0x{:04X}: R{} = R{} op R{}", pc, mov_dst, mov_src, arith_reg2);
    }
    return true;
}

// FusionEngine::fuse_push_pop - PUSH + POP elimination
bool FusionEngine::fuse_push_pop(CPU& cpu, const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    // PUSH: opcode(1) + reg(1) = 2 bytes
    // POP: opcode(1) + reg(1) = 2 bytes
    if (!can_lookahead(cpu, program, 4)) {
        FUSION_DEBUG("    PUSH+POP: cannot_lookahead(4), pc={}, prog_size={}", pc, program.size());
        return false;
    }

    uint8_t push_reg = program[pc + 1];
    FUSION_DEBUG("    PUSH+POP: push_reg=R{} raw_byte=0x{:02X}", push_reg, push_reg);
    
    if (!cpu.is_valid_register(static_cast<Register>(push_reg))) {
        FUSION_DEBUG("    PUSH+POP: invalid push_reg R{}", push_reg);
        return false;
    }

    Opcode next_op = peek_opcode(program, pc, 2);
    FUSION_DEBUG("    PUSH+POP: next_op={} (at pc+2={})", opcode_name(next_op), pc + 2);
    
    if (next_op != Opcode::POP) {
        FUSION_DEBUG("    PUSH+POP: next op is {} not POP, FAIL", opcode_name(next_op));
        return false;
    }

    uint8_t pop_reg = program[pc + 3];
    FUSION_DEBUG("    PUSH+POP: pop_reg=R{}", pop_reg);
    
    if (!cpu.is_valid_register(static_cast<Register>(pop_reg))) {
        FUSION_DEBUG("    PUSH+POP: invalid pop_reg R{}", pop_reg);
        return false;
    }

    if (push_reg == pop_reg) {
        // Same register: PUSH R0; POP R0 is a no-op - just skip both
        FUSION_DEBUG("    PUSH+POP: SUCCESS (same reg) at PC=0x{:04X}: R{} -> R{} (no-op)", pc, push_reg, pop_reg);
        cpu.set_pc(pc + 4);
    } else {
        // Different registers: PUSH R0; POP R1 -> MOV R1, R0
        uint64_t value = cpu.get_register_mode_aware(static_cast<Register>(push_reg));
        cpu.set_register_mode_aware(static_cast<Register>(pop_reg), value);
        cpu.set_pc(pc + 4);
        FUSION_DEBUG("    PUSH+POP: SUCCESS (diff reg) at PC=0x{:04X}: R{}({}) -> R{} (MOV)", pc, push_reg, value, pop_reg);
    }

    stats.record_fusion(FusionPattern::PUSH_POP);

    if (Config::debug) {
        DEBUG_INFO(Logging::DebugCategory::PERF_OPTIMIZATION, "[FUSION] PUSH+POP at PC=0x{:04X}: R{} -> R{} (eliminated)", pc, push_reg, pop_reg);
    }
    return true;
}

// FusionEngine::fuse_push_push_call - PUSH + PUSH + CALL fusion
bool FusionEngine::fuse_push_push_call(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    // PUSH: opcode(1) + reg(1) = 2 bytes
    // PUSH: opcode(1) + reg(1) = 2 bytes
    // CALL: opcode(1) + addr(4) = 5 bytes
    if (!can_lookahead(cpu, program, 9)) return false;

    uint8_t push1_reg = program[pc + 1];
    if (!cpu.is_valid_register(static_cast<Register>(push1_reg))) return false;

    Opcode second_op = peek_opcode(program, pc, 2);
    if (second_op != Opcode::PUSH) return false;

    uint8_t push2_reg = program[pc + 3];
    if (!cpu.is_valid_register(static_cast<Register>(push2_reg))) return false;

    Opcode third_op = peek_opcode(program, pc, 4);
    if (third_op != Opcode::CALL) return false;

    // Read CALL target address
    uint32_t call_addr = static_cast<uint32_t>(program[pc + 5]) |
                         (static_cast<uint32_t>(program[pc + 6]) << 8) |
                         (static_cast<uint32_t>(program[pc + 7]) << 16) |
                         (static_cast<uint32_t>(program[pc + 8]) << 24);

    // Check for call stack overflow
    size_t max_depth = cpu.get_effective_max_call_depth();
    if (cpu.get_call_depth() >= max_depth) {
        std::string context = fmt::format("Call depth: {} (max: {})", cpu.get_call_depth(), max_depth);
        std::string message = "Call stack overflow in fused PUSH+PUSH+CALL";
        Logging::ErrorHandler::instance().report_runtime(Logging::ErrorCode::CPU_CALL_STACK_OVERFLOW, message, pc, context);
        running = false;
        throw CPUException(message);
    }

    // Execute both pushes
    uint64_t val1 = cpu.get_register_mode_aware(static_cast<Register>(push1_reg));
    uint64_t val2 = cpu.get_register_mode_aware(static_cast<Register>(push2_reg));

    // Check stack space (2 pushes = 8 bytes)
    if (cpu.get_sp() < 16) {
        std::string message = "Stack overflow in fused PUSH+PUSH+CALL";
        Logging::ErrorHandler::instance().report_runtime(Logging::ErrorCode::CPU_STACK_OVERFLOW, message, pc, "");
        running = false;
        throw CPUException(message);
    }

    // Push first arg
    cpu.set_sp(cpu.get_sp() - 4);
    cpu.get_registers()[4] = cpu.get_sp();
    cpu.write_mem32(cpu.get_sp(), static_cast<uint32_t>(val1 & 0xFFFFFFFFU));

    // Push second arg
    cpu.set_sp(cpu.get_sp() - 4);
    cpu.get_registers()[4] = cpu.get_sp();
    cpu.write_mem32(cpu.get_sp(), static_cast<uint32_t>(val2 & 0xFFFFFFFFU));

    // Execute CALL
    cpu.set_arg_offset(8);
    uint32_t sp = cpu.get_sp() - 4;
    cpu.set_sp(sp);
    cpu.get_registers()[4] = sp;
    cpu.write_mem32(sp, cpu.get_fp());
    sp -= 4;
    cpu.set_sp(sp);
    cpu.get_registers()[4] = sp;
    uint32_t return_addr = pc + 9;
    cpu.write_mem32(sp, return_addr);
    cpu.set_fp(sp);
    cpu.increment_call_depth();
    cpu.set_pc(call_addr);

    stats.record_fusion(FusionPattern::PUSH_PUSH_CALL);

    if (Config::debug) {
        DEBUG_INFO(Logging::DebugCategory::PERF_OPTIMIZATION, "[FUSION] PUSH+PUSH+CALL at PC=0x{:04X}: args=R{},R{} target=0x{:04X}", pc, push1_reg, push2_reg, call_addr);
    }
    return true;
}

// FusionEngine::fuse_load_imm_store - LOAD_IMM + STORE
bool FusionEngine::fuse_load_imm_store(CPU& cpu, const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    // LOAD_IMM: opcode(1) + reg(1) + imm(4) = 6 bytes
    // STORE: opcode(1) + reg(1) + addr(4 for 32-bit) = 2 + addr_size bytes
    size_t addr_size = cpu.get_address_size();
    size_t total_size = 6 + 2 + addr_size;
    if (!can_lookahead(cpu, program, total_size)) {
        FUSION_DEBUG("    LOAD_IMM+STORE: cannot_lookahead({}), pc={}, prog_size={}", total_size, pc, program.size());
        return false;
    }

    uint8_t load_reg = program[pc + 1];
    uint32_t imm = static_cast<uint32_t>(program[pc + 2]) |
                   (static_cast<uint32_t>(program[pc + 3]) << 8) |
                   (static_cast<uint32_t>(program[pc + 4]) << 16) |
                   (static_cast<uint32_t>(program[pc + 5]) << 24);
    FUSION_DEBUG("    LOAD_IMM+STORE: load_reg=R{} imm={} addr_size={}", load_reg, imm, addr_size);

    if (!cpu.is_valid_register(static_cast<Register>(load_reg))) {
        FUSION_DEBUG("    LOAD_IMM+STORE: invalid load_reg R{}", load_reg);
        return false;
    }

    Opcode next_op = peek_opcode(program, pc, 6);
    if (next_op != Opcode::STORE) {
        FUSION_DEBUG("    LOAD_IMM+STORE: next op is {} not STORE, FAIL", opcode_name(next_op));
        return false;
    }

    uint8_t store_reg = program[pc + 7];
    if (store_reg != load_reg) {
        FUSION_DEBUG("    LOAD_IMM+STORE: store_reg(R{}) != load_reg(R{}), FAIL", store_reg, load_reg);
        return false;
    }

    uint64_t addr = cpu.read_address_from_program(program, pc + 8);
    if (addr >= cpu.get_memory().size()) {
        FUSION_DEBUG("    LOAD_IMM+STORE: addr 0x{:X} >= mem_size {}, OOB", addr, cpu.get_memory().size());
        return false;
    }

    // Write immediate value to register AND memory
    cpu.set_register_mode_aware(static_cast<Register>(load_reg), imm);
    cpu.get_memory()[addr] = static_cast<uint8_t>(imm & 0xFF);
    cpu.set_pc(pc + 6 + 2 + addr_size);

    stats.record_fusion(FusionPattern::LOAD_IMM_STORE);

    FUSION_DEBUG("    LOAD_IMM+STORE: SUCCESS at PC=0x{:04X}: imm={} -> [0x{:X}]", pc, imm, addr);
    
    if (Config::debug) {
        DEBUG_INFO(Logging::DebugCategory::PERF_OPTIMIZATION, "[FUSION] LOAD_IMM+STORE at PC=0x{:04X}: imm={} -> [0x{:X}]", pc, imm, addr);
    }
    return true;
}

// Global fusion engine instance
FusionEngine g_fusion_engine;

} // namespace InstructionFusion
