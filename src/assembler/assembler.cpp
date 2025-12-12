#include <iostream>
#include <ios>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include "assembler.hpp"
#include "opcodes.hpp"
#include "../debug/error_handler.hpp"
#include "../debug/debug_handler.hpp"
#include "../config.hpp"
#include "../../extern/fmt/include/fmt/format.h"

namespace Assembler {
uint32_t AssemblerEngine::db_next_addr = 0x100;

AssemblerEngine::AssemblerEngine() : current_address(0) {
    init_opcode_table();
    init_register_table();
}

AssemblerEngine::~AssemblerEngine() {
    DEBUG_INFO(Logging::DebugCategory::ASM_PARSING, "AssemblerEngine destructor called, clearing forward_refs with {} entries", forward_refs.size());
    forward_refs.clear();
}

void AssemblerEngine::init_opcode_table() {
    // Map mnemonics to opcodes based on DemiEngine CPU instruction set
    mnemonic_to_opcode["NOP"] = static_cast<uint8_t>(Opcode::NOP);
    mnemonic_to_opcode["LOAD_IMM"] = static_cast<uint8_t>(Opcode::LOAD_IMM);
    mnemonic_to_opcode["ADD"] = static_cast<uint8_t>(Opcode::ADD);
    mnemonic_to_opcode["SUB"] = static_cast<uint8_t>(Opcode::SUB);
    mnemonic_to_opcode["MOV"] = static_cast<uint8_t>(Opcode::MOV);
    mnemonic_to_opcode["JMP"] = static_cast<uint8_t>(Opcode::JMP);
    mnemonic_to_opcode["LOAD"] = static_cast<uint8_t>(Opcode::LOAD);
    mnemonic_to_opcode["LOADR"] = static_cast<uint8_t>(Opcode::LOADR);
    mnemonic_to_opcode["STORER"] = static_cast<uint8_t>(Opcode::STORER);
    mnemonic_to_opcode["STORE"] = static_cast<uint8_t>(Opcode::STORE);
    mnemonic_to_opcode["PUSH"] = static_cast<uint8_t>(Opcode::PUSH);
    mnemonic_to_opcode["POP"] = static_cast<uint8_t>(Opcode::POP);
    mnemonic_to_opcode["CMP"] = static_cast<uint8_t>(Opcode::CMP);
    mnemonic_to_opcode["JZ"] = static_cast<uint8_t>(Opcode::JZ);
    mnemonic_to_opcode["JE"] = static_cast<uint8_t>(Opcode::JZ);   // Alias for JZ
    mnemonic_to_opcode["JNZ"] = static_cast<uint8_t>(Opcode::JNZ);
    mnemonic_to_opcode["JNE"] = static_cast<uint8_t>(Opcode::JNZ); // Alias for JNZ
    mnemonic_to_opcode["JS"] = static_cast<uint8_t>(Opcode::JS);
    mnemonic_to_opcode["JNS"] = static_cast<uint8_t>(Opcode::JNS);
    mnemonic_to_opcode["JC"] = static_cast<uint8_t>(Opcode::JC);
    mnemonic_to_opcode["JNC"] = static_cast<uint8_t>(Opcode::JNC);
    mnemonic_to_opcode["JO"] = static_cast<uint8_t>(Opcode::JO);
    mnemonic_to_opcode["JNO"] = static_cast<uint8_t>(Opcode::JNO);
    mnemonic_to_opcode["JG"] = static_cast<uint8_t>(Opcode::JG);
    mnemonic_to_opcode["JL"] = static_cast<uint8_t>(Opcode::JL);
    mnemonic_to_opcode["JGE"] = static_cast<uint8_t>(Opcode::JGE);
    mnemonic_to_opcode["JLE"] = static_cast<uint8_t>(Opcode::JLE);
    mnemonic_to_opcode["MUL"] = static_cast<uint8_t>(Opcode::MUL);
    mnemonic_to_opcode["DIV"] = static_cast<uint8_t>(Opcode::DIV);
    mnemonic_to_opcode["MOD"] = static_cast<uint8_t>(Opcode::MOD);
    mnemonic_to_opcode["INC"] = static_cast<uint8_t>(Opcode::INC);
    mnemonic_to_opcode["DEC"] = static_cast<uint8_t>(Opcode::DEC);
    mnemonic_to_opcode["AND"] = static_cast<uint8_t>(Opcode::AND);
    mnemonic_to_opcode["OR"] = static_cast<uint8_t>(Opcode::OR);
    mnemonic_to_opcode["XOR"] = static_cast<uint8_t>(Opcode::XOR);
    mnemonic_to_opcode["NOT"] = static_cast<uint8_t>(Opcode::NOT);
    mnemonic_to_opcode["SHL"] = static_cast<uint8_t>(Opcode::SHL);
    mnemonic_to_opcode["SHR"] = static_cast<uint8_t>(Opcode::SHR);
    mnemonic_to_opcode["CALL"] = static_cast<uint8_t>(Opcode::CALL);
    mnemonic_to_opcode["RET"] = static_cast<uint8_t>(Opcode::RET);
    mnemonic_to_opcode["PUSH_ARG"] = static_cast<uint8_t>(Opcode::PUSH_ARG);
    mnemonic_to_opcode["POP_ARG"] = static_cast<uint8_t>(Opcode::POP_ARG);
    mnemonic_to_opcode["PUSH_FLAG"] = static_cast<uint8_t>(Opcode::PUSH_FLAG);
    mnemonic_to_opcode["POP_FLAG"] = static_cast<uint8_t>(Opcode::POP_FLAG);
    mnemonic_to_opcode["LEA"] = static_cast<uint8_t>(Opcode::LEA);
    mnemonic_to_opcode["SWAP"] = static_cast<uint8_t>(Opcode::SWAP);
    mnemonic_to_opcode["IN"] = static_cast<uint8_t>(Opcode::IN);
    mnemonic_to_opcode["OUT"] = static_cast<uint8_t>(Opcode::OUT);
    mnemonic_to_opcode["INB"] = static_cast<uint8_t>(Opcode::INB);
    mnemonic_to_opcode["OUTB"] = static_cast<uint8_t>(Opcode::OUTB);
    mnemonic_to_opcode["INW"] = static_cast<uint8_t>(Opcode::INW);
    mnemonic_to_opcode["OUTW"] = static_cast<uint8_t>(Opcode::OUTW);
    mnemonic_to_opcode["INL"] = static_cast<uint8_t>(Opcode::INL);
    mnemonic_to_opcode["OUTL"] = static_cast<uint8_t>(Opcode::OUTL);
    mnemonic_to_opcode["INSTR"] = static_cast<uint8_t>(Opcode::INSTR);
    mnemonic_to_opcode["OUTSTR"] = static_cast<uint8_t>(Opcode::OUTSTR);
    mnemonic_to_opcode["DB"] = static_cast<uint8_t>(Opcode::DB);
    mnemonic_to_opcode["DEBUG"] = static_cast<uint8_t>(Opcode::DEBUG);
    
    // Interrupt operations
    mnemonic_to_opcode["INT"] = static_cast<uint8_t>(Opcode::INT);
    mnemonic_to_opcode["IRET"] = static_cast<uint8_t>(Opcode::IRET);
    mnemonic_to_opcode["CLI"] = static_cast<uint8_t>(Opcode::CLI);
    mnemonic_to_opcode["STI"] = static_cast<uint8_t>(Opcode::STI);
    
    mnemonic_to_opcode["HALT"] = static_cast<uint8_t>(Opcode::HALT);

    // Extended operations
    mnemonic_to_opcode["ADD64"] = static_cast<uint8_t>(Opcode::ADD64);
    mnemonic_to_opcode["SUB64"] = static_cast<uint8_t>(Opcode::SUB64);
    mnemonic_to_opcode["MOV64"] = static_cast<uint8_t>(Opcode::MOV64);
    mnemonic_to_opcode["LOAD_IMM64"] = static_cast<uint8_t>(Opcode::LOAD_IMM64);
    mnemonic_to_opcode["MUL64"] = static_cast<uint8_t>(Opcode::MUL64);
    mnemonic_to_opcode["DIV64"] = static_cast<uint8_t>(Opcode::DIV64);
    mnemonic_to_opcode["AND64"] = static_cast<uint8_t>(Opcode::AND64);
    mnemonic_to_opcode["OR64"] = static_cast<uint8_t>(Opcode::OR64);
    mnemonic_to_opcode["XOR64"] = static_cast<uint8_t>(Opcode::XOR64);
    mnemonic_to_opcode["SHL64"] = static_cast<uint8_t>(Opcode::SHL64);
    mnemonic_to_opcode["SHR64"] = static_cast<uint8_t>(Opcode::SHR64);
    mnemonic_to_opcode["CMP64"] = static_cast<uint8_t>(Opcode::CMP64);
    mnemonic_to_opcode["NOT64"] = static_cast<uint8_t>(Opcode::NOT64);
    mnemonic_to_opcode["INC64"] = static_cast<uint8_t>(Opcode::INC64);
    mnemonic_to_opcode["DEC64"] = static_cast<uint8_t>(Opcode::DEC64);
    mnemonic_to_opcode["MOD64"] = static_cast<uint8_t>(Opcode::MOD64);
    mnemonic_to_opcode["MOVEX"] = static_cast<uint8_t>(Opcode::MOVEX);
    mnemonic_to_opcode["ADDEX"] = static_cast<uint8_t>(Opcode::ADDEX);
    mnemonic_to_opcode["SUBEX"] = static_cast<uint8_t>(Opcode::SUBEX);
    mnemonic_to_opcode["MULEX"] = static_cast<uint8_t>(Opcode::MULEX);
    mnemonic_to_opcode["DIVEX"] = static_cast<uint8_t>(Opcode::DIVEX);
    mnemonic_to_opcode["CMPEX"] = static_cast<uint8_t>(Opcode::CMPEX);
    mnemonic_to_opcode["LOADEX"] = static_cast<uint8_t>(Opcode::LOADEX);
    mnemonic_to_opcode["STOREX"] = static_cast<uint8_t>(Opcode::STOREX);
    mnemonic_to_opcode["PUSHEX"] = static_cast<uint8_t>(Opcode::PUSHEX);
    mnemonic_to_opcode["POPEX"] = static_cast<uint8_t>(Opcode::POPEX);
    mnemonic_to_opcode["MODE32"] = static_cast<uint8_t>(Opcode::MODE32);
    mnemonic_to_opcode["MODE64"] = static_cast<uint8_t>(Opcode::MODE64);
    mnemonic_to_opcode["MODECMP"] = static_cast<uint8_t>(Opcode::MODECMP);

    // FPU Operations
    mnemonic_to_opcode["FINIT"] = static_cast<uint8_t>(Opcode::FINIT);
    mnemonic_to_opcode["FLD"] = static_cast<uint8_t>(Opcode::FLD);
    mnemonic_to_opcode["FST"] = static_cast<uint8_t>(Opcode::FST);
    mnemonic_to_opcode["FSTP"] = static_cast<uint8_t>(Opcode::FSTP);
    mnemonic_to_opcode["FADD"] = static_cast<uint8_t>(Opcode::FADD);
    mnemonic_to_opcode["FSUB"] = static_cast<uint8_t>(Opcode::FSUB);
    mnemonic_to_opcode["FMUL"] = static_cast<uint8_t>(Opcode::FMUL);
    mnemonic_to_opcode["FDIV"] = static_cast<uint8_t>(Opcode::FDIV);
    mnemonic_to_opcode["FILD"] = static_cast<uint8_t>(Opcode::FILD);
    mnemonic_to_opcode["FIST"] = static_cast<uint8_t>(Opcode::FIST);
    mnemonic_to_opcode["FISTP"] = static_cast<uint8_t>(Opcode::FISTP);
    mnemonic_to_opcode["FABS"] = static_cast<uint8_t>(Opcode::FABS);
    mnemonic_to_opcode["FCHS"] = static_cast<uint8_t>(Opcode::FCHS);
    mnemonic_to_opcode["FSQRT"] = static_cast<uint8_t>(Opcode::FSQRT);
    mnemonic_to_opcode["FSIN"] = static_cast<uint8_t>(Opcode::FSIN);
    mnemonic_to_opcode["FCOS"] = static_cast<uint8_t>(Opcode::FCOS);
    mnemonic_to_opcode["FTAN"] = static_cast<uint8_t>(Opcode::FTAN);
    mnemonic_to_opcode["FCOMPP"] = static_cast<uint8_t>(Opcode::FCOMPP);
    mnemonic_to_opcode["FUCOMPP"] = static_cast<uint8_t>(Opcode::FUCOMPP);
    mnemonic_to_opcode["FCLEX"] = static_cast<uint8_t>(Opcode::FCLEX);
    mnemonic_to_opcode["FSTCW"] = static_cast<uint8_t>(Opcode::FSTCW);
    mnemonic_to_opcode["FLDCW"] = static_cast<uint8_t>(Opcode::FLDCW);
    mnemonic_to_opcode["FSTSW"] = static_cast<uint8_t>(Opcode::FSTSW);
    
    // Simple SIMD Operations (custom opcodes 0xD4-0xDF)
    mnemonic_to_opcode["VADD"] = 0xD4;
    mnemonic_to_opcode["VMUL"] = 0xD5;
    mnemonic_to_opcode["VDOT"] = 0xD6;
    mnemonic_to_opcode["VMAX"] = 0xD7;
    mnemonic_to_opcode["VBROADCAST"] = 0xD8;
    mnemonic_to_opcode["VCMPGT"] = 0xD9;
    mnemonic_to_opcode["PACKB"] = 0xDA;
    mnemonic_to_opcode["UNPACKB"] = 0xDB;
}

void AssemblerEngine::init_register_table() {
    // Legacy register names (R0-R7)
    for (int i = 0; i < 8; ++i) {
        register_to_number["R" + std::to_string(i)] = i;
        register_sizes["R" + std::to_string(i)] = 32; // Default to 32-bit for legacy
    }

    // x64-style register names
    register_to_number["RAX"] = 0; register_sizes["RAX"] = 64;
    register_to_number["RCX"] = 1; register_sizes["RCX"] = 64;
    register_to_number["RDX"] = 2; register_sizes["RDX"] = 64;
    register_to_number["RBX"] = 3; register_sizes["RBX"] = 64;
    register_to_number["RSP"] = 4; register_sizes["RSP"] = 64;
    register_to_number["RBP"] = 5; register_sizes["RBP"] = 64;
    register_to_number["RSI"] = 6; register_sizes["RSI"] = 64;
    register_to_number["RDI"] = 7; register_sizes["RDI"] = 64;

    // Extended registers R8-R15
    for (int i = 8; i < 16; ++i) {
        register_to_number["R" + std::to_string(i)] = i;
        register_sizes["R" + std::to_string(i)] = 64; // R8-R15 are 64-bit

        register_to_number["R" + std::to_string(i) + "D"] = i; // 32-bit alias
        register_sizes["R" + std::to_string(i) + "D"] = 32;
    }

    // Special registers (mapped to extended register numbers)
    register_to_number["RIP"] = 16; register_sizes["RIP"] = 64;
    register_to_number["RFLAGS"] = 17; register_sizes["RFLAGS"] = 64;

    // 32-bit register aliases (for x86 compatibility)
    register_to_number["EAX"] = 0; register_sizes["EAX"] = 32;
    register_to_number["ECX"] = 1; register_sizes["ECX"] = 32;
    register_to_number["EDX"] = 2; register_sizes["EDX"] = 32;
    register_to_number["EBX"] = 3; register_sizes["EBX"] = 32;
    register_to_number["ESP"] = 4; register_sizes["ESP"] = 32;
    register_to_number["EBP"] = 5; register_sizes["EBP"] = 32;
    register_to_number["ESI"] = 6; register_sizes["ESI"] = 32;
    register_to_number["EDI"] = 7; register_sizes["EDI"] = 32;
}

std::vector<uint8_t> AssemblerEngine::assemble(const Program& program) {
    // Clear previous state
    errors.clear();
    symbol_table.clear();
    forward_refs.clear();
    bytecode.clear();
    // Do not reset current_address; let directives set it as needed

    // Two-pass assembly
    db_next_addr = 0x100; // Only reset once here
    first_pass(program);
    if (has_errors()) return {};

    // Do NOT reset db_next_addr in second_pass or first_pass
    second_pass(program);
    if (has_errors()) return {};

    resolve_forward_references();
    if (has_errors()) return {};

    return bytecode;
}

void AssemblerEngine::first_pass(const Program& program) {
    current_address = 0;
    use_simple_db_format = false; // Reset flag
    
    // Detect DB format: count DB instructions and check for .org
    size_t db_count = 0;
    bool has_org = false;
    
    for (const auto& stmt : program.statements) {
        if (stmt->type == ASTNodeType::INSTRUCTION) {
            const auto& instruction = static_cast<const Instruction&>(*stmt);
            if (instruction.mnemonic == "DB" || instruction.mnemonic == "RESB") {
                db_count++;
            }
        } else if (stmt->type == ASTNodeType::DIRECTIVE) {
            const auto& directive = static_cast<const Directive&>(*stmt);
            if (directive.name == ".org") {
                has_org = true;
            }
        }
    }
    
    // Use simple format for multiple DBs or with .org
    use_simple_db_format = (db_count > 1) || has_org;

    // First, process all instructions/directives to update current_address and track highest address
    uint32_t highest_address = 0;
    for (size_t i = 0; i < program.statements.size(); ++i) {
        const auto& stmt = program.statements[i];
        // DB/RESB: skip size accounting, address is explicit or auto
        if ((stmt->type == ASTNodeType::INSTRUCTION && 
             (static_cast<const Instruction&>(*stmt).mnemonic == "DB" ||
              static_cast<const Instruction&>(*stmt).mnemonic == "RESB")) ||
            (stmt->type == ASTNodeType::DIRECTIVE && static_cast<const Directive&>(*stmt).name == ".db")) {
            continue;
        }
        // Normal instruction/directive size logic
        if (stmt->type == ASTNodeType::INSTRUCTION) {
            const auto& instruction = static_cast<const Instruction&>(*stmt);
            size_t size = get_instruction_size(instruction.mnemonic, instruction.operands);
            current_address += size;
            if (current_address > highest_address) highest_address = current_address;
        } else if (stmt->type == ASTNodeType::DIRECTIVE) {
            const auto& directive = static_cast<const Directive&>(*stmt);
            if (directive.name == ".org") {
                handle_org_directive(directive.arguments);
                if (current_address > highest_address) highest_address = current_address;
            } else if (directive.name == ".equ") {
                handle_equ_directive(directive.arguments);
                // .equ doesn't change current_address
            } else if (directive.name == ".dw") {
                current_address += directive.arguments.size() * 2;
                if (current_address > highest_address) highest_address = current_address;
            } else if (directive.name == ".dd") {
                current_address += directive.arguments.size() * 4;
                if (current_address > highest_address) highest_address = current_address;
            } else if (directive.name == ".string") {
                for (const auto& arg : directive.arguments) {
                    if (auto str_lit = dynamic_cast<const StringLiteralExpression*>(arg.get())) {
                        current_address += str_lit->value.length() + 1;
                    }
                }
                if (current_address > highest_address) highest_address = current_address;
            } else if (directive.name == ".align") {
                if (directive.arguments.size() == 1) {
                    bool is_symbol;
                    std::string symbol_name;
                    int64_t alignment = evaluate_expression(*directive.arguments[0], is_symbol, symbol_name);
                    if (!is_symbol && alignment > 0 && (alignment & (alignment - 1)) == 0) {
                        uint32_t align_mask = static_cast<uint32_t>(alignment - 1);
                        current_address = (current_address + align_mask) & ~align_mask;
                        if (current_address > highest_address) highest_address = current_address;
                    }
                }
            } else if (directive.name == ".bss" || directive.name == ".resb" || directive.name == "RESB") {
                if (directive.arguments.size() == 1) {
                    bool is_symbol;
                    std::string symbol_name;
                    int64_t bytes_to_reserve = evaluate_expression(*directive.arguments[0], is_symbol, symbol_name);
                    if (!is_symbol && bytes_to_reserve >= 0) {
                        current_address += static_cast<uint32_t>(bytes_to_reserve);
                        if (current_address > highest_address) highest_address = current_address;
                    }
                }
            } else if (directive.name == ".data") {
                handle_data_section();
            } else if (directive.name == ".text") {
                handle_text_section();
            }
        }
    }
    // Now set db_next_addr to highest_address, so DB data is placed after code and buffer zones
    db_next_addr = highest_address;
    // DB labels will be fixed during second pass when actual data locations are known
}

void Assembler::AssemblerEngine::second_pass(const Assembler::Program& program) {
    current_address = 0;
    bytecode.clear();
    entry_address = 0;
    std::string last_label_name; // Track the most recent label for DB assignment

    bool first_instruction_address_set = false;
    uint32_t first_instruction_address = 0;
    
    for (const auto& stmt : program.statements) {
        if (stmt->type == Assembler::ASTNodeType::DIRECTIVE) {
            const auto& directive = static_cast<const Assembler::Directive&>(*stmt);
            process_directive(directive);
        } else if (stmt->type == Assembler::ASTNodeType::LABEL) {
            const auto& label = static_cast<const Assembler::Label&>(*stmt);
            process_label(label); // Process the label to add it to symbol table
            last_label_name = label.name; // Remember this label for potential DB assignment
        } else if (stmt->type == Assembler::ASTNodeType::INSTRUCTION) {
            const auto& instruction = static_cast<const Instruction&>(*stmt);
            if (instruction.mnemonic != "DB") {
                // Pad bytecode to current_address before emitting
                while (bytecode.size() < current_address) {
                    bytecode.push_back(0);
                }
                // Track first instruction address for fallback
                if (!first_instruction_address_set) {
                    first_instruction_address = bytecode.size();
                    first_instruction_address_set = true;
                }
            }
            process_instruction(instruction);
        }
    }
    
    // After processing all statements, resolve entry point
    auto entry_it = symbol_table.find(entry_point_symbol);
    if (entry_it != symbol_table.end() && entry_it->second.defined) {
        // Use the specified entry point symbol if it exists and is defined
        entry_address = entry_it->second.address;
    } else if (first_instruction_address_set) {
        // Fall back to first instruction
        entry_address = first_instruction_address;
    } else {
        // No instructions at all - default to 0
        entry_address = 0;
    }
}

void Assembler::AssemblerEngine::process_label(const Assembler::Label& label) {
    if (symbol_table.find(label.name) != symbol_table.end()) {
        // If it exists, check if it was declared global/extern but not defined
        if (symbol_table[label.name].defined) {
            add_error("Label '" + label.name + "' already defined", label.line, label.column);
            return;
        }
        // It was declared (e.g. global), now we define it
        symbol_table[label.name].address = current_address;
        symbol_table[label.name].defined = true;
        symbol_table[label.name].section = current_section;
    } else {
        symbol_table[label.name] = Symbol(label.name, current_address, true);
        symbol_table[label.name].section = current_section;
    }
    last_label_name = label.name; // Remember this label for potential DB assignment
}

void Assembler::AssemblerEngine::process_directive(const Assembler::Directive& directive) {
    if (directive.name == ".memory") {
        handle_memory_directive(directive.arguments);
    } else if (directive.name == ".equ") {
        // EQU is already processed in first_pass, skip it here
        return;
    } else if (false) {
        // .db directive removed - use DB instruction instead
    } else if (directive.name == ".dw") {
        handle_dw_directive(directive.arguments);
    } else if (directive.name == ".dd") {
        handle_dd_directive(directive.arguments);
    } else if (directive.name == ".string") {
        handle_string_directive(directive.arguments);
    } else if (directive.name == ".align") {
        handle_align_directive(directive.arguments);
    } else if (directive.name == ".bss") {
        handle_bss_directive(directive.arguments);
    } else if (directive.name == ".resb" || directive.name == "RESB") {
        handle_bss_directive(directive.arguments); // RESB works the same as .bss
    } else if (directive.name == "section" || directive.name == ".section") {
        handle_section_directive(directive.arguments);
    } else if (directive.name == "global" || directive.name == ".global") {
        handle_global_directive(directive.arguments);
    } else if (directive.name == "extern" || directive.name == ".extern") {
        handle_extern_directive(directive.arguments);
    } else if (directive.name == ".data") {
        handle_data_section();
    } else if (directive.name == ".text") {
        handle_text_section();
    } else if (directive.name == ".org") {
    Logging::DebugHandler::instance().report(Logging::DebugCategory::ASM_DIRECTIVE, "Processing .org directive", Logging::DebugLevel::DETAIL);
        handle_org_directive(directive.arguments);
    DEBUG_INFO(Logging::DebugCategory::ASM_DIRECTIVE, "After .org directive, current_address: 0x{:04X}", current_address);
    } else {
        add_error("Unknown directive: " + directive.name, directive.line, directive.column);
    }
}

void Assembler::AssemblerEngine::process_instruction(const Assembler::Instruction& instruction) {
    encode_instruction(instruction);
}

void Assembler::AssemblerEngine::encode_instruction(const Assembler::Instruction& instruction) {
    std::string mnemonic = instruction.mnemonic;

    // Auto-detect 64-bit operations based on register sizes
    bool use_64bit = false;
    for (const auto& operand : instruction.operands) {
        if (auto reg_expr = dynamic_cast<const Assembler::RegisterExpression*>(operand.get())) {
            if (register_sizes[reg_expr->name] == 64) {
                use_64bit = true;
                break;
            }
        }
    }

    if (use_64bit) {
        if (mnemonic == "MOV") mnemonic = "MOV64";
        else if (mnemonic == "ADD") mnemonic = "ADD64";
        else if (mnemonic == "SUB") mnemonic = "SUB64";
        else if (mnemonic == "MUL") mnemonic = "MUL64";
        else if (mnemonic == "DIV") mnemonic = "DIV64";
        else if (mnemonic == "AND") mnemonic = "AND64";
        else if (mnemonic == "OR") mnemonic = "OR64";
        else if (mnemonic == "XOR") mnemonic = "XOR64";
        else if (mnemonic == "SHL") mnemonic = "SHL64";
        else if (mnemonic == "SHR") mnemonic = "SHR64";
        else if (mnemonic == "CMP") mnemonic = "CMP64";
        else if (mnemonic == "NOT") mnemonic = "NOT64";
        else if (mnemonic == "INC") mnemonic = "INC64";
        else if (mnemonic == "DEC") mnemonic = "DEC64";
        else if (mnemonic == "MOD") mnemonic = "MOD64";
        else if (mnemonic == "LOAD_IMM") mnemonic = "LOAD_IMM64";
        else if (mnemonic == "LOAD") mnemonic = "LOADEX";
        else if (mnemonic == "STORE") mnemonic = "STOREEX";
    }

    // Pad bytecode to current_address before emitting instruction
    while (bytecode.size() < current_address) {
        bytecode.push_back(0);
    }
    Logging::DebugHandler::instance().report(Logging::DebugCategory::ASM_ENCODING, fmt::format("Emitting instruction '{}' at address: 0x{:04X}", mnemonic, current_address), Logging::DebugLevel::DETAIL);
    
    // Handle DB pseudo-instruction (no opcode emission, just raw bytes)
    if (mnemonic == "DB") {
        if (instruction.operands.empty()) {
            add_error("DB requires at least one operand", instruction.line, instruction.column);
            return;
        }

        // Check if first operand is a string literal
        if (auto str_expr = dynamic_cast<const Assembler::StringLiteralExpression*>(instruction.operands[0].get())) {
            // String-based DB: emit string bytes, then optional additional operands
            std::string str_value = str_expr->value;
            
            // Emit string characters
            for (char c : str_value) {
                emit_byte(static_cast<uint8_t>(c));
            }
            
            // Emit any additional operands (length, terminator, etc.) - all optional
            for (size_t i = 1; i < instruction.operands.size(); ++i) {
                bool is_symbol;
                std::string symbol_name;
                int64_t value = evaluate_expression(*instruction.operands[i], is_symbol, symbol_name);
                
                if (is_symbol) {
                    emit_forward_ref(symbol_name, 1);
                } else {
                    emit_byte(static_cast<uint8_t>(value));
                }
            }
        } else {
            // Simple byte list - emit each operand as a byte
            for (const auto& operand : instruction.operands) {
                bool is_symbol;
                std::string symbol_name;
                int64_t value = evaluate_expression(*operand, is_symbol, symbol_name);
                
                if (is_symbol) {
                    emit_forward_ref(symbol_name, 1);
                } else {
                    if (value < 0 || value > 255) {
                        add_error("DB byte value " + std::to_string(value) + 
                                  " out of range (must be 0-255)", instruction.line, instruction.column);
                        return;
                    }
                    emit_byte(static_cast<uint8_t>(value));
                }
            }
        }
        return; // DB doesn't emit an opcode, so return early
    }
    
    // Handle RESB pseudo-instruction (reserve bytes - emit zeros)
    if (mnemonic == "RESB") {
        if (instruction.operands.size() != 1) {
            add_error("RESB requires exactly one operand (number of bytes)", instruction.line, instruction.column);
            return;
        }
        
        bool is_symbol;
        std::string symbol_name;
        int64_t count = evaluate_expression(*instruction.operands[0], is_symbol, symbol_name);
        
        if (is_symbol) {
            add_error("RESB cannot use forward references", instruction.line, instruction.column);
            return;
        }
        
        if (count < 0) {
            add_error("RESB count must be non-negative", instruction.line, instruction.column);
            return;
        }
        
        // Emit 'count' zero bytes
        for (int64_t i = 0; i < count; i++) {
            emit_byte(0);
        }
        return; // RESB doesn't emit an opcode, so return early
    }
    
    uint8_t opcode = get_opcode(mnemonic);
    if (opcode == 0xFF && mnemonic != "HALT") {
        add_error("Unknown instruction: " + mnemonic, instruction.line, instruction.column);
        return;
    }

    // Don't emit opcode for instructions that handle it themselves (like MOV which can be LOAD_IMM/LOAD/etc)
    if (mnemonic != "MOV" && mnemonic != "MOV64" && mnemonic != "MOVEX") {
        emit_byte(opcode);
    }
    // Handle different instruction formats
    if (mnemonic == "DEBUG") {
        // Format: DEBUG sub_opcode, [operands...]
        if (instruction.operands.empty()) {
            add_error("DEBUG requires at least 1 operand (sub-opcode)", instruction.line, instruction.column);
            return;
        }
        
        // Sub-opcode (Immediate)
        bool is_symbol;
        std::string symbol_name;
        int64_t sub_opcode = evaluate_expression(*instruction.operands[0], is_symbol, symbol_name);
        emit_byte(static_cast<uint8_t>(sub_opcode));
        
        // Handle operands based on sub-opcode
        if (sub_opcode == 0) { // PRINT
             // Operand 2: String or Register
             if (instruction.operands.size() < 2) {
                 add_error("DEBUG PRINT requires a second operand", instruction.line, instruction.column);
                 return;
             }
             
             if (auto str_expr = dynamic_cast<const Assembler::StringLiteralExpression*>(instruction.operands[1].get())) {
                 emit_byte(1); // Type: String
                 std::string s = str_expr->value;
                 if (s.length() > 255) s = s.substr(0, 255); // Limit length
                 emit_byte(static_cast<uint8_t>(s.length()));
                 for (char c : s) emit_byte(c);
             } else if (auto reg_expr = dynamic_cast<const Assembler::RegisterExpression*>(instruction.operands[1].get())) {
                 emit_byte(0); // Type: Register
                 emit_byte(get_register_number(reg_expr->name));
             } else {
                 add_error("DEBUG PRINT operand must be string or register", instruction.line, instruction.column);
             }
        } else if (sub_opcode == 3) { // MEMDUMP
             // Start, Length
             if (instruction.operands.size() < 3) {
                 add_error("DEBUG MEMDUMP requires start and length", instruction.line, instruction.column);
                 return;
             }
             
             int64_t start = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);
             if (is_symbol) {
                 emit_forward_ref(symbol_name, 4);
             } else {
                 emit_dword(static_cast<uint32_t>(start));
             }
             
             int64_t len = evaluate_expression(*instruction.operands[2], is_symbol, symbol_name);
             emit_dword(static_cast<uint32_t>(len));
             
        } else if (sub_opcode == 4) { // TRACE
             // Optional operand
             if (instruction.operands.size() > 1) {
                 int64_t val = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);
                 emit_byte(static_cast<uint8_t>(val));
             } else {
                 emit_byte(2); // 2 = Toggle (default)
             }
        } else if (sub_opcode == 5) { // ASSERT
             // Register/memory and value
             if (instruction.operands.size() < 3) {
                 add_error("DEBUG ASSERT requires register/memory and value", instruction.line, instruction.column);
                 return;
             }
             if (auto reg_expr = dynamic_cast<const Assembler::RegisterExpression*>(instruction.operands[1].get())) {
                 emit_byte(0); // Type: Register
                 emit_byte(get_register_number(reg_expr->name));
             } else {
                 emit_byte(1); // Type: Memory
                 int64_t addr = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);
                 emit_dword(static_cast<uint32_t>(addr));
             }
             int64_t value = evaluate_expression(*instruction.operands[2], is_symbol, symbol_name);
             emit_qword(static_cast<uint64_t>(value));
        } else if (sub_opcode == 6) { // DUMPSTACK
             // Optional depth
             if (instruction.operands.size() > 1) {
                 int64_t depth = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);
                 emit_dword(static_cast<uint32_t>(depth));
             } else {
                 emit_dword(16); // Default depth
             }
        } else if (sub_opcode == 7) { // WATCH
             // Address and length
             if (instruction.operands.size() < 3) {
                 add_error("DEBUG WATCH requires address and length", instruction.line, instruction.column);
                 return;
             }
             int64_t addr = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);
             emit_dword(static_cast<uint32_t>(addr));
             int64_t len = evaluate_expression(*instruction.operands[2], is_symbol, symbol_name);
             emit_dword(static_cast<uint32_t>(len));
        } else if (sub_opcode == 8) { // UNWATCH
             // Address
             if (instruction.operands.size() < 2) {
                 add_error("DEBUG UNWATCH requires address", instruction.line, instruction.column);
                 return;
             }
             int64_t addr = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);
             emit_dword(static_cast<uint32_t>(addr));
        } else if (sub_opcode == 9) { // CHECKPOINT
             // Label string
             if (instruction.operands.size() < 2) {
                 add_error("DEBUG CHECKPOINT requires label", instruction.line, instruction.column);
                 return;
             }
             if (auto str_expr = dynamic_cast<const Assembler::StringLiteralExpression*>(instruction.operands[1].get())) {
                 std::string s = str_expr->value;
                 if (s.length() > 255) s = s.substr(0, 255);
                 emit_byte(static_cast<uint8_t>(s.length()));
                 for (char c : s) emit_byte(c);
             } else {
                 add_error("DEBUG CHECKPOINT label must be string", instruction.line, instruction.column);
             }
        } else if (sub_opcode == 10) { // LOG
             // Level and message
             if (instruction.operands.size() < 3) {
                 add_error("DEBUG LOG requires level and message", instruction.line, instruction.column);
                 return;
             }
             int64_t level = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);
             emit_byte(static_cast<uint8_t>(level));
             if (auto str_expr = dynamic_cast<const Assembler::StringLiteralExpression*>(instruction.operands[2].get())) {
                 std::string s = str_expr->value;
                 if (s.length() > 255) s = s.substr(0, 255);
                 emit_byte(static_cast<uint8_t>(s.length()));
                 for (char c : s) emit_byte(c);
             } else {
                 add_error("DEBUG LOG message must be string", instruction.line, instruction.column);
             }
        } else if (sub_opcode == 11) { // DUMPREG
             // Single register
             if (instruction.operands.size() < 2) {
                 add_error("DEBUG DUMPREG requires register", instruction.line, instruction.column);
                 return;
             }
             if (auto reg_expr = dynamic_cast<const Assembler::RegisterExpression*>(instruction.operands[1].get())) {
                 emit_byte(get_register_number(reg_expr->name));
             } else {
                 add_error("DEBUG DUMPREG operand must be register", instruction.line, instruction.column);
             }
        } else if (sub_opcode == 12) { // MEMSET
             // Address, length, value
             if (instruction.operands.size() < 4) {
                 add_error("DEBUG MEMSET requires address, length, and value", instruction.line, instruction.column);
                 return;
             }
             int64_t addr = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);
             emit_dword(static_cast<uint32_t>(addr));
             int64_t len = evaluate_expression(*instruction.operands[2], is_symbol, symbol_name);
             emit_dword(static_cast<uint32_t>(len));
             int64_t value = evaluate_expression(*instruction.operands[3], is_symbol, symbol_name);
             emit_byte(static_cast<uint8_t>(value));
        } else if (sub_opcode == 13) { // STEP
             // Optional count
             if (instruction.operands.size() > 1) {
                 int64_t count = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);
                 emit_dword(static_cast<uint32_t>(count));
             } else {
                 emit_dword(1); // Default: single step
             }
        }
        return;
    }

    if (mnemonic == "LOAD_IMM") {
        // Format: LOAD_IMM reg, immediate
        // Supports 1-byte (8-bit regs) or 4-byte (32-bit regs) immediates based on register type
        if (instruction.operands.size() != 2) {
            add_error("LOAD_IMM requires 2 operands", instruction.line, instruction.column);
            return;
        }

        // Register operand
        std::string reg_name;
        if (auto reg_expr = dynamic_cast<const Assembler::RegisterExpression*>(instruction.operands[0].get())) {
            reg_name = reg_expr->name;
            emit_byte(get_register_number(reg_name));
        } else {
            add_error("First operand must be a register", instruction.line, instruction.column);
            return;
        }

        // Immediate operand - size depends on register type
        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);

        // Determine immediate size based on register size
        // Use register_sizes map to correctly identify 32-bit registers (including R0-R7 and EAX-EDI)
        bool is_32bit_reg = (register_sizes.find(reg_name) != register_sizes.end() && register_sizes[reg_name] == 32);
        size_t imm_size = is_32bit_reg ? 4 : 1;

        if (is_symbol) {
            emit_forward_ref(symbol_name, imm_size);
        } else {
            // Validate value range
            if (imm_size == 1) {
                if (value < 0 || value > 255) {
                    add_error("LOAD_IMM immediate value " + std::to_string(value) + 
                              " out of range for 8-bit register (must be 0-255)", 
                              instruction.line, instruction.column);
                    return;
                }
                emit_byte(static_cast<uint8_t>(value));
            } else { // 4-byte for 32-bit registers
                if (value < 0 || value > 0xFFFFFFFF) {
                    add_error("LOAD_IMM immediate value " + std::to_string(value) + 
                              " out of range for 32-bit register (must be 0-4294967295)", 
                              instruction.line, instruction.column);
                    return;
                }
                emit_dword(static_cast<uint32_t>(value));
            }
        }
    } else if (mnemonic == "LOAD_IMM64") {
        // Format: LOAD_IMM64 reg, immediate (64-bit)
        if (instruction.operands.size() != 2) {
            add_error("LOAD_IMM64 requires 2 operands", instruction.line, instruction.column);
            return;
        }

        // Register operand
        if (auto reg_expr = dynamic_cast<const Assembler::RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("First operand must be a register", instruction.line, instruction.column);
            return;
        }

        // Immediate operand (64-bit)
        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 8); // LOAD_IMM64 uses 8-byte immediate
        } else {
            // Emit 8 bytes little-endian
            for (int i = 0; i < 8; ++i) {
                emit_byte(static_cast<uint8_t>((value >> (8 * i)) & 0xFF));
            }
        }
    } else if (mnemonic == "MOV" || mnemonic == "MOV64" || mnemonic == "MOVEX") {
        if (instruction.operands.size() != 2) {
            add_error(mnemonic + " requires 2 operands", instruction.line, instruction.column);
            return;
        }

        auto dst = instruction.operands[0].get();
        auto src = instruction.operands[1].get();

        if (auto dst_reg = dynamic_cast<const Assembler::RegisterExpression*>(dst)) {
            uint8_t dst_reg_num = get_register_number(dst_reg->name);
            bool is_extended = (dst_reg_num >= 8 && dst_reg_num <= 15);
            
            if (auto src_reg = dynamic_cast<const Assembler::RegisterExpression*>(src)) {
                uint8_t src_reg_num = get_register_number(src_reg->name);
                bool src_extended = (src_reg_num >= 8 && src_reg_num <= 15);
                
                if (is_extended || src_extended) {
                    emit_byte(static_cast<uint8_t>(Opcode::MOVEX));
                } else if (register_sizes[dst_reg->name] == 64 || register_sizes[src_reg->name] == 64) {
                    emit_byte(static_cast<uint8_t>(Opcode::MOV64));
                } else {
                    emit_byte(static_cast<uint8_t>(Opcode::MOV));
                }
                emit_byte(dst_reg_num);
                emit_byte(src_reg_num);
            } else if (dynamic_cast<const Assembler::ImmediateExpression*>(src) || 
                       dynamic_cast<const Assembler::IdentifierExpression*>(src)) {
                
                int reg_size = register_sizes[dst_reg->name];
                // DEBUG_INFO(Logging::DebugCategory::ASM_PARSING, "MOV dst={} size={}", dst_reg->name, reg_size);
                
                if (reg_size >= 32) { // Use LOAD_IMM64 for 32-bit and 64-bit registers
                    emit_byte(static_cast<uint8_t>(Opcode::LOAD_IMM64));
                    emit_byte(dst_reg_num);
                    bool is_symbol;
                    std::string symbol_name;
                    int64_t val = evaluate_expression(*src, is_symbol, symbol_name);
                    if (is_symbol) emit_forward_ref(symbol_name, 8);
                    else for(int i=0; i<8; ++i) emit_byte((uint8_t)((val >> (i*8)) & 0xFF));
                } else {
                    // 8-bit register
                    emit_byte(static_cast<uint8_t>(Opcode::LOAD_IMM));
                    emit_byte(dst_reg_num);
                    bool is_symbol;
                    std::string symbol_name;
                    int64_t val = evaluate_expression(*src, is_symbol, symbol_name);
                    if (is_symbol) emit_forward_ref(symbol_name, 1);
                    else emit_byte((uint8_t)val);
                }
            } else if (auto mem_expr = dynamic_cast<const Assembler::MemoryReferenceExpression*>(src)) {
                if (dynamic_cast<const Assembler::RegisterExpression*>(mem_expr->base.get())) {
                    emit_byte(static_cast<uint8_t>(Opcode::LOADR));
                    emit_byte(dst_reg_num);
                    auto base_reg = static_cast<const Assembler::RegisterExpression*>(mem_expr->base.get());
                    emit_byte(get_register_number(base_reg->name));
                } else {
                    if (is_extended) emit_byte(static_cast<uint8_t>(Opcode::LOADEX));
                    else emit_byte(static_cast<uint8_t>(Opcode::LOAD));
                    emit_byte(dst_reg_num);
                    bool is_symbol;
                    std::string symbol_name;
                    int64_t val = evaluate_expression(*mem_expr->base, is_symbol, symbol_name);
                    if (is_extended) {
                        if (is_symbol) emit_forward_ref(symbol_name, 8);
                        else for(int i=0; i<8; ++i) emit_byte((uint8_t)((val >> (i*8)) & 0xFF));
                    } else {
                        if (is_symbol) emit_forward_ref(symbol_name, 4);
                        else emit_dword((uint32_t)val);
                    }
                }
            } else {
                add_error("Invalid source operand for MOV", instruction.line, instruction.column);
                return;
            }
        } else if (auto dst_mem = dynamic_cast<const Assembler::MemoryReferenceExpression*>(dst)) {
             if (auto src_reg = dynamic_cast<const Assembler::RegisterExpression*>(src)) {
                 uint8_t src_reg_num = get_register_number(src_reg->name);
                 bool is_extended = (src_reg_num >= 8 && src_reg_num <= 15);
                 if (dynamic_cast<const Assembler::RegisterExpression*>(dst_mem->base.get())) {
                     emit_byte(static_cast<uint8_t>(Opcode::STORER));
                     emit_byte(src_reg_num);
                     auto base_reg = static_cast<const Assembler::RegisterExpression*>(dst_mem->base.get());
                     emit_byte(get_register_number(base_reg->name));
                 } else {
                     if (is_extended) emit_byte(static_cast<uint8_t>(Opcode::STOREX));
                     else emit_byte(static_cast<uint8_t>(Opcode::STORE));
                     emit_byte(src_reg_num);
                     bool is_symbol;
                     std::string symbol_name;
                     int64_t val = evaluate_expression(*dst_mem->base, is_symbol, symbol_name);
                     if (is_extended) {
                         if (is_symbol) emit_forward_ref(symbol_name, 8);
                         else for(int i=0; i<8; ++i) emit_byte((uint8_t)((val >> (i*8)) & 0xFF));
                     } else {
                         if (is_symbol) emit_forward_ref(symbol_name, 4);
                         else emit_dword((uint32_t)val);
                     }
                 }
             } else {
                 add_error("MOV to memory requires register source", instruction.line, instruction.column);
                 return;
             }
        } else {
            add_error("Invalid destination operand for MOV", instruction.line, instruction.column);
            return;
        }
    } else if (mnemonic == "ADD" || mnemonic == "SUB" ||
               mnemonic == "CMP" ||
               mnemonic == "MUL" || mnemonic == "DIV" ||
               mnemonic == "MOD" || mnemonic == "AND" ||
               mnemonic == "OR" || mnemonic == "XOR" ||
               mnemonic == "ADD64" || mnemonic == "SUB64" ||
               mnemonic == "CMP64" || mnemonic == "MODECMP" ||
               mnemonic == "ADDEX" ||
               mnemonic == "SUBEX" || mnemonic == "CMPEX") {
        // Format: INSTRUCTION reg1, reg2
        if (instruction.operands.size() != 2) {
            add_error(mnemonic + " requires 2 operands", instruction.line, instruction.column);
            return;
        }

        for (size_t i = 0; i < instruction.operands.size(); ++i) {
            const auto& operand = instruction.operands[i];
            if (auto reg_expr = dynamic_cast<const Assembler::RegisterExpression*>(operand.get())) {
                emit_byte(get_register_number(reg_expr->name));
            } else {
                // Debug: print what type of operand we got
                std::string operand_type = "unknown";
                if (dynamic_cast<const Assembler::ImmediateExpression*>(operand.get())) {
                    operand_type = "immediate";
                } else if (dynamic_cast<const Assembler::IdentifierExpression*>(operand.get())) {
                    operand_type = "identifier";
                } else if (dynamic_cast<const Assembler::StringLiteralExpression*>(operand.get())) {
                    operand_type = "string";
                }
                add_error(mnemonic + " operand " + std::to_string(i+1) + " must be a register (got " + operand_type + ")", 
                         instruction.line, instruction.column);
                return;
            }
        }
    } else if (mnemonic == "MODE32" || mnemonic == "MODE64") {
        // Format: MODE32 or MODE64 (no operands)
        if (instruction.operands.size() != 0) {
            add_error(mnemonic + " requires no operands", instruction.line, instruction.column);
            return;
        }
        // Opcode already emitted above, no additional bytes needed
    } else if (mnemonic == "JMP" || mnemonic == "JZ" || mnemonic == "JE" ||
               mnemonic == "JNZ" || mnemonic == "JNE" || mnemonic == "JS" ||
               mnemonic == "JNS" || mnemonic == "JC" ||
               mnemonic == "JNC" || mnemonic == "JO" ||
               mnemonic == "JNO" || mnemonic == "JG" ||
               mnemonic == "JL" || mnemonic == "JGE" ||
               mnemonic == "JLE" || mnemonic == "CALL") {
        // Format: JUMP address
        if (instruction.operands.size() != 1) {
            add_error(mnemonic + " requires 1 operand", instruction.line, instruction.column);
            return;
        }

        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*instruction.operands[0], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 4); // DemiEngine uses 32-bit addresses for jumps
        } else {
            emit_dword(static_cast<uint32_t>(value));
        }
    } else if (mnemonic == "PUSH" || mnemonic == "POP" ||
               mnemonic == "INC" || mnemonic == "DEC" ||
               mnemonic == "NOT" || mnemonic == "INC64" ||
               mnemonic == "DEC64" || mnemonic == "NOT64") {
        // Format: INSTRUCTION reg
        if (instruction.operands.size() != 1) {
            add_error(mnemonic + " requires 1 operand", instruction.line, instruction.column);
            return;
        }

        if (auto reg_expr = dynamic_cast<const Assembler::RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("Operand must be a register", instruction.line, instruction.column);
            return;
        }
    } else if (mnemonic == "OUT" || mnemonic == "IN" ||
               mnemonic == "OUTB" || mnemonic == "INB" ||
               mnemonic == "OUTW" || mnemonic == "INW" ||
               mnemonic == "OUTL" || mnemonic == "INL" ||
               mnemonic == "OUTSTR" || mnemonic == "INSTR") {
        // Format: OUT reg, port  or  IN reg, port
        if (instruction.operands.size() != 2) {
            add_error(mnemonic + " requires 2 operands", instruction.line, instruction.column);
            return;
        }

        // Register operand
        if (auto reg_expr = dynamic_cast<const Assembler::RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("First operand must be a register", instruction.line, instruction.column);
            return;
        }

        // Port operand (immediate value)
        bool is_symbol;
        std::string symbol_name;
        int64_t port_value = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);

        if (is_symbol) {
            add_error("Port number must be an immediate value", instruction.line, instruction.column);
            return;
        } else {
            emit_byte(static_cast<uint8_t>(port_value));
        }
    } else if (mnemonic == "LOAD" || mnemonic == "LOADR" || mnemonic == "STORER" || mnemonic == "STORE" ||
               mnemonic == "LEA" || mnemonic == "SWAP") {
        // Format: LOAD reg, addr  or  LOADR reg, reg  or  STORER reg, reg  or  STORE reg, addr
        if (instruction.operands.size() != 2) {
            add_error(mnemonic + " requires 2 operands", instruction.line, instruction.column);
            return;
        }

        // Register operand
        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("First operand must be a register", instruction.line, instruction.column);
            return;
        }

        // Second operand: For LOADR/STORER it's a register, for others it's an address
        if (mnemonic == "LOADR" || mnemonic == "STORER") {
            // Second operand must be a register
            if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[1].get())) {
                emit_byte(get_register_number(reg_expr->name));
            } else {
                add_error("Second operand must be a register for " + mnemonic, instruction.line, instruction.column);
                return;
            }
        } else {
            // Address operand
            bool is_symbol;
            std::string symbol_name;
            int64_t addr_value = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);

            if (is_symbol) {
                emit_forward_ref(symbol_name, 4); // Use 32-bit addresses
            } else {
                emit_dword(static_cast<uint32_t>(addr_value));
            }
        }
    } else if (mnemonic == "LOADEX" || mnemonic == "STOREX") {
        // Format: LOADEX/STOREX reg, 64-bit_address
        if (instruction.operands.size() != 2) {
            add_error(mnemonic + " requires 2 operands", instruction.line, instruction.column);
            return;
        }

        // Register operand
        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("First operand must be a register", instruction.line, instruction.column);
            return;
        }

        // Address operand (8 bytes)
        bool is_symbol;
        std::string symbol_name;
        int64_t addr_value = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 8); // 8-byte address for extended operations
        } else {
            // Emit 8-byte address in little-endian format
            for (int i = 0; i < 8; i++) {
                emit_byte(static_cast<uint8_t>((addr_value >> (i * 8)) & 0xFF));
            }
        }
    } else if (mnemonic == "SHL" || mnemonic == "SHR") {
        // Format: SHL reg, immediate
        if (instruction.operands.size() != 2) {
            add_error(mnemonic + " requires 2 operands", instruction.line, instruction.column);
            return;
        }

        // Register operand
        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("First operand must be a register", instruction.line, instruction.column);
            return;
        }

        // Immediate operand
        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);

        if (is_symbol) {
            add_error("Shift amount must be an immediate value", instruction.line, instruction.column);
            return;
        } else {
            emit_byte(static_cast<uint8_t>(value));
        }
    } else if (mnemonic == "MUL64" || mnemonic == "DIV64" ||
               mnemonic == "AND64" || mnemonic == "OR64" ||
               mnemonic == "XOR64" || 
               mnemonic == "MOD64" || mnemonic == "MULEX" ||
               mnemonic == "DIVEX") {
        // Format: INSTRUCTION dest_reg, src_reg1, src_reg2 (3 operands)
        if (instruction.operands.size() != 3) {
            add_error(mnemonic + " requires 3 operands", instruction.line, instruction.column);
            return;
        }

        // Destination register (first operand)
        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("First operand must be a destination register", instruction.line, instruction.column);
            return;
        }

        // Source register 1 (second operand)
        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[1].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("Second operand must be a source register", instruction.line, instruction.column);
            return;
        }

        // Source register 2 (third operand)
        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[2].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("Third operand must be a source register", instruction.line, instruction.column);
            return;
        }
    } else if (mnemonic == "FLD" || mnemonic == "FST" || 
               mnemonic == "FSTP") {
        // FPU Load/Store instructions
        // Format: FLD <memory_addr> or FLD <immediate_double>
        // For simplicity, we'll support memory address operands
        if (instruction.operands.size() != 1) {
            add_error(mnemonic + " requires 1 operand", instruction.line, instruction.column);
            return;
        }

        // Check if it's an immediate value (for FLD only)
        if (mnemonic == "FLD") {
            if (auto imm_expr = dynamic_cast<const ImmediateExpression*>(instruction.operands[0].get())) {
                // FLD with immediate double value
                // Operand type 0x02 = immediate 64-bit double
                emit_byte(0x02);
                
                // Convert integer to double and emit as 64-bit value
                double double_val = static_cast<double>(imm_expr->value);
                uint64_t raw_double;
                std::memcpy(&raw_double, &double_val, sizeof(double));
                
                // Emit 8 bytes little-endian
                for (int i = 0; i < 8; ++i) {
                    emit_byte(static_cast<uint8_t>((raw_double >> (8 * i)) & 0xFF));
                }
                return;
            }
        }

        // Otherwise, treat as memory address
        // Operand type 0x01 = memory address (64-bit double)
        emit_byte(0x01);
        
        // Memory address operand
        bool is_symbol;
        std::string symbol_name;
        int64_t addr_value = evaluate_expression(*instruction.operands[0], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 4); // 32-bit address
        } else {
            // Emit 32-bit address
            emit_byte(static_cast<uint8_t>(addr_value & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 8) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 16) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 24) & 0xFF));
        }
    } else if (mnemonic == "FADD" || mnemonic == "FSUB" || 
               mnemonic == "FMUL" || mnemonic == "FDIV") {
        // FPU Arithmetic instructions
        // Format: FADD <memory_addr> or FADD <immediate_double>
        if (instruction.operands.size() != 1) {
            add_error(mnemonic + " requires 1 operand", instruction.line, instruction.column);
            return;
        }

        // Check if it's an immediate value
        if (auto imm_expr = dynamic_cast<const ImmediateExpression*>(instruction.operands[0].get())) {
            // Arithmetic with immediate double value
            // Operand type 0x02 = immediate 64-bit double
            emit_byte(0x02);
            
            // Convert integer to double and emit as 64-bit value
            double double_val = static_cast<double>(imm_expr->value);
            uint64_t raw_double;
            std::memcpy(&raw_double, &double_val, sizeof(double));
            
            // Emit 8 bytes little-endian
            for (int i = 0; i < 8; ++i) {
                emit_byte(static_cast<uint8_t>((raw_double >> (8 * i)) & 0xFF));
            }
            return;
        }

        // Otherwise, treat as memory address
        // Operand type 0x01 = memory address (64-bit double)
        emit_byte(0x01);
        
        // Memory address operand
        bool is_symbol;
        std::string symbol_name;
        int64_t addr_value = evaluate_expression(*instruction.operands[0], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 4); // 32-bit address
        } else {
            // Emit 32-bit address
            emit_byte(static_cast<uint8_t>(addr_value & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 8) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 16) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 24) & 0xFF));
        }
    } else if (instruction.mnemonic == "FILD") {
        // FILD - Load integer as floating point
        // Format: FILD <memory_addr> or FILD <immediate_int>
        if (instruction.operands.size() != 1) {
            add_error("FILD requires 1 operand", instruction.line, instruction.column);
            return;
        }

        // Check if it's an immediate value
        if (auto imm_expr = dynamic_cast<const ImmediateExpression*>(instruction.operands[0].get())) {
            // FILD with immediate 32-bit integer
            // Operand type 0x00 = immediate 32-bit integer
            emit_byte(0x00);
            
            // Emit 4 bytes little-endian
            int32_t int_val = static_cast<int32_t>(imm_expr->value);
            uint32_t raw_int = static_cast<uint32_t>(int_val);
            emit_byte(static_cast<uint8_t>(raw_int & 0xFF));
            emit_byte(static_cast<uint8_t>((raw_int >> 8) & 0xFF));
            emit_byte(static_cast<uint8_t>((raw_int >> 16) & 0xFF));
            emit_byte(static_cast<uint8_t>((raw_int >> 24) & 0xFF));
            return;
        }

        // Otherwise, treat as memory address
        // Operand type 0x01 = memory address (32-bit integer)
        emit_byte(0x01);
        
        // Memory address operand
        bool is_symbol;
        std::string symbol_name;
        int64_t addr_value = evaluate_expression(*instruction.operands[0], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 4); // 32-bit address
        } else {
            // Emit 32-bit address
            emit_byte(static_cast<uint8_t>(addr_value & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 8) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 16) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 24) & 0xFF));
        }
    } else if (instruction.mnemonic == "FIST" || instruction.mnemonic == "FISTP") {
        // FIST/FISTP - Store floating point as integer
        // Format: FIST <memory_addr> or FISTP <memory_addr>
        if (instruction.operands.size() != 1) {
            add_error(instruction.mnemonic + " requires 1 operand", instruction.line, instruction.column);
            return;
        }

        // Must be a memory address (no immediate for store operations)
        // Operand type 0x01 = memory address (32-bit integer)
        emit_byte(0x01);
        
        // Memory address operand
        bool is_symbol;
        std::string symbol_name;
        int64_t addr_value = evaluate_expression(*instruction.operands[0], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 4); // 32-bit address
        } else {
            // Emit 32-bit address
            emit_byte(static_cast<uint8_t>(addr_value & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 8) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 16) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 24) & 0xFF));
        }
    } else if (instruction.mnemonic == "FSTCW" || instruction.mnemonic == "FLDCW") {
        // FSTCW/FLDCW - Store/Load FPU control word
        // Format: FSTCW <memory_addr> or FLDCW <memory_addr>
        if (instruction.operands.size() != 1) {
            add_error(instruction.mnemonic + " requires 1 operand", instruction.line, instruction.column);
            return;
        }

        // Memory address operand (no operand type byte for these)
        bool is_symbol;
        std::string symbol_name;
        int64_t addr_value = evaluate_expression(*instruction.operands[0], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 4); // 32-bit address
        } else {
            // Emit 32-bit address directly
            emit_byte(static_cast<uint8_t>(addr_value & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 8) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 16) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 24) & 0xFF));
        }
    } else if (instruction.mnemonic == "FSTSW") {
        // FSTSW - Store FPU status word
        // Format: FSTSW <memory_addr> or FSTSW AX
        if (instruction.operands.size() != 1) {
            add_error("FSTSW requires 1 operand", instruction.line, instruction.column);
            return;
        }

        // Check if operand is AX register (R0 in our case)
        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[0].get())) {
            if (reg_expr->name == "R0" || reg_expr->name == "AX") {
                // Store to AX register
                emit_byte(0x01); // operand type = register
                return;
            }
        }

        // Otherwise, store to memory address
        emit_byte(0x00); // operand type = memory
        
        bool is_symbol;
        std::string symbol_name;
        int64_t addr_value = evaluate_expression(*instruction.operands[0], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 4); // 32-bit address
        } else {
            // Emit 32-bit address
            emit_byte(static_cast<uint8_t>(addr_value & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 8) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 16) & 0xFF));
            emit_byte(static_cast<uint8_t>((addr_value >> 24) & 0xFF));
        }
    } else if (instruction.mnemonic == "NOP" || instruction.mnemonic == "HALT" || 
               instruction.mnemonic == "RET" || instruction.mnemonic == "PUSH_FLAG" || 
               instruction.mnemonic == "POP_FLAG" || instruction.mnemonic == "FINIT" ||
               instruction.mnemonic == "FABS" || instruction.mnemonic == "FCHS" ||
               instruction.mnemonic == "FSQRT" || instruction.mnemonic == "FSIN" ||
               instruction.mnemonic == "FCOS" || instruction.mnemonic == "FTAN" ||
               instruction.mnemonic == "FEXP" || instruction.mnemonic == "FLN" ||
               instruction.mnemonic == "FLG2" || instruction.mnemonic == "FLDPI" ||
               instruction.mnemonic == "FLDZ" || instruction.mnemonic == "FLD1" ||
               // Interrupt instructions (no operands)
               instruction.mnemonic == "CLI" || instruction.mnemonic == "STI" ||
               instruction.mnemonic == "IRET" ||
               // SIMD instructions
               instruction.mnemonic == "VADD" || instruction.mnemonic == "VMUL" || 
               instruction.mnemonic == "VDOT" || instruction.mnemonic == "VMAX" || 
               instruction.mnemonic == "VBROADCAST" || instruction.mnemonic == "VCMPGT" ||
               instruction.mnemonic == "PACKB" || instruction.mnemonic == "UNPACKB") {
        // Instructions with no operands - opcode only
        if (instruction.operands.size() != 0) {
            add_error(instruction.mnemonic + " requires no operands", instruction.line, instruction.column);
            return;
        }
        // Opcode already emitted, no additional bytes needed
    } else if (instruction.mnemonic == "INT") {
        // INT instruction: opcode + vector byte
        if (instruction.operands.size() != 1) {
            add_error("INT requires exactly 1 operand (interrupt vector)", instruction.line, instruction.column);
            return;
        }
        
        bool is_symbol;
        std::string symbol;
        int64_t vector = evaluate_expression(*instruction.operands[0], is_symbol, symbol);
        
        if (is_symbol) {
            add_error("INT vector must be an immediate value, not a symbol", instruction.line, instruction.column);
            return;
        }
        
        if (vector < 0 || vector > 255) {
            add_error("INT vector must be between 0 and 255", instruction.line, instruction.column);
            return;
        }
        
        emit_byte(static_cast<uint8_t>(vector));
    } else {
        add_error("Instruction encoding not implemented: " + instruction.mnemonic, instruction.line, instruction.column);
    }
}

