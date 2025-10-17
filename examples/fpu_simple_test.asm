; Simple FPU Operations Test Program
; Tests basic floating-point operations in DemiEngine

; Test 1: Load floating-point values
LOAD_IMM64 R0, 0x4059000000000000  ; Load 100.0 (IEEE 754 double)
LOAD_IMM64 R1, 0x4024000000000000  ; Load 10.0 (IEEE 754 double)

; Test 2: FPU Load operations
FLD R0          ; Load 100.0 to FPU stack (ST0)
FLD R1          ; Load 10.0 to FPU stack (ST0), 100.0 -> ST1

; Test 3: FPU Arithmetic
FADD            ; ST0 = ST0 + ST1 = 10.0 + 100.0 = 110.0
FST R2          ; Store result to R2

FLD R0          ; Load 100.0 again
FLD R1          ; Load 10.0 again
FSUB            ; ST0 = ST0 - ST1 = 10.0 - 100.0 = -90.0
FST R3          ; Store result to R3

FLD R0          ; Load 100.0 again
FLD R1          ; Load 10.0 again
FMUL            ; ST0 = ST0 * ST1 = 10.0 * 100.0 = 1000.0
FST R4          ; Store result to R4

FLD R0          ; Load 100.0 again
FLD R1          ; Load 10.0 again
FDIV            ; ST0 = ST0 / ST1 = 10.0 / 100.0 = 0.1
FST R5          ; Store result to R5

; Test 4: Mathematical Functions
LOAD_IMM64 R6, 0x4040000000000000  ; Load 32.0
FLD R6          ; Load 32.0 to FPU stack
FSQRT           ; ST0 = sqrt(32.0) ≈ 5.657
FST R7          ; Store result to R7

; Test 5: Trigonometric Functions  
LOAD_IMM64 R8, 0x3FF921FB54442D18  ; Load π/2 (1.5707963267948966)
FLD R8          ; Load π/2 to FPU stack
FSIN            ; ST0 = sin(π/2) = 1.0
FST R9          ; Store result to R9

FLD R8          ; Load π/2 again
FCOS            ; ST0 = cos(π/2) ≈ 0.0
FST R10         ; Store result to R10

; Test 6: Sign Operations
LOAD_IMM64 R11, 0xC059000000000000 ; Load -100.0
FLD R11         ; Load -100.0 to FPU stack
FABS            ; ST0 = abs(-100.0) = 100.0
FST R12         ; Store result to R12

FLD R0          ; Load 100.0
FCHS            ; ST0 = -100.0
FSTP R13        ; Store result to R13 and pop stack

; Print results for verification
OUT R2          ; Should show 110.0
OUT R3          ; Should show -90.0  
OUT R4          ; Should show 1000.0
OUT R5          ; Should show 0.1
OUT R7          ; Should show ~5.657
OUT R9          ; Should show 1.0
OUT R10         ; Should show ~0.0
OUT R12         ; Should show 100.0
OUT R13         ; Should show -100.0

HALT