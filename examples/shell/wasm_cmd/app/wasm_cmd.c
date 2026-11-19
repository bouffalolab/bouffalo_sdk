#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bflb_mtimer.h"
#include "coremark_bench.h"
#include "shell.h"
#include "wasm_export.h"
#include "wasm_cmd_modules.h"

#define COREMARK_TARGET_US          10000000U
#define COREMARK_CALIBRATE_US       100000U
#define COREMARK_MAX_ITERATIONS     1000000U
#define COREMARK_WASM_STACK_SIZE    (32 * 1024)
#define COREMARK_HOST_HEAP_SIZE     0
#define COREMARK_FUNCTION_COUNT     7

typedef struct {
    uint32_t (*prepare)(uint32_t iterations);
    uint32_t (*execute)(void);
    uint32_t (*get_crc)(void);
    uint32_t (*get_list_crc)(void);
    uint32_t (*get_matrix_crc)(void);
    uint32_t (*get_state_crc)(void);
    uint32_t (*get_error)(void);
} coremark_api_t;

static uint32_t elapsed_us_since(uint64_t start)
{
    uint64_t elapsed = bflb_mtimer_get_time_us() - start;

    if (elapsed == 0) {
        return 1;
    }
    if (elapsed > UINT32_MAX) {
        return UINT32_MAX;
    }
    return (uint32_t)elapsed;
}

static int run_coremark_backend(const char *mode, const coremark_api_t *api,
                                const char *detail)
{
    uint32_t cal_iters = 1;
    uint32_t iterations;
    uint32_t elapsed_us;
    uint32_t status;
    uint32_t error;
    uint64_t scaled;
    uint64_t start;

    while (1) {
        if (api->prepare(cal_iters) != 0) {
            printf("CoreMark %s: prepare failed\r\n", mode);
            goto fail;
        }

        start = bflb_mtimer_get_time_us();
        status = api->execute();
        elapsed_us = elapsed_us_since(start);
        if (status != 0) {
            printf("CoreMark %s: execution failed\r\n", mode);
            goto fail;
        }

        if (elapsed_us >= COREMARK_CALIBRATE_US ||
            cal_iters >= COREMARK_MAX_ITERATIONS) {
            break;
        }
        if (cal_iters > COREMARK_MAX_ITERATIONS / 10) {
            cal_iters = COREMARK_MAX_ITERATIONS;
        } else {
            cal_iters *= 10;
        }
    }

    scaled = (uint64_t)cal_iters * COREMARK_TARGET_US;
    iterations = (uint32_t)((scaled + elapsed_us - 1) / elapsed_us);
    if (iterations == 0) {
        iterations = 1;
    } else if (iterations > COREMARK_MAX_ITERATIONS) {
        iterations = COREMARK_MAX_ITERATIONS;
    }

    if (api->prepare(iterations) != 0) {
        printf("CoreMark %s: prepare failed\r\n", mode);
        goto fail;
    }

    printf("CoreMark [%s]: running %lu iterations...\r\n",
           mode, (unsigned long)iterations);
    start = bflb_mtimer_get_time_us();
    status = api->execute();
    elapsed_us = elapsed_us_since(start);
    error = api->get_error();
    scaled = (uint64_t)iterations * 1000000000ULL / elapsed_us;

    printf("CoreMark [%s]: iterations=%lu elapsed_us=%lu\r\n",
           mode, (unsigned long)iterations, (unsigned long)elapsed_us);
    printf("CoreMark 1.0 [%s] : %lu.%03lu / compiler -O2 / Static\r\n",
           mode, (unsigned long)(scaled / 1000), (unsigned long)(scaled % 1000));
    printf("CRC [%s]: final=0x%04lx list=0x%04lx matrix=0x%04lx "
           "state=0x%04lx error=0x%lx %s\r\n",
           mode, (unsigned long)api->get_crc(),
           (unsigned long)api->get_list_crc(),
           (unsigned long)api->get_matrix_crc(),
           (unsigned long)api->get_state_crc(),
           (unsigned long)error, (status == 0 && error == 0) ? "PASS" : "FAIL");

    return (status == 0 && error == 0) ? 0 : -1;

fail:
    if (detail && detail[0]) {
        printf("CoreMark %s: %s\r\n", mode, detail);
    }
    return -1;
}

