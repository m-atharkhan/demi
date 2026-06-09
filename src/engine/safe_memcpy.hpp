#pragma once
#include <cstring>
#include <type_traits>

// Safe type-punning via memcpy with compile-time size verification.
// Use this instead of raw std::memcpy for scalar type-punning to satisfy
// static analysis tools (Codacy, flawfinder) that flag unbounded memcpy.
//
// Usage: float f = safe_bitcast<float>(raw_uint32);
//        uint64_t raw = safe_bitcast<uint64_t>(some_double);

template <typename To, typename From>
inline To safe_bitcast(const From& src) noexcept {
    static_assert(sizeof(To) == sizeof(From),
                  "safe_bitcast: source and destination must have the same size");
    static_assert(std::is_trivially_copyable_v<To> && std::is_trivially_copyable_v<From>,
                  "safe_bitcast: both types must be trivially copyable");
    To dest;
    std::memcpy(&dest, &src, sizeof(To));
    return dest;
}

// Same as safe_bitcast but writes into an existing destination variable.
template <typename To, typename From>
inline void safe_memcpy_to(To& dest, const From& src) noexcept {
    static_assert(sizeof(To) == sizeof(From),
                  "safe_memcpy_to: source and destination must have the same size");
    static_assert(std::is_trivially_copyable_v<To> && std::is_trivially_copyable_v<From>,
                  "safe_memcpy_to: both types must be trivially copyable");
    std::memcpy(&dest, &src, sizeof(To));
}
