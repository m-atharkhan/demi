#include "vdot.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include "../cpu_registers.hpp"
#include <fmt/format.h>

using namespace DemiEngine_Registers;

void handle_VDOT(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    Logger::instance().debug() << "Executing VDOT - Vector Dot Product" << std::endl;
    
    // Dot product: sum of (R0-R3) * (R4-R7), store result in R0
    
    auto& regs = cpu.get_registers(); // Use legacy register system like LOAD_IMM
    
    uint32_t a0 = regs[static_cast<uint8_t>(R0)];
    uint32_t a1 = regs[static_cast<uint8_t>(R1)];
    uint32_t a2 = regs[static_cast<uint8_t>(R2)];
    uint32_t a3 = regs[static_cast<uint8_t>(R3)];
    
    uint32_t b0 = regs[static_cast<uint8_t>(R4)];
    uint32_t b1 = regs[static_cast<uint8_t>(R5)];
    uint32_t b2 = regs[static_cast<uint8_t>(R6)];
    uint32_t b3 = regs[static_cast<uint8_t>(R7)];
    
    // Calculate dot product: a0*b0 + a1*b1 + a2*b2 + a3*b3
    uint32_t result = (a0 * b0) + (a1 * b1) + (a2 * b2) + (a3 * b3);
    
    // Store result in R0
    regs[static_cast<uint8_t>(R0)] = result;
    
    Logger::instance().debug() << fmt::format("VDOT: [{},{},{},{}] • [{},{},{},{}] = {}",
        a0, a1, a2, a3, b0, b1, b2, b3, result) << std::endl;
    
    cpu.set_pc(cpu.get_pc() + 1);
}