static const coremark_api_t direct_coremark_api = {
    .prepare = coremark_prepare,
    .execute = coremark_execute,
    .get_crc = coremark_get_crc,
    .get_list_crc = coremark_get_list_crc,
    .get_matrix_crc = coremark_get_matrix_crc,
    .get_state_crc = coremark_get_state_crc,
    .get_error = coremark_get_error,
};

static wasm_exec_env_t coremark_wasm_env;
static wasm_function_inst_t coremark_wasm_functions[COREMARK_FUNCTION_COUNT];
static char coremark_wasm_error[128];

static int call_coremark_wasm(uint32_t function_index, int has_arg,
                              uint32_t arg, uint32_t *value)
{
    uint32_t argv[1] = { arg };

    if (!coremark_wasm_env || !coremark_wasm_functions[function_index]) {
        snprintf(coremark_wasm_error, sizeof(coremark_wasm_error),
                 "CoreMark export unavailable");
        return -1;
    }
    if (!wasm_runtime_call_wasm(coremark_wasm_env,
                                coremark_wasm_functions[function_index],
                                has_arg ? 1 : 0, argv)) {
        snprintf(coremark_wasm_error, sizeof(coremark_wasm_error), "%s",
                 wasm_runtime_get_exception(
                     wasm_runtime_get_module_inst(coremark_wasm_env)));
        return -1;
    }
    *value = argv[0];
    return 0;
}

static uint32_t coremark_wasm_prepare(uint32_t iterations)
{
    uint32_t value = 1;

    return call_coremark_wasm(0, 1, iterations, &value) == 0 ? value : 1;
}

static uint32_t coremark_wasm_execute(void)
{
    uint32_t value = 1;

    return call_coremark_wasm(1, 0, 0, &value) == 0 ? value : 1;
}

static uint32_t coremark_wasm_get(uint32_t function_index)
{
    uint32_t value = 0;

    (void)call_coremark_wasm(function_index, 0, 0, &value);
    return value;
}

static uint32_t coremark_wasm_get_crc(void)
{
    return coremark_wasm_get(2);
}

static uint32_t coremark_wasm_get_list_crc(void)
{
    return coremark_wasm_get(3);
}

static uint32_t coremark_wasm_get_matrix_crc(void)
{
    return coremark_wasm_get(4);
}

static uint32_t coremark_wasm_get_state_crc(void)
{
    return coremark_wasm_get(5);
}

static uint32_t coremark_wasm_get_error(void)
{
    uint32_t value = UINT32_MAX;

    if (call_coremark_wasm(6, 0, 0, &value) != 0) {
        return UINT32_MAX;
    }
    return value;
}

static const coremark_api_t wasm_coremark_api = {
    .prepare = coremark_wasm_prepare,
    .execute = coremark_wasm_execute,
    .get_crc = coremark_wasm_get_crc,
    .get_list_crc = coremark_wasm_get_list_crc,
    .get_matrix_crc = coremark_wasm_get_matrix_crc,
    .get_state_crc = coremark_wasm_get_state_crc,
    .get_error = coremark_wasm_get_error,
};

static int run_coremark_wasm(const char *mode, const uint8_t *bytes,
                             uint32_t size)
{
    static const char *function_names[COREMARK_FUNCTION_COUNT] = {
        "coremark_prepare",
        "coremark_execute",
        "coremark_get_crc",
        "coremark_get_list_crc",
        "coremark_get_matrix_crc",
        "coremark_get_state_crc",
        "coremark_get_error",
    };
    wasm_module_t module = NULL;
    wasm_module_inst_t instance = NULL;
    RuntimeInitArgs args = { 0 };
    char error[128] = { 0 };
    int ret = -1;
    uint32_t i;

    coremark_wasm_env = NULL;
    memset(coremark_wasm_functions, 0, sizeof(coremark_wasm_functions));
    coremark_wasm_error[0] = '\0';

    args.mem_alloc_type = Alloc_With_System_Allocator;
    if (!wasm_runtime_full_init(&args)) {
        printf("CoreMark %s: WAMR init failed\r\n", mode);
        return -1;
    }

    module = wasm_runtime_load((uint8_t *)bytes, size, error, sizeof(error));
    if (!module) {
        printf("CoreMark %s: module load failed: %s\r\n", mode, error);
        goto done;
    }

    instance = wasm_runtime_instantiate(module, COREMARK_WASM_STACK_SIZE,
                                        COREMARK_HOST_HEAP_SIZE,
                                        error, sizeof(error));
    if (!instance) {
        printf("CoreMark %s: instantiate failed: %s\r\n", mode, error);
        goto done;
    }

    for (i = 0; i < sizeof(coremark_wasm_functions) /
                            sizeof(coremark_wasm_functions[0]);
         i++) {
        coremark_wasm_functions[i] =
            wasm_runtime_lookup_function(instance, function_names[i]);
        if (!coremark_wasm_functions[i]) {
            printf("CoreMark %s: missing export %s\r\n", mode, function_names[i]);
            goto done;
        }
    }

    coremark_wasm_env =
        wasm_runtime_create_exec_env(instance, COREMARK_WASM_STACK_SIZE);
    if (!coremark_wasm_env) {
        printf("CoreMark %s: exec environment unavailable\r\n", mode);
        goto done;
    }

    ret = run_coremark_backend(mode, &wasm_coremark_api, coremark_wasm_error);

done:
    if (coremark_wasm_env) {
        wasm_runtime_destroy_exec_env(coremark_wasm_env);
        coremark_wasm_env = NULL;
    }
    if (instance) {
        wasm_runtime_deinstantiate(instance);
    }
    if (module) {
        wasm_runtime_unload(module);
    }
    wasm_runtime_destroy();
    return ret;
}

