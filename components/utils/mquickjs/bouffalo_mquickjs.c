

/*
 * Bouffalo SDK adaptation for MQuickJS
 *
 * Copyright (c) 2017-2025 Fabrice Bellard
 * Copyright (c) 2017-2025 Charlie Gordon
 * Bouffalo Lab SDK integration
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>

#include "shell.h"
#include "bflb_mtimer.h"
#include "mm.h"

#include <FreeRTOS.h>
#include "task.h"
#include "semphr.h"

#include "cutils.h"
#include "mquickjs.h"
#include "bouffalo_mquickjs.h"

/* ============== Runtime Functions ============== */
/* These must be defined BEFORE including mqjs_stdlib.h because
 * the generated header references them in the function table.
 */

/* Time functions using Bouffalo SDK mtimer */
static int64_t get_time_ms(void)
{
    return (int64_t)bflb_mtimer_get_time_ms();
}

/* Date.now() - returns current timestamp in milliseconds */
static JSValue js_date_now(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    return JS_NewInt64(ctx, get_time_ms());
}

/* performance.now() - returns high-resolution timestamp */
static JSValue js_performance_now(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    return JS_NewInt64(ctx, get_time_ms());
}

/* print() - output to console */
static JSValue js_print(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    int i;
    JSValue v;

    for (i = 0; i < argc; i++) {
        if (i != 0)
            putchar(' ');
        v = argv[i];
        if (JS_IsString(ctx, v)) {
            JSCStringBuf buf;
            const char *str;
            size_t len;
            str = JS_ToCStringLen(ctx, &len, v, &buf);
            fwrite(str, 1, len, stdout);
        } else {
            JS_PrintValueF(ctx, argv[i], JS_DUMP_LONG);
        }
    }
    putchar('\r');
    putchar('\n');
    return JS_UNDEFINED;
}

/* gc() - trigger garbage collection */
static JSValue js_gc(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    JS_GC(ctx);
    return JS_UNDEFINED;
}

/* Timer list for setTimeout/clearTimeout */
typedef struct JSTimer {
    struct JSTimer *prev;
    struct JSTimer *next;
    int64_t timeout;
    JSValue func;
    int id;
} JSTimer;

static JSTimer *timer_list = NULL;
static int next_timer_id = 1;

/* setTimeout(callback, delay) */
static JSValue js_setTimeout(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    JSTimer *t;
    int64_t delay;

    if (argc < 2 || !JS_IsFunction(ctx, argv[0])) {
        return JS_ThrowTypeError(ctx, "setTimeout requires a function and delay");
    }

    if (JS_ToInt32(ctx, (int*)&delay, argv[1])) {
        delay = 0;
    }

    t = malloc(sizeof(JSTimer));
    if (!t) {
        return JS_ThrowOutOfMemory(ctx);
    }

    t->func = argv[0];
    t->timeout = get_time_ms() + delay;
    t->id = next_timer_id++;

    /* Add to list */
    t->prev = NULL;
    t->next = timer_list;
    if (timer_list) {
        timer_list->prev = t;
    }
    timer_list = t;

    return JS_NewInt32(ctx, t->id);
}

/* clearTimeout(id) */
static JSValue js_clearTimeout(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    int id;
    JSTimer *t;

    if (argc < 1 || JS_ToInt32(ctx, &id, argv[0])) {
        return JS_UNDEFINED;
    }

    for (t = timer_list; t != NULL; t = t->next) {
        if (t->id == id) {
            if (t->prev) t->prev->next = t->next;
            if (t->next) t->next->prev = t->prev;
            if (t == timer_list) timer_list = t->next;
            free(t);
            break;
        }
    }

    return JS_UNDEFINED;
}

/* Run expired timers */
static void run_timers(JSContext *ctx)
{
    JSTimer *t, *next;
    int64_t now = get_time_ms();

    for (t = timer_list; t != NULL; t = next) {
        next = t->next;
        if (now >= t->timeout) {
            /* Remove from list */
            if (t->prev) t->prev->next = t->next;
            if (t->next) t->next->prev = t->prev;
            if (t == timer_list) timer_list = t->next;

            /* Call the timer function */
            JS_PushArg(ctx, JS_UNDEFINED);  /* this */
            JS_PushArg(ctx, 0);             /* argc */
            JS_Call(ctx, 0);

            free(t);
        }
    }
}