int64_t Assembler::AssemblerEngine::evaluate_expression(const Assembler::Expression& expr, bool& is_symbol_ref, std::string& symbol_name) {
    is_symbol_ref = false;
    symbol_name.clear();

    switch (expr.type) {
        case Assembler::ASTNodeType::IMMEDIATE: {
            const auto& imm = static_cast<const Assembler::ImmediateExpression&>(expr);
            return imm.value;
        }

        case Assembler::ASTNodeType::IDENTIFIER: {
            const auto& id = static_cast<const Assembler::IdentifierExpression&>(expr);
            auto it = symbol_table.find(id.name);
            if (it != symbol_table.end() && it->second.defined) {
                return it->second.address;
            } else {
                is_symbol_ref = true;
                symbol_name = id.name;
                return 0; // Will be resolved later
            }
        }
        
        case Assembler::ASTNodeType::MEMORY_REF: {
            const auto& mem = static_cast<const Assembler::MemoryReferenceExpression&>(expr);
            // Recursively evaluate the base expression inside the memory reference
            if (mem.offset) {
                // Handle [base + offset] later if needed
                add_error("Memory reference with offset not yet supported");
                return 0;
            }
            return evaluate_expression(*mem.base, is_symbol_ref, symbol_name);
        }

        case Assembler::ASTNodeType::REGISTER: {
            const auto& reg = static_cast<const Assembler::RegisterExpression&>(expr);
            return get_register_number(reg.name);
        }

        default:
            add_error("Cannot evaluate expression");
            return 0;
    }
}

