#pragma once
#include "../cpu.hpp"
#include <vector>
#include <cstdint>

// VDOT - Vector Dot Product
// Computes dot product of two vectors and stores result in R0
void handle_VDOT(CPU& cpu, const std::vector<uint8_t>& program, bool& running);