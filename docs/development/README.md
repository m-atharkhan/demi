# Development & Planning Documentation

This directory contains development plans, roadmaps, and future feature specifications for DemiEngine.

## Contents

### Core Planning Documents

- **[FUTURE_OPCODES.md](FUTURE_OPCODES.md)** - Comprehensive plan for implementing 88 additional opcodes (SIMD, FPU, AVX, MMX, Extended 64-bit)
- **[DEMI_LANGUAGE_PLAN.md](DEMI_LANGUAGE_PLAN.md)** - Specification for the Demi high-level language frontend (Stage 6)
- **[NATIVE_COMPILER_PLAN.md](NATIVE_COMPILER_PLAN.md)** - Native x86-64 code generation implementation plan

### Implementation Notes

- **[REGISTER_IMPLEMENTATION_NOTES.md](REGISTER_IMPLEMENTATION_NOTES.md)** - Detailed notes on 134-register architecture implementation

### Historical Documents

- **[DEVELOPMENT_PLAN_UPDATE.md](DEVELOPMENT_PLAN_UPDATE.md)** - Historical development updates and milestones

## Development Phases

### Current Phase: Assembly Language Expansion (Stage 1)
**Status**: In Progress  
**Focus**: Implementing 88 planned opcodes to complete instruction set  
**Primary Document**: [FUTURE_OPCODES.md](FUTURE_OPCODES.md)

### Next Phases

1. **Stage 2**: Native Code Generation - x86-64 translation
2. **Stage 3**: D-ISA Assembler Enhancement - Advanced features
3. **Stage 4**: Complete Native Codegen Backend
4. **Stage 5**: Custom Linker
5. **Stage 6**: Demi Language Frontend (see [DEMI_LANGUAGE_PLAN.md](DEMI_LANGUAGE_PLAN.md))
6. **Stage 7**: Unified Toolchain Integration
7. **Stage 8**: JIT Compilation

## Related Documentation

- **Main Roadmap**: `/roadmap.md` (project root)
- **Quick Reference**: `/docs/QUICK_REFERENCE.md`
- **Codebase Documentation**: `/docs/codebase/`
- **Testing Documentation**: `/docs/testing/`

## Contributing

See `/CONTRIBUTING.md` for development guidelines and contribution process.
