# MicroQuickJS (MQuickJS) - JavaScript Engine for MCU

This document provides comprehensive guidance for working with MQuickJS, a JavaScript engine optimized for embedded systems.

## Overview

MicroQuickJS (MQuickJS) is a JavaScript engine targeted at embedded systems. It compiles and runs JavaScript programs using as little as **10 kB of RAM**. The whole engine requires about **100 kB of ROM** (ARM Thumb-2 code) including the C library.

### Key Features

- **Minimal RAM**: 10KB minimum for simple scripts
- **Minimal ROM**: ~100KB including C library
- **No malloc dependency**: Uses provided memory buffer
- **ROM-based stdlib**: Standard library compiled at build time
- **Bytecode support**: Pre-compile scripts for ROM execution
- **ES5+ subset**: Supports strict mode, typed arrays, `for of`

### Use Case

Running JavaScript on resource-constrained MCUs (BL616/BL618/BL808) for:
- Scriptable device behavior
- Dynamic configuration
- Rapid prototyping
- User-customizable logic

---

## File Structure

| File | Purpose |
|------|---------|
| `mquickjs.c` | Core engine implementation (bytecode compiler, interpreter, GC) |
| `mquickjs.h` | Public API header (include this in your application) |
| `mquickjs_priv.h` | Private headers (internal use only) |
| `mquickjs_build.c/h` | Stdlib build utility (generates stdlib tables) |
| `mquickjs_opcode.h` | Bytecode opcode definitions |
| `mqjs.c` | REPL (Read-Eval-Print Loop) implementation |
| `example.c` | C API example showing how to use the engine |
| `mqjs_stdlib.c` | Standard library definition for full REPL |
| `mqjs_stdlib.h` | Generated stdlib header (auto-generated) |
| `mquickjs_atom.h` | Generated atom table (auto-generated) |
| `example_stdlib.c` | Example stdlib with Rectangle class |
| `example_stdlib.h` | Generated example stdlib header |
| `cutils.c/h` | Utility functions (string handling, etc.) |
| `dtoa.c/h` | Double to ASCII conversion |
| `libm.c/h` | Internal mathematical library |
| `readline.c/h` | Line editing support |
| `readline_tty.c/h` | TTY-specific line editing |
| `list.h` | Linked list utilities |
| `softfp_template*.h` | Software floating-point templates |
| `tests/*.js` | JavaScript test files |

---

## Architecture

### Static Structure (Code Organization)

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                         │
│  (mqjs.c - REPL, example.c - Custom Application)            │
├─────────────────────────────────────────────────────────────┤
│                 Standard Library Layer                       │
│  (mqjs_stdlib.c - Define JS APIs, example_stdlib.c)         │
├─────────────────────────────────────────────────────────────┤
│                   Build Tools Layer                          │
│  (mquickjs_build.c - Generate stdlib tables for ROM)        │
├─────────────────────────────────────────────────────────────┤
│                     Core Engine                              │
│  (mquickjs.c - Parser, Compiler, VM, GC, Memory)            │
├─────────────────────────────────────────────────────────────┤
│                  Support Libraries                           │
│  (libm.c - Math, dtoa.c - Float conversion, cutils.c)       │
└─────────────────────────────────────────────────────────────┘
```

### Dynamic Structure (Runtime Call Flow)

```
Application Startup
    │
    ▼
┌─────────────────────────────────────────────┐
│ 1. Allocate memory buffer (malloc or static) │
│    mem_buf = malloc(mem_size);               │
└─────────────────────────────────────────────┘
    │
    ▼
┌─────────────────────────────────────────────┐
│ 2. Create JS Context                         │
│    ctx = JS_NewContext(mem_buf, mem_size,    │
│                        &js_stdlib);          │
│    └─> Load stdlib from ROM tables           │
│    └─> Initialize heap and GC                │
└─────────────────────────────────────────────┘
    │
    ▼
┌─────────────────────────────────────────────┐
│ 3. Configure callbacks (optional)            │
│    JS_SetLogFunc(ctx, log_func);             │
│    JS_SetInterruptHandler(ctx, handler);     │
└─────────────────────────────────────────────┘
    │
    ▼
