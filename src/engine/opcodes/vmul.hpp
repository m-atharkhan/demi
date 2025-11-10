#pragma once
#include "../cpu.hpp"
#include <vector>
#include <cstdint>

// VMUL - Vector Multiplication
// Multiplies vectors in XMM registers element-wise
void handle_VMUL(CPU& cpu, const std::vector<uint8_t>& program, bool& running);