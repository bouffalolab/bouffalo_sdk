#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bflb_gpio.h"
#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
#include "bflb_i2c.h"
#endif
#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"
#include "shell.h"
#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
#include <FreeRTOS.h>
#include <task.h>
#endif

#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
#include "pca9698.h"
#endif

#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
#ifndef CONFIG_GPIO_TESTER_I2C_FREQ
#define CONFIG_GPIO_TESTER_I2C_FREQ 400000
#endif
#ifndef CONFIG_GPIO_TESTER_I2C0_SDA_PIN
#define CONFIG_GPIO_TESTER_I2C0_SDA_PIN 11
#endif
#ifndef CONFIG_GPIO_TESTER_I2C0_SCL_PIN
#define CONFIG_GPIO_TESTER_I2C0_SCL_PIN 14
#endif
#ifndef CONFIG_GPIO_TESTER_I2C1_SDA_PIN
#define CONFIG_GPIO_TESTER_I2C1_SDA_PIN 17
#endif
#ifndef CONFIG_GPIO_TESTER_I2C1_SCL_PIN
#define CONFIG_GPIO_TESTER_I2C1_SCL_PIN 16
#endif
#ifndef CONFIG_GPIO_TESTER_I2C0_ADDR
#define CONFIG_GPIO_TESTER_I2C0_ADDR 0x20
#endif
#ifndef CONFIG_GPIO_TESTER_I2C1_ADDR
#define CONFIG_GPIO_TESTER_I2C1_ADDR 0x20
#endif
#endif

#define GPIOV_VERSION "1.2"
#define GPIOV_PARSE_BUFFER_SIZE 512
#define GPIOV_LEVELS_BUFFER_SIZE 1024

#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
#define GPIOV_MAX_PIN_COUNT 80U
#define GPIOV_BACKEND_IS_TESTER 1
#else
#define GPIOV_MAX_PIN_COUNT GPIO_PIN_MAX
#define GPIOV_BACKEND_IS_TESTER 0
#endif

enum gpiov_mode_type {
    GPIOV_MODE_NONE = 0,
    GPIOV_MODE_INPUT,
    GPIOV_MODE_OUTPUT,
    GPIOV_MODE_IRQ,
};

enum gpiov_backend_type {
    GPIOV_BACKEND_DUT = 0,
    GPIOV_BACKEND_TESTER,
};

struct gpiov_pin_state {
    bool configured;
    enum gpiov_mode_type mode;
    const char *pull_name;
    uint8_t drive;
    uint8_t irq_trig;
    bool last_level;
    volatile uint32_t irq_count;
    volatile uint64_t last_irq_ms;
};

struct gpiov_pin_group {
    uint8_t pins[GPIOV_MAX_PIN_COUNT];
    uint32_t count;
    bool selected[GPIOV_MAX_PIN_COUNT];
};

struct gpiov_config_options {
    uint32_t pull_cfg;
    const char *pull_name;
    uint32_t drive_cfg;
    uint8_t drive_value;
    uint8_t irq_trig;
    uint32_t init_level;
    bool pull_specified;
    bool drive_specified;
    bool trig_specified;
    bool init_specified;
};

struct gpiov_backend_warning {
    bool ignored_pull;
    bool ignored_drive;
    bool ignored_trig;
};

struct gpiov_backend_status {
    const char *backend_name;
    const char *bus_name;
    uint8_t expander_addr;
    uint8_t bank;
    uint8_t bit;
};

static struct bflb_device_s *g_gpio;
static struct bflb_device_s *g_uart0;
static struct gpiov_pin_state g_pin_state[GPIOV_MAX_PIN_COUNT];
static bool g_backend_ready;
static enum gpiov_backend_type g_backend_type = GPIOV_BACKEND_DUT;

#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
#if !defined(GPIO_FUNC_I2C1)
#error "CONFIG_GPIO_TESTER requires GPIO_FUNC_I2C1 support"
#endif

static struct bflb_device_s *g_i2c0;
static struct bflb_device_s *g_i2c1;
static struct pca9698_device g_expander_i2c0;
static struct pca9698_device g_expander_i2c1;
#endif

static void gpiov_reply(const char *result, const char *fmt, ...);
static int gpiov_ping_cmd(int argc, char **argv);
static int gpiov_help_cmd(int argc, char **argv);
static int gpiov_config_cmd(int argc, char **argv);
static int gpiov_config_many_cmd(int argc, char **argv);
static int gpiov_write_cmd(int argc, char **argv);
static int gpiov_write_pattern_cmd(int argc, char **argv);
static int gpiov_read_cmd(int argc, char **argv);
static int gpiov_read_many_cmd(int argc, char **argv);
static int gpiov_read_expect_cmd(int argc, char **argv);
static int gpiov_pulse_cmd(int argc, char **argv);
static int gpiov_irq_get_cmd(int argc, char **argv);
static int gpiov_irq_get_many_cmd(int argc, char **argv);
static int gpiov_irq_clear_cmd(int argc, char **argv);
static int gpiov_irq_clear_many_cmd(int argc, char **argv);
static int gpiov_irq_wait_cmd(int argc, char **argv);
static int gpiov_irq_expect_cmd(int argc, char **argv);
static int gpiov_status_cmd(int argc, char **argv);
static int gpiov_status_many_cmd(int argc, char **argv);
static int gpiov_deinit_cmd(int argc, char **argv);

static bool gpiov_is_reserved_pin(uint8_t pin);
static const char *gpiov_reserved_pins_name(void);
static bool gpiov_parse_pin(const char *text, uint8_t *pin);
static bool gpiov_parse_pin_group(const char *text, struct gpiov_pin_group *group);
static bool gpiov_parse_u32(const char *text, uint32_t *value);
static const char *gpiov_mode_name(enum gpiov_mode_type mode);
static bool gpiov_parse_mode(const char *text, enum gpiov_mode_type *mode);
static bool gpiov_parse_pull(const char *text, uint32_t *pull_cfg, const char **pull_name);
static bool gpiov_parse_drive(const char *text, uint32_t *drive_cfg, uint8_t *drive_value);
static bool gpiov_parse_irq_trig(const char *text, uint8_t *trig_mode, const char **trig_name);
static const char *gpiov_irq_trig_name(uint8_t trig_mode);
static void gpiov_init_config_options(struct gpiov_config_options *options);
static bool gpiov_parse_config_options(int argc, char **argv, int start_index,
                                       struct gpiov_config_options *options,
                                       const char **error_reason, const char **error_value);
static uint32_t gpiov_make_cfgset(enum gpiov_mode_type mode, uint32_t pull_cfg, uint32_t drive_cfg);
static int gpiov_native_configure_pin(uint8_t pin, enum gpiov_mode_type mode, uint32_t pull_cfg,
                                      const char *pull_name, uint32_t drive_cfg, uint8_t drive_value,
                                      uint8_t irq_trig, uint32_t init_level);
static int gpiov_backend_init(void);
static uint32_t gpiov_backend_pin_count(void);
static const char *gpiov_backend_name(void);
static bool gpiov_backend_supports_irq(void);
static int gpiov_backend_configure_pin(uint8_t pin, enum gpiov_mode_type mode,
                                       const struct gpiov_config_options *options,
                                       struct gpiov_backend_warning *warning);
static int gpiov_backend_write_pin(uint8_t pin, bool level);
static int gpiov_backend_read_pin(uint8_t pin, bool *level);
static int gpiov_backend_deinit_pin(uint8_t pin);
static int gpiov_backend_get_status(uint8_t pin, struct gpiov_backend_status *status);
static bool gpiov_group_contains(const struct gpiov_pin_group *group, uint8_t pin);
static bool gpiov_group_find_reserved_pin(const struct gpiov_pin_group *group, uint8_t *reserved_pin);
static void gpiov_format_pin_levels(const struct gpiov_pin_group *group, char *buffer, size_t buffer_size);
static void gpiov_format_pin_modes(const struct gpiov_pin_group *group, char *buffer, size_t buffer_size);
static void gpiov_format_pin_pulls(const struct gpiov_pin_group *group, char *buffer, size_t buffer_size);
static void gpiov_format_irq_counts(const struct gpiov_pin_group *group, char *buffer, size_t buffer_size);
static void gpiov_format_irq_deltas(const struct gpiov_pin_group *group, const uint32_t *baseline,
                                    char *buffer, size_t buffer_size);
static void gpiov_format_backend_warning(const struct gpiov_backend_warning *warning,
                                         char *buffer, size_t buffer_size);
static void gpiov_irq_handler(uint8_t pin);

