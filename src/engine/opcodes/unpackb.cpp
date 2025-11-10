#include "unpackb.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include "../cpu_registers.hpp"
#include <fmt/format.h>

using namespace DemiEngine_Registers;

void handle_UNPACKB(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 >= program.size()) {
        Logger::instance().error() << "UNPACKB: Insufficient instruction bytes" << std::endl;
        running = false;
        return;
    }

    // Unpack value from R4 into R0-R3
    auto& regs = cpu.get_registers(); // Use legacy register system like LOAD_IMM
    
    uint32_t packed = regs[static_cast<uint8_t>(R4)];
    
    // Extract 4 bytes from the 32-bit value
    uint8_t b0 = (packed >> 24) & 0xFF;  // Most significant byte
    uint8_t b1 = (packed >> 16) & 0xFF;
    uint8_t b2 = (packed >> 8) & 0xFF;
    uint8_t b3 = packed & 0xFF;          // Least significant byte
    
    // Store unpacked bytes in R0-R3
    regs[static_cast<uint8_t>(R0)] = b0;
    regs[static_cast<uint8_t>(R1)] = b1;
    regs[static_cast<uint8_t>(R2)] = b2;
    regs[static_cast<uint8_t>(R3)] = b3;
    
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}] UNPACKB: Unpacked 0x{:02X} = [0x{:02X},0x{:02X},0x{:02X},0x{:02X}]",
        cpu.get_pc(), packed, b0, b1, b2, b3) << std::endl;
    
    cpu.set_pc(cpu.get_pc() + 1);
}