uint8_t Assembler::AssemblerEngine::get_register_number(const std::string& reg_name) {
    auto it = register_to_number.find(reg_name);
    if (it != register_to_number.end()) {
        return it->second;
    }
    add_error("Unknown register: " + reg_name);
    return 0;
}

uint8_t Assembler::AssemblerEngine::get_opcode(const std::string& mnemonic) {
    auto it = mnemonic_to_opcode.find(mnemonic);
    if (it != mnemonic_to_opcode.end()) {
        return it->second;
    }
    return 0xFF; // Invalid opcode
}

void Assembler::AssemblerEngine::emit_byte(uint8_t byte) {
    bytecode.push_back(byte);
    current_address++;
}

void Assembler::AssemblerEngine::emit_word(uint16_t word) {
    emit_byte(word & 0xFF);
    emit_byte((word >> 8) & 0xFF);
}

void Assembler::AssemblerEngine::emit_dword(uint32_t dword) {
    emit_byte(dword & 0xFF);
    emit_byte((dword >> 8) & 0xFF);
    emit_byte((dword >> 16) & 0xFF);
    emit_byte((dword >> 24) & 0xFF);
}

void Assembler::AssemblerEngine::emit_qword(uint64_t qword) {
    emit_byte(qword & 0xFF);
    emit_byte((qword >> 8) & 0xFF);
    emit_byte((qword >> 16) & 0xFF);
    emit_byte((qword >> 24) & 0xFF);
    emit_byte((qword >> 32) & 0xFF);
    emit_byte((qword >> 40) & 0xFF);
    emit_byte((qword >> 48) & 0xFF);
    emit_byte((qword >> 56) & 0xFF);
}

