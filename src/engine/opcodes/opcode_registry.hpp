#pragma once
#include <functional>
#include <vector>
#include <array>
#include <cstdint>

// Forward declarations
class CPU;

// Opcode handler function type - be explicit about types
using OpcodeHandler = void(*)(CPU&, const std::vector<uint8_t>&, bool&);

/**
 * Centralized opcode registry to eliminate duplication between
 * threaded and fallback dispatchers.
 * 
 * This ensures that both dispatchers use the same opcode mappings
 * and handler functions, eliminating maintenance overhead when
 * adding new opcodes.
 */
class OpcodeRegistry {
public:
    // Get the singleton instance
    static OpcodeRegistry& instance();
    
    // Register an opcode handler
    void register_opcode(uint8_t opcode, OpcodeHandler handler);
    
    // Get handler for an opcode (returns nullptr if not registered)
    OpcodeHandler get_handler(uint8_t opcode) const;
    
    // Get all registered opcodes for dispatch table generation
    const std::array<OpcodeHandler, 256>& get_handlers() const;
    
    // Initialize all opcode handlers (called once at startup)
    void initialize_handlers();
    
    // Check if an opcode is registered
    bool is_registered(uint8_t opcode) const;

private:
    OpcodeRegistry() = default;
    std::array<OpcodeHandler, 256> handlers_{};
    bool initialized_ = false;
};

// Macro to make opcode registration easier
#define REGISTER_OPCODE(opcode_value, handler_func) \
    OpcodeRegistry::instance().register_opcode(opcode_value, handler_func)
