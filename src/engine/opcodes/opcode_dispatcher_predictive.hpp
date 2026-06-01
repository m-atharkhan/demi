#pragma once
#include "opcode_dispatcher_inlined.hpp"

// Branch-predictive dispatcher with speculative execution on predicted branches
void dispatch_opcode_predictive(CPU& cpu, 
                               const std::vector<uint8_t>& program, 
                               bool& running);

/**
 * Handle branch instruction with prediction and speculation
 * @param cpu CPU instance
 * @param program Program bytecode
 * @param running Execution control flag
 * @param opcode Branch instruction opcode
 * @return True if branch was handled predictively
 */
bool handle_predictive_branch(CPU& cpu,
                             const std::vector<uint8_t>& program,
                             bool& running,
                             uint8_t opcode);

/**
 * Main predictive dispatcher that selects best execution strategy
 * @param cpu CPU instance
 * @param program Program bytecode
 * @param running Execution control flag
 */
void dispatch_opcode_with_prediction(CPU& cpu, 
                                   const std::vector<uint8_t>& program, 
                                   bool& running);