#include "packb.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include "../cpu_registers.hpp"
#include <fmt/format.h>

using namespace DemiEngine_Registers;

void handle_PACKB(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 >= program.size()) {
        Logger::instance().error() << "PACKB: Insufficient instruction bytes" << std::endl;
        running = false;
        return;
    }

    uint8_t dest_reg = program[cpu.get_pc() + 1];
    uint8_t src_reg = program[cpu.get_pc() + 2];
    
    // Pack bytes from R0-R3 into R4 as a 32-bit value
    auto& regs = cpu.get_registers(); // Use legacy register system like LOAD_IMM
    
    uint32_t b0 = regs[static_cast<uint8_t>(R0)];
    uint32_t b1 = regs[static_cast<uint8_t>(R1)];
    uint32_t b2 = regs[static_cast<uint8_t>(R2)];
    uint32_t b3 = regs[static_cast<uint8_t>(R3)];
    
    // Pack into 32-bit value: R0 in MSB, R3 in LSB
    uint32_t packed = ((b0 & 0xFF) << 24) | 
                      ((b1 & 0xFF) << 16) | 
                      ((b2 & 0xFF) << 8) | 
                      (b3 & 0xFF);
    
    // Store packed result in R4
    regs[static_cast<uint8_t>(R4)] = packed;
    
    Logger::instance().debug() << fmt::format("PACKB: Packed bytes [0x{:02X},0x{:02X},0x{:02X},0x{:02X}] = 0x{:08X}",
        b0, b1, b2, b3, packed) << std::endl;
    
    cpu.set_pc(cpu.get_pc() + 1);
}