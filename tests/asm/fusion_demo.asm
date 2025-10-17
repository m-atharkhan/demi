# Simple program to test instruction fusion
# Run with: ./bin/demi-engine -d -A tests/asm/fusion_demo.asm

# Pattern 1: LOAD_IMM + ADD (should fuse)
LOAD_IMM R0, 10
ADD R0, R1      # Fusion: immediate add

# Pattern 2: LOAD_IMM + SUB (should fuse)
LOAD_IMM R2, 20
SUB R2, R3      # Fusion: immediate sub

# Pattern 3: CMP + JZ (should fuse)
LOAD_IMM R4, 5
CMP R0, R4
JZ skip_section # Fusion: compare and branch

# This section gets skipped
LOAD_IMM R5, 99

skip_section:
# Pattern 4: Regular instructions (no fusion)
MOV R6, R0
INC R6

HALT
