#pragma once
#include "opcode_handler.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"

// Threaded code dispatcher using computed goto (GCC/Clang only; falls back to switch-case otherwise)

// Feature detection for computed goto support
// Computed goto is a GCC extension available since GCC 2.95
// Also supported by Clang and some other compilers
// IMPORTANT: Disabled when AddressSanitizer is active due to incompatibility
#if defined(__SANITIZE_ADDRESS__)
    #define USE_THREADED_CODE 0  // AddressSanitizer detected (GCC) - use fallback
#elif defined(__has_feature)
    #if __has_feature(address_sanitizer)
        #define USE_THREADED_CODE 0  // AddressSanitizer detected (Clang) - use fallback
    #elif defined(__clang__)
        #define USE_THREADED_CODE 1  // Clang with computed goto support
    #else
        #define USE_THREADED_CODE 0  // Fallback for unknown compiler
    #endif
#elif defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95))
    #define USE_THREADED_CODE 1  // GCC with computed goto support
#elif defined(__clang__)
    #define USE_THREADED_CODE 1  // Clang also supports computed goto
#else
    #define USE_THREADED_CODE 0  // Fallback to switch-case for other compilers
#endif

// High-performance dispatcher using computed goto
void dispatch_opcode_threaded(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// Fallback dispatcher for non-GCC compilers
void dispatch_opcode_fallback(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// Main dispatcher function that selects the best implementation
inline void dispatch_opcode_optimized(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
#if USE_THREADED_CODE
    dispatch_opcode_threaded(cpu, program, running);
#else
    dispatch_opcode_fallback(cpu, program, running);
#endif
}