┌─────────────────────────────────────────────┐
│ 4. Execute JavaScript                        │
│    val = JS_Eval(ctx, script, len,           │
│                  filename, flags);           │
│    └─> JS_Parse() → Compile to bytecode      │
│    └─> JS_Run() → Execute bytecode           │
└─────────────────────────────────────────────┘
    │
    ▼
┌─────────────────────────────────────────────┐
│ 5. Cleanup                                   │
│    JS_FreeContext(ctx);  // Call finalizers  │
│    free(mem_buf);        // Free memory      │
└─────────────────────────────────────────────┘
```

---

## Built-in APIs

### Global Functions

| Function | Description |
|----------|-------------|
| `print(...args)` | Print to console |
| `gc()` | Trigger garbage collection |
| `load(filename)` | Load and execute a JavaScript file |
| `parseInt(str, radix)` | Parse string to integer |
| `parseFloat(str)` | Parse string to float |
| `eval(code)` | Evaluate JavaScript code (global scope only) |
| `isNaN(value)` | Check if value is NaN |
| `isFinite(value)` | Check if value is finite |
| `setTimeout(callback, delay)` | Set a timeout (ms) |
| `clearTimeout(id)` | Clear a timeout |

### Global Properties

| Property | Description |
|----------|-------------|
| `Infinity` | Infinity value |
| `NaN` | Not-a-Number value |
| `undefined` | Undefined value |
| `globalThis` | Reference to global object |

### Built-in Objects

#### Object
- `hasOwnProperty()`, `toString()`
- `defineProperty()`, `getPrototypeOf()`, `setPrototypeOf()`, `create()`, `keys()`

#### Function
- `call()`, `apply()`, `bind()`, `toString()`
- Properties: `prototype`, `length`, `name`

#### Array
- `concat()`, `push()`, `pop()`, `shift()`, `unshift()`
- `slice()`, `splice()`, `reverse()`, `sort()`
- `indexOf()`, `lastIndexOf()`, `includes()`
- `forEach()`, `map()`, `filter()`, `reduce()`, `reduceRight()`
- `every()`, `some()`, `find()`, `findIndex()`
- `join()`, `toString()`, `isArray()`
- Property: `length`

#### String
- `charAt()`, `charCodeAt()`, `codePointAt()`
- `slice()`, `substring()`, `concat()`
- `indexOf()`, `lastIndexOf()`, `includes()`
- `match()`, `replace()`, `replaceAll()`, `search()`, `split()`
- `toLowerCase()`, `toUpperCase()`
- `trim()`, `trimStart()`, `trimEnd()`
- `repeat()`, `toString()`
- Static: `fromCharCode()`, `fromCodePoint()`
- Property: `length`

#### Number
- `toExponential()`, `toFixed()`, `toPrecision()`, `toString()`
- Static: `parseInt()`, `parseFloat()`
- Properties: `MAX_VALUE`, `MIN_VALUE`, `NaN`, `EPSILON`, etc.

#### Math
- `min()`, `max()`, `abs()`, `sign()`
- `floor()`, `ceil()`, `round()`, `trunc()`
- `sqrt()`, `pow()`, `random()`
- `imul()`, `clz32()`, `fround()`
- Constants: `E`, `PI`, `SQRT2`, `LN2`, etc.

#### Date (Limited)
- `Date.now()` - Get current timestamp in ms

#### Error Types
- `Error`, `EvalError`, `RangeError`, `ReferenceError`
- `SyntaxError`, `TypeError`, `URIError`, `InternalError`

#### console
- `console.log(...args)` - Print to console

#### performance
- `performance.now()` - Get high-resolution timestamp in ms

#### Typed Arrays
- `ArrayBuffer`, `Uint8ClampedArray`
- `Int8Array`, `Uint8Array`
- `Int16Array`, `Uint16Array`
- `Int32Array`, `Uint32Array`
- `Float32Array`, `Float64Array`

---

## stdlib C Function Mappings

### System-Dependent Functions (Require Porting for MCU)

| JS API | C Function | System Dependency | MCU Replacement |
|--------|------------|-------------------|-----------------|
| `print()` / `console.log()` | `js_print()` | `putchar()`, `fwrite(stdout)` | `bflb_uart_putchar()` |
| `Date.now()` | `js_date_now()` | `gettimeofday()` | `bflb_mtimer_get_ms()` |
| `performance.now()` | `js_performance_now()` | `clock_gettime()` / `gettimeofday()` | `bflb_mtimer_get_ms()` |
| `load()` | `js_load()` | `fopen()`, `fread()` | LittleFS/FatFS APIs |
| `setTimeout()` | `js_setTimeout()` | `get_time_ms()` | `bflb_mtimer_get_ms()` + FreeRTOS |

### Internal Functions (No System Dependency)

| JS API | C Function | Notes |
|--------|------------|-------|
| `gc()` | `js_gc()` | Calls internal `JS_GC()` |
| `parseInt()` | `js_number_parseInt()` | Pure engine implementation |
| `parseFloat()` | `js_number_parseFloat()` | Pure engine implementation |
| `eval()` | `js_global_eval()` | Pure engine implementation |
| `isNaN()` | `js_global_isNaN()` | Pure engine implementation |
| `isFinite()` | `js_global_isFinite()` | Pure engine implementation |
| `Math.*` | `js_math_*()` | Uses internal `libm.c` |
| `Array.*` | `js_array_*()` | Pure engine implementation |
| `String.*` | `js_string_*()` | Pure engine implementation |
| `Object.*` | `js_object_*()` | Pure engine implementation |
| `JSON.parse()` | `js_json_parse()` | Pure engine implementation |
| `JSON.stringify()` | `js_json_stringify()` | Pure engine implementation |
| `RegExp.*` | `js_regexp_*()` | Pure engine implementation |

---

## Threading Model

### IMPORTANT: No Multi-threading Support

MQuickJS is **single-threaded**:
- No mutex/lock mechanisms in the engine
- Garbage collector is NOT thread-safe
- Memory allocator is NOT thread-safe
- `JSContext` cannot be shared between threads

### MCU Integration Requirement

The JS VM **MUST** run in a dedicated FreeRTOS task:

```c
#include "FreeRTOS.h"
#include "task.h"
#include "mquickjs.h"

