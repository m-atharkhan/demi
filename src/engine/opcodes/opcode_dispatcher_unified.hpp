#pragma once
#include <vector>
#include <cstdint>

// Forward declaration
class CPU;

/**
 * Unified opcode dispatcher that eliminates duplication between
 * threaded and fallback dispatchers.
 * 
 * This uses the OpcodeRegistry to ensure both dispatcher types
 * use exactly the same opcode mappings and handlers.
 */

// Unified threaded dispatcher (uses computed goto if available)
void dispatch_opcode_unified(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// Unified fallback dispatcher (uses switch-case)
void dispatch_opcode_unified_fallback(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// Check if threaded code is supported
#ifndef USE_THREADED_CODE
#ifdef __GNUC__
#define USE_THREADED_CODE 1
#else
#define USE_THREADED_CODE 0
#endif
#endif