#pragma once
#include "../cpu.hpp"
#include <vector>
#include <cstdint>

// VMAX - Vector Maximum
// Finds maximum element in vector R0-R3 and stores result in R0
void handle_VMAX(CPU& cpu, const std::vector<uint8_t>& program, bool& running);