#define JS_MEM_SIZE     (64 * 1024)  // 64KB for JS heap
#define JS_TASK_STACK   (8 * 1024)   // 8KB task stack
#define JS_TASK_PRIORITY 5

static void js_log_func(void *opaque, const void *buf, size_t buf_len)
{
    // Replace with Bouffalo SDK UART output
    // bflb_uart_putbuf(uart0, buf, buf_len);
    fwrite(buf, 1, buf_len, stdout);
}

static void js_task(void *pvParameters)
{
    JSContext *ctx;
    uint8_t *mem_buf;
    JSValue ret;

    // Allocate memory for JS VM
    mem_buf = pvPortMalloc(JS_MEM_SIZE);
    if (!mem_buf) {
        printf("Failed to allocate JS memory\n");
        vTaskDelete(NULL);
        return;
    }

    // Create JS context
    ctx = JS_NewContext(mem_buf, JS_MEM_SIZE, &js_stdlib);
    JS_SetLogFunc(ctx, js_log_func);

    // Execute initial script
    const char *script = "print('Hello from JS VM!');";
    ret = JS_Eval(ctx, script, strlen(script), "<init>", 0);

    if (JS_IsException(ret)) {
        JSValue exc = JS_GetException(ctx);
        JS_PrintValueF(ctx, exc, JS_DUMP_LONG);
    }

    // Main event loop
    while (1) {
        // Process JS timers/events
        run_timers(ctx);

        // Yield to other tasks
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Cleanup (never reached in this example)
    JS_FreeContext(ctx);
    vPortFree(mem_buf);
    vTaskDelete(NULL);
}

void start_js_vm(void)
{
    xTaskCreate(js_task, "JS_VM", JS_TASK_STACK, NULL,
                JS_TASK_PRIORITY, NULL);
}
```

### Thread-Safe MCU Driver Access

When adding JS APIs for hardware drivers:

1. **Direct Access** (if driver API is thread-safe):
```c
static JSValue js_gpio_write(JSContext *ctx, JSValue *this_val,
                             int argc, JSValue *argv)
{
    int pin, value;
    JS_ToInt32(ctx, &pin, argv[0]);
    JS_ToInt32(ctx, &value, argv[1]);
    bflb_gpio_set(pin, value);  // Direct call if thread-safe
    return JS_UNDEFINED;
}
```

2. **Queue-Based Access** (for non-thread-safe drivers):
```c
static QueueHandle_t gpio_queue;

typedef struct {
    int pin;
    int value;
    SemaphoreHandle_t done;
} gpio_request_t;

static JSValue js_gpio_write(JSContext *ctx, JSValue *this_val,
                             int argc, JSValue *argv)
{
    gpio_request_t req;
    req.pin = ...;
    req.value = ...;
    req.done = xSemaphoreCreateBinary();

    xQueueSend(gpio_queue, &req, portMAX_DELAY);
    xSemaphoreTake(req.done, portMAX_DELAY);  // Wait for completion

    vSemaphoreDelete(req.done);
    return JS_UNDEFINED;
}
```

---

## Adding New JS APIs for MCU Drivers

### Step 1: Define C Functions

Create C functions that wrap Bouffalo SDK APIs:

```c
// Example: GPIO bindings
#include "bflb_gpio.h"

static JSValue js_gpio_init(JSContext *ctx, JSValue *this_val,
                            int argc, JSValue *argv)
{
    int pin, mode;

    if (JS_ToInt32(ctx, &pin, argv[0]))
        return JS_ThrowTypeError(ctx, "pin must be a number");
    if (JS_ToInt32(ctx, &mode, argv[1]))
        return JS_ThrowTypeError(ctx, "mode must be a number");

    // Call Bouffalo SDK API
    bflb_gpio_init(pin, mode);

    return JS_UNDEFINED;
}

static JSValue js_gpio_write(JSContext *ctx, JSValue *this_val,
                             int argc, JSValue *argv)
{
    int pin, value;

    JS_ToInt32(ctx, &pin, argv[0]);
    JS_ToInt32(ctx, &value, argv[1]);

    bflb_gpio_set(pin, value);
    return JS_UNDEFINED;
}

static JSValue js_gpio_read(JSContext *ctx, JSValue *this_val,
                            int argc, JSValue *argv)
{
    int pin;
    JS_ToInt32(ctx, &pin, argv[0]);

    int value = bflb_gpio_get(pin);
    return JS_NewInt32(ctx, value);
}
```

### Step 2: Define Property Table

```c
static const JSPropDef js_gpio_proto[] = {
    JS_PROP_END,
};

static const JSPropDef js_gpio[] = {
    JS_CFUNC_DEF("init", 2, js_gpio_init),
    JS_CFUNC_DEF("write", 2, js_gpio_write),
    JS_CFUNC_DEF("read", 1, js_gpio_read),
    // Constants
    JS_PROP_INT_DEF("OUTPUT", GPIO_OUTPUT, 0),
    JS_PROP_INT_DEF("INPUT", GPIO_INPUT, 0),
    JS_PROP_INT_DEF("PULLUP", GPIO_PULLUP, 0),
    JS_PROP_INT_DEF("PULLDOWN", GPIO_PULLDOWN, 0),
    JS_PROP_END,
};

static const JSClassDef js_gpio_class =
    JS_OBJECT_DEF("GPIO", js_gpio);
```

### Step 3: Add to Global Object

Modify `js_global_object[]` in your stdlib file:

```c
static const JSPropDef js_global_object[] = {
    // ... existing objects ...
    JS_PROP_CLASS_DEF("GPIO", &js_gpio_class),
    // ... more MCU bindings ...
    JS_PROP_END,
};
```

### Step 4: Define Class ID (for classes with opaque data)

```c
#define JS_CLASS_GPIO (JS_CLASS_USER + 0)
#define JS_CLASS_UART (JS_CLASS_USER + 1)
// ... more classes ...

#define JS_CLASS_COUNT (JS_CLASS_USER + N)
```

### Step 5: Rebuild stdlib

```bash
# Generate stdlib header
./mqjs_stdlib > mqjs_stdlib.h

# Or for custom stdlib
gcc -o my_stdlib my_stdlib.c mquickjs_build.c -lm
./my_stdlib > my_stdlib.h
```

---

## Memory Management

### Key Concepts

1. **Compacting GC**: Objects can move during garbage collection
2. **JSGCRef**: Use for stable references when objects might move
3. **No explicit free**: No `JS_FreeValue()` needed (unlike QuickJS)

### Using JSGCRef

When you need a stable reference across allocations:

```c
static JSValue my_function(JSContext *ctx, JSValue *this_val,
                           int argc, JSValue *argv)
{
    JSGCRef obj1_ref, obj2_ref;
    JSValue *obj1, *obj2, ret;

    ret = JS_EXCEPTION;

    obj1 = JS_PushGCRef(ctx, &obj1_ref);
    obj2 = JS_PushGCRef(ctx, &obj2_ref);

    *obj1 = JS_NewObject(ctx);
    if (JS_IsException(*obj1))
        goto fail;

    *obj2 = JS_NewObject(ctx);  // obj1 may move here!
    if (JS_IsException(*obj2))
        goto fail;

    JS_SetPropertyStr(ctx, *obj1, "x", *obj2);
    ret = *obj1;

fail:
    JS_PopGCRef(ctx, &obj2_ref);
    JS_PopGCRef(ctx, &obj1_ref);
    return ret;
}
```

### Opaque Data for Objects

```c
typedef struct {
    int pin;
    int mode;
} GPIOData;

static JSValue js_gpio_constructor(JSContext *ctx, JSValue *this_val,
                                   int argc, JSValue *argv)
{
    GPIOData *data;
    JSValue obj;

    if (!(argc & FRAME_CF_CTOR))
        return JS_ThrowTypeError(ctx, "must be called with new");

    obj = JS_NewObjectClassUser(ctx, JS_CLASS_GPIO);
    data = malloc(sizeof(*data));
    JS_SetOpaque(ctx, obj, data);

    JS_ToInt32(ctx, &data->pin, argv[0]);
    JS_ToInt32(ctx, &data->mode, argv[1]);

    return obj;
}

static void js_gpio_finalizer(JSContext *ctx, void *opaque)
{
    GPIOData *data = opaque;
    free(data);
}

// In class definition:
static const JSClassDef js_gpio_class =
    JS_CLASS_DEF("GPIO", 1, js_gpio_constructor, JS_CLASS_GPIO,
                 js_gpio, js_gpio_proto, NULL, js_gpio_finalizer);
```

---

## Bytecode Compilation

### Compile to Bytecode

```bash
# Compile script to bytecode
./mqjs -o script.bin script.js

# Force 32-bit bytecode (for 32-bit MCU from 64-bit host)
./mqjs -m32 -o script.bin script.js
```

### Execute Bytecode on MCU

```c
// Load bytecode from ROM/Flash
extern const uint8_t script_bytecode[];
extern const uint32_t script_bytecode_len;

// Relocate bytecode (if stored in ROM)
JS_RelocateBytecode(ctx, (uint8_t *)script_bytecode, script_bytecode_len);

// Load and run
JSValue func = JS_LoadBytecode(ctx, script_bytecode);
JSValue result = JS_Run(ctx, func);
```

---

## JavaScript Subset Reference

### Supported (ES5+ subset)

- Strict mode only
- Basic types: number, string, boolean, object, array
- Typed arrays
- `for of` loops (arrays only)
- Math functions: `imul`, `clz32`, `trunc`, `log2`, `log10`
- Date: `Date.now()` only
- Exponentiation operator (`**`)
- `\u{hex}` in string literals
- RegExp: `s`, `y`, `u` flags

### NOT Supported

- `with` statement
- Direct `eval` (only global eval)
- Array holes (e.g., `[1, , 3]` is error)
- Value boxing (`new Number(1)` not supported)
- Custom iterators
- Full Unicode property support in RegExp
- `Date` constructor (only `Date.now()`)

---

## Building

```bash
# Build REPL and example
make

# Build for ARM32
make CONFIG_ARM32=y

# Small build (optimized for size)
make CONFIG_SMALL=y

# Run tests
make test

# Run benchmarks
make microbench
```

---

## POSIX API Compatibility with Bouffalo SDK

This section analyzes the POSIX APIs used by mquickjs and their support status in Bouffalo SDK.

### API Compatibility Summary

| Category | APIs Used | Bouffalo SDK Support | Notes |
|----------|-----------|---------------------|-------|
| **File I/O** | fopen, fclose, fread, fwrite, fseek, ftell, fprintf, fflush | ✅ Supported | Via newlib + LittleFS/FatFS |
| **Memory** | malloc, free, realloc, calloc, strdup | ✅ Supported | Via newlib + SDK kmalloc |
| **Time** | clock_gettime, gettimeofday, nanosleep | ⚠️ Partial | gettimeofday not in FreeRTOS POSIX |
| **Console** | putchar, fwrite(stdout) | ✅ Supported | Via newlib TTY port |
| **String** | strlen, strcmp, memcpy, memmove, memset, memcmp, strchr, strtol, strtod | ✅ Supported | Full newlib support |
| **Math** | sin, cos, sqrt, pow, etc. | ✅ Supported | Internal libm.c (no dependency) |
| **Signal** | signal, sigaction | ❌ NOT Supported | FreeRTOS POSIX has no signals |
| **Control** | setjmp, longjmp, abort, atexit | ✅ Supported | Standard C library |

### Detailed API Analysis

#### ✅ Fully Supported APIs

**File I/O (via LittleFS/FatFS port):**
```
Source files: mqjs.c, example.c
APIs: fopen, fclose, fread, fwrite, fseek, ftell, fprintf, fflush
Support: components/libc/newlib/syscalls.c
```

The newlib syscalls layer provides:
- `_open_r`, `_close_r`, `_read_r`, `_write_r`, `_lseek_r`
- LittleFS backend via `_open_file_lfs_r`, etc.
- FatFS backend via `_open_file_r`, etc.
- TTY console via `_write_tty_r`, `_read_tty_r`

**Memory Management:**
```
Source files: mqjs.c, example.c, dtoa.c, mquickjs_build.c
APIs: malloc, free, realloc, calloc, strdup
Support: components/libc/newlib/syscalls.c → kmalloc/kfree
```

**String Functions:**
```
Source files: All .c files
APIs: strlen, strcmp, strncmp, memcpy, memmove, memset, memcmp, strchr, strtol, strtod
Support: Full newlib implementation
```

**Math Functions:**
```
Source files: libm.c (internal)
APIs: sin, cos, tan, sqrt, pow, exp, log, etc.
Support: Internal implementation in libm.c - NO system dependency!
```

#### ⚠️ Partially Supported APIs (Need Porting)

**Time Functions:**
```
Source files: mqjs.c:78,85,93,734; example.c:198,205,213
APIs: clock_gettime(CLOCK_MONOTONIC), gettimeofday(), nanosleep()
```

| Function | Bouffalo SDK Status | Replacement |
|----------|---------------------|-------------|
| `clock_gettime(CLOCK_MONOTONIC, &ts)` | ⚠️ Ignores clock_id | Use `bflb_mtimer_get_ms()` |
| `gettimeofday(&tv, NULL)` | ⚠️ In newlib, may need verification | Use `bflb_mtimer_get_time_us()` |
| `nanosleep(&ts, NULL)` | ✅ Supported | FreeRTOS `vTaskDelay()` |

**Porting Example:**
```c
// Original (mqjs.c:78-87)
#if defined(__linux__) || defined(__APPLE__)
static int64_t get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);
}
#else
static int64_t get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + (tv.tv_usec / 1000);
}
#endif

