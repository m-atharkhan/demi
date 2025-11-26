#!/bin/bash

# Test Runner for DemiEngine Tests
# Organizes and runs all test categories

DEMI_ENGINE="./bin/demi-engine-debug"
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
    
    if $DEMI_ENGINE "$test_file" > /dev/null 2>&1; then
        echo -e "${GREEN}PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}FAIL${NC}"
        ((FAILED++))
    fi
    ((TOTAL++))
}

# Build the engine first
echo -e "${BLUE}Building DemiEngine...${NC}"

# Start build in background and capture PID
make -q > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Build up to date${NC}"
    BUILD_STATUS=0
else
    make > /tmp/make_output.log 2>&1 &
    BUILD_PID=$!

    # Show spinner while building
    spin='-\|/'
    i=0
    while kill -0 $BUILD_PID 2>/dev/null; do
        i=$(( (i+1) %4 ))
        printf "\rBuilding... ${spin:$i:1}"
        sleep 0.1
    done

    # Check build result
    wait $BUILD_PID
    BUILD_STATUS=$?
fi

if [ $BUILD_STATUS -ne 0 ]; then
    printf "\r${RED}Build failed!${NC}\n"
    cat /tmp/make_output.log
    rm -f /tmp/make_output.log
    exit 1
fi

printf "\r${GREEN}Build successful${NC}\n"
rm -f /tmp/make_output.log
echo

# Run all tests in tests/ directory
echo -e "${YELLOW}=== Running Tests ===${NC}"
for test_file in tests/*.test.asm; do
    if [ -f "$test_file" ]; then
        run_test "$test_file"
    fi
done

# Run benchmarks (separate category, not counted in pass/fail)
echo -e "${YELLOW}=== Benchmarks ===${NC}"
if [ -d "benchmarks" ]; then
    for benchmark in benchmarks/*.asm; do
        if [ -f "$benchmark" ]; then
            benchmark_name=$(basename "$benchmark" .asm)
            echo "Running benchmark: $benchmark_name"
            $DEMI_ENGINE "$benchmark" > /dev/null 2>&1
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
