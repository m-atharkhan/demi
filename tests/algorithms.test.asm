; Complex Algorithms Test Suite  
; Tests for more complex algorithmic operations

.test "fibonacci calculation" {
    .description "Calculates the 5th Fibonacci number using iterative approach"
    .author "DemiEngine Team"
    .category "Algorithms"
    .tag "fibonacci"
    .tag "iterative"
    .tag "complex"
    
    LOAD_IMM EAX, 0    ; fib(0) = 0
    LOAD_IMM EBX, 1    ; fib(1) = 1  
    LOAD_IMM ECX, 5    ; target: fib(5)
    LOAD_IMM EDX, 1    ; counter starts at 1
    
fib_loop:
    CMP EDX, ECX
    JGE fib_done
    
    MOV ESI, EBX        ; temp = current
    ADD EBX, EAX        ; current = current + previous
    MOV EAX, ESI        ; previous = temp
    INC EDX            ; counter++
    JMP fib_loop
    
fib_done:
    ; EBX should contain fib(5) = 5
    .assert_reg EBX, 5
}

.test "factorial calculation" {
    .description "Calculates factorial of 5 (5!) using iterative multiplication"
    .author "DemiEngine Team"
    .category "Algorithms"
    .tag "factorial"
    .tag "iterative"
    .tag "complex"
    
    LOAD_IMM EAX, 5    ; Calculate 5!
    LOAD_IMM EBX, 1    ; Result accumulator
    LOAD_IMM ECX, 1    ; Counter
    
fact_loop:
    CMP ECX, EAX
    JG fact_done
    MUL EBX, ECX        ; result *= counter
    INC ECX            ; counter++
    JMP fact_loop
    
fact_done:
    ; 5! = 5*4*3*2*1 = 120
    .assert_reg EBX, 120
}

.test "maximum of three numbers" {
    .description "Finds the maximum value among three numbers using comparisons"
    .author "DemiEngine Team"
    .category "Algorithms"
    .tag "max"
    .tag "comparison"
    .tag "control-flow"
    
    LOAD_IMM EAX, 25   ; First number
    LOAD_IMM EBX, 42   ; Second number  
    LOAD_IMM ECX, 17   ; Third number
    
    ; Find max of EAX and EBX
    CMP EAX, EBX
    JG first_larger
    MOV EDX, EBX        ; EBX is larger
    JMP compare_third
first_larger:
    MOV EDX, EAX        ; EAX is larger
    
compare_third:
    ; Compare current max with ECX
    CMP EDX, ECX
    JG max_found
    MOV EDX, ECX        ; ECX is largest
    
max_found:
    .assert_reg EDX, 42  ; Maximum should be 42
}

.test "sum array of numbers" {
    .description "Calculates the sum of multiple values stored in registers"
    .author "DemiEngine Team"
    .category "Algorithms"
    .tag "accumulator"
    .tag "sum"
    
    ; Array values: 10, 15, 7, 23, 8
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 15  
    LOAD_IMM ECX, 7
    LOAD_IMM EDX, 23
    LOAD_IMM ESI, 8
    
    ; Sum them up
    LOAD_IMM EDI, 0    ; Sum accumulator
    ADD EDI, EAX
    ADD EDI, EBX
    ADD EDI, ECX
    ADD EDI, EDX
    ADD EDI, ESI
    
    ; Total: 10+15+7+23+8 = 63
    .assert_reg EDI, 63
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
    LOAD_IMM EAX, 0    ; left = 0
    LOAD_IMM EBX, 6    ; right = 6  
    LOAD_IMM ECX, 7    ; target = 7
    LOAD_IMM EDX, 255  ; result = -1 (not found)
    
search_loop:
    CMP EAX, EBX
    JG search_done
    
    ; Calculate mid = (left + right) / 2
    MOV ESI, EAX
    ADD ESI, EBX
    SHR ESI, 1         ; Divide by 2
    
    ; Simulate array access: arr[mid]
    ; For simplicity, use lookup: mid*2+1 gives values [1,3,5,7,9,11,13]
    MOV EDI, ESI
    SHL EDI, 1         ; mid * 2
    INC EDI            ; mid * 2 + 1
    
    CMP EDI, ECX
    JZ found
    JL search_right
    
search_left:
    MOV EBX, ESI
    DEC EBX
    JMP search_loop
    
search_right:
    MOV EAX, ESI
    INC EAX
    JMP search_loop
    
found:
    MOV EDX, ESI        ; Found at index mid
    
search_done:
    .assert_reg EDX, 3  ; Should find 7 at index 3
}