// Ported for Bouffalo SDK
#include "bflb_mtimer.h"

static int64_t get_time_ms(void)
{
    return (int64_t)(bflb_mtimer_get_time_us() / 1000);
}
```

#### ❌ NOT Supported APIs (Must Remove/Replace)

**Signal Handling:**
```
Source files: readline_tty.c:126,157
APIs: signal(SIGINT, ...), sigaction(SIGINT, ...)
Status: NOT IMPLEMENTED in FreeRTOS POSIX
```

**Impact:** The readline TTY module uses signals for Ctrl+C handling. This must be replaced with polling-based input.

**Solution:** Replace readline_tty.c with a polling-based UART input implementation:

```c
// Instead of signal-based Ctrl+C handling
// Use polling with timeout

#include "bflb_uart.h"

static int check_interrupt(struct bflb_device_s *uart)
{
    uint8_t c;
    if (bflb_uart_rxints_masked(uart) & UART_RX_STS_RX_DONE) {
        bflb_uart_getchar(uart, &c);
        if (c == 0x03) {  // Ctrl+C
            return 1;  // Interrupt requested
        }
    }
    return 0;
}
```

### FreeRTOS POSIX Compliance (from components/os/freertos/posix/)

| POSIX Area | Compliance | Notes |
|------------|------------|-------|
| Threads (pthread) | ~90% | No pthread_cancel |
| Mutexes | 100% | All types supported |
| Condition Variables | 100% | Complete |
| Semaphores | 100% | Complete |
| Clock/Timer | 60% | Basic timing, no system time |
| File I/O | 70% | TTY + LittleFS/FatFS |
| **Signals** | **0%** | **NOT IMPLEMENTED** |
| Process Management | 0% | Single process only |
| Sockets | 0% | No network API |

### Newlib syscalls Support (from components/libc/newlib/syscalls.c)

**File Descriptor Ranges:**
```
0x0000-0x0FFF: TTY/Console
0x4000-0x4FFF: FatFS
0x8000-0x8FFF: LittleFS
```

**Implemented syscalls:**
- `_open_r`, `_close_r`, `_read_r`, `_write_r`, `_lseek_r`
- `_stat_r`, `_fstat_r`, `_rename_r`, `_unlink_r`
- `_mkdir_r`, `_rmdir_r`, `_chdir_r`, `_getcwd_r`
- `_malloc_r`, `_free_r`, `_realloc_r`, `_calloc_r`

**NOT Implemented:**
- `sbrk` - returns ENOSYS
- Process functions (execve, fork, getpid, kill, wait)

### Porting Requirements Summary

To run mquickjs on Bouffalo SDK, the following changes are required:

| Component | Change Required | Effort |
|-----------|-----------------|--------|
| **Time functions** | Replace with `bflb_mtimer_*` APIs | Low |
| **Signal handling** | Remove readline_tty.c, use polling UART | Medium |
| **Console I/O** | Replace putchar/fwrite with `bflb_uart_*` | Low |
| **File I/O** | Use LittleFS POSIX port (already works) | None |
| **Memory** | Use newlib (already works via kmalloc) | None |

### Required Configuration

```
# In defconfig or via menuconfig
CONFIG_NEWLIB=y           # Enable newlib
CONFIG_NEWLIB_LITTLEFS=y  # Enable LittleFS POSIX port (for load() function)
CONFIG_POSIX=y            # Enable FreeRTOS POSIX layer
```

### Minimal Porting Example

```c
// mquickjs_port.c - Bouffalo SDK porting layer

