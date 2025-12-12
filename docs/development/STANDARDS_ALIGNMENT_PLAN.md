# Demi Standards Alignment Plan

## Overview

This document outlines the current state of coding standards in the Demi project, identifies gaps between documented guidelines and actual practice, and provides a migration strategy to improve alignment.

**Status**: Work in Progress
**Last Updated**: 2025-12-03
**Owner**: Development Team

## Executive Summary

The Demi project currently has **mixed adherence** to its documented coding standards. The codebase has evolved organically with pragmatic design decisions that differ from the initially documented standards (which referenced LLVM conventions). This plan documents the **actual conventions in use** and provides a roadmap for gradual alignment where beneficial.

### Key Findings

✅ **What Works Well** (Keep As-Is):

- Consistent use of `snake_case` for functions/methods across the entire codebase
- `PascalCase` for classes and types universally applied
- K&R style bracing for most code
- Strong use of Doxygen comments for public APIs
- Good security practices (RAII, smart pointers in new code)

⚠️ **Mixed Adherence** (Document Actual Practice):

- Member variable naming: Mix of trailing `_`, no prefix, and plain names
- Indentation: Mostly 4 spaces, some files use 2 spaces
- Namespace naming: Mix of `PascalCase` and `snake_case`
- Comment styles: Mix of `/** */`, `///`, and `//`

❌ **Gaps to Address** (Future Improvement):

- Inconsistent const correctness (many non-mutating methods missing `const`)
- Some raw `new`/`delete` in legacy code (should migrate to smart pointers)
- Mixed error handling patterns (exceptions, return codes, error counts)
- Legacy C-style code in some older modules

## Current State Analysis

### Naming Conventions (Actual Practice)

#### ✅ Well-Established Patterns

| Category             | Convention                   | Example                                 | Adherence |
| -------------------- | ---------------------------- | --------------------------------------- | --------- |
| Classes/Types        | `PascalCase`                 | `CPU`, `DebugHandler`, `TokenType`      | 100%      |
| Functions/Methods    | `snake_case`                 | `parse_expression()`, `add_value_arg()` | 95%+      |
| Namespaces (Project) | `PascalCase`                 | `Logging`, `Assembler`, `Testing`       | 90%       |
| Enum Values          | `PascalCase` or `UPPER_CASE` | `DebugLevel::TRACE`, `MODE_64BIT`       | 95%       |

#### ⚠️ Mixed Patterns (Need Documentation Update)

| Category              | Variants Found                       | Recommendation                                                 |
| --------------------- | ------------------------------------ | -------------------------------------------------------------- |
| Private Members       | `name_`, `name`, `m_name` (rare)     | **Document all as acceptable**; prefer `_` suffix for new code |
| Indentation           | 4 spaces (majority), 2 spaces (some) | **4 spaces default**; maintain consistency per file            |
| Namespaces (Compound) | `DemiEngine_Registers`, `vhw`        | **Accept both** patterns based on context                      |
| Constants             | `UPPER_CASE`, `kPascalCase`          | **Prefer `UPPER_CASE`** for new code                           |

### Code Formatting (Actual Practice)

#### Well-Followed Standards

- **Braces**: K&R style (opening on same line) - ~90% adherence
- **Pointer/Reference**: Attach to type (`int* ptr`) - ~85% adherence
- **Line Length**: Most files ~100-120 characters
- **Include Order**: System headers first, then project headers - well followed

#### Areas Needing Attention

- **Indentation**: Need to standardize on 4 spaces uniformly
- **Trailing Whitespace**: Some files have inconsistent trailing whitespace
- **Comment Formatting**: Mix of `//`, `///`, and `/** */` styles

### Documentation Comments

#### Current State

| Style                   | Usage                          | Quality   |
| ----------------------- | ------------------------------ | --------- |
| `/** @brief */` Doxygen | Public APIs, complex functions | Excellent |
| `///` Brief Doxygen     | Some member variables          | Good      |
| `//` Inline comments    | Implementation details         | Variable  |

#### Recommendations

- ✅ **Keep**: `/** */` for all public APIs and complex functions
- ✅ **Encourage**: `///` for member variable documentation
- ✅ **Standardize**: Use `//` consistently for implementation comments
- 📝 **Improve**: Add `@param`, `@return`, `@throws` tags consistently

## Standards Adherence by Module

### High Adherence (>90%)

- ✅ `src/debug/*` - Modern, well-documented, follows standards closely
- ✅ `src/test/*` - Clean architecture, good naming, strong documentation
- ✅ `src/assembler/demi_assembler.*` - Recent refactor, excellent standards

