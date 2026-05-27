#include <engine/engine_c_api.h>

#include <stdio.h>
#include <stdlib.h>

static void on_stdout(int fd, const uint8_t* data, size_t size, void* user_data) {
    (void)user_data;
    fprintf(stdout, "[guest fd=%d] %.*s", fd, (int)size, (const char*)data);
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
    const char* hex_path = (argc > 1) ? argv[1] : "hello.hex";

    demi_config_t cfg = demi_engine_default_config();
    cfg.enable_sandbox = true;
    cfg.strict_io = true;
    cfg.io_root_path = "/tmp/demi_vfs";
    cfg.max_execution_ticks = 5 * 1000 * 1000;

    demi_engine_t vm = demi_engine_create(&cfg);
    if (!vm) {
        fprintf(stderr, "demi_engine_create failed\n");
        return 1;
    }

    demi_engine_set_stdout_hook(vm, on_stdout, NULL);

    size_t size = 0;
    uint8_t* bytes = read_file(hex_path, &size);
    if (!bytes) {
        fprintf(stderr, "failed to read %s\n", hex_path);
        demi_engine_destroy(vm);
        return 1;
    }

    // Load at base 0 so label addresses emitted by the assembler
    // match the VM's linear memory for syscalls like sys_write.
    if (!demi_engine_load_executable(vm, bytes, size, 0)) {
        fprintf(stderr, "load failed: (%d) %s\n",
                demi_engine_last_error_code(vm),
                demi_engine_last_error(vm));
        free(bytes);
        demi_engine_destroy(vm);
        return 1;
    }

    free(bytes);

    demi_result_t r = (demi_result_t)demi_engine_run(vm);
    if (r != DEMI_FINISHED) {
        fprintf(stderr, "run returned %d: (%d) %s\n",
                (int)r,
                demi_engine_last_error_code(vm),
                demi_engine_last_error(vm));
    }

    demi_engine_destroy(vm);
    return 0;
}
