; Test file for control flow (jumps and comparisons)

#test "unconditional jump" {
    #description "Tests unconditional JMP instruction"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "jump"
    #tag "basic"
    LOAD_IMM R0, 5
    JMP skip
    LOAD_IMM R0, 99     ; Should be skipped
skip:
    #assert_reg R0, 5
}

#test "compare equal and jump if zero" {
    #description "Tests CMP instruction with JZ conditional jump"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "conditional"
    #tag "comparison"
    LOAD_IMM R0, 10
    LOAD_IMM R1, 10
    CMP R0, R1
    JZ equal
    LOAD_IMM R2, 0
    JMP done
equal:
    LOAD_IMM R2, 1
done:
    #assert_reg R2, 1
}

#test "jump if greater" {
    #description "Tests JG conditional jump when first operand is greater"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "conditional"
    #tag "comparison"
    LOAD_IMM R0, 20
    LOAD_IMM R1, 10
    CMP R0, R1
    JG greater
    LOAD_IMM R2, 0
    JMP done
greater:
    LOAD_IMM R2, 1
done:
    #assert_reg R2, 1
}

#test "jump if less" {
    #description "Tests JL conditional jump when first operand is less"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "conditional"
    #tag "comparison"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 15
    CMP R0, R1
    JL less
    LOAD_IMM R2, 0
    JMP done
less:
    LOAD_IMM R2, 1
done:
    #assert_reg R2, 1
}

#test "jump if greater or equal" {
    #description "Tests JGE conditional jump when operands are equal"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "conditional"
    #tag "comparison"
    LOAD_IMM R0, 10
    LOAD_IMM R1, 10
    CMP R0, R1
    JGE ge
    LOAD_IMM R2, 0
    JMP done
ge:
    LOAD_IMM R2, 1
done:
    #assert_reg R2, 1
}

#test "jump if less or equal" {
    #description "Tests JLE conditional jump when first operand is less"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "conditional"
    #tag "comparison"
    LOAD_IMM R0, 8
    LOAD_IMM R1, 10
    CMP R0, R1
    JLE le
    LOAD_IMM R2, 0
    JMP done
le:
    LOAD_IMM R2, 1
done:
    #assert_reg R2, 1
}