void Assembler::AssemblerEngine::emit_forward_ref(const std::string& symbol, size_t size, bool relative) {
    forward_refs.push_back({current_address, symbol, size, relative});
        DEBUG_DETAIL(Logging::DebugCategory::ASM_FORWARD_REF, "emit_forward_ref: symbol='{}', address=0x{:04X}, size={}, relative={}", symbol, current_address, size, relative);

    // Emit placeholder bytes
    for (size_t i = 0; i < size; ++i) {
        emit_byte(0);
    }
}

size_t Assembler::AssemblerEngine::get_instruction_size(const std::string& mnemonic, const std::vector<std::unique_ptr<Assembler::Expression>>& operands) {
    // Basic instruction size calculations for Demi Engine
    if (mnemonic == "NOP" || mnemonic == "HALT" || mnemonic == "RET" ||
        mnemonic == "PUSH_FLAG" || mnemonic == "POP_FLAG" ||
        // Interrupt operations (no operands)
        mnemonic == "CLI" || mnemonic == "STI" || mnemonic == "IRET" ||
        // SIMD instructions with no operands
        mnemonic == "VADD" || mnemonic == "VMUL" || mnemonic == "VDOT" ||
        mnemonic == "VMAX" || mnemonic == "VBROADCAST" || mnemonic == "VCMPGT" ||
        mnemonic == "PACKB" || mnemonic == "UNPACKB") {
        return 1;
    } else if (mnemonic == "INT") {
        return 2; // opcode + vector byte
    } else if (mnemonic == "LOAD_IMM") {
        // Size depends on register type: 1-byte for 8-bit regs, 4-byte for 32-bit regs
        if (operands.size() >= 1) {
            if (auto reg_expr = dynamic_cast<const Assembler::RegisterExpression*>(operands[0].get())) {
                // Use register_sizes map to correctly identify 32-bit registers
                bool is_32bit_reg = (register_sizes.find(reg_expr->name) != register_sizes.end() && 
                                     register_sizes[reg_expr->name] == 32);
                return is_32bit_reg ? 6 : 3; // opcode + register + (4 or 1)-byte immediate
            }
        }
        return 3; // default: opcode + register + 1-byte immediate
    } else if (mnemonic == "LOAD_IMM64") {
        return 10; // opcode + register + 8-byte immediate
    } else if (mnemonic == "ADD" || mnemonic == "SUB" || mnemonic == "MOV" ||
               mnemonic == "CMP" || mnemonic == "MUL" || mnemonic == "DIV" ||
               mnemonic == "MOD" || mnemonic == "AND" || mnemonic == "OR" ||
               mnemonic == "XOR" || mnemonic == "ADD64" || mnemonic == "SUB64" ||
               mnemonic == "MOV64" || mnemonic == "CMP64" || mnemonic == "MUL64" ||
               mnemonic == "DIV64" || mnemonic == "MOD64" || mnemonic == "AND64" ||
               mnemonic == "OR64" || mnemonic == "XOR64") {
        return 3; // opcode + reg1 + reg2 (2-operand instructions)
    } else if (mnemonic == "MULEX" || mnemonic == "DIVEX") {
        return 4; // opcode + dest_reg + src_reg1 + src_reg2 (3-operand instructions)
    } else if (mnemonic == "JMP" || mnemonic == "JZ" || mnemonic == "JE" ||
               mnemonic == "JNZ" || mnemonic == "JNE" ||
               mnemonic == "JS" || mnemonic == "JNS" || mnemonic == "JC" ||
               mnemonic == "JNC" || mnemonic == "JO" || mnemonic == "JNO" ||
               mnemonic == "JG" || mnemonic == "JL" || mnemonic == "JGE" ||
               mnemonic == "JLE" || mnemonic == "CALL") {
        return 5; // opcode + 4-byte address
    } else if (mnemonic == "PUSH" || mnemonic == "POP" || mnemonic == "INC" ||
               mnemonic == "DEC" || mnemonic == "NOT" || mnemonic == "INC64" ||
               mnemonic == "DEC64" || mnemonic == "NOT64") {
        return 2; // opcode + register
    } else if (mnemonic == "OUT" || mnemonic == "IN" || mnemonic == "OUTB" ||
               mnemonic == "INB" || mnemonic == "OUTW" || mnemonic == "INW" ||
               mnemonic == "OUTL" || mnemonic == "INL" || mnemonic == "OUTSTR" ||
               mnemonic == "INSTR" || mnemonic == "LOAD" || mnemonic == "LOADR" ||
               mnemonic == "STORER" || mnemonic == "STORE" ||
               mnemonic == "LEA" || mnemonic == "SWAP" || mnemonic == "SHL" ||
               mnemonic == "SHR") {
        return 3; // opcode + register + address/port/immediate/register
    } else if (mnemonic == "LOADEX" || mnemonic == "STOREX") {
        return 10; // opcode + register + 8-byte address
    }

    return 1; // Default size
}