/* File loading helper */
static uint8_t *load_file(const char *filename, int *plen)
{
    FILE *f;
    uint8_t *buf;
    int buf_len;

    f = fopen(filename, "rb");
    if (!f) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    buf_len = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (buf_len < 0) {
        fclose(f);
        return NULL;
    }

    buf = malloc(buf_len + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    fread(buf, 1, buf_len, f);
    buf[buf_len] = '\0';
    fclose(f);

    if (plen) *plen = buf_len;
    return buf;
}

/* load(filename) - load and execute a JavaScript file */
static JSValue js_load(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    const char *filename;
    int buf_len;
    uint8_t *buf;
    JSValue ret;
    JSCStringBuf cbuf;

    if (argc < 1) {
        return JS_ThrowTypeError(ctx, "load requires a filename");
    }

    filename = JS_ToCString(ctx, argv[0], &cbuf);
    buf = load_file(filename, &buf_len);

    if (!buf) {
        return JS_ThrowTypeError(ctx, "cannot load file '%s'", filename);
    }

    ret = JS_Eval(ctx, (const char *)buf, buf_len, filename, 0);
    free(buf);

    return ret;
}

/* JS log output - convert \n to \r\n for serial console */
static void js_log_func(void *opaque, const void *buf, size_t buf_len)
{
    const uint8_t *p = (const uint8_t *)buf;
    const uint8_t *end = p + buf_len;

    while (p < end) {
        const uint8_t *q = p;
        while (q < end && *q != '\n')
            q++;
        if (q > p)
            fwrite(p, 1, q - p, stdout);
        if (q < end) {
            putchar('\r');
            putchar('\n');
            p = q + 1;
        } else {
            break;
        }
    }
}

/* ============== Include Generated Stdlib ============== */
#include "mqjs_stdlib.h"

/* ============== JS VM Configuration ============== */

#define JS_MEM_SIZE         (256 * 1024)  /* 256KB for JS heap */
#define JS_TASK_STACK_SIZE  (8 * 1024)    /* 8KB task stack */
#define JS_TASK_PRIORITY    5
#define JS_MAX_SCRIPT_SIZE  (32 * 1024)   /* 32KB max script size */

/* JS task queue for shell commands */
typedef struct {
    char filename[256];
    int result;
} js_request_t;

static QueueHandle_t js_queue;
static TaskHandle_t js_task_handle;

/* Global state for async execution */
static volatile int js_busy = 0;
static char js_current_file[256];

static uint8_t *load_js_file(const char *filename, int *plen)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Error: Cannot open %s\r\n", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (len < 0) {
        printf("Error: Cannot get file size for %s\r\n", filename);
        fclose(f);
        return NULL;
    }

    if (len > JS_MAX_SCRIPT_SIZE) {
        printf("Error: Script too large (%d > %d)\r\n", len, JS_MAX_SCRIPT_SIZE);
        fclose(f);
        return NULL;
    }
    printf("Loading script: %s (%d bytes)\r\n", filename, len);
    uint8_t *buf = malloc(len + 1);
    if (!buf) {
        printf("Error: Out of memory\r\n");
        fclose(f);
        return NULL;
    }

    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);

    if (plen) *plen = len;
    return buf;
}

/* JS VM Task */
static void js_vm_task(void *pvParameters)
{
    uint8_t *mem_buf = malloc(JS_MEM_SIZE);
    if (!mem_buf) {
        printf("Error: Failed to allocate JS memory (%d bytes)\r\n", JS_MEM_SIZE);
        vTaskDelete(NULL);
        return;
    }

    JSContext *ctx = JS_NewContext(mem_buf, JS_MEM_SIZE, &js_stdlib);
    JS_SetLogFunc(ctx, js_log_func);

    /* Set random seed from hardware timer */
    uint64_t time_us = bflb_mtimer_get_time_us();
    JS_SetRandomSeed(ctx, time_us ^ (time_us >> 32));

    printf("JS VM ready (heap: %d bytes)\r\n", JS_MEM_SIZE);

    js_request_t req;
    while (1) {
        /* Wait for request from shell */
        if (xQueueReceive(js_queue, &req, pdMS_TO_TICKS(100)) == pdTRUE) {
            /* Mark busy */
            js_busy = 1;
            strncpy(js_current_file, req.filename, sizeof(js_current_file) - 1);
            js_current_file[sizeof(js_current_file) - 1] = '\0';

            int script_len;
            uint8_t *script = load_js_file(req.filename, &script_len);

            if (script) {
                JSValue val = JS_Eval(ctx, (char *)script, script_len, req.filename, 0);

                if (JS_IsException(val)) {
                    JSValue exc = JS_GetException(ctx);
                    printf("JS Error: ");
                    JS_PrintValueF(ctx, exc, JS_DUMP_LONG);
                    printf("\r\n");
                    req.result = -1;
                } else {
                    req.result = 0;
                }
                free(script);
            } else {
                req.result = -1;
            }

            /* Print completion status */
            if (req.result == 0) {
                printf("\r\nJS completed: %s\r\n", req.filename);
            } else {
                printf("\r\nJS failed: %s\r\n", req.filename);
            }

            /* Clear busy state */
            js_busy = 0;
            js_current_file[0] = '\0';
        } else {
            /* No request, run timers */
            run_timers(ctx);
        }
    }

    /* Never reached */
    JS_FreeContext(ctx);
    free(mem_buf);
    vTaskDelete(NULL);
}

