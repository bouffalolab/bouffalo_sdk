# nethub Framework

nethub is an efficient multi-interface network data forwarding framework that supports zerocopy data transmission and statically configured forwarding rules.

## Features

- **Multi-interface Support**: Supports various interface types including STA, AP, TCPIPSTA, TCPIPAP, TCPIPNAT, SDIO, USB, SPI, BRIDGE, and more
- **Zerocopy Transmission**: Achieves efficient data transmission through callback mechanism, avoiding data copying
- **Static Forwarding Rules**: Defines forwarding rules at compile time to reduce runtime overhead and improve performance
- **Concise API**: Minimal interface design for easy integration and use
- **Unified Logging System**: Linux kernel-style 4-level logging control (nh_err, nh_warn, nh_info, nh_debug)
- **One-click Testing**: Built-in make test command for quick function verification

## Architecture Design

### Core Components

1. **Interface Manager** (`nhif_ops.h`, `nhif_manager.c`)
   - Manages all registered network interfaces
   - Provides standardized interface operation abstraction

2. **Forwarding Engine** (`nh_forward.h`, `nh_forward.c`)
   - Implements packet forwarding logic
   - Supports multiple forwarding rule types: FORWARD, CONDITIONAL, BYPASS

3. **Core Framework** (`nethub.h`, `nethub.c`)
   - Coordinates various components
   - Provides unified initialization and control interfaces

4. **Logging System** (`nh_log.h`)
   - Linux kernel-style 4-level logging control
   - Unified logging macros: nh_err, nh_warn, nh_info, nh_debug
   - Selective inclusion at compile time for code size optimization

5. **Forwarding Rules Configuration** (`forwardrules/`)
   - Predefined static forwarding rule configurations
   - SDIO WiFi rules: STA → TCPIPSTA → SDIO
   - Hostless rules: STA ↔ TCPIPSTA, AP ↔ TCPIPAP

### Data Structures

#### nh_skb_t - Packet Buffer
```c
typedef struct nh_skb {
    uint8_t *data;              // Data buffer pointer
    uint32_t len;               // Data length
    void (*free_cb)(void *priv, void *frame_elem); // Free callback
    void *cb_priv;              // Callback private parameter
    void *cb_arg;               // Callback argument
    struct nh_skb *next;        // Linked list pointer
} nh_skb_t;
```

#### nhif_ops - Interface Operations
```c
struct nhif_ops {
    nhif_type_t type;                      // Interface type
    int (*init)(void);                     // Initialization
    int (*deinit)(void);                   // Deinitialization
    nhforward_callback_t input;            // Input callback
    nhforward_callback_t output;           // Output callback
};
```

### Forwarding Rule Types

- **NH_RULE_TYPE_FORWARD**: Direct forwarding to specified interface
- **NH_RULE_TYPE_CONDITIONAL**: Conditional forwarding based on previous interface's return value
- **NH_RULE_TYPE_BYPASS**: Bypass this interface without forwarding

## Configuration System

The nethub framework uses a static configuration system where forwarding rules are defined at compile time to reduce runtime overhead.

### Forwarding Rules Configuration

The framework provides predefined forwarding rule configurations located in the `forwardrules/` directory:

- **`forwardrules/sdiowifi_rules.c`** - SDIO WiFi application forwarding rules
  - Forwarding rules: STA → TCPIPSTA → SDIO chain forwarding
  - Usage: `nethub_init(sdiowifi_get_forward_config())`
  - Suitable for WiFi devices requiring SDIO communication

- **`forwardrules/hostless_rules.c`** - Hostless application forwarding rules
  - Forwarding rules: STA ↔ TCPIPSTA, AP ↔ TCPIPAP bidirectional forwarding
  - Usage: `nethub_init(hostless_get_forward_config())`
  - Suitable for standalone embedded devices

Each configuration file provides a unified interface: `*_get_forward_config()`

### Log Level Configuration

