#pragma once
#include "../cpu.hpp"
#include <vector>
#include <cstdint>

// PACKB - Pack Bytes
// Packs bytes from R0-R3 into a single 32-bit value in R4
void handle_PACKB(CPU& cpu, const std::vector<uint8_t>& program, bool& running);