#include "opcode_dispatcher_predictive.hpp"
#include "opcode_dispatcher_inlined.hpp"
#include "../cpu.hpp"
#include "../branch_prediction.hpp"
#include "../cpu_flags.hpp"
#include "../../debug/debug_handler.hpp"
#include "../../config.hpp"  // For Config::debug

void dispatch_opcode_with_prediction(CPU& cpu, 
                                   const std::vector<uint8_t>& program, 
                                   bool& running) {
    DEBUG_TRACE(Logging::DebugCategory::CPU_DISPATCHER, "Dispatcher entry at PC=0x{:04X}", cpu.get_pc());
    
    // Use predictive dispatcher if branch prediction is enabled
    if (cpu.get_branch_predictor().is_enabled()) {
        Logger::instance().debug() << fmt::format("[DISPATCH_WITH_PREDICTION] Using predictive dispatcher") << std::endl;
        dispatch_opcode_predictive(cpu, program, running);
        Logger::instance().debug() << fmt::format("[DISPATCH_WITH_PREDICTION] Returned from predictive dispatcher") << std::endl;
    } else {
        Logger::instance().debug() << fmt::format("[DISPATCH_WITH_PREDICTION] Using optimized inlined dispatcher") << std::endl;
        // Fall back to standard optimized dispatcher
        dispatch_opcode_inlined_optimized(cpu, program, running);
        Logger::instance().debug() << fmt::format("[DISPATCH_WITH_PREDICTION] Returned from inlined dispatcher") << std::endl;
    }
}
#include "../cpu.hpp"
#include "../cpu_flags.hpp"
#include "../../config.hpp"
#include "../../debug/logger.hpp"
#include <fmt/format.h>

