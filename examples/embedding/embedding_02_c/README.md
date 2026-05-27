# Embedding Example 02 — C API

Minimal embedding using `demi/engine_c_api.h`.

## Run

From WSL:

```bash
make run
```

This assembles `hello.asm`, then runs it via the C API and prints guest stdout via `demi_engine_set_stdout_hook`.