#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "FreeRTOS.h"
#include "task.h"

// Time function replacement
int64_t get_time_ms(void)
{
    return (int64_t)(bflb_mtimer_get_time_us() / 1000);
}

// Console output replacement
static struct bflb_device_s *console_uart;

void port_console_init(void)
{
    console_uart = bflb_device_get_by_name("uart0");
}

void port_putchar(char c)
{
    bflb_uart_putchar(console_uart, c);
}

void port_puts(const char *s, size_t len)
{
    bflb_uart_putbuf(console_uart, s, len);
}

// Sleep replacement
void port_sleep_ms(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}
```

---

## Time Function Implementation Details

如果需要实现 `clock_gettime` 和 `gettimeofday`，可以按以下方式实现：

### Option 1: Minimal Implementation (for mquickjs only)
直接在 mquickjs 中使用 `bflb_mtimer_get_time_us()` 替换 POSIX 时间函数。
```c
#include "bflb_mtimer.h"

static int64_t get_time_ms(void)
{
    return (int64_t)(bflb_mtimer_get_time_us() / 1000);
}

static JSValue js_date_now(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    return JS_NewInt64(ctx, get_time_ms());
}

static JSValue js_performance_now(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    return JS_NewInt64(ctx, get_time_ms());
}
```

### Option 2: Full POSIX Implementation (for newlib port)
未来可移植到 `components/libc/newlib/syscalls.c` 中：

```c
#include <time.h>
#include <sys/time.h>
#include "bflb_mtimer.h"
#include "FreeRTOS.h"
#include "task.h"

