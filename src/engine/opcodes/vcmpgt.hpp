#pragma once
#include "../cpu.hpp"
#include <vector>
#include <cstdint>

// VCMPGT - Vector Compare Greater Than
// Compares R0-R3 > R4-R7 element-wise, stores result as 0xFF/0x00 in R0-R3
void handle_VCMPGT(CPU& cpu, const std::vector<uint8_t>& program, bool& running);