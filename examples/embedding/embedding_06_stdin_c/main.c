#include <demi/engine_c_api.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stdin_state {
    const uint8_t* data;
    size_t size;
    int sent;
} stdin_state_t;

static int g_debug = 0;

static void on_stdout(int fd, const uint8_t* data, size_t size, void* user_data) {
    (void)user_data;
    if (g_debug) {
        fprintf(stdout, "[guest fd=%d] %.*s", fd, (int)size, (const char*)data);
    } else {
        fwrite(data, 1, size, stdout);
    }
}

static void on_stdin(size_t max_count, uint8_t* data, size_t* out_size, void* user_data) {
    stdin_state_t* st = (stdin_state_t*)user_data;
    if (!st || st->sent || !st->data || st->size == 0) {
        *out_size = 0;
        return;
    }

    size_t n = st->size < max_count ? st->size : max_count;
    /* Bounds: n = min(st->size, max_count). st->data is st->size bytes,
       data is at least max_count bytes (caller guarantee). */
    memcpy(data, st->data, n);
    st->sent = 1;
    *out_size = n;
}

static uint8_t* read_file(const char* path, size_t* out_size) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n <= 0) { fclose(f); return NULL; }
    uint8_t* buf = (uint8_t*)malloc((size_t)n);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)n, f) != (size_t)n) { fclose(f); free(buf); return NULL; }
    fclose(f);
    *out_size = (size_t)n;
    return buf;
}

int main(int argc, char** argv) {
    const uint32_t echo_input_entry_pc = 0x200;
    const char* hex_path = "echo_input.hex";
    const char* host_input = "Input from C host stdin hook\n";
    int positional = 0;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--debug") == 0) {
            g_debug = 1;
            continue;
        }
        if (positional == 0) {
            hex_path = argv[i];
        } else if (positional == 1) {
            host_input = argv[i];
        }
        positional++;
    }

    demi_config_t cfg = demi_engine_default_config();
    cfg.enable_sandbox = true;
    cfg.strict_io = true;
    cfg.max_execution_ticks = 5 * 1000 * 1000;

    demi_engine_t vm = demi_engine_create(&cfg);
    if (!vm) {
        fprintf(stderr, "demi_engine_create failed\n");
        return 1;
    }

    /* host_input is guaranteed non-null: either a string literal or argv[i] */
    stdin_state_t stdin_state = {
        (const uint8_t*)host_input,
        host_input ? strlen(host_input) : 0,
        0
    };

    demi_engine_enable_output_capture(vm, true);
    demi_engine_set_stdout_hook(vm, on_stdout, NULL);
    demi_engine_set_stdin_hook(vm, on_stdin, &stdin_state);

    size_t size = 0;
    uint8_t* bytes = read_file(hex_path, &size);
    if (!bytes) {
        fprintf(stderr, "failed to read %s\n", hex_path);
        demi_engine_destroy(vm);
        return 1;
    }

    if (!demi_engine_load_executable(vm, bytes, size, 0)) {
        fprintf(stderr, "load failed: (%d) %s\n",
                demi_engine_last_error_code(vm),
                demi_engine_last_error(vm));
        free(bytes);
        demi_engine_destroy(vm);
        return 1;
    }
    free(bytes);

    // echo_input.asm uses .data/.text; entry symbol _start is at 0x200.
    if (!demi_engine_set_pc(vm, echo_input_entry_pc)) {
        fprintf(stderr, "set_pc failed: (%d) %s\n",
                demi_engine_last_error_code(vm),
                demi_engine_last_error(vm));
        demi_engine_destroy(vm);
        return 1;
    }

    (void)demi_engine_run(vm);

    size_t out_n = 0;
    const uint8_t* out = demi_engine_get_output_buffer(vm, &out_n);
    if (!out || out_n == 0) {
        fprintf(stderr, "no output captured\n");
    } else {
        if (g_debug) {
            fprintf(stdout, "\n[captured %zu bytes] ", out_n);
        }
        fwrite(out, 1, out_n, stdout);
        if (out[out_n - 1] != '\n') {
            fputc('\n', stdout);
        }
    }

    demi_engine_destroy(vm);
    return 0;
}

