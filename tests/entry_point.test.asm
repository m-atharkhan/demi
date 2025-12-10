; Test entry point functionality

.test "entry_point_default" {
    .description "Test default entry point (_start)"
    .author "bobrossrtx"
    .category "Assembler"
    
    section .text
    
    _start:
        LOAD_IMM R0, 42
        HALT
    
    other_label:
        LOAD_IMM R0, 99
        HALT
    
    .assert_reg R0, 42
}

.test "entry_point_explicit" {
    .description "Entry point should be first instruction if _start doesn't exist"
    .author "bobrossrtx"
    .category "Assembler"
    
    LOAD_IMM R0, 123
    HALT
    
    .assert_reg R0, 123
}

.test "entry_point_directive_label" {
    .description "Test .entry_point directive with label"
    .author "bobrossrtx"
    .category "Assembler"
    .entry_point custom_start
    
    _start:
        LOAD_IMM R0, 1
        HALT
        
    custom_start:
        LOAD_IMM R0, 2
        HALT
        
    .assert_reg R0, 2
}

.test "entry_point_directive_address" {
    .description "Test .entry_point directive with numeric address"
    .author "bobrossrtx"
    .category "Assembler"
    .entry_point 0x10
    
    .org 0x00
    _start:
        LOAD_IMM R0, 1
        HALT
        
    .org 0x10
    custom_start:
        LOAD_IMM R0, 3
        HALT
        
    .assert_reg R0, 3
}
