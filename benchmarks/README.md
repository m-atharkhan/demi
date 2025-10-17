# Benchmarks

This directory contains performance benchmarks and stress tests for the DemiEngine virtual machine.

## Benchmark Files

- `fpu_performance.asm` - FPU operation performance and throughput testing

## Purpose

Benchmarks are designed to:

1. **Measure Performance**: Quantify operation execution speed
2. **Stress Test**: Push the system to its limits
3. **Validate Scalability**: Test behavior under heavy loads  
4. **Profile Operations**: Identify performance bottlenecks
5. **Regression Testing**: Ensure optimizations don't break functionality

## FPU Performance Benchmark

The FPU benchmark (`fpu_performance.asm`) tests:

- **Operation Throughput**: How many FPU operations per second
- **Stack Management**: Efficiency of ST register operations
- **Memory Performance**: FPU memory load/store speed
- **Mixed Workloads**: Combination of different FPU operations

### Benchmark Structure

```assembly
; Setup phase - Initialize test data
; Timing loop - Execute operations repeatedly  
; Results - Calculate and display performance metrics
```

## Running Benchmarks

From project root:

```bash
# Run FPU performance benchmark
./bin/demi-engine --assembly benchmarks/fpu_performance.asm

# With timing information
time ./bin/demi-engine --assembly benchmarks/fpu_performance.asm
```

## Benchmark Categories

### Computational Benchmarks
- Pure computation speed (arithmetic operations)
- Algorithm implementation efficiency
- Mathematical function performance

### Memory Benchmarks  
- Memory access patterns
- Cache behavior simulation
- Load/store operation speed

### System Benchmarks
- Overall VM overhead
- Instruction decode performance
- Register file access speed

## Performance Metrics

Benchmarks typically measure:

- **Instructions Per Second (IPS)**: Raw execution speed
- **Operations Per Second**: Domain-specific metrics (FLOPS for FPU)
- **Latency**: Time for individual operations
- **Throughput**: Sustained operation rate
- **Memory Bandwidth**: Data movement speed

## Benchmark Best Practices

1. **Warm-up**: Run operations before timing to stabilize performance
2. **Multiple Runs**: Average results across multiple executions
3. **Consistent Environment**: Run benchmarks under similar conditions
4. **Baseline Comparison**: Compare against reference implementations
5. **Documentation**: Record system specifications and conditions

## Expected Results

Benchmarks help validate:

- Performance meets design requirements
- No significant performance regressions
- System scales appropriately with workload size
- Resource utilization is efficient

## Adding New Benchmarks

When creating new benchmarks:

1. Focus on specific performance aspects
2. Include timing and measurement code
3. Document expected performance characteristics
4. Test on representative workloads
5. Provide baseline measurements for comparison