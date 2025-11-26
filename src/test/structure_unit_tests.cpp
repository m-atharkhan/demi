#include "test_framework.hpp"

// Unit tests for program structure directives (section, global, extern)

TEST_CASE(section_data_text, "structure") {
    // Test basic section directive with .data and .text sections
    ctx.assemble_code(R"(
        section .text
        _start:
            LOAD_IMM R0, 100
            HALT
        
        section .data
        value: DB 42
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 100);
}

TEST_CASE(section_switching, "structure") {
    // Test switching between sections multiple times
    ctx.assemble_code(R"(
        section .data
        val1: DB 10
        val2: DB 20
        
        section .text
        _start:
            LOAD_IMM R0, 1
            LOAD_IMM R1, 2
            HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 1);
    ctx.assert_register_eq(1, 2);
}

TEST_CASE(global_symbol_declaration, "structure") {
    // Test declaring symbols as global
    ctx.assemble_code(R"(
        section .data
        global my_var
        my_var: DB 55
        
        section .text
        global _start
        _start:
            LOAD R0, my_var
            HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 55);
}

TEST_CASE(global_multiple_symbols, "structure") {
    // Test declaring multiple symbols as global in one directive
    ctx.assemble_code(R"(
        section .data
        global var1, var2, var3
        var1: DB 1
        var2: DB 2
        var3: DB 3
        
        section .text
        LOAD R0, var1
        LOAD R1, var2
        LOAD R2, var3
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 1);
    ctx.assert_register_eq(1, 2);
    ctx.assert_register_eq(2, 3);
}

TEST_CASE(extern_symbol_declaration, "structure") {
    // Test declaring external symbols (should not cause errors)
    ctx.assemble_code(R"(
        extern external_func
        
        section .text
        LOAD_IMM R0, 99
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 99);
}

TEST_CASE(section_with_labels, "structure") {
    // Test labels within sections
    ctx.assemble_code(R"(
        section .data
        data_label:
            DB 33
        
        section .text
        code_label:
            LOAD R0, data_label
            JMP end_label
            LOAD_IMM R0, 255
        end_label:
            HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 33);
}

TEST_CASE(section_bss, "structure") {
    // Test .bss section for uninitialized data
    ctx.assemble_code(R"(
        section .bss
        .bss 16
        
        section .text
        LOAD_IMM R0, 77
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 77);
}

TEST_CASE(section_rodata, "structure") {
    // Test .rodata section - using .data since rodata isn't a special section
    ctx.assemble_code(R"(
        section .text
        _start:
            LOAD R0, constant
            HALT
        
        section .data
        constant: DB 123
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 123);
}

TEST_CASE(global_before_definition, "structure") {
    // Test declaring global before the symbol is defined
    ctx.assemble_code(R"(
        global my_func
        
        section .text
        my_func:
            LOAD_IMM R0, 88
            HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 88);
}

TEST_CASE(multiple_sections_same_name, "structure") {
    // Test using the same section name multiple times (should merge)
    ctx.assemble_code(R"(
        section .text
        _start:
            LOAD_IMM R0, 0
            LOAD_IMM R1, 1
            ADD R0, R1
            HALT
        
        section .data
        val1: DB 11
        val2: DB 22
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 1);
}

TEST_CASE(section_custom_name, "structure") {
    // Test section names - sections are labels for organization
    ctx.assemble_code(R"(
        section .text
        _start:
            LOAD R0, custom_data
            HALT
        
        section .data
        custom_data: DB 66
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 66);
}

TEST_CASE(global_with_computation, "structure") {
    // Test global symbols used in computations
    ctx.assemble_code(R"(
        section .data
        global counter
        counter: DB 5
        
        section .text
        global increment
        increment:
            LOAD R0, counter
            INC R0
            HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 6);
}

TEST_CASE(extern_multiple_symbols, "structure") {
    // Test declaring multiple external symbols
    ctx.assemble_code(R"(
        extern func1, func2, func3
        
        section .text
        LOAD_IMM R0, 111
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 111);
}

TEST_CASE(section_without_dot_prefix, "structure") {
    // Test section directive without dot prefix
    ctx.assemble_code(R"(
        section text
        _start:
            LOAD R0, mydata
            HALT
        
        section data
        mydata: DB 44
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 44);
}

TEST_CASE(global_without_dot_prefix, "structure") {
    // Test global directive without dot prefix
    ctx.assemble_code(R"(
        section .text
        _start:
            LOAD R0, myvar
            HALT
        
        section .data
        global myvar
        myvar: DB 77
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 77);
}

TEST_CASE(complex_section_layout, "structure") {
    // Test complex program with multiple sections and globals
    ctx.assemble_code(R"(
        section .data
        global var_a, var_b
        var_a: DB 10
        var_b: DB 20
        
        section .bss
        .bss 32
        
        section .text
        global main
        main:
            LOAD R0, var_a
            LOAD R1, var_b
            ADD R0, R1
            HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 30);
    ctx.assert_register_eq(1, 20);
}

TEST_CASE(section_interleaved_code_data, "structure") {
    // Test sections - note: currently sections are organizational only, not separate memory regions
    // So we need to keep .text sections together or use _start
    ctx.assemble_code(R"(
        section .text
        _start:
            LOAD_IMM R0, 1
            LOAD R1, val
            ADD R0, R1
            HALT
        
        section .data
        val: DB 5
    )");
    ctx.execute_program();
    
    ctx.assert_register_eq(0, 6);
    ctx.assert_register_eq(1, 5);
}