### Moderate Adherence (70-90%)

- ⚠️ `src/engine/cpu.*` - Mix of modern and legacy patterns
- ⚠️ `src/assembler/assembler.*` - Good structure, some legacy naming
- ⚠️ `src/main.cpp` - Large file with mixed patterns

### Lower Adherence (<70% - Legacy Code)

- ⚠️ `src/engine/opcodes/*` - Many individual files, inconsistent formatting
- ⚠️ Some older device implementations

## Migration Strategy

### Phase 1: Documentation Alignment (COMPLETED ✅)

**Goal**: Update documented standards to match actual practice

**Actions Taken**:

- ✅ Changed function naming from `camelCase` to `snake_case`
- ✅ Removed `m_` prefix requirement for members
- ✅ Documented K&R bracing as standard
- ✅ Changed namespace convention from `lowercase` to `PascalCase`
- ✅ Added note about 4-space indentation with flexibility

### Phase 2: Low-Hanging Fruit (Quick Wins)

**Timeline**: 1-2 months
**Effort**: Low
**Impact**: Medium

**Tasks**:

1. **Const Correctness Pass** (~2-3 days)
   - Add `const` to non-mutating methods
   - Mark `const` parameters where appropriate
   - Add `const` to getters
2. **Comment Standardization** (~1 week)
   - Ensure all public APIs have `/** @brief */` comments
   - Add missing `@param`, `@return`, `@throws` tags
   - Standardize inline comment style

3. **Indentation Cleanup** (~1 day)
   - Run automated formatter on files with 2-space indentation
   - Standardize to 4 spaces project-wide

4. **Include Guard Cleanup** (~1 day)
   - Ensure all headers use `#pragma once` consistently
   - Remove old-style include guards

### Phase 3: Incremental Refactoring

**Timeline**: 6-12 months (ongoing)
**Effort**: Medium
**Impact**: High

**Tasks**:

1. **Smart Pointer Migration** (as code is touched)
   - Replace `new`/`delete` with `std::unique_ptr`/`std::shared_ptr`
   - Use `std::make_unique`/`std::make_shared`
   - No mass refactor - do incrementally

2. **Member Variable Consistency** (as code is touched)
   - Prefer `name_` suffix for new private members
   - Don't rename existing code unless refactoring module
   - Document exceptions in code comments

3. **Error Handling Unification**
   - Design consistent error handling strategy
   - Gradually migrate to exceptions for recoverable errors
   - Use error codes for expected failures
   - Maintain `Config::error_count` for compatibility

4. **Modern C++17 Features**
   - Use `std::optional` for optional return values
   - Use `std::variant` for type-safe unions
   - Use `std::filesystem` for path operations (already in use)
   - Structured bindings where appropriate

### Phase 4: Module Modernization

**Timeline**: 12-18 months
**Effort**: High
**Impact**: High

**Priority Modules** (in order):

1. `src/engine/opcodes/*` - Consolidate and standardize
2. `src/engine/cpu.*` - Refactor legacy sections
3. Older device implementations
4. Test infrastructure improvements

**Approach**:

- One module at a time
- Full test coverage before refactoring
- Maintain backward compatibility
- Document all changes

## Standards We Will NOT Adopt

The following were in the old instructions but don't match our codebase and won't be adopted:

❌ **LLVM Coding Standards** - Too strict for our project style
❌ **Allman Braces** - K&R is well-established
❌ **camelCase Functions** - `snake_case` is universal in our code
❌ **m\_ Prefix** - Not widely used, trailing `_` preferred
❌ **Strict 100-char Lines** - 100-120 flexible is more practical
❌ **2-space Indentation** - 4 spaces is more readable for our code

## Tooling and Automation

### Recommended Tools

1. **clang-format** (Future)
   - Create `.clang-format` file matching our standards
   - Use for new files only initially
   - Gradually apply to modules during refactoring

2. **clang-tidy** (Current Use)
   - Enable modernize checks
   - Enable readability checks
   - Integrate into CI/CD pipeline

3. **cppcheck** (Current Use)
   - Run regularly for security vulnerabilities
   - Check for common C++ mistakes

4. **Documentation Validation**
   - Doxygen warnings in CI
   - Ensure all public APIs documented

### Automation Strategy

- **Pre-commit Hooks**: Run formatting checks
- **CI/CD Validation**: Enforce standards on new code
- **Editor Config**: Provide `.editorconfig` for consistent settings
- **Documentation**: Generate Doxygen docs automatically

## Guidelines for Contributors