void Assembler::AssemblerEngine::handle_db_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    if (args.empty()) {
        add_error(".db directive requires at least one argument");
        return;
    }

    // Check if this is a simple byte list (like standard .db directive)
    bool all_immediates = true;
    for (const auto& arg : args) {
        if (!dynamic_cast<const Assembler::ImmediateExpression*>(arg.get())) {
            all_immediates = false;
            break;
        }
    }

    if (all_immediates) {
        // Standard .db directive: emit bytes directly
        for (const auto& arg : args) {
            bool is_symbol;
            std::string symbol_name;
            int64_t value = evaluate_expression(*arg, is_symbol, symbol_name);
            
            if (is_symbol) {
                emit_forward_ref(symbol_name, 1);
            } else {
                if (value < 0 || value > 255) {
                    add_error(".db byte value " + std::to_string(value) + " out of range (must be 0-255)");
                    return;
                }
                emit_byte(static_cast<uint8_t>(value));
            }
        }
        return;
    }

    // Original string-based DB handling (for compatibility with existing DB instruction tests)
    if (args.size() < 2) {
        add_error(".db directive requires at least a string and length, or immediate value(s)");
        return;
    }
    
    size_t str_idx = 0;
    size_t len_idx = 1;
    // Use static member db_next_addr
    uint32_t target_addr = 0;
    if (args.size() == 3) {
        // Explicit address
        size_t addr_idx = 2;
        if (auto addr_imm = dynamic_cast<const Assembler::ImmediateExpression*>(args[addr_idx].get())) {
            target_addr = static_cast<uint32_t>(addr_imm->value);
        } else {
            add_error(".db address must be an immediate value");
            return;
        }
    } else {
        // Auto address
        target_addr = AssemblerEngine::db_next_addr;
    }
    current_address = target_addr;
    while (bytecode.size() < current_address) {
        bytecode.push_back(0);
    }

    // Emit string (with escapes)
    size_t db_start = current_address;
    size_t db_len = 0;
    size_t actual_string_len = 0;
    if (auto str_lit = dynamic_cast<const Assembler::StringLiteralExpression*>(args[str_idx].get())) {
        actual_string_len = str_lit->value.length();
        for (char c : str_lit->value) {
            emit_byte(static_cast<uint8_t>(c));
            db_len++;
        }
        emit_byte(0); // Null terminator for OUTSTR
        db_len++;
    } else {
        add_error(".db first argument must be a string literal or all arguments must be byte values");
        return;
    }

    // Emit length
    bool is_symbol;
    std::string symbol_name;
    int64_t value = evaluate_expression(*args[len_idx], is_symbol, symbol_name);
    
    // Validate that specified length doesn't exceed actual string length (including null terminator)
    if (!is_symbol && value > static_cast<int64_t>(actual_string_len + 1)) {
        add_error(".db length (" + std::to_string(value) + 
                  ") exceeds actual string length (" + std::to_string(actual_string_len) + ")");
        return;
    }
    
    if (is_symbol) {
        emit_forward_ref(symbol_name, 1);
        db_len++;
    } else {
        emit_byte(static_cast<uint8_t>(value));
        db_len++;
    }
    // Set current_address to end of DB data
    current_address = db_start + db_len;
    // If auto address, increment db_next_addr
    if (args.size() == 2) {
        AssemblerEngine::db_next_addr += db_len;
    }
    
    // Fix any label that should point to this DB data
    // Look for the most recent label that might reference this DB
    if (!last_label_name.empty()) {
        // Update the symbol to point to where the actual string data starts (db_start)
        auto it = symbol_table.find(last_label_name);
        if (it != symbol_table.end()) {
            symbol_table[last_label_name] = Symbol(last_label_name, static_cast<uint32_t>(db_start), true);
        }
        last_label_name.clear(); // Clear after use
    }
}

