#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "wasm_export.h"
#include "wasm_cmd_modules.h"

static int run_module(const char *mode, uint8_t *bytes, uint32_t size)
{
    wasm_module_t module = NULL;
    wasm_module_inst_t instance = NULL;
    wasm_exec_env_t env = NULL;
    wasm_function_inst_t main_func;
    RuntimeInitArgs args = { 0 };
    char error[128] = { 0 };
    uint32_t ret = 1;
    int result = -1;

    args.mem_alloc_type = Alloc_With_System_Allocator;
    if (!wasm_runtime_full_init(&args)) {
        printf("WAMR %s: init failed\r\n", mode);
        return -1;
    }

    module = wasm_runtime_load(bytes, size, error, sizeof(error));
    if (!module) {
        goto done;
    }
    instance = wasm_runtime_instantiate(module, 4096, 4096, error, sizeof(error));
    if (!instance) {
        goto done;
    }
    main_func = wasm_runtime_lookup_function(instance, "main");
    env = wasm_runtime_create_exec_env(instance, 4096);
    if (!main_func || !env) {
        snprintf(error, sizeof(error), "main export or exec environment unavailable");
        goto done;
    }
    if (!wasm_runtime_call_wasm(env, main_func, 0, &ret)) {
        snprintf(error, sizeof(error), "%s", wasm_runtime_get_exception(instance));
        goto done;
    }
    result = (int)ret;

done:
    if (result != 0) {
        printf("WAMR %s: %s (result=%d)\r\n", mode,
               error[0] ? error : "execution failed", result);
    }
    if (env) {
        wasm_runtime_destroy_exec_env(env);
    }
    if (instance) {
        wasm_runtime_deinstantiate(instance);
    }
    if (module) {
        wasm_runtime_unload(module);
    }
    wasm_runtime_destroy();
    return result;
}

static int wasm_cmd(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: wasm_cmd interp|aot\r\n");
        return -1;
    }
#ifdef CONFIG_WAMR_INTERP
    if (strcmp(argv[1], "interp") == 0) {
        return run_module("interp", hello_wasm, sizeof(hello_wasm));
    }
#endif
#ifdef CONFIG_WAMR_AOT
    if (strcmp(argv[1], "aot") == 0) {
        return run_module("aot", hello_aot, sizeof(hello_aot));
    }
#endif
    printf("WAMR mode unavailable: %s\r\n", argv[1]);
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(wasm_cmd, wasm_cmd, Run WAMR hello world (interp or aot).);
