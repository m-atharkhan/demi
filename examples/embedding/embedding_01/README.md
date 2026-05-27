# Embedding Example 01 — C++ API

This example hosts Demi inside a parent C++ program using the `demi::Engine` embedding API.

## Run

From WSL:

```bash
make run
```

This will:
- Build `libdemi` from the repository root if needed
- Assemble `hello.asm` into `hello.hex`
- Run the VM and capture stdout via `set_stdout_hook`