```c
/* Log level configuration */
#define CONFIG_NETHUB_LOG_LEVEL      4        /**< Log level: DEBUG */

/* Log level description */
/* 0: No log output */
/* 1: ERROR level (nh_err) */
/* 2: WARNING level (nh_err, nh_warn) */
/* 3: INFO level (nh_err, nh_warn, nh_info) */
/* 4: DEBUG level (nh_err, nh_warn, nh_info, nh_debug) */
```

## Usage

### 1. Include Header Files
```c
#include "nethub.h"
```

### 2. Implement Interface Operations
```c
static nh_forward_result_t my_if_input(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  // Avoid unused parameter warning
    // Handle received data
    printf("Received data: length %u\n", skb->len);
    return NH_FORWARD_CONTINUE;
}

static nh_forward_result_t my_if_output(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  // Avoid unused parameter warning
    // Handle data to be sent
    printf("Sending data: length %u\n", skb->len);
    return NH_FORWARD_CONTINUE;
}

static const struct nhif_ops my_if_ops = {
    .type = NHIF_TYPE_STA,
    .init = my_if_init,
    .deinit = my_if_deinit,
    .input = my_if_input,
    .output = my_if_output
};
```

### 3. Register Interface
```c
int ret = nhif_register(&my_if_ops, private_data);
if (ret != NH_OK) {
    printf("Interface registration failed: %d\n", ret);
    return -1;
}
```

### 4. Initialize Framework
```c
ret = nethub_init(sdiowifi_get_forward_config());  // Use predefined configuration
if (ret != NH_OK) {
    printf("nethub initialization failed: %d\n", ret);
    return -1;
}
```

### 5. Start Framework
```c
ret = nethub_start();
if (ret != NH_OK) {
    printf("nethub start failed: %d\n", ret);
    return -1;
}
```

### 6. Process Data
```c
// When interface receives data
nh_skb_t *skb = create_skb(...);
nethub_process_input(skb, NHIF_TYPE_STA);
```

### 7. Clean Up Resources
```c
nethub_stop();
nethub_deinit();
```

## Custom Forwarding Rules

You can customize forwarding behavior by defining your own forwarding rule tables:

```c
static const nh_forward_rule_t custom_sta_rules[] = {
    {NHIF_TYPE_TCPIPSTA, NH_RULE_TYPE_FORWARD, false},
    {NHIF_TYPE_BRIDGE, NH_RULE_TYPE_CONDITIONAL, true}
};

static const nh_if_forward_table_t custom_if_tables[] = {
    [NHIF_TYPE_STA] = {custom_sta_rules, 2},
    [NHIF_TYPE_SDIO] = {custom_sdio_rules, 1},
};

static const nh_forward_config_t custom_config = {
    .if_tables = custom_if_tables,
    .if_count = NHIF_TYPE_MAX
};

// Initialize with custom configuration
nethub_init(&custom_config);
```

## Build Examples

```bash
mkdir build
cd build
cmake ..
make

# One-click run all example tests
make test

# Or run examples individually
./examples/sdiowifi/sdiowifi_demo
./examples/hostless/hostless_demo
```

### make test Command

The `make test` command automatically:
- Checks and builds the project (if needed)
- Runs all example programs one by one
- Provides clear output titles for each test
- Automatically handles timeouts and error conditions
- Provides test completion summary

This is the recommended way to verify nethub framework functionality.

## Quick Integration

The nethub framework can be integrated into other projects as a submodule, providing predefined forwarding rule configurations.

### Supported Application Configurations

1. **SDIO WiFi Configuration** (`forwardrules/sdiowifi_rules.c`)
   - Supports SDIO transmission interface
   - Forwarding rules: STA → TCPIPSTA → SDIO chain forwarding
   - Suitable for WiFi devices requiring SDIO communication

2. **Hostless Configuration** (`forwardrules/hostless_rules.c`)
   - Standalone scenarios without SDIO dependency
   - Forwarding rules: STA ↔ TCPIPSTA, AP ↔ TCPIPAP bidirectional forwarding
   - Suitable for standalone embedded devices

### Integration Steps

#### 1. Add Submodule
```bash
git submodule add https://github.com/your-repo/nethub.git libs/nethub
```

