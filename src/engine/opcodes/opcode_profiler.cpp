#include "opcode_profiler.hpp"
#include "../cpu.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/format.h>
#include <algorithm>
#include <iomanip>

OpcodeProfiler& OpcodeProfiler::instance() {
    static OpcodeProfiler inst;
    return inst;
}

void OpcodeProfiler::reset() {
    counts_.fill(0);
    total_count_ = 0;
}

std::vector<OpcodeProfiler::Hotspot> OpcodeProfiler::get_hotspots(size_t top_n) const {
    std::vector<Hotspot> hotspots;
    hotspots.reserve(NUM_OPCODES);

    for (size_t i = 0; i < NUM_OPCODES; i++) {
        if (counts_[i] > 0) {
            double pct = (total_count_ > 0)
                ? (static_cast<double>(counts_[i]) / static_cast<double>(total_count_) * 100.0)
                : 0.0;
            hotspots.push_back({static_cast<uint8_t>(i), counts_[i], pct});
        }
    }

    // Sort by count descending
    std::sort(hotspots.begin(), hotspots.end(),
        [](const Hotspot& a, const Hotspot& b) { return a.count > b.count; });

    if (hotspots.size() > top_n) {
        hotspots.resize(top_n);
    }

    return hotspots;
}

static const char* opcode_name_from_byte(uint8_t op) {
    switch (static_cast<Opcode>(op)) {
        case Opcode::NOP:       return "NOP";
        case Opcode::LOAD_IMM:  return "LOAD_IMM";
        case Opcode::ADD:       return "ADD";
        case Opcode::SUB:       return "SUB";
        case Opcode::MOV:       return "MOV";
        case Opcode::JMP:       return "JMP";
        case Opcode::LOAD:      return "LOAD";
        case Opcode::STORE:     return "STORE";
        case Opcode::PUSH:      return "PUSH";
        case Opcode::POP:       return "POP";
        case Opcode::CMP:       return "CMP";
        case Opcode::JZ:        return "JZ";
        case Opcode::JNZ:       return "JNZ";
        case Opcode::JS:        return "JS";
        case Opcode::JNS:       return "JNS";
        case Opcode::JC:        return "JC";
        case Opcode::JNC:       return "JNC";
        case Opcode::JO:        return "JO";
        case Opcode::JNO:       return "JNO";
        case Opcode::JG:        return "JG";
        case Opcode::JL:        return "JL";
        case Opcode::JGE:       return "JGE";
        case Opcode::JLE:       return "JLE";
        case Opcode::MOD:       return "MOD";
        case Opcode::MUL:       return "MUL";
        case Opcode::DIV:       return "DIV";
        case Opcode::INC:       return "INC";
        case Opcode::DEC:       return "DEC";
        case Opcode::AND:       return "AND";
        case Opcode::OR:        return "OR";
        case Opcode::XOR:       return "XOR";
        case Opcode::NOT:       return "NOT";
        case Opcode::SHL:       return "SHL";
        case Opcode::SHR:       return "SHR";
        case Opcode::CALL:      return "CALL";
        case Opcode::RET:       return "RET";
        case Opcode::PUSH_ARG:  return "PUSH_ARG";
        case Opcode::POP_ARG:   return "POP_ARG";
        case Opcode::PUSH_FLAG: return "PUSH_FLAG";
        case Opcode::POP_FLAG:  return "POP_FLAG";
        case Opcode::LEA:       return "LEA";
        case Opcode::SWAP:      return "SWAP";
        case Opcode::IN:        return "IN";
        case Opcode::OUT:       return "OUT";
        case Opcode::INB:       return "INB";
        case Opcode::OUTB:      return "OUTB";
        case Opcode::INW:       return "INW";
        case Opcode::OUTW:      return "OUTW";
        case Opcode::INL:       return "INL";
        case Opcode::OUTL:      return "OUTL";
        case Opcode::INSTR:     return "INSTR";
        case Opcode::OUTSTR:    return "OUTSTR";
        case Opcode::DB:        return "DB";
        case Opcode::LOADR:     return "LOADR";
        case Opcode::DEBUG:     return "DEBUG";
        case Opcode::STORER:    return "STORER";
        case Opcode::ADD64:     return "ADD64";
        case Opcode::SUB64:     return "SUB64";
        case Opcode::MOV64:     return "MOV64";
        case Opcode::LOAD_IMM64:return "LOAD_IMM64";
        case Opcode::MUL64:     return "MUL64";
        case Opcode::DIV64:     return "DIV64";
        case Opcode::AND64:     return "AND64";
        case Opcode::OR64:      return "OR64";
        case Opcode::XOR64:     return "XOR64";
        case Opcode::NOT64:     return "NOT64";
        case Opcode::SHL64:     return "SHL64";
        case Opcode::SHR64:     return "SHR64";
        case Opcode::CMP64:     return "CMP64";
        case Opcode::INC64:     return "INC64";
        case Opcode::DEC64:     return "DEC64";
        case Opcode::MOD64:     return "MOD64";
        case Opcode::MOVEX:     return "MOVEX";
        case Opcode::ADDEX:     return "ADDEX";
        case Opcode::SUBEX:     return "SUBEX";
        case Opcode::MULEX:     return "MULEX";
        case Opcode::DIVEX:     return "DIVEX";
        case Opcode::CMPEX:     return "CMPEX";
        case Opcode::LOADEX:    return "LOADEX";
        case Opcode::STOREX:    return "STOREX";
        case Opcode::PUSHEX:    return "PUSHEX";
        case Opcode::POPEX:     return "POPEX";
        case Opcode::MODE32:    return "MODE32";
        case Opcode::MODE64:    return "MODE64";
        case Opcode::MODECMP:   return "MODECMP";
        case Opcode::MODEFLAG:  return "MODEFLAG";
        case Opcode::VADD:      return "VADD";
        case Opcode::VMUL:      return "VMUL";
        case Opcode::VDOT:      return "VDOT";
        case Opcode::VMAX:      return "VMAX";
        case Opcode::VBROADCAST:return "VBROADCAST";
        case Opcode::VCMPGT:    return "VCMPGT";
        case Opcode::PACKB:     return "PACKB";
        case Opcode::UNPACKB:   return "UNPACKB";
        case Opcode::HALT:      return "HALT";
        default:                return "UNKNOWN";
    }
}

