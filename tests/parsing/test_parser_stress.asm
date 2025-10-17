; Assembly Parser Stress Test
; Tests parser limits and edge cases

.data
    very_long_symbol_name_that_tests_identifier_parsing_limits: .dd 42.0
    π: .dd 3.14159265                    ; Unicode in comments
    mixed_123_name: .dd 1.23456789012345 ; Mixed alphanumeric
    
.text
start_of_very_long_label_name_that_should_parse_correctly:
    ; Test maximum precision floating point
    FLD 3.1415926535897932384626433832795028841971693993751
    
    ; Test many decimal places  
    FLD 0.123456789012345678901234567890
    
    ; Test scientific-like notation (as regular decimals)
    FLD 1000000.0               ; Large number
    FLD 0.000001                ; Small number
    
    ; Test complex ST() operations
    FLD ST(0)
    FLD ST(1) 
    FLD ST(2)
    FLD ST(3)
    FLD ST(4)
    FLD ST(5)
    FLD ST(6)
    FLD ST(7)                   ; Test maximum ST index
    
    ; Test nested symbol references
    FLD very_long_symbol_name_that_tests_identifier_parsing_limits
    FLD mixed_123_name
    
    ; Test rapid-fire operations
    FLD 1.1
    FLD 1.2
    FLD 1.3
    FLD 1.4
    FLD 1.5
    FADD 0.1
    FADD 0.2
    FADD 0.3
    
    HALT

; Stress tests:
; - Very long symbol names
; - High precision floating point numbers
; - Maximum ST register indices  
; - Rapid sequential operations
; - Mixed data types and operations
; - Symbol resolution stress