void Assembler::AssemblerEngine::handle_dw_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    for (const auto& arg : args) {
        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*arg, is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 2);
        } else {
            emit_word(static_cast<uint16_t>(value));
        }
    }
}

void Assembler::AssemblerEngine::handle_dd_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    for (const auto& arg : args) {
        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*arg, is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 4);
        } else {
            emit_dword(static_cast<uint32_t>(value));
        }
    }
}

void Assembler::AssemblerEngine::handle_string_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    for (const auto& arg : args) {
        if (auto str_lit = dynamic_cast<const Assembler::StringLiteralExpression*>(arg.get())) {
            for (char c : str_lit->value) {
                emit_byte(static_cast<uint8_t>(c));
            }
            emit_byte(0); // Null terminator
        } else {
            add_error("String directive requires string literal");
        }
    }
}

void Assembler::AssemblerEngine::handle_equ_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    if (args.size() != 2) {
        add_error(".equ directive requires exactly two arguments: name and value");
        return;
    }

    // First argument should be an identifier (the constant name)
    auto ident_expr = dynamic_cast<const Assembler::IdentifierExpression*>(args[0].get());
    if (!ident_expr) {
        add_error(".equ directive: first argument must be an identifier");
        return;
    }

    std::string const_name = ident_expr->name;

    // Second argument is the value
    bool is_symbol;
    std::string symbol_name;
    int64_t value = evaluate_expression(*args[1], is_symbol, symbol_name);

    if (is_symbol) {
        add_error(".equ directive: cannot use forward references for constant value");
        return;
    }

    // Check if constant already exists
    if (symbol_table.find(const_name) != symbol_table.end() && symbol_table[const_name].defined) {
        add_error("Constant '" + const_name + "' already defined");
        return;
    }

    // Add constant to symbol table with the evaluated value
    symbol_table[const_name] = Symbol(const_name, static_cast<uint32_t>(value), true);
    symbol_table[const_name].section = current_section;
}