static int run_hello_module(const char *mode, uint8_t *bytes, uint32_t size)
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

static int run_hello(const char *mode)
{
#ifdef CONFIG_WAMR_INTERP
    if (strcmp(mode, "interp") == 0) {
        return run_hello_module("interp", hello_wasm, sizeof(hello_wasm));
    }
#endif
#ifdef CONFIG_WAMR_AOT
    if (strcmp(mode, "aot") == 0) {
        return run_hello_module("aot", hello_aot, sizeof(hello_aot));
    }
#endif
    printf("WAMR mode unavailable: %s\r\n", mode);
    return -1;
}

static int run_coremark(const char *mode)
{
    if (strcmp(mode, "direct") == 0) {
        return run_coremark_backend("direct", &direct_coremark_api, NULL);
    }
#ifdef CONFIG_WAMR_INTERP
    if (strcmp(mode, "interp") == 0) {
        return run_coremark_wasm("interp", coremark_wasm,
                                 sizeof(coremark_wasm));
    }
#endif
#ifdef CONFIG_WAMR_AOT
    if (strcmp(mode, "aot") == 0) {
        return run_coremark_wasm("aot", coremark_aot, sizeof(coremark_aot));
    }
    if (strcmp(mode, "aot-fast") == 0) {
        return run_coremark_wasm("aot-fast", coremark_fast_aot,
                                 sizeof(coremark_fast_aot));
    }
    if (strcmp(mode, "aot-dynamic") == 0) {
        return run_coremark_wasm("aot-dynamic", coremark_dynamic_aot,
                                 sizeof(coremark_dynamic_aot));
    }
    if (strcmp(mode, "aot-dynamic-fast") == 0) {
        return run_coremark_wasm("aot-dynamic-fast", coremark_dynamic_fast_aot,
                                 sizeof(coremark_dynamic_fast_aot));
    }
    if (strcmp(mode, "aot-xip") == 0) {
        return run_coremark_wasm("aot-xip", coremark_xip_aot,
                                 sizeof(coremark_xip_aot));
    }
    if (strcmp(mode, "aot-xip-fast") == 0) {
        return run_coremark_wasm("aot-xip-fast", coremark_xip_fast_aot,
                                 sizeof(coremark_xip_fast_aot));
    }
#endif
    printf("CoreMark mode unavailable: %s\r\n", mode);
    return -1;
}

static int wasm_cmd(int argc, char **argv)
{
    if (argc == 2) {
        return run_hello(argv[1]);
    }
    if (argc == 3 && strcmp(argv[1], "coremark") == 0) {
        return run_coremark(argv[2]);
    }

    printf("Usage:\r\n");
    printf("  wasm_cmd interp|aot\r\n");
    printf("  wasm_cmd coremark direct|interp|aot|aot-fast|aot-dynamic|"
           "aot-dynamic-fast|aot-xip|aot-xip-fast\r\n");
    return -1;
}

SHELL_CMD_EXPORT_ALIAS(wasm_cmd, wasm_cmd,
                       Run WAMR hello or CoreMark (direct, interp, aot).);
