.test "Memdump Test" {
    .description "Verify memdump output"
    .author "bobrossrtx"
    .category "Memory"
    
    ; Write some recognizable pattern to memory
    LOAD_IMM EAX, 0x41 ; 'A'
    STORE EAX, 0x100
    LOAD_IMM EAX, 0x42 ; 'B'
    STORE EAX, 0x101
    LOAD_IMM EAX, 0x43 ; 'C'
    STORE EAX, 0x102
    
    halt
}
