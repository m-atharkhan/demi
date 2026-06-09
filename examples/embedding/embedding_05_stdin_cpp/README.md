# Embedding Example 05 - stdin hook (C++ API)

Demonstrates host-provided stdin using `demi::Engine::set_stdin_hook`.

The guest program prompts for input, performs `sys_read` from fd 0, and echoes
the bytes back using `sys_write`.

## Run

From WSL:

```bash
make run
```

For verbose hook/debug output:

```bash
./embedding_05_stdin_cpp echo_input.hex --debug
```

