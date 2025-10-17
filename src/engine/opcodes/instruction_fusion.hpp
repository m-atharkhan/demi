#pragma once
#include "../cpu.hpp"
#include "../../assembler/opcodes.hpp"
#include "../../debug/logger.hpp"
#include <vector>
#include <cstdint>

// Instruction Fusion Optimizer
// Detects and executes common instruction patterns as fused operations
// for improved performance by reducing instruction dispatch overhead
//
// PERFORMANCE BENEFITS:
// - Eliminates redundant PC updates and bounds checks
// - Reduces branch mispredictions in dispatcher
// - Combines memory operations efficiently
// - Expected 1.5-2x speedup for fusible sequences
//
// SUPPORTED FUSION PATTERNS:
// 1. LOAD_IMM + ADD/SUB/MUL/AND/OR/XOR  -> Immediate arithmetic
// 2. LOAD_IMM + CMP                     -> Immediate compare
// 3. CMP + JZ/JNZ/JG/JL/JGE/JLE        -> Compare-and-branch
// 4. ADD/SUB/MUL + STORE               -> Arithmetic-to-memory
// 5. LOAD + ADD/SUB/MUL                -> Memory-arithmetic
// 6. INC/DEC + CMP                     -> Inc/Dec-compare
// 7. MOV + arithmetic                  -> Move-arithmetic

namespace InstructionFusion {

// Fusion pattern identifiers
enum class FusionPattern {
    NONE = 0,
    LOAD_IMM_ADD,        // LOAD_IMM Rx, imm + ADD Rx, Ry -> ADD Rx, imm
    LOAD_IMM_SUB,        // LOAD_IMM Rx, imm + SUB Rx, Ry -> SUB Rx, imm
    LOAD_IMM_MUL,        // LOAD_IMM Rx, imm + MUL Rx, Ry -> MUL Rx, imm
    LOAD_IMM_AND,        // LOAD_IMM Rx, imm + AND Rx, Ry -> AND Rx, imm
    LOAD_IMM_OR,         // LOAD_IMM Rx, imm + OR Rx, Ry -> OR Rx, imm
    LOAD_IMM_XOR,        // LOAD_IMM Rx, imm + XOR Rx, Ry -> XOR Rx, imm
    LOAD_IMM_CMP,        // LOAD_IMM Rx, imm + CMP Rx, Ry -> CMP Rx, imm
    CMP_JZ,              // CMP Rx, Ry + JZ addr -> Fused compare-branch
    CMP_JNZ,             // CMP Rx, Ry + JNZ addr -> Fused compare-branch
    CMP_JG,              // CMP Rx, Ry + JG addr -> Fused compare-branch
    CMP_JL,              // CMP Rx, Ry + JL addr -> Fused compare-branch
    CMP_JGE,             // CMP Rx, Ry + JGE addr -> Fused compare-branch
    CMP_JLE,             // CMP Rx, Ry + JLE addr -> Fused compare-branch
    ADD_STORE,           // ADD Rx, Ry + STORE [addr], Rx -> Add-to-memory
    SUB_STORE,           // SUB Rx, Ry + STORE [addr], Rx -> Sub-from-memory
    INC_CMP,             // INC Rx + CMP Rx, Ry -> Inc-and-compare
    DEC_CMP,             // DEC Rx + CMP Rx, Ry -> Dec-and-compare
    LOAD_ADD,            // LOAD Rx, [addr] + ADD Rx, Ry -> Memory-add
    LOAD_SUB,            // LOAD Rx, [addr] + SUB Rx, Ry -> Memory-sub
    MOV_ADD,             // MOV Rx, Ry + ADD Rx, Rz -> Move-add
    MOV_SUB              // MOV Rx, Ry + SUB Rx, Rz -> Move-sub
};

// Fusion statistics for profiling
struct FusionStats {
    uint64_t total_attempts = 0;
    uint64_t successful_fusions = 0;
    uint64_t pattern_counts[static_cast<int>(FusionPattern::MOV_SUB) + 1] = {0};
    
    void record_fusion(FusionPattern pattern) {
        successful_fusions++;
        pattern_counts[static_cast<int>(pattern)]++;
    }
    
    double fusion_rate() const {
        return total_attempts > 0 ? 
            (static_cast<double>(successful_fusions) / total_attempts * 100.0) : 0.0;
    }
};

// Pattern detection and execution
class FusionEngine {
private:
    FusionStats stats;
    bool enabled;
    
    // Helper: Check if we have enough bytes for lookahead
    inline bool can_lookahead(const CPU& cpu, const std::vector<uint8_t>& program, size_t bytes) const {
        return cpu.get_pc() + bytes < program.size();
    }
    
    // Helper: Read opcode at offset
    inline Opcode peek_opcode(const std::vector<uint8_t>& program, uint32_t pc, size_t offset) const {
        return static_cast<Opcode>(program[pc + offset]);
    }
    
    // Helper: Read register at offset
    inline uint8_t peek_register(const std::vector<uint8_t>& program, uint32_t pc, size_t offset) const {
        return program[pc + offset];
    }
    
public:
    FusionEngine() : enabled(true) {}
    
    void enable() { enabled = true; }
    void disable() { enabled = false; }
    bool is_enabled() const { return enabled; }
    
    const FusionStats& get_stats() const { return stats; }
    void reset_stats() { stats = FusionStats(); }
    
    // Main fusion detection and execution
    // Returns true if fusion was applied, false otherwise
    bool try_fuse_and_execute(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
    
private:
    // Pattern-specific fusion handlers
    bool fuse_load_imm_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
    bool fuse_cmp_branch(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
    bool fuse_arithmetic_store(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
    bool fuse_inc_dec_cmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
    bool fuse_load_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
    bool fuse_mov_arithmetic(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
};

// Global fusion engine instance
extern FusionEngine g_fusion_engine;

// Convenience function for integration
inline bool try_instruction_fusion(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    return g_fusion_engine.try_fuse_and_execute(cpu, program, running);
}

} // namespace InstructionFusion
