#pragma once
#include "../cpu.hpp"
#include <vector>
#include <cstdint>

// VADD - Vector Addition
// Adds vectors in XMM registers element-wise
void handle_VADD(CPU& cpu, const std::vector<uint8_t>& program, bool& running);