void Assembler::AssemblerEngine::handle_org_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    if (args.size() != 1) {
        add_error(".org directive requires exactly one argument");
        return;
    }

    bool is_symbol;
    std::string symbol_name;
    int64_t value = evaluate_expression(*args[0], is_symbol, symbol_name);

    if (is_symbol) {
        add_error(".org directive cannot use forward references");
        return;
    }

    uint32_t new_address = static_cast<uint32_t>(value);
    
    // Check if .org is moving backwards
    if (new_address < current_address) {
        add_error(".org directive cannot move address backwards from 0x" + 
                  std::to_string(current_address) + " to 0x" + std::to_string(new_address));
        return;
    }

    current_address = new_address;

    // Pad bytecode to the new address
    while (bytecode.size() < current_address) {
        bytecode.push_back(0);
    }
}

void Assembler::AssemblerEngine::handle_align_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    if (args.size() != 1) {
        add_error(".align directive requires exactly one argument (alignment boundary)");
        return;
    }

    bool is_symbol;
    std::string symbol_name;
    int64_t alignment = evaluate_expression(*args[0], is_symbol, symbol_name);

    if (is_symbol) {
        add_error(".align directive cannot use forward references");
        return;
    }

    if (alignment <= 0 || (alignment & (alignment - 1)) != 0) {
        add_error(".align directive requires a power of 2 alignment value (1, 2, 4, 8, 16, etc.)");
        return;
    }

    // Calculate aligned address
    uint32_t align_mask = static_cast<uint32_t>(alignment - 1);
    uint32_t aligned_address = (current_address + align_mask) & ~align_mask;

    // Pad with zeros to reach aligned address
    while (current_address < aligned_address) {
        emit_byte(0);
    }
}

