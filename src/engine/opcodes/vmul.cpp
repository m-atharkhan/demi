#include "vmul.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include "../cpu_registers.hpp"
#include <fmt/format.h>

using namespace DemiEngine_Registers;

void handle_VMUL(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    Logger::instance().debug() << "Executing VMUL - Vector Multiplication" << std::endl;
    
    // Vector multiplication: R0-R3 = R0-R3 * R4-R7
    // Element-wise multiplication of two 4-element vectors
    
    auto& regs = cpu.get_registers(); // Use legacy register system like LOAD_IMM
    
    uint32_t a0 = regs[static_cast<uint8_t>(R0)];
    uint32_t a1 = regs[static_cast<uint8_t>(R1)];
    uint32_t a2 = regs[static_cast<uint8_t>(R2)];
    uint32_t a3 = regs[static_cast<uint8_t>(R3)];
    
    uint32_t b0 = regs[static_cast<uint8_t>(R4)];
    uint32_t b1 = regs[static_cast<uint8_t>(R5)];
    uint32_t b2 = regs[static_cast<uint8_t>(R6)];
    uint32_t b3 = regs[static_cast<uint8_t>(R7)];
    
    Logger::instance().debug() << fmt::format("VMUL raw register values: R0={:08X}, R1={:08X}, R2={:08X}, R3={:08X}, R4={:08X}, R5={:08X}, R6={:08X}, R7={:08X}",
        a0, a1, a2, a3, b0, b1, b2, b3) << std::endl;
    
    // Multiplication with overflow wraparound
    uint32_t result0 = a0 * b0;
    uint32_t result1 = a1 * b1;
    uint32_t result2 = a2 * b2;
    uint32_t result3 = a3 * b3;
    
    // Store results back to R0-R3
    regs[static_cast<uint8_t>(R0)] = result0;
    regs[static_cast<uint8_t>(R1)] = result1;
    regs[static_cast<uint8_t>(R2)] = result2;
    regs[static_cast<uint8_t>(R3)] = result3;
    
    Logger::instance().debug() << fmt::format("VMUL: [{},{},{},{}] * [{},{},{},{}] = [{},{},{},{}]",
        a0, a1, a2, a3, b0, b1, b2, b3, result0, result1, result2, result3) << std::endl;
    
    cpu.set_pc(cpu.get_pc() + 1);
}