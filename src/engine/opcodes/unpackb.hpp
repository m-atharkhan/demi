#pragma once
#include "../cpu.hpp"
#include <vector>
#include <cstdint>

// UNPACKB - Unpack Bytes
// Unpacks value from R4 into separate bytes in R0-R3
void handle_UNPACKB(CPU& cpu, const std::vector<uint8_t>& program, bool& running);