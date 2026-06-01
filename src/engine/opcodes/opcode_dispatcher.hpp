#pragma once
#include "opcode_handler.hpp"
#include "../cpu.hpp"

void dispatch_opcode(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
