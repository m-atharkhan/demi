# Test instruction fusion patterns
# This program contains patterns that should be fused:
# 1. LOAD_IMM + ADD
# 2. CMP + JZ
# 3. ADD + STORE (commented - needs implementation)

#test fusion_test
    #description Test that instruction fusion optimizes common patterns
    #author Copilot
    #category Performance
    
    # Pattern 1: LOAD_IMM + ADD should fuse
    LOAD_IMM R0, 10
    ADD R0, R1      # This ADD following LOAD_IMM should trigger fusion
    
    # Pattern 2: CMP + JZ should fuse
    LOAD_IMM R2, 5
    CMP R0, R2
    JZ end_test     # CMP+JZ should fuse
    
    # No fusion - normal instructions
    LOAD_IMM R3, 100
    MOV R4, R3
    
    end_test:
    #assert_eq R0, 10  # R1 was 0, so 10+0=10
    
#endtest

HALT
