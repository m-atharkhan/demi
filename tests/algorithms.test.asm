; Complex Algorithms Test Suite  
; Tests for more complex algorithmic operations

.test "fibonacci calculation" {
    .description "Calculates the 5th Fibonacci number using iterative approach"
    .author "DemiEngine Team"
    .category "Algorithms"
    .tag "fibonacci"
    .tag "iterative"
    .tag "complex"
    
    LOAD_IMM R0, 0    ; fib(0) = 0
    LOAD_IMM R1, 1    ; fib(1) = 1  
    LOAD_IMM R2, 5    ; target: fib(5)
    LOAD_IMM R3, 1    ; counter starts at 1
    
fib_loop:
    CMP R3, R2
    JGE fib_done
    
    MOV R4, R1        ; temp = current
    ADD R1, R0        ; current = current + previous
    MOV R0, R4        ; previous = temp
    INC R3            ; counter++
    JMP fib_loop
    
fib_done:
    ; R1 should contain fib(5) = 5
    .assert_reg R1, 5
}

.test "factorial calculation" {
    .description "Calculates factorial of 5 (5!) using iterative multiplication"
    .author "DemiEngine Team"
    .category "Algorithms"
    .tag "factorial"
    .tag "iterative"
    .tag "complex"
    
    LOAD_IMM R0, 5    ; Calculate 5!
    LOAD_IMM R1, 1    ; Result accumulator
    LOAD_IMM R2, 1    ; Counter
    
fact_loop:
    CMP R2, R0
    JG fact_done
    MUL R1, R2        ; result *= counter
    INC R2            ; counter++
    JMP fact_loop
    
fact_done:
    ; 5! = 5*4*3*2*1 = 120
    .assert_reg R1, 120
}

.test "maximum of three numbers" {
    .description "Finds the maximum value among three numbers using comparisons"
    .author "DemiEngine Team"
    .category "Algorithms"
    .tag "max"
    .tag "comparison"
    .tag "control-flow"
    
    LOAD_IMM R0, 25   ; First number
    LOAD_IMM R1, 42   ; Second number  
    LOAD_IMM R2, 17   ; Third number
    
    ; Find max of R0 and R1
    CMP R0, R1
    JG first_larger
    MOV R3, R1        ; R1 is larger
    JMP compare_third
first_larger:
    MOV R3, R0        ; R0 is larger
    
compare_third:
    ; Compare current max with R2
    CMP R3, R2
    JG max_found
    MOV R3, R2        ; R2 is largest
    
max_found:
    .assert_reg R3, 42  ; Maximum should be 42
}

.test "sum array of numbers" {
    .description "Calculates the sum of multiple values stored in registers"
    .author "DemiEngine Team"
    .category "Algorithms"
    .tag "accumulator"
    .tag "sum"
    
    ; Array values: 10, 15, 7, 23, 8
    LOAD_IMM R0, 10
    LOAD_IMM R1, 15  
    LOAD_IMM R2, 7
    LOAD_IMM R3, 23
    LOAD_IMM R4, 8
    
    ; Sum them up
    LOAD_IMM R5, 0    ; Sum accumulator
    ADD R5, R0
    ADD R5, R1
    ADD R5, R2
    ADD R5, R3
    ADD R5, R4
    
    ; Total: 10+15+7+23+8 = 63
    .assert_reg R5, 63
}

.test "binary search simulation" {
    .description "Simulates binary search algorithm"
    .author "DemiEngine Team"
    .category "Algorithms"
    .tag "search"
    .tag "binary"
    .tag "simulation"
    
    ; Simulate searching for value 7 in sorted array [1,3,5,7,9,11,13]
    ; Array indices: 0,1,2,3,4,5,6
    LOAD_IMM R0, 0    ; left = 0
    LOAD_IMM R1, 6    ; right = 6  
    LOAD_IMM R2, 7    ; target = 7
    LOAD_IMM R3, 255  ; result = -1 (not found)
    
search_loop:
    CMP R0, R1
    JG search_done
    
    ; Calculate mid = (left + right) / 2
    MOV R4, R0
    ADD R4, R1
    SHR R4, 1         ; Divide by 2
    
    ; Simulate array access: arr[mid]
    ; For simplicity, use lookup: mid*2+1 gives values [1,3,5,7,9,11,13]
    MOV R5, R4
    SHL R5, 1         ; mid * 2
    INC R5            ; mid * 2 + 1
    
    CMP R5, R2
    JZ found
    JL search_right
    
search_left:
    MOV R1, R4
    DEC R1
    JMP search_loop
    
search_right:
    MOV R0, R4
    INC R0
    JMP search_loop
    
found:
    MOV R3, R4        ; Found at index mid
    
search_done:
    .assert_reg R3, 3  ; Should find 7 at index 3
}