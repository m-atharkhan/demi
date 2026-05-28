#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

// Opcode execution profiler for hotspot analysis
// Tracks execution counts for each opcode (0-255)
// Disabled by default in release builds for zero overhead
class OpcodeProfiler {
public:
    static constexpr size_t NUM_OPCODES = 256;

    // Get singleton instance
    static OpcodeProfiler& instance();

    // Reset all counters to zero
    void reset();

    // Record an opcode execution (inline for minimal overhead when enabled)
    void record(uint8_t opcode) {
        if (enabled_) {
            counts_[opcode]++;
            total_count_++;
        }
    }

    // Enable/disable profiling
    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }
    bool is_enabled() const { return enabled_; }

    // Get execution count for a specific opcode
    uint64_t get_count(uint8_t opcode) const { return counts_[opcode]; }

    // Get total execution count
    uint64_t get_total_count() const { return total_count_; }

    // Get all counts as a const reference
    const std::array<uint64_t, NUM_OPCODES>& get_counts() const { return counts_; }

    // Find the top N hotspots (sorted by count descending)
    struct Hotspot {
        uint8_t opcode;
        uint64_t count;
        double percentage;
    };
    std::vector<Hotspot> get_hotspots(size_t top_n = 10) const;

    // Print profiling summary to debug output
    void print_summary() const;

    // Print detailed profiling data to debug output
    void print_detailed() const;

private:
    OpcodeProfiler() { reset(); }
    std::array<uint64_t, NUM_OPCODES> counts_{};
    uint64_t total_count_ = 0;
    bool enabled_ = false;
};

// Inline profiling macro for use in dispatchers
// When profiling is disabled, this compiles to nothing (zero overhead)
#ifndef NDEBUG
    #define OPCODE_PROFILE(op) OpcodeProfiler::instance().record(op)
#else
    #define OPCODE_PROFILE(op) ((void)0)
#endif