void Assembler::AssemblerEngine::handle_bss_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    if (args.size() != 1) {
        add_error(".bss directive requires exactly one argument (number of bytes to reserve)");
        return;
    }

    bool is_symbol;
    std::string symbol_name;
    int64_t bytes_to_reserve = evaluate_expression(*args[0], is_symbol, symbol_name);

    if (is_symbol) {
        add_error(".bss directive cannot use forward references");
        return;
    }

    if (bytes_to_reserve < 0) {
        add_error(".bss directive requires a non-negative number of bytes");
        return;
    }

    // Reserve uninitialized space by advancing current_address and padding with zeros
    for (int64_t i = 0; i < bytes_to_reserve; i++) {
        emit_byte(0);
    }
}

void Assembler::AssemblerEngine::handle_memory_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    if (args.size() != 1) {
        add_error(".memory directive requires exactly one argument (memory size in bytes)");
        return;
    }

    bool is_symbol;
    std::string symbol_name;
    int64_t size = evaluate_expression(*args[0], is_symbol, symbol_name);

    if (is_symbol) {
        add_error(".memory directive cannot use forward references");
        return;
    }

    if (size < 256) {
        add_error(".memory directive requires at least 256 bytes");
        return;
    }

    if (size > 64 * 1024 * 1024) { // 64MB max
        add_error(".memory directive cannot exceed 64MB (67108864 bytes)");
        return;
    }

    memory_size = static_cast<size_t>(size);
    
    if (!Config::test_mode) {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::ASM_DIRECTIVE, fmt::format("Memory size set to {} bytes ({:.1f} KB)", 
                                                 memory_size, memory_size / 1024.0), Logging::DebugLevel::INFO);
    }
}

void Assembler::AssemblerEngine::resolve_forward_references() {
    for (const auto& ref : forward_refs) {
            DEBUG_DETAIL(Logging::DebugCategory::ASM_FORWARD_REF, "resolve_forward_references: symbol='{}', address=0x{:04X}, size={}, relative={}", ref.symbol, ref.address, ref.size, ref.relative);
        auto it = symbol_table.find(ref.symbol);
        if (it == symbol_table.end() || !it->second.defined) {
            add_error("Undefined symbol: " + ref.symbol);
            continue;
        }

        uint32_t address = it->second.address;
        if (ref.relative) {
            address = address - (ref.address + ref.size);
        }

        // Patch the bytecode
        if (ref.address + ref.size > bytecode.size()) {
            add_error("Forward reference out of bounds");
            continue;
        }

        for (size_t i = 0; i < ref.size; ++i) {
            bytecode[ref.address + i] = (address >> (8 * i)) & 0xFF;
        }
    }
}

void Assembler::AssemblerEngine::add_error(const std::string& message) {
    errors.push_back("Assembly error: " + message);
    Logging::ErrorHandler::instance().report_parse(Logging::ErrorCode::ASM_GENERIC, message);
}

void Assembler::AssemblerEngine::add_error(const std::string& message, size_t line, size_t column) {
    errors.push_back("Line " + std::to_string(line) + ", Column " + std::to_string(column) + ": " + message);
    Logging::ErrorHandler::instance().report_parse(Logging::ErrorCode::ASM_DIRECTIVE_ERROR, message, "", line, column);
}

void Assembler::AssemblerEngine::handle_section_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    if (args.empty()) {
        add_error("section directive requires a section name");
        return;
    }
    
    // Get section name
    std::string section_name;
    if (auto id = dynamic_cast<const IdentifierExpression*>(args[0].get())) {
        section_name = id->name;
    } else if (auto str = dynamic_cast<const StringLiteralExpression*>(args[0].get())) {
        section_name = str->value;
    } else {
        add_error("section directive requires an identifier or string");
        return;
    }
    
    current_section = section_name;
}

void Assembler::AssemblerEngine::handle_global_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    for (const auto& arg : args) {
        std::string symbol_name;
        if (auto id = dynamic_cast<const IdentifierExpression*>(arg.get())) {
            symbol_name = id->name;
        } else {
            add_error("global directive requires symbol identifiers");
            continue;
        }
        
        // Create or update symbol
        if (symbol_table.find(symbol_name) == symbol_table.end()) {
            symbol_table[symbol_name] = Symbol(symbol_name, 0, false);
        }
        symbol_table[symbol_name].is_global = true;
    }
}

void Assembler::AssemblerEngine::handle_extern_directive(const std::vector<std::unique_ptr<Assembler::Expression>>& args) {
    for (const auto& arg : args) {
        std::string symbol_name;
        if (auto id = dynamic_cast<const IdentifierExpression*>(arg.get())) {
            symbol_name = id->name;
        } else {
            add_error("extern directive requires symbol identifiers");
            continue;
        }
        
        // Create symbol marked as defined (externally) so we don't error on undefined reference
        if (symbol_table.find(symbol_name) == symbol_table.end()) {
            symbol_table[symbol_name] = Symbol(symbol_name, 0, true); // Mark defined to pass checks
            symbol_table[symbol_name].is_global = true;
        }
    }
}

void Assembler::AssemblerEngine::handle_data_section() {
    current_section = ".data";
}

void Assembler::AssemblerEngine::handle_text_section() {
    current_section = ".text";
}

Architecture Assembler::AssemblerEngine::detect_architecture(const Program& program) {
    // Scan program for architecture indicators
    for (const auto& stmt : program.statements) {
        if (stmt->type == ASTNodeType::DIRECTIVE) {
            const auto& directive = static_cast<const Directive&>(*stmt);
            if (directive.name == ".bits") {
                if (!directive.arguments.empty()) {
                    if (auto imm = dynamic_cast<const ImmediateExpression*>(directive.arguments[0].get())) {
                        if (imm->value == 64) return Architecture::X64;
                        if (imm->value == 32) return Architecture::X86;
                    }
                }
            }
        } else if (stmt->type == ASTNodeType::INSTRUCTION) {
            const auto& instruction = static_cast<const Instruction&>(*stmt);
            
            // Check for 64-bit specific instructions
            if (instruction.mnemonic == "MODE64") return Architecture::X64;
            
            // Check for 64-bit instruction suffixes
            if (instruction.mnemonic.length() > 2) {
                std::string suffix2 = instruction.mnemonic.substr(instruction.mnemonic.length() - 2);
                if (suffix2 == "64" || suffix2 == "EX") return Architecture::X64;
            }
            
            // Check operands for 64-bit registers
            for (const auto& operand : instruction.operands) {
                if (auto reg = dynamic_cast<const RegisterExpression*>(operand.get())) {
                    std::string name = reg->name;
                    // Check for R8-R15
                    if (name.length() >= 2 && name[0] == 'R') {
                        if (isdigit(name[1])) {
                            int num = std::stoi(name.substr(1));
                            if (num >= 8 && num <= 15) return Architecture::X64;
                        }
                    }
                    // Check for standard 64-bit registers
                    if (name == "RAX" || name == "RBX" || name == "RCX" || name == "RDX" ||
                        name == "RSI" || name == "RDI" || name == "RBP" || name == "RSP" ||
                        name == "RIP" || name == "RFLAGS") {
                        return Architecture::X64;
                    }
                }
            }
        }
    }
    
    // Default to X86 if no 64-bit features detected
    return Architecture::X86;
}

} // namespace Assembler