### For New Code

✅ **Must Follow**:

- Use `snake_case` for functions/methods
- Use `PascalCase` for classes/types
- Use K&R bracing style
- Document all public APIs with `/** @brief */`
- Use 4-space indentation
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Mark const correctness
- Use RAII for all resources

⚠️ **Should Follow** (Strong Preference):

- Prefer `_` suffix for private members
- Use `std::optional` for optional returns
- Structured bindings where appropriate
- `#pragma once` for include guards

✓ **May Vary** (Context-Dependent):

- Namespace naming based on subsystem
- Comment density (more for complex logic)
- Line length up to 120 characters

### For Existing Code

When modifying existing code:

1. **Match the existing style** of that file/module
2. Don't mix style changes with functional changes
3. If refactoring, update to modern standards
4. Add tests for any modified behavior
5. Update documentation to match changes

## Measuring Progress

### Metrics to Track

1. **Documentation Coverage**
   - % of public functions with Doxygen comments
   - % of functions with complete param/return docs
   - Target: >95% for public APIs

2. **Modern C++ Usage**
   - Smart pointer vs raw pointer ratio
   - Use of `std::optional`, `std::variant`
   - Use of auto type deduction
   - Target: 100% smart pointers in new code

3. **Const Correctness**
   - % of non-mutating methods marked const
   - % of parameters that should be const
   - Target: >90% for refactored modules

4. **Automated Checks**
   - clang-tidy warnings
   - cppcheck issues
   - Compiler warnings (treat as errors)
   - Target: Zero warnings

### Quarterly Review

Review progress every quarter:

- Update this document with achievements
- Identify problem areas needing attention
- Celebrate improvements
- Adjust priorities based on project needs

## Exceptions and Pragmatism

### When to Deviate

Standards are guidelines, not laws. Acceptable reasons to deviate:

✅ **Performance-Critical Code**: Optimize over style when profiling shows benefit
✅ **Compatibility**: Maintain API compatibility with external libraries
✅ **Readability**: If standards hurt readability in specific context
✅ **Legacy Integration**: Matching style of external legacy code

**Rule**: Document all intentional deviations with comments explaining why.

## Conclusion

The Demi project has **strong foundational standards** with room for improvement in consistency. This plan provides a **pragmatic roadmap** that:

1. ✅ Acknowledges what works well (keep it)
2. ⚠️ Documents actual practice (be honest)
3. 📝 Identifies improvements (be realistic)
4. 🎯 Provides clear migration path (be gradual)

**Key Principle**: **Consistency within modules** is more important than forcing project-wide uniformity immediately.

**Success Criteria**:

- Code is readable and maintainable
- Standards are documented and match reality
- New code follows modern best practices
- Old code is gradually improved as touched

---

## Appendix: Quick Reference

### Naming Quick Reference

```cpp
// ✅ Correct Naming
class CPU {                          // PascalCase for class
public:
    void execute_instruction();      // snake_case for method
    uint32_t get_register(size_t i); // snake_case for function

private:
    uint32_t program_counter_;       // Preferred: trailing underscore
    std::vector<uint8_t> memory;     // Also acceptable: no prefix
};

namespace Logging {                  // PascalCase for namespace
    enum class DebugLevel {          // PascalCase for enum
        TRACE,                       // UPPER_CASE for enum values
        DEBUG,
        INFO
    };
}

const size_t MAX_MEMORY_SIZE = 4096; // UPPER_CASE for constants
```

### Documentation Quick Reference

```cpp
/**
 * @brief Execute a single CPU instruction
 *
 * Fetches, decodes, and executes one instruction from the bytecode program.
 * Updates CPU state including registers, flags, and program counter.
 *
 * @param program Vector of bytecode to execute
 * @param running Reference to execution flag (set to false to stop)
 * @throws CPUException if instruction is invalid or causes runtime error
 * @throws std::out_of_range if memory access is out of bounds
 */
void execute(const std::vector<uint8_t>& program, bool& running);
```

### Formatting Quick Reference

```cpp
// K&R Bracing Style
void function_name() {
    if (condition) {
        do_something();
    } else {
        do_something_else();
    }
}

// Class Definition
class ClassName {
public:
    ClassName();
    ~ClassName();

private:
    int member_variable_;
};

// Pointer/Reference Declaration
int* pointer;
const std::string& reference;
std::unique_ptr<Object> smart_ptr;
```

---

**Document Version**: 1.0
**Next Review**: 2026-03-03
**Maintained By**: Development Team
**Feedback**: Submit issues or PRs to improve this plan