/* clock_gettime implementation */
int _clock_gettime_r(struct _reent *reent, clockid_t clk_id, struct timespec *tp)
{
    uint64_t time_us;

    if (tp == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    time_us = bflb_mtimer_get_time_us();
    /* Both clocks return same value (monotonic time since boot) */
    tp->tv_sec = time_us / 1000000;
    tp->tv_nsec = (time_us % 1000000) * 1000;

    return 0;
}

/* gettimeofday implementation */
int _gettimeofday_r(struct _reent *reent, struct timeval *tp, void *tzvp)
{
    uint64_t time_us;

    if (tp != NULL) {
        time_us = bflb_mtimer_get_time_us();
        tp->tv_sec = time_us / 1000000;
        tp->tv_usec = time_us % 1000000;
    }

    /* Ignore timezone parameter */
    return 0;
}

/* nanosleep implementation */
int _nanosleep_r(struct _reent *reent, const struct timespec *rqtp, struct timespec *rmtp)
{
    uint64_t start_us, elapsed_us, target_us;

    if (rqtp == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    target_us = (uint64_t)rqtp->tv_sec * 1000000 + rqtp->tv_nsec / 1000;

    start_us = bflb_mtimer_get_time_us();

    /* Use FreeRTOS delay for longer sleeps, busy-wait for short */
    if (target_us >= 1000) {
        TickType_t ticks = pdMS_TO_TICKS(target_us / 1000);
        vTaskDelay(ticks);
    } else {
        /* Busy wait for sub-millisecond delays */
        while ((bflb_mtimer_get_time_us() - start_us) < target_us) {
            /* spin */
        }
    }

    if (rmtp != NULL) {
        elapsed_us = bflb_mtimer_get_time_us() - start_us;
        rmtp->tv_sec = elapsed_us / 1000000;
        rmtp->tv_nsec = (elapsed_us % 1000000) * 1000;
    }

    return 0;
}
```

### Files to Modify
| 文件 | 修改内容 |
|------|---------|
| `examples/mquickjs/mqjs.c` | 替换 get_time_ms(), js_date_now(), js_performance_now() |
| `examples/mquickjs/example.c` | 替换 get_time_ms(), js_date_now(), js_performance_now() |
| `components/libc/newlib/syscalls.c` | (未来) 添加 _clock_gettime_r, _gettimeofday_r, _nanosleep_r |

### 需要包含的头文件
```c
#include "bflb_mtimer.h"
#include "FreeRTOS.h"
#include "task.h"
```

---

## References

- **Original MQuickJS**: https://bellard.org/mquickjs/
- **QuickJS**: https://bellard.org/quickjs/
- **Bouffalo SDK**: https://github.com/bouffalolab/bouffalo_sdk