/* ============== JS Shell Commands ============== */

/* Shell command: Run JS file */
static void cmd_js_run(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: js_run <filename>\r\n");
        printf("Example: js_run /lfs/hello.js\r\n");
        return;
    }

    /* Check if already executing */
    if (js_busy) {
        printf("Error: JS VM busy, running %s\r\n", js_current_file);
        return;
    }

    /* Send async request (no blocking) */
    js_request_t req;
    strncpy(req.filename, argv[1], sizeof(req.filename) - 1);
    req.filename[sizeof(req.filename) - 1] = '\0';
    req.result = -1;

    if (xQueueSend(js_queue, &req, pdMS_TO_TICKS(100)) != pdTRUE) {
        printf("Error: Failed to queue request\r\n");
        return;
    }

    printf("JS execution started: %s\r\n", argv[1]);
    /* Return immediately - shell remains responsive! */
}
SHELL_CMD_EXPORT_ALIAS(cmd_js_run, js_run, Run JavaScript file from LittleFS);

/* Shell command: Evaluate JS expression */
static void cmd_js_eval(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: js_eval \"expression\"\r\n");
        printf("Example: js_eval \"print(1 + 2)\"\r\n");
        return;
    }

    /* Simple inline eval without task queue (for quick expressions) */
    uint8_t *mem_buf = malloc(JS_MEM_SIZE);
    if (!mem_buf) {
        printf("Error: Out of memory\r\n");
        return;
    }

    JSContext *ctx = JS_NewContext(mem_buf, JS_MEM_SIZE, &js_stdlib);
    JS_SetLogFunc(ctx, js_log_func);

    /* Set random seed */
    uint64_t time_us = bflb_mtimer_get_time_us();
    JS_SetRandomSeed(ctx, time_us ^ (time_us >> 32));

    JSValue val = JS_Eval(ctx, argv[1], strlen(argv[1]), "<shell>", 0);

    if (JS_IsException(val)) {
        JSValue exc = JS_GetException(ctx);
        printf("Error: ");
        JS_PrintValueF(ctx, exc, JS_DUMP_LONG);
        printf("\r\n");
    }

    JS_FreeContext(ctx);
    free(mem_buf);
}
SHELL_CMD_EXPORT_ALIAS(cmd_js_eval, js_eval, Evaluate JS expression);

/* Shell command: Show JS VM status */
static void cmd_js_status(int argc, char **argv)
{
    if (js_busy) {
        printf("JS VM: busy (running %s)\r\n", js_current_file);
    } else {
        printf("JS VM: idle\r\n");
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_js_status, js_status, Show JS VM status);

/* Shell command: Show JS VM info */
static void cmd_js_info(int argc, char **argv)
{
    printf("MQuickJS on Bouffalo SDK\r\n");
    printf("  JS Heap: %d bytes\r\n", JS_MEM_SIZE);
    printf("  Task Stack: %d bytes\r\n", JS_TASK_STACK_SIZE);
    printf("  Max Script: %d bytes\r\n", JS_MAX_SCRIPT_SIZE);
    printf("  Status: %s\r\n", js_busy ? "busy" : "idle");
    if (js_busy) {
        printf("  Running: %s\r\n", js_current_file);
    }
    printf("\r\nCommands:\r\n");
    printf("  js_run <file>  - Run JS from LittleFS\r\n");
    printf("  js_eval <expr> - Evaluate JS expression\r\n");
    printf("  js_status      - Show VM status\r\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_js_info, js_info, Show JS VM information);

/* ============== Initialization ============== */

void bouffalo_mquickjs_init(void)
{
    int ret;

    printf("\r\nJS Commands:\r\n");
    printf("  js_run <file>  - Run JS from LittleFS\r\n");
    printf("  js_eval <expr> - Evaluate JS expression\r\n");
    printf("  js_status      - Show VM status\r\n");
    printf("  js_info        - Show VM information\r\n");

    /* Create JS request queue */
    js_queue = xQueueCreate(4, sizeof(js_request_t));
    if (!js_queue) {
        printf("Error: Failed to create JS queue\r\n");
        while (1);
    }

    /* Create dedicated JS VM task */
    ret = xTaskCreate(js_vm_task, "JS_VM", JS_TASK_STACK_SIZE,
                      NULL, JS_TASK_PRIORITY, &js_task_handle);
    if (ret != pdPASS) {
        printf("Error: Failed to create JS task\r\n");
        while (1);
    }
}