static void gpiov_reply(const char *result, const char *fmt, ...)
{
    va_list args;

    printf("GPIOV %s", result);
    if ((fmt != NULL) && (fmt[0] != '\0')) {
        printf(" ");
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
    printf("\r\n");
}

static bool gpiov_is_reserved_pin(uint8_t pin)
{
#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
    (void)pin;
    return false;
#else
#if defined(BL616CL)
    return (pin == GPIO_PIN_34) || (pin == GPIO_PIN_35);
#elif defined(BL616)
    return (pin == GPIO_PIN_21) || (pin == GPIO_PIN_22);
#else
    (void)pin;
    return false;
#endif
#endif
}

static const char *gpiov_reserved_pins_name(void)
{
#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
    return "none";
#else
#if defined(BL616CL)
    return "GPIO34/GPIO35";
#elif defined(BL616)
    return "GPIO21/GPIO22";
#else
    return "none";
#endif
#endif
}

static bool gpiov_parse_u32(const char *text, uint32_t *value)
{
    char *endptr = NULL;
    unsigned long parsed;

    if ((text == NULL) || (value == NULL) || (text[0] == '\0')) {
        return false;
    }

    parsed = strtoul(text, &endptr, 0);
    if ((endptr == text) || (*endptr != '\0')) {
        return false;
    }

    *value = (uint32_t)parsed;
    return true;
}

static bool gpiov_parse_pin(const char *text, uint8_t *pin)
{
    uint32_t pin_value;

    if (!gpiov_parse_u32(text, &pin_value)) {
        return false;
    }
    if (pin_value >= gpiov_backend_pin_count()) {
        return false;
    }

    *pin = (uint8_t)pin_value;
    return true;
}

static bool gpiov_parse_pin_group(const char *text, struct gpiov_pin_group *group)
{
    char pins_copy[GPIOV_PARSE_BUFFER_SIZE];
    char *token;
    char *saveptr = NULL;
    uint8_t pin;

    if ((text == NULL) || (group == NULL) || (text[0] == '\0')) {
        return false;
    }
    if (strlen(text) >= sizeof(pins_copy)) {
        return false;
    }

    memset(group, 0, sizeof(*group));
    strcpy(pins_copy, text);

    token = strtok_r(pins_copy, ",", &saveptr);
    while (token != NULL) {
        if (!gpiov_parse_pin(token, &pin)) {
            return false;
        }
        if (group->selected[pin]) {
            return false;
        }

        group->selected[pin] = true;
        group->pins[group->count++] = pin;
        token = strtok_r(NULL, ",", &saveptr);
    }

    return group->count != 0U;
}

static const char *gpiov_mode_name(enum gpiov_mode_type mode)
{
    switch (mode) {
        case GPIOV_MODE_INPUT:
            return "in";
        case GPIOV_MODE_OUTPUT:
            return "out";
        case GPIOV_MODE_IRQ:
            return "irq";
        default:
            return "none";
    }
}

static bool gpiov_parse_mode(const char *text, enum gpiov_mode_type *mode)
{
    if ((text == NULL) || (mode == NULL)) {
        return false;
    }

    if (strcmp(text, "in") == 0) {
        *mode = GPIOV_MODE_INPUT;
        return true;
    }
    if (strcmp(text, "out") == 0) {
        *mode = GPIOV_MODE_OUTPUT;
        return true;
    }
    if (strcmp(text, "irq") == 0) {
        *mode = GPIOV_MODE_IRQ;
        return true;
    }

    return false;
}

static bool gpiov_parse_pull(const char *text, uint32_t *pull_cfg, const char **pull_name)
{
    if ((text == NULL) || (pull_cfg == NULL) || (pull_name == NULL)) {
        return false;
    }

    if (strcmp(text, "up") == 0) {
        *pull_cfg = GPIO_PULLUP;
        *pull_name = "up";
        return true;
    }
    if (strcmp(text, "down") == 0) {
        *pull_cfg = GPIO_PULLDOWN;
        *pull_name = "down";
        return true;
    }
    if (strcmp(text, "float") == 0) {
        *pull_cfg = GPIO_FLOAT;
        *pull_name = "float";
        return true;
    }

    return false;
}

static bool gpiov_parse_drive(const char *text, uint32_t *drive_cfg, uint8_t *drive_value)
{
    uint32_t drive;

    if ((text == NULL) || (drive_cfg == NULL) || (drive_value == NULL)) {
        return false;
    }
    if (!gpiov_parse_u32(text, &drive) || (drive > 3U)) {
        return false;
    }

    *drive_value = (uint8_t)drive;
    switch (drive) {
        case 0:
            *drive_cfg = GPIO_DRV_0;
            return true;
        case 1:
            *drive_cfg = GPIO_DRV_1;
            return true;
        case 2:
            *drive_cfg = GPIO_DRV_2;
            return true;
        case 3:
            *drive_cfg = GPIO_DRV_3;
            return true;
        default:
            return false;
    }
}

static bool gpiov_parse_irq_trig(const char *text, uint8_t *trig_mode, const char **trig_name)
{
    uint32_t numeric;

    if ((text == NULL) || (trig_mode == NULL) || (trig_name == NULL)) {
        return false;
    }

    if (strcmp(text, "sync_fall") == 0) {
        *trig_mode = GPIO_INT_TRIG_MODE_SYNC_FALLING_EDGE;
        *trig_name = "sync_fall";
        return true;
    }
    if (strcmp(text, "sync_rise") == 0) {
        *trig_mode = GPIO_INT_TRIG_MODE_SYNC_RISING_EDGE;
        *trig_name = "sync_rise";
        return true;
    }
    if (strcmp(text, "sync_low") == 0) {
        *trig_mode = GPIO_INT_TRIG_MODE_SYNC_LOW_LEVEL;
        *trig_name = "sync_low";
        return true;
    }
    if (strcmp(text, "sync_high") == 0) {
        *trig_mode = GPIO_INT_TRIG_MODE_SYNC_HIGH_LEVEL;
        *trig_name = "sync_high";
        return true;
    }
#if defined(BL616) || defined(BL616CL) || defined(BL618DG) || defined(BL808)
    if (strcmp(text, "sync_both") == 0) {
        *trig_mode = GPIO_INT_TRIG_MODE_SYNC_FALLING_RISING_EDGE;
        *trig_name = "sync_both";
        return true;
    }
#endif
    if (strcmp(text, "async_fall") == 0) {
        *trig_mode = GPIO_INT_TRIG_MODE_ASYNC_FALLING_EDGE;
        *trig_name = "async_fall";
        return true;
    }
    if (strcmp(text, "async_rise") == 0) {
        *trig_mode = GPIO_INT_TRIG_MODE_ASYNC_RISING_EDGE;
        *trig_name = "async_rise";
        return true;
    }
    if (strcmp(text, "async_low") == 0) {
        *trig_mode = GPIO_INT_TRIG_MODE_ASYNC_LOW_LEVEL;
        *trig_name = "async_low";
        return true;
    }
    if (strcmp(text, "async_high") == 0) {
        *trig_mode = GPIO_INT_TRIG_MODE_ASYNC_HIGH_LEVEL;
        *trig_name = "async_high";
        return true;
    }

    if (gpiov_parse_u32(text, &numeric) && (numeric <= 11U)) {
        *trig_mode = (uint8_t)numeric;
        *trig_name = gpiov_irq_trig_name(*trig_mode);
        return true;
    }

    return false;
}

static const char *gpiov_irq_trig_name(uint8_t trig_mode)
{
    switch (trig_mode) {
        case GPIO_INT_TRIG_MODE_SYNC_FALLING_EDGE:
            return "sync_fall";
        case GPIO_INT_TRIG_MODE_SYNC_RISING_EDGE:
            return "sync_rise";
        case GPIO_INT_TRIG_MODE_SYNC_LOW_LEVEL:
            return "sync_low";
        case GPIO_INT_TRIG_MODE_SYNC_HIGH_LEVEL:
            return "sync_high";
#if defined(BL616) || defined(BL616CL) || defined(BL618DG) || defined(BL808)
        case GPIO_INT_TRIG_MODE_SYNC_FALLING_RISING_EDGE:
            return "sync_both";
#endif
        case GPIO_INT_TRIG_MODE_ASYNC_FALLING_EDGE:
            return "async_fall";
        case GPIO_INT_TRIG_MODE_ASYNC_RISING_EDGE:
            return "async_rise";
        case GPIO_INT_TRIG_MODE_ASYNC_LOW_LEVEL:
            return "async_low";
        case GPIO_INT_TRIG_MODE_ASYNC_HIGH_LEVEL:
            return "async_high";
        default:
            return "unknown";
    }
}

static void gpiov_init_config_options(struct gpiov_config_options *options)
{
    options->pull_cfg = GPIO_FLOAT;
    options->pull_name = "float";
    options->drive_cfg = GPIO_DRV_0;
    options->drive_value = 0;
    options->irq_trig = GPIO_INT_TRIG_MODE_SYNC_RISING_EDGE;
    options->init_level = 0;
    options->pull_specified = false;
    options->drive_specified = false;
    options->trig_specified = false;
    options->init_specified = false;
}

static bool gpiov_parse_config_options(int argc, char **argv, int start_index,
                                       struct gpiov_config_options *options,
                                       const char **error_reason, const char **error_value)
{
    int arg_index;

    gpiov_init_config_options(options);
    *error_reason = NULL;
    *error_value = NULL;

    for (arg_index = start_index; arg_index < argc; arg_index++) {
        if (strncmp(argv[arg_index], "pull=", 5) == 0) {
            if (!gpiov_parse_pull(argv[arg_index] + 5, &options->pull_cfg, &options->pull_name)) {
                *error_reason = "invalid_pull";
                *error_value = argv[arg_index] + 5;
                return false;
            }
            options->pull_specified = true;
        } else if (strncmp(argv[arg_index], "drive=", 6) == 0) {
            if (!gpiov_parse_drive(argv[arg_index] + 6, &options->drive_cfg, &options->drive_value)) {
                *error_reason = "invalid_drive";
                *error_value = argv[arg_index] + 6;
                return false;
            }
            options->drive_specified = true;
        } else if (strncmp(argv[arg_index], "init=", 5) == 0) {
            if (!gpiov_parse_u32(argv[arg_index] + 5, &options->init_level) || (options->init_level > 1U)) {
                *error_reason = "invalid_init";
                *error_value = argv[arg_index] + 5;
                return false;
            }
            options->init_specified = true;
        } else if (strncmp(argv[arg_index], "trig=", 5) == 0) {
            const char *trig_name = NULL;

            if (!gpiov_parse_irq_trig(argv[arg_index] + 5, &options->irq_trig, &trig_name)) {
                *error_reason = "invalid_trig";
                *error_value = argv[arg_index] + 5;
                return false;
            }
            options->trig_specified = true;
        } else {
            *error_reason = "unknown_option";
            *error_value = argv[arg_index];
            return false;
        }
    }

    return true;
}

static uint32_t gpiov_make_cfgset(enum gpiov_mode_type mode, uint32_t pull_cfg, uint32_t drive_cfg)
{
    uint32_t cfgset = GPIO_SMT_EN | pull_cfg | drive_cfg;

    if (mode == GPIOV_MODE_OUTPUT) {
        cfgset |= (GPIO_OUTPUT | GPIO_INPUT);
    } else {
        cfgset |= GPIO_INPUT;
    }

    return cfgset;
}

static uint32_t gpiov_backend_pin_count(void)
{
    return GPIOV_MAX_PIN_COUNT;
}

static const char *gpiov_backend_name(void)
{
    return (g_backend_type == GPIOV_BACKEND_TESTER) ? "tester" : "dut";
}

static bool gpiov_backend_supports_irq(void)
{
    return g_backend_type != GPIOV_BACKEND_TESTER;
}

static bool gpiov_group_contains(const struct gpiov_pin_group *group, uint8_t pin)
{
    if ((group == NULL) || (pin >= GPIOV_MAX_PIN_COUNT)) {
        return false;
    }

    return group->selected[pin];
}

static bool gpiov_group_find_reserved_pin(const struct gpiov_pin_group *group, uint8_t *reserved_pin)
{
    uint32_t index;

    if ((group == NULL) || (reserved_pin == NULL)) {
        return false;
    }

    for (index = 0; index < group->count; index++) {
        if (gpiov_is_reserved_pin(group->pins[index])) {
            *reserved_pin = group->pins[index];
            return true;
        }
    }

    return false;
}

static void gpiov_format_pin_levels(const struct gpiov_pin_group *group, char *buffer, size_t buffer_size)
{
    uint32_t index;
    size_t offset = 0U;

    if ((group == NULL) || (buffer == NULL) || (buffer_size == 0U)) {
        return;
    }

    buffer[0] = '\0';
    for (index = 0; index < group->count; index++) {
        bool level = false;
        int written;

        if (gpiov_backend_read_pin(group->pins[index], &level) == 0) {
            g_pin_state[group->pins[index]].last_level = level;
        } else {
            level = g_pin_state[group->pins[index]].last_level;
        }

        written = snprintf(buffer + offset, buffer_size - offset, "%s%u:%u",
                           (index == 0U) ? "" : ",",
                           group->pins[index],
                           level ? 1U : 0U);
        if ((written < 0) || ((size_t)written >= (buffer_size - offset))) {
            buffer[buffer_size - 1U] = '\0';
            return;
        }
        offset += (size_t)written;
    }
}

static void gpiov_format_pin_modes(const struct gpiov_pin_group *group, char *buffer, size_t buffer_size)
{
    uint32_t index;
    size_t offset = 0U;

    if ((group == NULL) || (buffer == NULL) || (buffer_size == 0U)) {
        return;
    }

    buffer[0] = '\0';
    for (index = 0; index < group->count; index++) {
        int written = snprintf(buffer + offset, buffer_size - offset, "%s%u:%s",
                               (index == 0U) ? "" : ",",
                               group->pins[index],
                               gpiov_mode_name(g_pin_state[group->pins[index]].mode));

        if ((written < 0) || ((size_t)written >= (buffer_size - offset))) {
            buffer[buffer_size - 1U] = '\0';
            return;
        }
        offset += (size_t)written;
    }
}

static void gpiov_format_pin_pulls(const struct gpiov_pin_group *group, char *buffer, size_t buffer_size)
{
    uint32_t index;
    size_t offset = 0U;

    if ((group == NULL) || (buffer == NULL) || (buffer_size == 0U)) {
        return;
    }

    buffer[0] = '\0';
    for (index = 0; index < group->count; index++) {
        const char *pull_name = g_pin_state[group->pins[index]].pull_name;
        int written = snprintf(buffer + offset, buffer_size - offset, "%s%u:%s",
                               (index == 0U) ? "" : ",",
                               group->pins[index],
                               (pull_name != NULL) ? pull_name : "na");

        if ((written < 0) || ((size_t)written >= (buffer_size - offset))) {
            buffer[buffer_size - 1U] = '\0';
            return;
        }
        offset += (size_t)written;
    }
}

static void gpiov_format_irq_counts(const struct gpiov_pin_group *group, char *buffer, size_t buffer_size)
{
    uint32_t index;
    size_t offset = 0U;

    if ((group == NULL) || (buffer == NULL) || (buffer_size == 0U)) {
        return;
    }

    buffer[0] = '\0';
    for (index = 0; index < group->count; index++) {
        int written = snprintf(buffer + offset, buffer_size - offset, "%s%u:%lu",
                               (index == 0U) ? "" : ",",
                               group->pins[index],
                               (unsigned long)g_pin_state[group->pins[index]].irq_count);

        if ((written < 0) || ((size_t)written >= (buffer_size - offset))) {
            buffer[buffer_size - 1U] = '\0';
            return;
        }
        offset += (size_t)written;
    }
}

static void gpiov_format_irq_deltas(const struct gpiov_pin_group *group, const uint32_t *baseline,
                                    char *buffer, size_t buffer_size)
{
    uint32_t index;
    size_t offset = 0U;

    if ((group == NULL) || (baseline == NULL) || (buffer == NULL) || (buffer_size == 0U)) {
        return;
    }

    buffer[0] = '\0';
    for (index = 0; index < group->count; index++) {
        uint8_t pin = group->pins[index];
        uint32_t delta = g_pin_state[pin].irq_count - baseline[pin];
        int written = snprintf(buffer + offset, buffer_size - offset, "%s%u:%lu",
                               (index == 0U) ? "" : ",",
                               pin,
                               (unsigned long)delta);

        if ((written < 0) || ((size_t)written >= (buffer_size - offset))) {
            buffer[buffer_size - 1U] = '\0';
            return;
        }
        offset += (size_t)written;
    }
}

static void gpiov_format_backend_warning(const struct gpiov_backend_warning *warning,
                                         char *buffer, size_t buffer_size)
{
    size_t offset = 0U;
    bool first = true;

    if ((warning == NULL) || (buffer == NULL) || (buffer_size == 0U)) {
        return;
    }

    buffer[0] = '\0';
    if (!warning->ignored_pull && !warning->ignored_drive && !warning->ignored_trig) {
        return;
    }

    offset = (size_t)snprintf(buffer, buffer_size, " warning=");
    if (offset >= buffer_size) {
        buffer[buffer_size - 1U] = '\0';
        return;
    }

    if (warning->ignored_pull) {
        offset += (size_t)snprintf(buffer + offset, buffer_size - offset, "%signored_pull", first ? "" : ",");
        first = false;
    }
    if (warning->ignored_drive && (offset < buffer_size)) {
        offset += (size_t)snprintf(buffer + offset, buffer_size - offset, "%signored_drive", first ? "" : ",");
        first = false;
    }
    if (warning->ignored_trig && (offset < buffer_size)) {
        (void)snprintf(buffer + offset, buffer_size - offset, "%signored_trig", first ? "" : ",");
    }
}

static void gpiov_irq_handler(uint8_t pin)
{
    bool level = false;

    if (pin >= GPIOV_MAX_PIN_COUNT) {
        return;
    }

    g_pin_state[pin].irq_count++;
    g_pin_state[pin].last_irq_ms = bflb_mtimer_get_time_ms();
    if (gpiov_backend_read_pin(pin, &level) == 0) {
        g_pin_state[pin].last_level = level;
    }
    bflb_gpio_int_clear(g_gpio, pin);
}

static int gpiov_native_configure_pin(uint8_t pin, enum gpiov_mode_type mode, uint32_t pull_cfg,
                                      const char *pull_name, uint32_t drive_cfg, uint8_t drive_value,
                                      uint8_t irq_trig, uint32_t init_level)
{
    struct gpiov_pin_state *state = &g_pin_state[pin];
    uint32_t cfgset = gpiov_make_cfgset(mode, pull_cfg, drive_cfg);

    bflb_gpio_irq_detach(pin);
    bflb_gpio_int_mask(g_gpio, pin, true);
    bflb_gpio_int_clear(g_gpio, pin);
    bflb_gpio_deinit(g_gpio, pin);
    bflb_gpio_init(g_gpio, pin, cfgset);

    if (mode == GPIOV_MODE_OUTPUT) {
        if (init_level != 0U) {
            bflb_gpio_set(g_gpio, pin);
            state->last_level = true;
        } else {
            bflb_gpio_reset(g_gpio, pin);
            state->last_level = false;
        }
    } else if (mode == GPIOV_MODE_IRQ) {
        state->irq_count = 0;
        state->last_irq_ms = 0;
        bflb_gpio_int_init(g_gpio, pin, irq_trig);
        bflb_gpio_int_clear(g_gpio, pin);
        bflb_gpio_irq_attach(pin, gpiov_irq_handler);
        bflb_gpio_int_mask(g_gpio, pin, false);
        bflb_irq_enable(g_gpio->irq_num);
        state->last_level = bflb_gpio_read(g_gpio, pin);
    } else {
        state->last_level = bflb_gpio_read(g_gpio, pin);
    }

    state->configured = true;
    state->mode = mode;
    state->pull_name = pull_name;
    state->drive = drive_value;
    state->irq_trig = irq_trig;

    return 0;
}

#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
static int gpiov_tester_select_expander(uint8_t pin, struct pca9698_device **expander,
                                        const char **bus_name, uint8_t *local_pin)
{
    if ((expander == NULL) || (bus_name == NULL) || (local_pin == NULL)) {
        return -1;
    }
    if (pin >= GPIOV_MAX_PIN_COUNT) {
        return -1;
    }

    if (pin < PCA9698_PIN_COUNT) {
        *expander = &g_expander_i2c0;
        *bus_name = "i2c0";
        *local_pin = pin;
    } else {
        *expander = &g_expander_i2c1;
        *bus_name = "i2c1";
        *local_pin = (uint8_t)(pin - PCA9698_PIN_COUNT);
    }

    return 0;
}

static int gpiov_tester_init_i2c_bus(struct bflb_device_s **i2c_dev, const char *name,
                                     uint8_t sda_pin, uint8_t scl_pin, uint32_t gpio_func)
{
    *i2c_dev = bflb_device_get_by_name(name);
    if (*i2c_dev == NULL) {
        return -1;
    }

    bflb_gpio_init(g_gpio, sda_pin, gpio_func | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(g_gpio, scl_pin, gpio_func | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_i2c_init(*i2c_dev, CONFIG_GPIO_TESTER_I2C_FREQ);
    return 0;
}
#endif

static int gpiov_backend_init(void)
{
#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
    g_backend_type = GPIOV_BACKEND_TESTER;
    if (gpiov_tester_init_i2c_bus(&g_i2c0, "i2c0",
                                  CONFIG_GPIO_TESTER_I2C0_SDA_PIN,
                                  CONFIG_GPIO_TESTER_I2C0_SCL_PIN,
                                  GPIO_FUNC_I2C0) != 0) {
        return -1;
    }
    if (gpiov_tester_init_i2c_bus(&g_i2c1, "i2c1",
                                  CONFIG_GPIO_TESTER_I2C1_SDA_PIN,
                                  CONFIG_GPIO_TESTER_I2C1_SCL_PIN,
                                  GPIO_FUNC_I2C1) != 0) {
        return -1;
    }
    if (pca9698_init(&g_expander_i2c0, g_i2c0, (uint8_t)CONFIG_GPIO_TESTER_I2C0_ADDR) != 0) {
        return -1;
    }
    if (pca9698_init(&g_expander_i2c1, g_i2c1, (uint8_t)CONFIG_GPIO_TESTER_I2C1_ADDR) != 0) {
        return -1;
    }
    return 0;
#else
    g_backend_type = GPIOV_BACKEND_DUT;
    return 0;
#endif
}

static int gpiov_backend_configure_pin(uint8_t pin, enum gpiov_mode_type mode,
                                       const struct gpiov_config_options *options,
                                       struct gpiov_backend_warning *warning)
{
    if ((options == NULL) || (warning == NULL) || !g_backend_ready) {
        return -1;
    }

    memset(warning, 0, sizeof(*warning));

#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
    if (g_backend_type == GPIOV_BACKEND_TESTER) {
        struct gpiov_pin_state *state = &g_pin_state[pin];
        struct pca9698_device *expander;
        const char *bus_name;
        uint8_t local_pin;

        if (mode == GPIOV_MODE_IRQ) {
            return -2;
        }
        if (gpiov_tester_select_expander(pin, &expander, &bus_name, &local_pin) != 0) {
            return -1;
        }

        warning->ignored_pull = options->pull_specified;
        warning->ignored_drive = options->drive_specified;
        warning->ignored_trig = options->trig_specified;

        if (mode == GPIOV_MODE_OUTPUT) {
            if (pca9698_write_pin(expander, local_pin, options->init_level != 0U) != 0) {
                return -1;
            }
            if (pca9698_set_direction(expander, local_pin, false) != 0) {
                return -1;
            }
            state->last_level = (options->init_level != 0U);
        } else {
            if (pca9698_set_direction(expander, local_pin, true) != 0) {
                return -1;
            }
            if (pca9698_read_pin(expander, local_pin, &state->last_level) != 0) {
                return -1;
            }
        }

        state->configured = true;
        state->mode = mode;
        state->pull_name = options->pull_name;
        state->drive = options->drive_value;
        state->irq_trig = options->irq_trig;
        state->irq_count = 0;
        state->last_irq_ms = 0;
        (void)bus_name;
        return 0;
    }
#endif

    return gpiov_native_configure_pin(pin, mode, options->pull_cfg, options->pull_name,
                                      options->drive_cfg, options->drive_value,
                                      options->irq_trig, options->init_level);
}

static int gpiov_backend_write_pin(uint8_t pin, bool level)
{
    if (!g_backend_ready) {
        return -1;
    }

#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
    if (g_backend_type == GPIOV_BACKEND_TESTER) {
        struct pca9698_device *expander;
        const char *bus_name;
        uint8_t local_pin;

        if (gpiov_tester_select_expander(pin, &expander, &bus_name, &local_pin) != 0) {
            return -1;
        }
        (void)bus_name;
        return pca9698_write_pin(expander, local_pin, level);
    }
#endif

    if (level) {
        bflb_gpio_set(g_gpio, pin);
    } else {
        bflb_gpio_reset(g_gpio, pin);
    }
    return 0;
}

static int gpiov_backend_read_pin(uint8_t pin, bool *level)
{
    if ((level == NULL) || !g_backend_ready) {
        return -1;
    }

#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
    if (g_backend_type == GPIOV_BACKEND_TESTER) {
        struct pca9698_device *expander;
        const char *bus_name;
        uint8_t local_pin;

        if (gpiov_tester_select_expander(pin, &expander, &bus_name, &local_pin) != 0) {
            return -1;
        }
        (void)bus_name;
        return pca9698_read_pin(expander, local_pin, level);
    }
#endif

    *level = bflb_gpio_read(g_gpio, pin);
    return 0;
}

static int gpiov_backend_deinit_pin(uint8_t pin)
{
    if (!g_backend_ready) {
        return -1;
    }

#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
    if (g_backend_type == GPIOV_BACKEND_TESTER) {
        struct pca9698_device *expander;
        const char *bus_name;
        uint8_t local_pin;

        if (gpiov_tester_select_expander(pin, &expander, &bus_name, &local_pin) != 0) {
            return -1;
        }
        (void)bus_name;
        return pca9698_deinit_pin(expander, local_pin);
    }
#endif

    bflb_gpio_irq_detach(pin);
    bflb_gpio_int_mask(g_gpio, pin, true);
    bflb_gpio_int_clear(g_gpio, pin);
    bflb_gpio_deinit(g_gpio, pin);
    return 0;
}

static int gpiov_backend_get_status(uint8_t pin, struct gpiov_backend_status *status)
{
    if ((status == NULL) || !g_backend_ready) {
        return -1;
    }

    memset(status, 0, sizeof(*status));
    status->backend_name = gpiov_backend_name();

#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
    if (g_backend_type == GPIOV_BACKEND_TESTER) {
        struct pca9698_device *expander;
        const char *bus_name;
        uint8_t local_pin;

        if (gpiov_tester_select_expander(pin, &expander, &bus_name, &local_pin) != 0) {
            return -1;
        }
        if (pca9698_get_bank_bit(local_pin, &status->bank, &status->bit) != 0) {
            return -1;
        }
        status->bus_name = bus_name;
        status->expander_addr = expander->address;
        return 0;
    }
#endif

    return 0;
}

static int gpiov_ping_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    gpiov_reply("OK", "version=%s backend=%s backend_ready=%u chip_gpio_max=%u reserved_pins=%s",
                GPIOV_VERSION,
                gpiov_backend_name(),
                g_backend_ready ? 1U : 0U,
                (unsigned int)gpiov_backend_pin_count(),
                gpiov_reserved_pins_name());
    return 0;
}

static int gpiov_help_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("gpiov_ping\r\n");
    printf("gpiov_help\r\n");
    printf("gpiov_config <pin> <in|out|irq> [pull=up|down|float] [drive=0|1|2|3] [init=0|1] [trig=...]\r\n");
    printf("gpiov_config_many <pins_csv> <in|out|irq> [pull=up|down|float] [drive=0|1|2|3] [init=0|1] [trig=...]\r\n");
    printf("gpiov_write <pin> <0|1>\r\n");
    printf("gpiov_write_pattern <pins_csv> <target_pin> <target_level> <other_level>\r\n");
    printf("gpiov_read <pin>\r\n");
    printf("gpiov_read_many <pins_csv>\r\n");
    printf("gpiov_read_expect <pins_csv> <target_pin> <target_level> <other_level>\r\n");
    printf("gpiov_pulse <pin> <count> <high_ms> <low_ms>\r\n");
    printf("gpiov_irq_get <pin>\r\n");
    printf("gpiov_irq_get_many <pins_csv>\r\n");
    printf("gpiov_irq_clear <pin>\r\n");
    printf("gpiov_irq_clear_many <pins_csv>\r\n");
    printf("gpiov_irq_wait <pin> <expected_count> <timeout_ms>\r\n");
    printf("gpiov_irq_expect <pins_csv> <target_pin> <expected_delta> <timeout_ms>\r\n");
    printf("gpiov_status <pin>\r\n");
    printf("gpiov_status_many <pins_csv>\r\n");
    printf("gpiov_deinit <pin>\r\n");
    gpiov_reply("OK", "help_printed=1 backend=%s", gpiov_backend_name());
    return 0;
}

static int gpiov_config_cmd(int argc, char **argv)
{
    uint8_t pin;
    enum gpiov_mode_type mode;
    struct gpiov_config_options options;
    struct gpiov_backend_warning warning;
    const char *error_reason;
    const char *error_value;
    char warning_text[96];
    int rc;

    if (argc < 3) {
        gpiov_reply("FAIL", "reason=usage gpiov_config <pin> <in|out|irq> [pull=...] [drive=...] [init=...] [trig=...]");
        return 1;
    }
    if (!g_backend_ready) {
        gpiov_reply("FAIL", "reason=backend_not_ready backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin(argv[1], &pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[1]);
        return 1;
    }
    if (gpiov_is_reserved_pin(pin)) {
        gpiov_reply("FAIL", "reason=pin_reserved pin=%u", pin);
        return 1;
    }
    if (!gpiov_parse_mode(argv[2], &mode)) {
        gpiov_reply("FAIL", "reason=invalid_mode value=%s", argv[2]);
        return 1;
    }
    if (!gpiov_parse_config_options(argc, argv, 3, &options, &error_reason, &error_value)) {
        gpiov_reply("FAIL", "reason=%s value=%s", error_reason, error_value);
        return 1;
    }

    rc = gpiov_backend_configure_pin(pin, mode, &options, &warning);
    if (rc == -2) {
        gpiov_reply("FAIL", "reason=irq_not_supported backend=%s", gpiov_backend_name());
        return 1;
    }
    if (rc != 0) {
        gpiov_reply("FAIL", "reason=backend_config_failed pin=%u backend=%s", pin, gpiov_backend_name());
        return 1;
    }

    gpiov_format_backend_warning(&warning, warning_text, sizeof(warning_text));
    gpiov_reply("OK", "pin=%u mode=%s pull=%s drive=%u init=%u trig=%s%s",
                pin, gpiov_mode_name(mode), options.pull_name, options.drive_value,
                options.init_level, gpiov_irq_trig_name(options.irq_trig), warning_text);
    return 0;
}

static int gpiov_config_many_cmd(int argc, char **argv)
{
    struct gpiov_pin_group group;
    enum gpiov_mode_type mode;
    struct gpiov_config_options options;
    struct gpiov_backend_warning warning;
    const char *error_reason;
    const char *error_value;
    uint8_t reserved_pin;
    uint32_t index;
    char warning_text[96];
    int rc;

    if (argc < 3) {
        gpiov_reply("FAIL", "reason=usage gpiov_config_many <pins_csv> <in|out|irq> [pull=...] [drive=...] [init=...] [trig=...]");
        return 1;
    }
    if (!g_backend_ready) {
        gpiov_reply("FAIL", "reason=backend_not_ready backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin_group(argv[1], &group)) {
        gpiov_reply("FAIL", "reason=invalid_pin_group value=%s", argv[1]);
        return 1;
    }
    if (gpiov_group_find_reserved_pin(&group, &reserved_pin)) {
        gpiov_reply("FAIL", "reason=pin_reserved pin=%u", reserved_pin);
        return 1;
    }
    if (!gpiov_parse_mode(argv[2], &mode)) {
        gpiov_reply("FAIL", "reason=invalid_mode value=%s", argv[2]);
        return 1;
    }
    if (!gpiov_parse_config_options(argc, argv, 3, &options, &error_reason, &error_value)) {
        gpiov_reply("FAIL", "reason=%s value=%s", error_reason, error_value);
        return 1;
    }

    memset(&warning, 0, sizeof(warning));
    for (index = 0; index < group.count; index++) {
        struct gpiov_backend_warning pin_warning;

        rc = gpiov_backend_configure_pin(group.pins[index], mode, &options, &pin_warning);
        if (rc == -2) {
            gpiov_reply("FAIL", "reason=irq_not_supported backend=%s", gpiov_backend_name());
            return 1;
        }
        if (rc != 0) {
            gpiov_reply("FAIL", "reason=backend_config_failed pin=%u backend=%s",
                        group.pins[index], gpiov_backend_name());
            return 1;
        }

        warning.ignored_pull |= pin_warning.ignored_pull;
        warning.ignored_drive |= pin_warning.ignored_drive;
        warning.ignored_trig |= pin_warning.ignored_trig;
    }

    gpiov_format_backend_warning(&warning, warning_text, sizeof(warning_text));
    gpiov_reply("OK", "pins=%s count=%lu mode=%s pull=%s drive=%u init=%u trig=%s%s",
                argv[1], (unsigned long)group.count, gpiov_mode_name(mode),
                options.pull_name, options.drive_value, options.init_level,
                gpiov_irq_trig_name(options.irq_trig), warning_text);
    return 0;
}

static int gpiov_write_cmd(int argc, char **argv)
{
    uint8_t pin;
    uint32_t level;

    if (argc != 3) {
        gpiov_reply("FAIL", "reason=usage gpiov_write <pin> <0|1>");
        return 1;
    }
    if (!g_backend_ready) {
        gpiov_reply("FAIL", "reason=backend_not_ready backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin(argv[1], &pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[1]);
        return 1;
    }
    if (gpiov_is_reserved_pin(pin)) {
        gpiov_reply("FAIL", "reason=pin_reserved pin=%u", pin);
        return 1;
    }
    if ((g_pin_state[pin].configured == false) || (g_pin_state[pin].mode != GPIOV_MODE_OUTPUT)) {
        gpiov_reply("FAIL", "reason=pin_not_output pin=%u", pin);
        return 1;
    }
    if (!gpiov_parse_u32(argv[2], &level) || (level > 1U)) {
        gpiov_reply("FAIL", "reason=invalid_level value=%s", argv[2]);
        return 1;
    }
    if (gpiov_backend_write_pin(pin, level != 0U) != 0) {
        gpiov_reply("FAIL", "reason=backend_write_failed pin=%u backend=%s", pin, gpiov_backend_name());
        return 1;
    }

    g_pin_state[pin].last_level = (level != 0U);
    gpiov_reply("OK", "pin=%u level=%u", pin, level);
    return 0;
}

static int gpiov_write_pattern_cmd(int argc, char **argv)
{
    struct gpiov_pin_group group;
    uint8_t target_pin;
    uint8_t reserved_pin;
    uint32_t target_level;
    uint32_t other_level;
    uint32_t index;
    char levels[GPIOV_LEVELS_BUFFER_SIZE];

    if (argc != 5) {
        gpiov_reply("FAIL", "reason=usage gpiov_write_pattern <pins_csv> <target_pin> <target_level> <other_level>");
        return 1;
    }
    if (!g_backend_ready) {
        gpiov_reply("FAIL", "reason=backend_not_ready backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin_group(argv[1], &group)) {
        gpiov_reply("FAIL", "reason=invalid_pin_group value=%s", argv[1]);
        return 1;
    }
    if (gpiov_group_find_reserved_pin(&group, &reserved_pin)) {
        gpiov_reply("FAIL", "reason=pin_reserved pin=%u", reserved_pin);
        return 1;
    }
    if (!gpiov_parse_pin(argv[2], &target_pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[2]);
        return 1;
    }
    if (!gpiov_group_contains(&group, target_pin)) {
        gpiov_reply("FAIL", "reason=target_not_in_group pin=%u", target_pin);
        return 1;
    }
    if (!gpiov_parse_u32(argv[3], &target_level) || (target_level > 1U)) {
        gpiov_reply("FAIL", "reason=invalid_target_level value=%s", argv[3]);
        return 1;
    }
    if (!gpiov_parse_u32(argv[4], &other_level) || (other_level > 1U)) {
        gpiov_reply("FAIL", "reason=invalid_other_level value=%s", argv[4]);
        return 1;
    }

    for (index = 0; index < group.count; index++) {
        uint8_t pin = group.pins[index];
        uint32_t level = (pin == target_pin) ? target_level : other_level;

        if ((g_pin_state[pin].configured == false) || (g_pin_state[pin].mode != GPIOV_MODE_OUTPUT)) {
            gpiov_reply("FAIL", "reason=pin_not_output pin=%u", pin);
            return 1;
        }
        if (gpiov_backend_write_pin(pin, level != 0U) != 0) {
            gpiov_reply("FAIL", "reason=backend_write_failed pin=%u backend=%s", pin, gpiov_backend_name());
            return 1;
        }
        g_pin_state[pin].last_level = (level != 0U);
    }

    gpiov_format_pin_levels(&group, levels, sizeof(levels));
    gpiov_reply("OK", "pins=%s target_pin=%u target_level=%u other_level=%u levels=%s",
                argv[1], target_pin, target_level, other_level, levels);
    return 0;
}

static int gpiov_read_cmd(int argc, char **argv)
{
    uint8_t pin;
    bool level;

    if (argc != 2) {
        gpiov_reply("FAIL", "reason=usage gpiov_read <pin>");
        return 1;
    }
    if (!g_backend_ready) {
        gpiov_reply("FAIL", "reason=backend_not_ready backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin(argv[1], &pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[1]);
        return 1;
    }
    if (gpiov_backend_read_pin(pin, &level) != 0) {
        gpiov_reply("FAIL", "reason=backend_read_failed pin=%u backend=%s", pin, gpiov_backend_name());
        return 1;
    }

    g_pin_state[pin].last_level = level;
    gpiov_reply("OK", "pin=%u level=%u", pin, level ? 1U : 0U);
    return 0;
}

static int gpiov_read_many_cmd(int argc, char **argv)
{
    struct gpiov_pin_group group;
    char levels[GPIOV_LEVELS_BUFFER_SIZE];

    if (argc != 2) {
        gpiov_reply("FAIL", "reason=usage gpiov_read_many <pins_csv>");
        return 1;
    }
    if (!g_backend_ready) {
        gpiov_reply("FAIL", "reason=backend_not_ready backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin_group(argv[1], &group)) {
        gpiov_reply("FAIL", "reason=invalid_pin_group value=%s", argv[1]);
        return 1;
    }

    gpiov_format_pin_levels(&group, levels, sizeof(levels));
    gpiov_reply("OK", "pins=%s count=%lu levels=%s",
                argv[1], (unsigned long)group.count, levels);
    return 0;
}

static int gpiov_read_expect_cmd(int argc, char **argv)
{
    struct gpiov_pin_group group;
    uint8_t target_pin;
    uint8_t reserved_pin;
    uint32_t target_level;
    uint32_t other_level;
    uint32_t index;
    char levels[GPIOV_LEVELS_BUFFER_SIZE];

    if (argc != 5) {
        gpiov_reply("FAIL", "reason=usage gpiov_read_expect <pins_csv> <target_pin> <target_level> <other_level>");
        return 1;
    }
    if (!g_backend_ready) {
        gpiov_reply("FAIL", "reason=backend_not_ready backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin_group(argv[1], &group)) {
        gpiov_reply("FAIL", "reason=invalid_pin_group value=%s", argv[1]);
        return 1;
    }
    if (gpiov_group_find_reserved_pin(&group, &reserved_pin)) {
        gpiov_reply("FAIL", "reason=pin_reserved pin=%u", reserved_pin);
        return 1;
    }
    if (!gpiov_parse_pin(argv[2], &target_pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[2]);
        return 1;
    }
    if (!gpiov_group_contains(&group, target_pin)) {
        gpiov_reply("FAIL", "reason=target_not_in_group pin=%u", target_pin);
        return 1;
    }
    if (!gpiov_parse_u32(argv[3], &target_level) || (target_level > 1U)) {
        gpiov_reply("FAIL", "reason=invalid_target_level value=%s", argv[3]);
        return 1;
    }
    if (!gpiov_parse_u32(argv[4], &other_level) || (other_level > 1U)) {
        gpiov_reply("FAIL", "reason=invalid_other_level value=%s", argv[4]);
        return 1;
    }

    for (index = 0; index < group.count; index++) {
        uint8_t pin = group.pins[index];
        bool level;
        uint32_t expected_level = (pin == target_pin) ? target_level : other_level;

        if ((g_pin_state[pin].configured == false) || (g_pin_state[pin].mode != GPIOV_MODE_INPUT)) {
            gpiov_reply("FAIL", "reason=pin_not_input pin=%u", pin);
            return 1;
        }
        if (gpiov_backend_read_pin(pin, &level) != 0) {
            gpiov_reply("FAIL", "reason=backend_read_failed pin=%u backend=%s", pin, gpiov_backend_name());
            return 1;
        }

        g_pin_state[pin].last_level = level;
        if ((uint32_t)(level ? 1U : 0U) != expected_level) {
            gpiov_format_pin_levels(&group, levels, sizeof(levels));
            gpiov_reply("FAIL", "reason=input_mismatch pin=%u expected=%u actual=%u levels=%s",
                        pin, expected_level, level ? 1U : 0U, levels);
            return 1;
        }
    }

    gpiov_format_pin_levels(&group, levels, sizeof(levels));
    gpiov_reply("OK", "pins=%s target_pin=%u target_level=%u other_level=%u levels=%s",
                argv[1], target_pin, target_level, other_level, levels);
    return 0;
}

static int gpiov_pulse_cmd(int argc, char **argv)
{
    uint8_t pin;
    uint32_t count;
    uint32_t high_ms;
    uint32_t low_ms;
    uint32_t index;

    if (argc != 5) {
        gpiov_reply("FAIL", "reason=usage gpiov_pulse <pin> <count> <high_ms> <low_ms>");
        return 1;
    }
    if (!g_backend_ready) {
        gpiov_reply("FAIL", "reason=backend_not_ready backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin(argv[1], &pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[1]);
        return 1;
    }
    if (gpiov_is_reserved_pin(pin)) {
        gpiov_reply("FAIL", "reason=pin_reserved pin=%u", pin);
        return 1;
    }
    if ((g_pin_state[pin].configured == false) || (g_pin_state[pin].mode != GPIOV_MODE_OUTPUT)) {
        gpiov_reply("FAIL", "reason=pin_not_output pin=%u", pin);
        return 1;
    }
    if (!gpiov_parse_u32(argv[2], &count) || (count == 0U)) {
        gpiov_reply("FAIL", "reason=invalid_count value=%s", argv[2]);
        return 1;
    }
    if (!gpiov_parse_u32(argv[3], &high_ms)) {
        gpiov_reply("FAIL", "reason=invalid_high_ms value=%s", argv[3]);
        return 1;
    }
    if (!gpiov_parse_u32(argv[4], &low_ms)) {
        gpiov_reply("FAIL", "reason=invalid_low_ms value=%s", argv[4]);
        return 1;
    }

    if (gpiov_backend_write_pin(pin, false) != 0) {
        gpiov_reply("FAIL", "reason=backend_write_failed pin=%u backend=%s", pin, gpiov_backend_name());
        return 1;
    }
    g_pin_state[pin].last_level = false;

    for (index = 0; index < count; index++) {
        if (gpiov_backend_write_pin(pin, true) != 0) {
            gpiov_reply("FAIL", "reason=backend_write_failed pin=%u backend=%s", pin, gpiov_backend_name());
            return 1;
        }
        g_pin_state[pin].last_level = true;
        if (high_ms != 0U) {
            bflb_mtimer_delay_ms(high_ms);
        }
        if (gpiov_backend_write_pin(pin, false) != 0) {
            gpiov_reply("FAIL", "reason=backend_write_failed pin=%u backend=%s", pin, gpiov_backend_name());
            return 1;
        }
        g_pin_state[pin].last_level = false;
        if (low_ms != 0U) {
            bflb_mtimer_delay_ms(low_ms);
        }
    }

    gpiov_reply("OK", "pin=%u pulses=%lu high_ms=%lu low_ms=%lu final_level=0",
                pin, (unsigned long)count, (unsigned long)high_ms, (unsigned long)low_ms);
    return 0;
}

static int gpiov_irq_get_cmd(int argc, char **argv)
{
    uint8_t pin;

    if (argc != 2) {
        gpiov_reply("FAIL", "reason=usage gpiov_irq_get <pin>");
        return 1;
    }
    if (!gpiov_backend_supports_irq()) {
        gpiov_reply("FAIL", "reason=irq_not_supported backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin(argv[1], &pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[1]);
        return 1;
    }
    if ((g_pin_state[pin].configured == false) || (g_pin_state[pin].mode != GPIOV_MODE_IRQ)) {
        gpiov_reply("FAIL", "reason=pin_not_irq pin=%u", pin);
        return 1;
    }

    gpiov_reply("OK", "pin=%u irq_count=%lu last_irq_ms=%llu level=%u", pin,
                (unsigned long)g_pin_state[pin].irq_count,
                (unsigned long long)g_pin_state[pin].last_irq_ms,
                g_pin_state[pin].last_level ? 1U : 0U);
    return 0;
}

static int gpiov_irq_get_many_cmd(int argc, char **argv)
{
    struct gpiov_pin_group group;
    uint8_t reserved_pin;
    uint32_t index;
    char counts[GPIOV_LEVELS_BUFFER_SIZE];
    char levels[GPIOV_LEVELS_BUFFER_SIZE];

    if (argc != 2) {
        gpiov_reply("FAIL", "reason=usage gpiov_irq_get_many <pins_csv>");
        return 1;
    }
    if (!gpiov_backend_supports_irq()) {
        gpiov_reply("FAIL", "reason=irq_not_supported backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin_group(argv[1], &group)) {
        gpiov_reply("FAIL", "reason=invalid_pin_group value=%s", argv[1]);
        return 1;
    }
    if (gpiov_group_find_reserved_pin(&group, &reserved_pin)) {
        gpiov_reply("FAIL", "reason=pin_reserved pin=%u", reserved_pin);
        return 1;
    }

    for (index = 0; index < group.count; index++) {
        uint8_t pin = group.pins[index];

        if ((g_pin_state[pin].configured == false) || (g_pin_state[pin].mode != GPIOV_MODE_IRQ)) {
            gpiov_reply("FAIL", "reason=pin_not_irq pin=%u", pin);
            return 1;
        }
    }

    gpiov_format_irq_counts(&group, counts, sizeof(counts));
    gpiov_format_pin_levels(&group, levels, sizeof(levels));
    gpiov_reply("OK", "pins=%s count=%lu irq_counts=%s levels=%s",
                argv[1], (unsigned long)group.count, counts, levels);
    return 0;
}

static int gpiov_irq_clear_cmd(int argc, char **argv)
{
    uint8_t pin;

    if (argc != 2) {
        gpiov_reply("FAIL", "reason=usage gpiov_irq_clear <pin>");
        return 1;
    }
    if (!gpiov_backend_supports_irq()) {
        gpiov_reply("FAIL", "reason=irq_not_supported backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin(argv[1], &pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[1]);
        return 1;
    }
    if ((g_pin_state[pin].configured == false) || (g_pin_state[pin].mode != GPIOV_MODE_IRQ)) {
        gpiov_reply("FAIL", "reason=pin_not_irq pin=%u", pin);
        return 1;
    }

    g_pin_state[pin].irq_count = 0;
    g_pin_state[pin].last_irq_ms = 0;
    bflb_gpio_int_clear(g_gpio, pin);
    gpiov_reply("OK", "pin=%u irq_count=0", pin);
    return 0;
}

static int gpiov_irq_clear_many_cmd(int argc, char **argv)
{
    struct gpiov_pin_group group;
    uint8_t reserved_pin;
    uint32_t index;

    if (argc != 2) {
        gpiov_reply("FAIL", "reason=usage gpiov_irq_clear_many <pins_csv>");
        return 1;
    }
    if (!gpiov_backend_supports_irq()) {
        gpiov_reply("FAIL", "reason=irq_not_supported backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin_group(argv[1], &group)) {
        gpiov_reply("FAIL", "reason=invalid_pin_group value=%s", argv[1]);
        return 1;
    }
    if (gpiov_group_find_reserved_pin(&group, &reserved_pin)) {
        gpiov_reply("FAIL", "reason=pin_reserved pin=%u", reserved_pin);
        return 1;
    }

    for (index = 0; index < group.count; index++) {
        uint8_t pin = group.pins[index];

        if ((g_pin_state[pin].configured == false) || (g_pin_state[pin].mode != GPIOV_MODE_IRQ)) {
            gpiov_reply("FAIL", "reason=pin_not_irq pin=%u", pin);
            return 1;
        }

        g_pin_state[pin].irq_count = 0;
        g_pin_state[pin].last_irq_ms = 0;
        bflb_gpio_int_clear(g_gpio, pin);
    }

    gpiov_reply("OK", "pins=%s count=%lu irq_count=0", argv[1], (unsigned long)group.count);
    return 0;
}

static int gpiov_irq_wait_cmd(int argc, char **argv)
{
    uint8_t pin;
    uint32_t expected_count;
    uint32_t timeout_ms;
    uint32_t start_count;
    uint64_t start_ms;

    if (argc != 4) {
        gpiov_reply("FAIL", "reason=usage gpiov_irq_wait <pin> <expected_count> <timeout_ms>");
        return 1;
    }
    if (!gpiov_backend_supports_irq()) {
        gpiov_reply("FAIL", "reason=irq_not_supported backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin(argv[1], &pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[1]);
        return 1;
    }
    if ((g_pin_state[pin].configured == false) || (g_pin_state[pin].mode != GPIOV_MODE_IRQ)) {
        gpiov_reply("FAIL", "reason=pin_not_irq pin=%u", pin);
        return 1;
    }
    if (!gpiov_parse_u32(argv[2], &expected_count) || (expected_count == 0U)) {
        gpiov_reply("FAIL", "reason=invalid_expected_count value=%s", argv[2]);
        return 1;
    }
    if (!gpiov_parse_u32(argv[3], &timeout_ms)) {
        gpiov_reply("FAIL", "reason=invalid_timeout_ms value=%s", argv[3]);
        return 1;
    }

    start_count = g_pin_state[pin].irq_count;
    start_ms = bflb_mtimer_get_time_ms();
    while ((bflb_mtimer_get_time_ms() - start_ms) <= timeout_ms) {
        if ((g_pin_state[pin].irq_count - start_count) >= expected_count) {
            gpiov_reply("OK", "pin=%u delta_irq_count=%lu total_irq_count=%lu waited_ms=%llu", pin,
                        (unsigned long)(g_pin_state[pin].irq_count - start_count),
                        (unsigned long)g_pin_state[pin].irq_count,
                        (unsigned long long)(bflb_mtimer_get_time_ms() - start_ms));
            return 0;
        }
        bflb_mtimer_delay_ms(1);
    }

    gpiov_reply("FAIL", "reason=irq_timeout pin=%u delta_irq_count=%lu expected_count=%lu timeout_ms=%lu", pin,
                (unsigned long)(g_pin_state[pin].irq_count - start_count),
                (unsigned long)expected_count,
                (unsigned long)timeout_ms);
    return 1;
}

static int gpiov_irq_expect_cmd(int argc, char **argv)
{
    struct gpiov_pin_group group;
    uint8_t target_pin;
    uint8_t reserved_pin;
    uint32_t expected_delta;
    uint32_t timeout_ms;
    uint32_t baseline[GPIOV_MAX_PIN_COUNT];
    uint32_t index;
    uint64_t start_ms;
    char deltas[GPIOV_LEVELS_BUFFER_SIZE];

    if (argc != 5) {
        gpiov_reply("FAIL", "reason=usage gpiov_irq_expect <pins_csv> <target_pin> <expected_delta> <timeout_ms>");
        return 1;
    }
    if (!gpiov_backend_supports_irq()) {
        gpiov_reply("FAIL", "reason=irq_not_supported backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin_group(argv[1], &group)) {
        gpiov_reply("FAIL", "reason=invalid_pin_group value=%s", argv[1]);
        return 1;
    }
    if (gpiov_group_find_reserved_pin(&group, &reserved_pin)) {
        gpiov_reply("FAIL", "reason=pin_reserved pin=%u", reserved_pin);
        return 1;
    }
    if (!gpiov_parse_pin(argv[2], &target_pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[2]);
        return 1;
    }
    if (!gpiov_group_contains(&group, target_pin)) {
        gpiov_reply("FAIL", "reason=target_not_in_group pin=%u", target_pin);
        return 1;
    }
    if (!gpiov_parse_u32(argv[3], &expected_delta) || (expected_delta == 0U)) {
        gpiov_reply("FAIL", "reason=invalid_expected_delta value=%s", argv[3]);
        return 1;
    }
    if (!gpiov_parse_u32(argv[4], &timeout_ms)) {
        gpiov_reply("FAIL", "reason=invalid_timeout_ms value=%s", argv[4]);
        return 1;
    }

    memset(baseline, 0, sizeof(baseline));
    for (index = 0; index < group.count; index++) {
        uint8_t pin = group.pins[index];

        if ((g_pin_state[pin].configured == false) || (g_pin_state[pin].mode != GPIOV_MODE_IRQ)) {
            gpiov_reply("FAIL", "reason=pin_not_irq pin=%u", pin);
            return 1;
        }
        baseline[pin] = g_pin_state[pin].irq_count;
    }

    start_ms = bflb_mtimer_get_time_ms();
    while ((bflb_mtimer_get_time_ms() - start_ms) <= timeout_ms) {
        uint32_t target_delta = g_pin_state[target_pin].irq_count - baseline[target_pin];

        for (index = 0; index < group.count; index++) {
            uint8_t pin = group.pins[index];
            uint32_t delta = g_pin_state[pin].irq_count - baseline[pin];

            if ((pin != target_pin) && (delta != 0U)) {
                gpiov_format_irq_deltas(&group, baseline, deltas, sizeof(deltas));
                gpiov_reply("FAIL", "reason=unexpected_irq pin=%u target_pin=%u deltas=%s",
                            pin, target_pin, deltas);
                return 1;
            }
        }

        if (target_delta > expected_delta) {
            gpiov_format_irq_deltas(&group, baseline, deltas, sizeof(deltas));
            gpiov_reply("FAIL", "reason=unexpected_target_irq_count target_pin=%u expected_delta=%lu actual_delta=%lu deltas=%s",
                        target_pin, (unsigned long)expected_delta, (unsigned long)target_delta, deltas);
            return 1;
        }
        if (target_delta == expected_delta) {
            gpiov_format_irq_deltas(&group, baseline, deltas, sizeof(deltas));
            gpiov_reply("OK", "pins=%s target_pin=%u expected_delta=%lu waited_ms=%llu deltas=%s",
                        argv[1], target_pin, (unsigned long)expected_delta,
                        (unsigned long long)(bflb_mtimer_get_time_ms() - start_ms), deltas);
            return 0;
        }

        bflb_mtimer_delay_ms(1);
    }

    gpiov_format_irq_deltas(&group, baseline, deltas, sizeof(deltas));
    gpiov_reply("FAIL", "reason=irq_timeout target_pin=%u expected_delta=%lu timeout_ms=%lu deltas=%s",
                target_pin, (unsigned long)expected_delta, (unsigned long)timeout_ms, deltas);
    return 1;
}

static int gpiov_status_cmd(int argc, char **argv)
{
    uint8_t pin;
    bool level;
    struct gpiov_pin_state *state;
    struct gpiov_backend_status status;

    if (argc != 2) {
        gpiov_reply("FAIL", "reason=usage gpiov_status <pin>");
        return 1;
    }
    if (!g_backend_ready) {
        gpiov_reply("FAIL", "reason=backend_not_ready backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin(argv[1], &pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[1]);
        return 1;
    }
    if (gpiov_backend_read_pin(pin, &level) != 0) {
        gpiov_reply("FAIL", "reason=backend_read_failed pin=%u backend=%s", pin, gpiov_backend_name());
        return 1;
    }
    if (gpiov_backend_get_status(pin, &status) != 0) {
        gpiov_reply("FAIL", "reason=backend_status_failed pin=%u backend=%s", pin, gpiov_backend_name());
        return 1;
    }

    state = &g_pin_state[pin];
    state->last_level = level;
    if (g_backend_type == GPIOV_BACKEND_TESTER) {
        gpiov_reply("OK", "pin=%u configured=%u mode=%s reserved=%u pull=%s drive=%u level=%u irq_count=%lu trig=%s backend=%s bus=%s expander_addr=0x%02x bank=%u bit=%u",
                    pin,
                    state->configured ? 1U : 0U,
                    gpiov_mode_name(state->mode),
                    gpiov_is_reserved_pin(pin) ? 1U : 0U,
                    state->pull_name ? state->pull_name : "na",
                    state->drive,
                    level ? 1U : 0U,
                    (unsigned long)state->irq_count,
                    gpiov_irq_trig_name(state->irq_trig),
                    status.backend_name,
                    status.bus_name ? status.bus_name : "na",
                    status.expander_addr,
                    status.bank,
                    status.bit);
    } else {
        gpiov_reply("OK", "pin=%u configured=%u mode=%s reserved=%u pull=%s drive=%u level=%u irq_count=%lu trig=%s backend=%s",
                    pin,
                    state->configured ? 1U : 0U,
                    gpiov_mode_name(state->mode),
                    gpiov_is_reserved_pin(pin) ? 1U : 0U,
                    state->pull_name ? state->pull_name : "na",
                    state->drive,
                    level ? 1U : 0U,
                    (unsigned long)state->irq_count,
                    gpiov_irq_trig_name(state->irq_trig),
                    status.backend_name);
    }
    return 0;
}

static int gpiov_status_many_cmd(int argc, char **argv)
{
    struct gpiov_pin_group group;
    char modes[GPIOV_LEVELS_BUFFER_SIZE];
    char pulls[GPIOV_LEVELS_BUFFER_SIZE];
    char levels[GPIOV_LEVELS_BUFFER_SIZE];
    char counts[GPIOV_LEVELS_BUFFER_SIZE];

    if (argc != 2) {
        gpiov_reply("FAIL", "reason=usage gpiov_status_many <pins_csv>");
        return 1;
    }
    if (!g_backend_ready) {
        gpiov_reply("FAIL", "reason=backend_not_ready backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin_group(argv[1], &group)) {
        gpiov_reply("FAIL", "reason=invalid_pin_group value=%s", argv[1]);
        return 1;
    }

    gpiov_format_pin_modes(&group, modes, sizeof(modes));
    gpiov_format_pin_pulls(&group, pulls, sizeof(pulls));
    gpiov_format_pin_levels(&group, levels, sizeof(levels));
    gpiov_format_irq_counts(&group, counts, sizeof(counts));
    gpiov_reply("OK", "pins=%s count=%lu modes=%s pulls=%s levels=%s irq_counts=%s backend=%s",
                argv[1], (unsigned long)group.count, modes, pulls, levels, counts, gpiov_backend_name());
    return 0;
}

static int gpiov_deinit_cmd(int argc, char **argv)
{
    uint8_t pin;

    if (argc != 2) {
        gpiov_reply("FAIL", "reason=usage gpiov_deinit <pin>");
        return 1;
    }
    if (!g_backend_ready) {
        gpiov_reply("FAIL", "reason=backend_not_ready backend=%s", gpiov_backend_name());
        return 1;
    }
    if (!gpiov_parse_pin(argv[1], &pin)) {
        gpiov_reply("FAIL", "reason=invalid_pin value=%s", argv[1]);
        return 1;
    }
    if (gpiov_is_reserved_pin(pin)) {
        gpiov_reply("FAIL", "reason=pin_reserved pin=%u", pin);
        return 1;
    }
    if (gpiov_backend_deinit_pin(pin) != 0) {
        gpiov_reply("FAIL", "reason=backend_deinit_failed pin=%u backend=%s", pin, gpiov_backend_name());
        return 1;
    }

    memset(&g_pin_state[pin], 0, sizeof(g_pin_state[pin]));
    g_pin_state[pin].pull_name = "na";
    g_pin_state[pin].irq_trig = GPIO_INT_TRIG_MODE_SYNC_RISING_EDGE;
    gpiov_reply("OK", "pin=%u deinit=1", pin);
    return 0;
}

int main(void)
{
#if !defined(CONFIG_FREERTOS) || !CONFIG_FREERTOS
    int ch;
#endif
    uint32_t pin;

    board_init();
    g_gpio = bflb_device_get_by_name("gpio");
    g_uart0 = bflb_device_get_by_name("uart0");

    for (pin = 0; pin < GPIOV_MAX_PIN_COUNT; pin++) {
        g_pin_state[pin].configured = false;
        g_pin_state[pin].mode = GPIOV_MODE_NONE;
        g_pin_state[pin].pull_name = "na";
        g_pin_state[pin].drive = 0;
        g_pin_state[pin].irq_trig = GPIO_INT_TRIG_MODE_SYNC_RISING_EDGE;
        g_pin_state[pin].last_level = false;
        g_pin_state[pin].irq_count = 0;
        g_pin_state[pin].last_irq_ms = 0;
    }

    g_backend_ready = (gpiov_backend_init() == 0);

    printf("gpio_validation_cli ready backend=%s backend_ready=%u\r\n",
           gpiov_backend_name(), g_backend_ready ? 1U : 0U);
#if defined(CONFIG_GPIO_TESTER) && CONFIG_GPIO_TESTER
    printf("Tester mode: GPIO0-39 -> i2c0@0x%02x, GPIO40-79 -> i2c1@0x%02x\r\n",
           (unsigned int)CONFIG_GPIO_TESTER_I2C0_ADDR,
           (unsigned int)CONFIG_GPIO_TESTER_I2C1_ADDR);
    printf("Tester mode ignores pull/drive/trig options and does not support irq commands\r\n");
#else
    printf("Reserved pins: %s are protected for console\r\n", gpiov_reserved_pins_name());
#endif
    if (!g_backend_ready) {
        printf("Backend initialization failed, gpiov_* commands will report backend_not_ready\r\n");
    }
    printf("Type gpiov_help for available commands\r\n");

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    /* Use the interrupt-driven shell path so pasted commands do not overrun UART RX while echoing. */
    shell_init_with_task(g_uart0);
    shell_set_echo(false);
    vTaskStartScheduler();
    while (1) {
    }
#else
    shell_init();
    shell_set_echo(false);
    while (1) {
        while ((ch = bflb_uart_getchar(g_uart0)) != -1) {
            shell_handler(ch);
        }
    }
#endif
}

SHELL_CMD_EXPORT_ALIAS(gpiov_ping_cmd, gpiov_ping, "gpio validation handshake");
SHELL_CMD_EXPORT_ALIAS(gpiov_help_cmd, gpiov_help, "show gpio validation commands");
SHELL_CMD_EXPORT_ALIAS(gpiov_config_cmd, gpiov_config, "configure pin: gpiov_config <pin> <in|out|irq> [pull=...] [drive=...] [init=...] [trig=...]");
SHELL_CMD_EXPORT_ALIAS(gpiov_config_many_cmd, gpiov_config_many, "configure pins: gpiov_config_many <pins_csv> <in|out|irq> [pull=...] [drive=...] [init=...] [trig=...]");
SHELL_CMD_EXPORT_ALIAS(gpiov_write_cmd, gpiov_write, "drive output: gpiov_write <pin> <0|1>");
SHELL_CMD_EXPORT_ALIAS(gpiov_write_pattern_cmd, gpiov_write_pattern, "strict output pattern: gpiov_write_pattern <pins_csv> <target_pin> <target_level> <other_level>");
SHELL_CMD_EXPORT_ALIAS(gpiov_read_cmd, gpiov_read, "read level: gpiov_read <pin>");
SHELL_CMD_EXPORT_ALIAS(gpiov_read_many_cmd, gpiov_read_many, "read levels: gpiov_read_many <pins_csv>");
SHELL_CMD_EXPORT_ALIAS(gpiov_read_expect_cmd, gpiov_read_expect, "strict input check: gpiov_read_expect <pins_csv> <target_pin> <target_level> <other_level>");
SHELL_CMD_EXPORT_ALIAS(gpiov_pulse_cmd, gpiov_pulse, "pulse output: gpiov_pulse <pin> <count> <high_ms> <low_ms>");
SHELL_CMD_EXPORT_ALIAS(gpiov_irq_get_cmd, gpiov_irq_get, "read irq counter: gpiov_irq_get <pin>");
SHELL_CMD_EXPORT_ALIAS(gpiov_irq_get_many_cmd, gpiov_irq_get_many, "read irq counters: gpiov_irq_get_many <pins_csv>");
SHELL_CMD_EXPORT_ALIAS(gpiov_irq_clear_cmd, gpiov_irq_clear, "clear irq counter: gpiov_irq_clear <pin>");
SHELL_CMD_EXPORT_ALIAS(gpiov_irq_clear_many_cmd, gpiov_irq_clear_many, "clear irq counters: gpiov_irq_clear_many <pins_csv>");
SHELL_CMD_EXPORT_ALIAS(gpiov_irq_wait_cmd, gpiov_irq_wait, "wait for irq count: gpiov_irq_wait <pin> <expected_count> <timeout_ms>");
SHELL_CMD_EXPORT_ALIAS(gpiov_irq_expect_cmd, gpiov_irq_expect, "strict irq check: gpiov_irq_expect <pins_csv> <target_pin> <expected_delta> <timeout_ms>");
SHELL_CMD_EXPORT_ALIAS(gpiov_status_cmd, gpiov_status, "show pin status: gpiov_status <pin>");
SHELL_CMD_EXPORT_ALIAS(gpiov_status_many_cmd, gpiov_status_many, "show pins status: gpiov_status_many <pins_csv>");
SHELL_CMD_EXPORT_ALIAS(gpiov_deinit_cmd, gpiov_deinit, "deinit pin: gpiov_deinit <pin>");