void OpcodeProfiler::print_summary() const {
    auto hotspots = get_hotspots(10);

    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PROFILER] Total executions: {}", total_count_),
        Logging::DebugLevel::INFO);

    if (hotspots.empty()) {
        Logging::DebugHandler::instance().report(
            Logging::DebugCategory::CPU_EXECUTION,
            "[PROFILER] No opcode executions recorded",
            Logging::DebugLevel::INFO);
        return;
    }

    std::string summary = "[PROFILER] Top hotspots:\n";
    for (const auto& h : hotspots) {
        summary += fmt::format("  {:<15} {:>10} ({:>5.1f}%)\n",
            opcode_name_from_byte(h.opcode), h.count, h.percentage);
    }

    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::CPU_EXECUTION,
        summary,
        Logging::DebugLevel::INFO);
}

void OpcodeProfiler::print_detailed() const {
    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("[PROFILER] Detailed opcode execution counts (total: {}):", total_count_),
        Logging::DebugLevel::INFO);

    std::string detailed;
    for (size_t i = 0; i < NUM_OPCODES; i++) {
        if (counts_[i] > 0) {
            detailed += fmt::format("  0x{:02X} {:<15} {:>10}\n",
                i, opcode_name_from_byte(static_cast<uint8_t>(i)), counts_[i]);
        }
    }

    if (detailed.empty()) {
        detailed = "  No executions recorded\n";
    }

    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::CPU_EXECUTION,
        detailed,
        Logging::DebugLevel::INFO);
}
