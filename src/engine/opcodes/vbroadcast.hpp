#pragma once
#include "../cpu.hpp"
#include <vector>
#include <cstdint>

// VBROADCAST - Vector Broadcast
// Broadcasts value from R0 to R1, R2, R3, R4
void handle_VBROADCAST(CPU& cpu, const std::vector<uint8_t>& program, bool& running);