#### 2. Configure CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.15)
project(my_app)

# Add nethub header file paths
include_directories(libs/nethub/include)
include_directories(libs/nethub/forwardrules)

# Collect nethub source files
set(NETHUB_SOURCES
    libs/nethub/src/nethub.c
    libs/nethub/src/nh_forward.c
    libs/nethub/src/nhif_manager.c
    libs/nethub/forwardrules/sdiowifi_rules.c  # or hostless_rules.c
)

# Create executable file
add_executable(my_app
    ${NETHUB_SOURCES}
    main.c
)
```

#### 3. Application Code Example
```c
#include <stdio.h>
#include "nethub.h"

/* Declare forwarding rule configuration function */
extern const nh_forward_config_t* sdiowifi_get_forward_config(void);
// or
// extern const nh_forward_config_t* hostless_get_forward_config(void);

int main(void)
{
    // Initialize nethub framework
    int ret = nethub_init(sdiowifi_get_forward_config());
    if (ret != NH_OK) {
        printf("nethub initialization failed: %d\n", ret);
        return -1;
    }

    // Start nethub framework
    ret = nethub_start();
    if (ret != NH_OK) {
        printf("nethub start failed: %d\n", ret);
        return -1;
    }

    // Your application logic...

    // Clean up resources
    nethub_stop();
    nethub_deinit();

    return 0;
}
```

## Directory Structure

```
nethub_proj/
├── include/               # Header files
│   ├── nethub_types.h    # Core data types
│   ├── nhif_ops.h        # Interface operations definition
│   ├── nh_forward.h      # Forwarding rules configuration
│   └── nethub.h          # Main interface
├── src/                  # Source files
│   ├── nethub.c          # Core framework implementation
│   ├── nhif_manager.c    # Interface manager
│   └── nh_forward.c      # Forwarding engine
├── forwardrules/         # Forwarding rules configuration
│   ├── sdiowifi_rules.c  # SDIO WiFi application forwarding rules
│   └── hostless_rules.c  # Hostless application forwarding rules
├── examples/             # Application examples
│   ├── sdiowifi/         # SDIO WiFi application example
│   └── hostless/         # Hostless application example
├── Kconfig               # Configuration file
├── Makefile              # Build and test scripts
├── CMakeLists.txt        # CMake build configuration
└── README.md             # This document
```

## API Reference

### nethub Core APIs
- `nethub_init()` - Initialize framework
- `nethub_start()` - Start framework
- `nethub_stop()` - Stop framework
- `nethub_deinit()` - Deinitialize framework
- `nethub_process_input()` - Process input data

### Interface Management APIs
- `nhif_register()` - Register interface
- `nhif_unregister()` - Unregister interface
- `nhif_find_by_type()` - Find interface by type

### Forwarding Configuration APIs
- `sdiowifi_get_forward_config()` - Get SDIO WiFi forwarding configuration
- `hostless_get_forward_config()` - Get Hostless forwarding configuration
- `nh_forward_print_rules()` - Print current forwarding rules

## Error Codes

- `NH_OK` (0) - Operation successful
- `NH_ERR_INVALID_PARAM` (-1) - Invalid parameter
- `NH_ERR_NOT_FOUND` (-2) - Interface not found
- `NH_ERR_ALREADY_EXISTS` (-3) - Interface already exists
- `NH_ERR_NO_MEMORY` (-4) - Out of memory
- `NH_ERR_NOT_INITIALIZED` (-5) - Not initialized
- `NH_ERR_INVALID_STATE` (-6) - Invalid state
- `NH_ERR_INTERNAL` (-7) - Internal error

## Notes

1. **Memory Management**: nh_skb_t memory management is the responsibility of the caller
2. **Thread Safety**: Current implementation is not thread-safe and should be used in single-threaded environments
3. **Callback Functions**: input/output callback functions should return as quickly as possible to avoid blocking
4. **Rule Configuration**: Forwarding rule tables cannot be modified at runtime and must be set during initialization

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Contributing

Issues and Pull Requests are welcome to improve the nethub framework.