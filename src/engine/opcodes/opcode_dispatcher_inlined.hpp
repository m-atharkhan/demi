#pragma once
#include <vector>
#include <cstdint>

// Forward declaration
class CPU;

// High-performance dispatcher with inlined handlers for common operations

// Check if threaded code is supported
#ifndef USE_THREADED_CODE
#ifdef __GNUC__
#define USE_THREADED_CODE 1
#else
#define USE_THREADED_CODE 0
#endif
#endif

// High-performance inlined dispatcher (uses computed goto if available)
void dispatch_opcode_inlined(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// Fallback inlined dispatcher (uses switch-case with inlined operations)
void dispatch_opcode_inlined_fallback(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// Main dispatcher function that selects the best inlined implementation
inline void dispatch_opcode_inlined_optimized(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
#if USE_THREADED_CODE
    dispatch_opcode_inlined(cpu, program, running);
#else
    dispatch_opcode_inlined_fallback(cpu, program, running);
#endif
}