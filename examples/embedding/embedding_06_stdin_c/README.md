# Embedding Example 06 - stdin hook (C API)

Demonstrates host-provided stdin using `demi_engine_set_stdin_hook`.

The guest program prompts for input, reads from stdin (fd 0), and echoes the
received bytes back to stdout.

## Run

From WSL:

```bash
make run
```

For verbose hook/debug output:

```bash
./embedding_06_stdin_c echo_input.hex --debug
```

