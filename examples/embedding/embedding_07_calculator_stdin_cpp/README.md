# Embedding Example 07 - interactive calculator (C++ API)

Runs the line-based calculator guest program through the embedding API and
forwards host console input to VM stdin using `set_stdin_hook`.

The guest accepts one expression per line:

- `12 + 34`
- `10*5`
- `-7 / 2`

Type `q` and press Enter to exit.

## Run

From WSL:

```bash
make run
```

For verbose hook/debug output:

```bash
./embedding_07_calculator_stdin_cpp line_calculator.hex --debug
```

