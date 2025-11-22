#include "branch_prediction.hpp"
#include "cpu.hpp"  // Include full CPU definition
#include "../debug/logger.hpp"
#include "../config.hpp"
#include <fmt/format.h>
#include <algorithm>

namespace BranchPrediction {

BranchPredictor::BranchPredictor()
    : pattern_history_table(PHT_SIZE)
    , branch_target_buffer(BTB_SIZE)
{
    // Initialize Pattern History Table with weak not-taken
    for (auto& entry : pattern_history_table) {
        entry.state = PredictionConfidence::WEAK_NOT_TAKEN;
        entry.access_count = 0;
    }
    
    // Initialize Branch Target Buffer
    for (auto& entry : branch_target_buffer) {
        entry.valid = false;
        entry.pc = 0;
        entry.target = 0;
        entry.last_access = 0;
    }
    
    // Only log initialization if not in test mode
    static bool logged_initialization = false;
    if (!Config::test_mode && !logged_initialization) {
        Logger::instance().info() << fmt::format(
            "BranchPredictor initialized: BHT_SIZE={}, BTB_SIZE={}, PHT_SIZE={}",
            BHT_SIZE, BTB_SIZE, PHT_SIZE
        ) << std::endl;
        logged_initialization = true;
    }
}

BranchPrediction BranchPredictor::predict(uint32_t pc, uint8_t opcode) {
    BranchPrediction prediction{};
    
    if (!prediction_enabled || !is_branch_instruction(opcode)) {
        // Default to not taken for non-branch instructions or disabled prediction
        prediction.predicted_taken = false;
        prediction.predicted_target = 0;
        prediction.confidence = PredictionConfidence::STRONG_NOT_TAKEN;
        prediction.hit_btb = false;
        return prediction;
    }
    
    stats.total_predictions++;
    
    // For unconditional branches, always predict taken
    if (is_unconditional_branch(opcode)) {
        prediction.predicted_taken = true;
        prediction.confidence = PredictionConfidence::STRONG_TAKEN;
        
        // Look up target in BTB
        BTBEntry* btb_entry = find_btb_entry(pc);
        if (btb_entry && btb_entry->valid) {
            prediction.predicted_target = btb_entry->target;
            prediction.hit_btb = true;
            stats.btb_hits++;
            btb_entry->last_access = ++access_counter;
        } else {
            prediction.predicted_target = 0; // Target unknown
            prediction.hit_btb = false;
            stats.btb_misses++;
        }
        
        return prediction;
    }
    
    // For conditional branches, use hybrid prediction
    prediction.predicted_taken = predict_hybrid(pc);
    
    // Determine confidence based on prediction strength
    uint16_t ghr_index = get_ghr_index(pc);
    prediction.confidence = get_pht_prediction(ghr_index);
    
    // Look up target in BTB if predicting taken
    if (prediction.predicted_taken) {
        BTBEntry* btb_entry = find_btb_entry(pc);
        if (btb_entry && btb_entry->valid) {
            prediction.predicted_target = btb_entry->target;
            prediction.hit_btb = true;
            stats.btb_hits++;
            btb_entry->last_access = ++access_counter;
        } else {
            prediction.predicted_target = 0; // Target unknown
            prediction.hit_btb = false;
            stats.btb_misses++;
        }
    } else {
        prediction.predicted_target = pc + 1; // Fall through for not taken
        prediction.hit_btb = false;
    }
    
    return prediction;
}

void BranchPredictor::update(const BranchOutcome& outcome) {
    if (!prediction_enabled || !is_branch_instruction(outcome.opcode)) {
        return;
    }
    
    // Update statistics
    if (outcome.actually_taken) {
        stats.taken_branches++;
    } else {
        stats.not_taken_branches++;
    }
    
    // Update Pattern History Table
    uint16_t ghr_index = get_ghr_index(outcome.pc);
    update_pht_entry(ghr_index, outcome.actually_taken);
    
    // Update local history
    update_local_history(outcome.pc, outcome.actually_taken);
    
    // Update global history
    update_global_history(outcome.actually_taken);
    
    // Update Branch Target Buffer
    if (outcome.actually_taken) {
        insert_btb_entry(outcome.pc, outcome.actual_target);
    }
}

bool BranchPredictor::is_branch_instruction(uint8_t opcode) {
    // Create temporary instance to access member methods
    BranchPredictor temp;
    return temp.is_conditional_branch(opcode) || temp.is_unconditional_branch(opcode);
}

uint16_t BranchPredictor::get_ghr_index(uint32_t pc) const {
    // Combine PC and global history for PHT indexing
    uint16_t pc_bits = static_cast<uint16_t>(pc & ((1 << (16 - GHR_BITS)) - 1));
    return (pc_bits << GHR_BITS) | (global_history_register & ((1 << GHR_BITS) - 1));
}

uint16_t BranchPredictor::get_local_history(uint32_t pc) const {
    auto it = local_history.find(pc);
    return it != local_history.end() ? it->second : 0;
}

void BranchPredictor::update_local_history(uint32_t pc, bool taken) {
    uint16_t& history = local_history[pc];
    history = ((history << 1) | (taken ? 1 : 0)) & ((1 << LOCAL_HISTORY_BITS) - 1);
}

void BranchPredictor::update_global_history(bool taken) {
    global_history_register = ((global_history_register << 1) | (taken ? 1 : 0)) 
                              & ((1 << GHR_BITS) - 1);
}

BTBEntry* BranchPredictor::find_btb_entry(uint32_t pc) {
    // Simple hash for BTB indexing
    size_t index = pc % BTB_SIZE;
    
    // Check direct mapped entry
    if (branch_target_buffer[index].valid && branch_target_buffer[index].pc == pc) {
        return &branch_target_buffer[index];
    }
    
    // Check for collision - linear probing
    for (size_t i = 1; i < BTB_SIZE; ++i) {
        size_t probe_index = (index + i) % BTB_SIZE;
        if (branch_target_buffer[probe_index].valid && 
            branch_target_buffer[probe_index].pc == pc) {
            return &branch_target_buffer[probe_index];
        }
    }
    
    return nullptr; // Not found
}

void BranchPredictor::insert_btb_entry(uint32_t pc, uint32_t target) {
    size_t index = pc % BTB_SIZE;
    
    // Try direct mapped position first
    if (!branch_target_buffer[index].valid || 
        branch_target_buffer[index].pc == pc) {
        branch_target_buffer[index] = {pc, target, true, ++access_counter};
        return;
    }
    
    // Find LRU entry for replacement
    size_t lru_index = index;
    uint64_t oldest_access = branch_target_buffer[index].last_access;
    
    for (size_t i = 0; i < BTB_SIZE; ++i) {
        if (!branch_target_buffer[i].valid) {
            // Found empty slot
            branch_target_buffer[i] = {pc, target, true, ++access_counter};
            return;
        }
        
        if (branch_target_buffer[i].last_access < oldest_access) {
            lru_index = i;
            oldest_access = branch_target_buffer[i].last_access;
        }
    }
    
    // Replace LRU entry
    branch_target_buffer[lru_index] = {pc, target, true, ++access_counter};
}

PredictionConfidence BranchPredictor::get_pht_prediction(uint16_t index) const {
    return pattern_history_table[index % PHT_SIZE].state;
}

void BranchPredictor::update_pht_entry(uint16_t index, bool taken) {
    PHTPEntry& entry = pattern_history_table[index % PHT_SIZE];
    entry.access_count++;
    
    // 2-bit saturating counter update
    switch (entry.state) {
        case PredictionConfidence::STRONG_NOT_TAKEN:
            if (taken) {
                entry.state = PredictionConfidence::WEAK_NOT_TAKEN;
            }
            break;
            
        case PredictionConfidence::WEAK_NOT_TAKEN:
            if (taken) {
                entry.state = PredictionConfidence::WEAK_TAKEN;
            } else {
                entry.state = PredictionConfidence::STRONG_NOT_TAKEN;
            }
            break;
            
        case PredictionConfidence::WEAK_TAKEN:
            if (taken) {
                entry.state = PredictionConfidence::STRONG_TAKEN;
            } else {
                entry.state = PredictionConfidence::WEAK_NOT_TAKEN;
            }
            break;
            
        case PredictionConfidence::STRONG_TAKEN:
            if (!taken) {
                entry.state = PredictionConfidence::WEAK_TAKEN;
            }
            break;
    }
}

bool BranchPredictor::predict_global(uint32_t pc) const {
    uint16_t ghr_index = get_ghr_index(pc);
    PredictionConfidence confidence = get_pht_prediction(ghr_index);
    return confidence == PredictionConfidence::WEAK_TAKEN || 
           confidence == PredictionConfidence::STRONG_TAKEN;
}

bool BranchPredictor::predict_local(uint32_t pc) const {
    uint16_t history = get_local_history(pc);
    // Simple heuristic: predict taken if more than half of recent history was taken
    uint8_t taken_count = 0;
    for (size_t i = 0; i < LOCAL_HISTORY_BITS; ++i) {
        if (history & (1 << i)) {
            taken_count++;
        }
    }
    return taken_count > (LOCAL_HISTORY_BITS / 2);
}

bool BranchPredictor::predict_hybrid(uint32_t pc) const {
    // Use global predictor as primary, local as secondary
    // In a more sophisticated implementation, we'd have a meta-predictor
    // to choose between global and local predictors
    
    bool global_prediction = predict_global(pc);
    bool local_prediction = predict_local(pc);
    
    // For simplicity, weight global predictor more heavily
    // but use local predictor to break ties in weak predictions
    uint16_t ghr_index = get_ghr_index(pc);
    PredictionConfidence confidence = get_pht_prediction(ghr_index);
    
    if (confidence == PredictionConfidence::STRONG_TAKEN ||
        confidence == PredictionConfidence::STRONG_NOT_TAKEN) {
        return global_prediction; // Strong confidence in global predictor
    } else {
        // Weak confidence - use local predictor as tie-breaker
        return local_prediction;
    }
}

bool BranchPredictor::is_conditional_branch(uint8_t opcode) const {
    // Map DemiEngine opcodes to conditional branches
    switch (static_cast<Opcode>(opcode)) {
        case Opcode::JZ:   // Jump if zero
        case Opcode::JNZ:  // Jump if not zero
        case Opcode::JS:   // Jump if sign
        case Opcode::JNS:  // Jump if not sign
        case Opcode::JC:   // Jump if carry
        case Opcode::JNC:  // Jump if not carry
        case Opcode::JO:   // Jump if overflow
        case Opcode::JNO:  // Jump if not overflow
        case Opcode::JG:   // Jump if greater
        case Opcode::JL:   // Jump if less
        case Opcode::JGE:  // Jump if greater or equal
        case Opcode::JLE:  // Jump if less or equal
            return true;
        default:
            return false;
    }
}

bool BranchPredictor::is_unconditional_branch(uint8_t opcode) const {
    // Map DemiEngine opcodes to unconditional branches
    switch (static_cast<Opcode>(opcode)) {
        case Opcode::JMP:  // Unconditional jump
        case Opcode::CALL: // Subroutine call
        case Opcode::RET:  // Return from subroutine
            return true;
        default:
            return false;
    }
}

} // namespace BranchPrediction