#!/bin/bash

# Test Runner for DemiEngine Assembly Tests
# Organizes and runs all test categories

DEMI_ENGINE="./bin/demi-engine"
PASSED=0
FAILED=0
TOTAL=0

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== DemiEngine Test Suite ===${NC}"
echo

# Function to run a single test
run_test() {
    local test_file="$1"
    local test_name=$(basename "$test_file" .asm)
    
    echo -n "Running $test_name... "
    
    if $DEMI_ENGINE --assembly "$test_file" --verbose=false > /dev/null 2>&1; then
        echo -e "${GREEN}PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}FAIL${NC}"
        ((FAILED++))
    fi
    ((TOTAL++))
}

# Function to run tests in a directory
run_test_category() {
    local category="$1"
    local directory="$2"
    
    echo -e "${YELLOW}=== $category Tests ===${NC}"
    
    if [ -d "$directory" ]; then
        for test_file in "$directory"/*.asm; do
            if [ -f "$test_file" ]; then
                run_test "$test_file"
            fi
        done
    else
        echo "Directory $directory not found"
    fi
    echo
}

# Build the engine first
echo -e "${BLUE}Building DemiEngine...${NC}"
if ! make clean && make > /dev/null 2>&1; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi
echo -e "${GREEN}Build successful${NC}"
echo

# Run test categories
run_test_category "Basic CPU" "tests/basic"
run_test_category "FPU Operations" "tests/fpu" 
run_test_category "Parser Features" "tests/parsing"
run_test_category "Examples" "examples"

# Run benchmarks (separate category, not counted in pass/fail)
echo -e "${YELLOW}=== Benchmarks ===${NC}"
if [ -d "benchmarks" ]; then
    for benchmark in benchmarks/*.asm; do
        if [ -f "$benchmark" ]; then
            benchmark_name=$(basename "$benchmark" .asm)
            echo "Running benchmark: $benchmark_name"
            $DEMI_ENGINE --assembly "$benchmark" --verbose=false
        fi
    done
fi
echo

# Summary
echo -e "${BLUE}=== Test Summary ===${NC}"
echo "Total Tests: $TOTAL"
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed${NC}"
    exit 1
fi