// External handler declarations for non-inlined operations
extern void handle_sub(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_call(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_load(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_store(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_push(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_pop(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_cmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jz(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jnz(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void dispatch_opcode(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

void dispatch_opcode_predictive(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    Logger::instance().debug() << fmt::format("[DISPATCH_PREDICTIVE] ENTRY: PC=0x{:04X}", pc) << std::endl;
    
    // Bounds checking
    if (__builtin_expect(pc >= program.size(), 0)) {
        Logger::instance().debug() << fmt::format("[DISPATCH_PREDICTIVE] PC out of bounds") << std::endl;
        running = false;
        return;
    }
    
    uint8_t opcode = program[pc];
    Logger::instance().debug() << fmt::format("[DISPATCH_PREDICTIVE] Processing opcode 0x{:02X}", opcode) << std::endl;
    
    // Check if this is a branch instruction that should be predicted
    if (BranchPrediction::BranchPredictor::is_branch_instruction(opcode)) {
        Logger::instance().debug() << fmt::format("[DISPATCH_PREDICTIVE] Handling as branch instruction") << std::endl;
        if (handle_predictive_branch(cpu, program, running, opcode)) {
            Logger::instance().debug() << fmt::format("[DISPATCH_PREDICTIVE] Branch handled predictively") << std::endl;
            return; // Branch handled predictively
        }
        // Fall through to standard handling if prediction failed
        Logger::instance().debug() << fmt::format("[DISPATCH_PREDICTIVE] Branch prediction failed, falling through") << std::endl;
    } else {
        Logger::instance().debug() << fmt::format("[DISPATCH_PREDICTIVE] Not a branch instruction, processing normally") << std::endl;
    }
    
    Logger::instance().debug() << fmt::format("[DISPATCH_PREDICTIVE] Using switch case for opcode 0x{:02X}", opcode) << std::endl;
    // Execute instruction using optimized inlined dispatcher
    // This maintains the performance benefits of inlining while adding prediction
    switch (opcode) {
        case static_cast<uint8_t>(Opcode::NOP): {
            #ifndef NDEBUG
            if (Config::debug) {
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [NOP] PC={}", pc, pc) << std::endl;
                cpu.print_state("NOP");
            }
            #endif
            cpu.set_pc(pc + 1);
            break;
        }
        
        case static_cast<uint8_t>(Opcode::LOAD_IMM): {
            #ifndef NDEBUG
            if (__builtin_expect(pc + 2 >= program.size(), 0)) {
                #ifdef VM_DEBUG_BOUNDS
                Logger::instance().error() << fmt::format("[LOAD_IMM] Out of bounds at PC={}", pc) << std::endl;
                #endif
                running = false;
                return;
            }
            #endif
            
            uint8_t reg = program[pc + 1];
            uint8_t value = program[pc + 2];
            
            #ifndef NDEBUG
            if (__builtin_expect(reg >= cpu.get_registers().size(), 0)) {
                #ifdef VM_DEBUG_BOUNDS
                Logger::instance().error() << fmt::format("[LOAD_IMM] Invalid register R{}", reg) << std::endl;
                #endif
                running = false;
                return;
            }
            #endif
            
            #ifndef NDEBUG
            if (Config::debug) {
                Logger::instance().debug() << fmt::format(
                    "[PC=0x{:04X}] [LOAD_IMM] R{} = {}", pc, reg, value) << std::endl;
            }
            #endif
            
            cpu.get_registers()[reg] = value;
            // Also update the 64-bit register array to maintain consistency
            if (reg < cpu.get_registers_64().size()) {
                cpu.get_registers_64()[reg] = static_cast<uint64_t>(value);
            }
            cpu.set_pc(pc + 3);
            break;
        }
        
        case static_cast<uint8_t>(Opcode::ADD): {
            #ifndef NDEBUG
            if (__builtin_expect(pc + 2 >= program.size(), 0)) {
                running = false;
                return;
            }
            #endif
            
            uint8_t reg1 = program[pc + 1];
            uint8_t reg2 = program[pc + 2];
            
            #ifndef NDEBUG
            if (__builtin_expect(reg1 >= cpu.get_registers().size() || 
                                 reg2 >= cpu.get_registers().size(), 0)) {
                running = false;
                return;
            }
            #endif
            
            uint64_t old_value = cpu.get_registers()[reg1];
            uint64_t result = old_value + cpu.get_registers()[reg2];
            
            // Set flags for overflow and carry
            uint32_t flags = cpu.get_flags();
            if (result < old_value) {
                flags |= FLAG_CARRY;
            } else {
                flags &= ~FLAG_CARRY;
            }
            
            if ((old_value & 0x80000000) == (cpu.get_registers()[reg2] & 0x80000000) &&
                (result & 0x80000000) != (old_value & 0x80000000)) {
                flags |= FLAG_OVERFLOW;
            } else {
                flags &= ~FLAG_OVERFLOW;
            }
            
            if (result == 0) {
                flags |= FLAG_ZERO;
            } else {
                flags &= ~FLAG_ZERO;
            }
            
            cpu.set_flags(flags);
            cpu.get_registers()[reg1] = result;
            
            #ifndef NDEBUG
            if (Config::debug) {
                Logger::instance().debug() << fmt::format(
                    "[PC=0x{:04X}] [ADD] R{} = {} + {} = {}", 
                    pc, reg1, old_value, cpu.get_registers()[reg2], result) << std::endl;
            }
            #endif
            
            cpu.set_pc(pc + 3);
            break;
        }
        
        case static_cast<uint8_t>(Opcode::MOV): {
            #ifndef NDEBUG
            if (__builtin_expect(pc + 2 >= program.size(), 0)) {
                running = false;
                return;
            }
            #endif
            
            uint8_t reg1 = program[pc + 1];
            uint8_t reg2 = program[pc + 2];
            
            #ifndef NDEBUG
            if (__builtin_expect(reg1 >= cpu.get_registers().size() || 
                                 reg2 >= cpu.get_registers().size(), 0)) {
                running = false;
                return;
            }
            #endif
            
            cpu.get_registers()[reg1] = cpu.get_registers()[reg2];
            
            #ifndef NDEBUG
            if (Config::debug) {
                Logger::instance().debug() << fmt::format(
                    "[PC=0x{:04X}] [MOV] R{} = R{} ({})", 
                    pc, reg1, reg2, cpu.get_registers()[reg1]) << std::endl;
            }
            #endif
            
            cpu.set_pc(pc + 3);
            break;
        }
        
        case static_cast<uint8_t>(Opcode::HALT): {
            // DEBUG: Add simple output to see if we reach HALT
            if (Config::debug) {
                printf("DEBUG: HALT instruction reached, PC=0x%04X\n", cpu.get_pc());
            }
            fflush(stdout);
            
            #ifndef NDEBUG
            if (Config::debug) {
                // FIXED: Remove Logger call to prevent deadlock
                // Logger::instance().debug() << fmt::format("[PC=0x{:04X}] [HALT]", pc) << std::endl;
            }
            #endif
            
            running = false;
            cpu.set_pc(cpu.get_pc() + 1);  // Advance PC past HALT instruction
            if (Config::debug) {
                printf("DEBUG: HALT processed, running=false, PC=0x%04X\n", cpu.get_pc());
            }
            fflush(stdout);
            break;
        }
        
        // For complex operations, delegate to external handlers
        default: {
            // FIXED: Remove Logger calls to prevent hanging
            // Logger::instance().debug() << fmt::format("[DISPATCH_PREDICTIVE] Falling back to inlined dispatcher for opcode 0x{:02X}", opcode) << std::endl;
            // Fall back to inlined dispatcher for complex operations (including 64-bit instructions)
            dispatch_opcode_inlined_optimized(cpu, program, running);
            // Logger::instance().debug() << fmt::format("[DISPATCH_PREDICTIVE] Returned from inlined dispatcher") << std::endl;
            break;
        }
    }
}

bool handle_predictive_branch(CPU& cpu, const std::vector<uint8_t>& program, 
                             bool& running, uint8_t opcode) {
    uint32_t pc = cpu.get_pc();
    auto& branch_predictor = cpu.get_branch_predictor();
    
    // Get branch prediction
    auto prediction = branch_predictor.predict(pc, opcode);
    
    // For unconditional branches, prediction is always correct
    if (opcode == static_cast<uint8_t>(Opcode::JMP) ||
        opcode == static_cast<uint8_t>(Opcode::CALL)) {
        
        // Execute the branch normally
        if (opcode == static_cast<uint8_t>(Opcode::JMP)) {
            handle_jmp(cpu, program, running);
        } else if (opcode == static_cast<uint8_t>(Opcode::CALL)) {
            handle_call(cpu, program, running);
        }
        
        // Update predictor with outcome
        BranchPrediction::BranchOutcome outcome;
        outcome.pc = pc;
        outcome.actually_taken = true;
        outcome.actual_target = cpu.get_pc();
        outcome.opcode = opcode;
        branch_predictor.update(outcome);
        
        return true;
    }
    
    // For conditional branches, we need to evaluate the condition
    bool condition_met = false;
    uint32_t target_address = 0;
    
    // Extract target address (assuming 1-byte operand)
    if (pc + 1 < program.size()) {
        target_address = program[pc + 1];
    }
    
    // Evaluate branch condition
    uint32_t flags = cpu.get_flags();
    switch (static_cast<Opcode>(opcode)) {
        case Opcode::JZ:
            condition_met = (flags & FLAG_ZERO) != 0;
            break;
        case Opcode::JNZ:
            condition_met = (flags & FLAG_ZERO) == 0;
            break;
        case Opcode::JC:
            condition_met = (flags & FLAG_CARRY) != 0;
            break;
        case Opcode::JNC:
            condition_met = (flags & FLAG_CARRY) == 0;
            break;
        case Opcode::JS:
            condition_met = (flags & FLAG_SIGN) != 0;
            break;
        case Opcode::JNS:
            condition_met = (flags & FLAG_SIGN) == 0;
            break;
        case Opcode::JO:
            condition_met = (flags & FLAG_OVERFLOW) != 0;
            break;
        case Opcode::JNO:
            condition_met = (flags & FLAG_OVERFLOW) == 0;
            break;
        default:
            return false; // Unknown branch type
    }
    
    // Check if prediction was correct
    bool prediction_correct = (prediction.predicted_taken == condition_met);
    
    if (prediction_correct) {
        // Prediction was correct
        // Execute branch normally
        if (condition_met) {
            cpu.set_pc(target_address);
        } else {
            cpu.set_pc(pc + 2); // Skip to next instruction
        }
    } else {
        // Prediction was wrong - execute correctly
        if (condition_met) {
            cpu.set_pc(target_address);
        } else {
            cpu.set_pc(pc + 2);
        }
    }
    
    // Update branch predictor with actual outcome
    BranchPrediction::BranchOutcome outcome;
    outcome.pc = pc;
    outcome.actually_taken = condition_met;
    outcome.actual_target = condition_met ? target_address : (pc + 2);
    outcome.opcode = opcode;
    branch_predictor.update(outcome);
    
    #ifndef NDEBUG
    if (Config::debug) {
        Logger::instance().debug() << fmt::format(
            "[BRANCH_PREDICTION] PC=0x{:04X}, opcode=0x{:02X}, predicted={}, actual={}, correct={}",
            pc, opcode, prediction.predicted_taken, condition_met, prediction_correct
        ) << std::endl;
    }
    #endif
    
    return true;
}