/**
 * @file iperf_cli.c
 * @brief Shell argument parsing for the compact iPerf2 public API.
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <lwip/ip4_addr.h>
#include <shell.h>
#include <semphr.h>
#include <task.h>
#include <utils_getopt.h>

/** @brief Logging category for shell lifecycle and argument errors. */
#define DBG_TAG "IPERF_CLI"
#include "log.h"

#include "bflb_iperf.h"

/** @brief Dynamic one-shot reaper stack depth in StackType_t units. */
#define IPERF_CLEANUP_STACK 512U
/** @brief Reaper priority, one level above the idle task. */
#define IPERF_CLEANUP_PRIORITY (tskIDLE_PRIORITY + 1U)

/** @brief Mutex-protected ownership state of the single CLI instance slot. */
typedef enum {
    IPERF_CLI_EMPTY = 0, /**< No owned instance; a new create may claim the slot. */
    IPERF_CLI_ACTIVE,   /**< Create succeeded; completion not yet recorded by CLI. */
    IPERF_CLI_FINISHED, /**< Completion recorded; still owns a handle to reap. */
    IPERF_CLI_CLOSING,  /**< One owner claimed destroy and may be waiting unlocked. */
} iperf_cli_state_t;

/* The SDK shell serializes command entry. Create this permanent mutex in the
 * first command, before any worker exists; never lazily initialize in callbacks.
 * All slot fields below are read/written with this mutex held. */
/** @brief Permanent mutex initialized by serialized shell entry, never deleted. */
static SemaphoreHandle_t s_cli_lock;
/** @brief Owned slot handle, published by create while the CLI mutex is held. */
static bflb_iperf_t *s_cli_iperf;
/** @brief Slot ownership state; all runtime access holds s_cli_lock. */
static iperf_cli_state_t s_cli_state;
/* Zero is unused. Refuse new instances at UINTPTR_MAX rather than reuse a token. */
/** @brief Monotonic nonzero generation token; never wraps or reuses a value. */
static uintptr_t s_cli_generation;

/** @brief User-visible usage text for the iperf shell command. */
#define IPERF_USAGE                                                          \
    "iperf -s|-c <IPv4-address> [-u] [-A socket|raw] [-p port] [-l bytes] "  \
    "[-t sec|-n bytes] [-i sec] [-b bit/s[K|M]] [-S tos] [-N] [-B host]\r\n" \
    "iperf -a\r\n"                                                           \
    "iperf [-h]\r\n"                                                         \
    "  -s        server mode\r\n"                                            \
    "  -c addr   client mode (IPv4 address)\r\n"                             \
    "  -u        UDP (default TCP)\r\n"                                      \
    "  -A name   backend: socket or raw (default raw)\r\n"                   \
    "  -p port   server port (default 5001)\r\n"                             \
    "  -l bytes  buffer/datagram length\r\n"                                 \
    "  -t sec    client duration (default 10)\r\n"                           \
    "  -n bytes  client byte limit instead of duration\r\n"                  \
    "  -i sec    report interval, zero disables periodic reports\r\n"        \
    "  -b rate   UDP client bandwidth in bit/s; K/M suffix accepted\r\n"     \
    "  -S tos    IPv4 TOS value\r\n"                                         \
    "  -N        disable Nagle for TCP client\r\n"                           \
    "  -B addr   bind local IPv4 address\r\n"                                \
    "  -a        synchronously stop and destroy the current test\r\n"

/**
 * @brief Claim the slot and destroy outside the CLI mutex.
 * @return bflb_iperf_destroy() status; zero on release, otherwise its error code.
 * @pre s_cli_lock held, state ACTIVE or FINISHED, pointer non-NULL.
 * @post Returns with s_cli_lock held; on error the handle remains owned.
 * @note CLOSING excludes other owners while the mutex is dropped for the
 * blocking destroy wait. The mutex is reacquired before updating slot state.
 * @warning Task context only, never the instance event callback.
 */
static int iperf_cli_destroy_locked(void)
{
    bflb_iperf_t *iperf = s_cli_iperf;
    int result;

    s_cli_state = IPERF_CLI_CLOSING;
    xSemaphoreGive(s_cli_lock);
    result = bflb_iperf_destroy(iperf);
    xSemaphoreTake(s_cli_lock, portMAX_DELAY);
    if (result == BFLB_IPERF_OK) {
        s_cli_iperf = NULL;
        s_cli_state = IPERF_CLI_EMPTY;
        LOG_I("iperf instance destroyed\r\n");
    } else {
        /* No other path can take CLOSING. Public destroy errors leave a valid
         * instance unchanged. FINISHED may have arrived while unlocked, so do
         * not blindly restore ACTIVE and lose the opportunity to reclaim it. */
        bflb_iperf_state_t state = bflb_iperf_get_state(iperf);

        s_cli_state = (state == BFLB_IPERF_STATE_DONE || state == BFLB_IPERF_STATE_ERROR) ?
                          IPERF_CLI_FINISHED : IPERF_CLI_ACTIVE;
        LOG_E("unexpected destroy failure: %d; handle retained\r\n", result);
    }
    return result;
}

/**
 * @brief Reap only the generation captured by value when this task was created.
 * @param[in] arg Generation encoded as a pointer value, never dereferenced.
 * @post Matching FINISHED slot is offered for destruction; this task then
 * self-deletes even if stale or destruction fails.
 * @note Never dereference an old instance or rely on reused allocator addresses.
 */
static void iperf_cli_cleanup_task(void *arg)
{
    uintptr_t generation = (uintptr_t)arg;

    xSemaphoreTake(s_cli_lock, portMAX_DELAY);
    if (s_cli_generation == generation && s_cli_state == IPERF_CLI_FINISHED) {
        iperf_cli_destroy_locked();
    }
    xSemaphoreGive(s_cli_lock);
    vTaskDelete(NULL);
}

/**
 * @brief Schedule one temporary reaper after completion, never wait for it.
 * @param[in] iperf Callback instance; unused, not retained by the reaper.
 * @param[in] event Lifecycle event; STARTED is ignored.
 * @param[in] result Callback-lifetime snapshot used for failure logging.
 * @param[in] user_data Generation encoded by value at create time.
 * @pre The permanent CLI mutex exists before any worker can emit events.
 * @post Matching ACTIVE becomes FINISHED; task allocation failure keeps
 * FINISHED and its handle for -a or a later command to reclaim.
 * @note CLOSING already has a destroy owner. This callback never destroys or
 * waits for a reaper; FINISHED must return before core completion is signalled.
 * @note create runs under s_cli_lock but never waits for this callback. The
 * task may start immediately and block on the mutex; task creation does not
 * join it, so no lock cycle is introduced.
 */
static void iperf_cli_event_cb(bflb_iperf_t *iperf,
                                bflb_iperf_event_t event,
                                const bflb_iperf_result_t *result,
                                void *user_data)
{
    uintptr_t generation = (uintptr_t)user_data;

    (void)iperf;
    if (event != BFLB_IPERF_EVENT_FINISHED) {
        return;
    }
    if (result->error != 0) {
        LOG_E("iperf test failed: state=%d error=%d bytes=%llu duration_us=%llu\r\n",
              result->state, result->error,
              (unsigned long long)result->bytes,
              (unsigned long long)result->duration_us);
    }

    xSemaphoreTake(s_cli_lock, portMAX_DELAY);
    if (s_cli_generation == generation && s_cli_state == IPERF_CLI_ACTIVE) {
        s_cli_state = IPERF_CLI_FINISHED;
        if (xTaskCreate(iperf_cli_cleanup_task, "iperf_cleanup",
                        IPERF_CLEANUP_STACK, (void *)generation,
                        IPERF_CLEANUP_PRIORITY, NULL) != pdPASS) {
            /* Keep FINISHED: -a or any subsequent command can reclaim it. */
            LOG_E("cleanup task allocation failed; next command will reclaim instance\r\n");
        }
    }
    /* CLOSING already has an owner waiting for this callback to return. */
    xSemaphoreGive(s_cli_lock);
}

/**
 * @brief Parse an unsigned 64-bit integer with C base-prefix support.
 * @pre value is non-NULL; output is unchanged on failure.
 * @param[in] text NUL-terminated numeric string.
 * @param[out] value Parsed value.
 * @retval 0 Parse succeeded.
 * @retval -1 Input is invalid or out of range.
 */
static int iperf_parse_u64(const char *text, uint64_t *value)
{
    char *end;
    unsigned long long parsed;

    if (text == NULL || text[0] == '-' || text[0] == '\0') {
        return -1;
    }
    errno = 0;
    parsed = strtoull(text, &end, 0);
    if (errno != 0 || end == text || *end != '\0') {
        return -1;
    }
    *value = (uint64_t)parsed;
    return 0;
}

/**
 * @brief Parse an unsigned 32-bit integer.
 * @pre value is non-NULL; output is unchanged on failure.
 * @param[in] text NUL-terminated numeric string.
 * @param[out] value Parsed value.
 * @retval 0 Parse succeeded.
 * @retval -1 Input is invalid or out of range.
 */
static int iperf_parse_u32(const char *text, uint32_t *value)
{
    uint64_t parsed;

    if (iperf_parse_u64(text, &parsed) < 0 || parsed > UINT32_MAX) {
        return -1;
    }
    *value = (uint32_t)parsed;
    return 0;
}

/**
 * @brief Parse an unsigned 16-bit integer.
 * @pre value is non-NULL; output is unchanged on failure.
 * @param[in] text NUL-terminated numeric string.
 * @param[out] value Parsed value.
 * @retval 0 Parse succeeded.
 * @retval -1 Input is invalid or out of range.
 */
static int iperf_parse_u16(const char *text, uint16_t *value)
{
    uint32_t parsed;

    if (iperf_parse_u32(text, &parsed) < 0 || parsed > UINT16_MAX) {
        return -1;
    }
    *value = (uint16_t)parsed;
    return 0;
}

/**
 * @brief Parse an unsigned 8-bit integer.
 * @pre value is non-NULL; output is unchanged on failure.
 * @param[in] text NUL-terminated numeric string.
 * @param[out] value Parsed value.
 * @retval 0 Parse succeeded.
 * @retval -1 Input is invalid or out of range.
 */
static int iperf_parse_u8(const char *text, uint8_t *value)
{
    uint32_t parsed;

    if (iperf_parse_u32(text, &parsed) < 0 || parsed > UINT8_MAX) {
        return -1;
    }
    *value = (uint8_t)parsed;
    return 0;
}

/**
 * @brief Parse an IPv4 address into lwIP network byte order.
 * @param[in] text Non-NULL IPv4 string in a form accepted by ip4addr_aton().
 * @param[out] address Parsed IPv4 address in network byte order.
 * @retval 0 Parse succeeded.
 * @retval -1 Input is not a valid IPv4 address.
 * @pre address is non-NULL; output is unchanged on failure.
 */
static int iperf_parse_ip4(const char *text, uint32_t *address)
{
    ip4_addr_t ip4;

    if (!ip4addr_aton(text, &ip4)) {
        return -1;
    }
    *address = ip4.addr;
    return 0;
}

/**
 * @brief Parse a bandwidth in bits per second with an optional K/M suffix.
 * @param[in] text NUL-terminated integer with optional K, k, M, or m suffix.
 * @param[out] value Parsed bandwidth in bits per second.
 * @retval 0 Parse succeeded.
 * @retval -1 Input is invalid or exceeds UINT32_MAX.
 * @note K and M use decimal multipliers of 1000 and 1000000.
 * @pre value is non-NULL; output is unchanged on failure.
 */
static int iperf_parse_bandwidth(const char *text, uint32_t *value)
{
    char *end;
    uint64_t multiplier = 1U;
    unsigned long long parsed;

    if (text == NULL || text[0] == '-' || text[0] == '\0') {
        return -1;
    }
    errno = 0;
    parsed = strtoull(text, &end, 0);
    if (errno != 0 || end == text) {
        return -1;
    }
    if ((end[0] == 'K' || end[0] == 'k') && end[1] == '\0') {
        multiplier = 1000U;
    } else if ((end[0] == 'M' || end[0] == 'm') && end[1] == '\0') {
        multiplier = 1000000U;
    } else if (end[0] != '\0') {
        return -1;
    }
    if ((uint64_t)parsed > UINT32_MAX / multiplier) {
        return -1;
    }
    *value = (uint32_t)((uint64_t)parsed * multiplier);
    return 0;
}

/**
 * @brief Parse the iperf shell command and invoke the public API.
 *
 * The command manages one instance. The -a option synchronously stops and
 * destroys it; a completed instance is reaped by a temporary task. Client and bind
 * addresses are parsed by lwIP's ip4addr_aton(), not resolved as hostnames.
 *
 * @param[in] argc Number of shell arguments.
 * @param[in] argv Shell argument vector.
 * @pre SDK shell serializes command entry, including first mutex creation.
 * @note The mutex is permanent. Creation publishes the output handle before
 * worker execution; callbacks cannot inspect the slot until ACTIVE is committed
 * and the mutex released. Generation exhaustion rejects new instances instead
 * of allowing an old reaper token to match a reused allocation.
 *
 * @note Commands are rejected while asynchronous destruction is pending.
 */
static void iperf_cmd(int argc, char **argv)
{
    bflb_iperf_config_t config;
    getopt_env_t env;
    uint8_t client = 0U;
    uint8_t server = 0U;
    uint8_t stop = 0U;
    uint8_t bandwidth_set = 0U;
    uint8_t time_set = 0U;
    uint8_t amount_set = 0U;
    int option;

    /* Initialization is serialized by the single SDK shell command task. */
    if (s_cli_lock == NULL) {
        s_cli_lock = xSemaphoreCreateMutex();
        if (s_cli_lock == NULL) {
            LOG_E("failed to create CLI mutex\r\n");
            return;
        }
    }
    xSemaphoreTake(s_cli_lock, portMAX_DELAY);
    if (s_cli_state == IPERF_CLI_CLOSING) {
        xSemaphoreGive(s_cli_lock);
        LOG_E("previous test is still releasing\r\n");
        return;
    }
    if (s_cli_state == IPERF_CLI_FINISHED &&
        iperf_cli_destroy_locked() != BFLB_IPERF_OK) {
        xSemaphoreGive(s_cli_lock);
        return;
    }
    xSemaphoreGive(s_cli_lock);

    if (argc <= 1 ||
        (argc == 2 && strcmp(argv[1], "-h") == 0)) {
        LOG_I(IPERF_USAGE);
        return;
    }
    /* Start from public API defaults, then override only explicit CLI options. */
    bflb_iperf_config_init(&config);
    utils_getopt_init(&env, 0);
    while ((option = utils_getopt(&env, argc, argv, ":aA:c:sup:l:t:n:i:b:S:NB:h")) != -1) {
        switch (option) {
            case 'a':
                stop = 1U;
                break;
            case 'A':
                if (strcmp(env.optarg, "socket") == 0) {
                    config.backend = BFLB_IPERF_BACKEND_SOCKET;
                } else if (strcmp(env.optarg, "raw") == 0) {
                    config.backend = BFLB_IPERF_BACKEND_RAW;
                } else {
                    LOG_E("backend must be socket or raw\r\n");
                    return;
                }
                break;
            case 'c':
                if (iperf_parse_ip4(env.optarg, &config.remote_ip4) < 0) {
                    LOG_E("invalid client IPv4 address\r\n");
                    return;
                }
                client = 1U;
                break;
            case 's':
                server = 1U;
                break;
            case 'u':
                config.proto = BFLB_IPERF_PROTO_UDP;
                break;
            case 'p':
                if (iperf_parse_u16(env.optarg, &config.port) < 0) {
                    goto invalid_value;
                }
                break;
            case 'l':
                if (iperf_parse_u16(env.optarg, &config.buffer_len) < 0) {
                    goto invalid_value;
                }
                break;
            case 't':
                if (iperf_parse_u32(env.optarg, &config.duration_s) < 0 ||
                    config.duration_s == 0U) {
                    goto invalid_value;
                }
                time_set = 1U;
                break;
            case 'n':
                if (iperf_parse_u64(env.optarg, &config.amount_bytes) < 0 ||
                    config.amount_bytes == 0U) {
                    goto invalid_value;
                }
                config.duration_s = 0U;
                amount_set = 1U;
                break;
            case 'i':
                if (iperf_parse_u16(env.optarg, &config.interval_s) < 0) {
                    goto invalid_value;
                }
                break;
            case 'b':
                if (iperf_parse_bandwidth(env.optarg, &config.bandwidth_bps) < 0) {
                    goto invalid_value;
                }
                bandwidth_set = 1U;
                break;
            case 'S':
                if (iperf_parse_u8(env.optarg, &config.tos) < 0) {
                    goto invalid_value;
                }
                break;
            case 'N':
                config.tcp_nodelay = 1U;
                break;
            case 'B':
                if (iperf_parse_ip4(env.optarg, &config.local_ip4) < 0) {
                    LOG_E("invalid local IPv4 address\r\n");
                    return;
                }
                break;
            case 'h':
                LOG_I(IPERF_USAGE);
                return;
            case ':':
                LOG_E("option -%c requires a value\r\n", env.optopt);
                return;
            default:
                LOG_I(IPERF_USAGE);
                return;
        }
    }

    /* Reject positional arguments left after utils_getopt() finishes. */
    if (env.optind < argc) {
        LOG_E("unexpected argument: %s\r\n", argv[env.optind]);
        return;
    }
    /* Keep stop as a standalone control command, not a test configuration. */
    if (stop != 0U) {
        if (client != 0U || server != 0U) {
            LOG_E("-a cannot be combined with -c or -s\r\n");
            return;
        }
        xSemaphoreTake(s_cli_lock, portMAX_DELAY);
        if (s_cli_state == IPERF_CLI_CLOSING) {
            LOG_E("previous test is still releasing\r\n");
        } else if (s_cli_state == IPERF_CLI_EMPTY) {
            LOG_I("no iperf test exists\r\n");
        } else {
            iperf_cli_destroy_locked();
        }
        xSemaphoreGive(s_cli_lock);
        return;
    }
    /* Exactly one endpoint role is required before building the API request. */
    if (client == server) {
        LOG_E("select exactly one of -c and -s\r\n");
        return;
    }
    if (time_set != 0U && amount_set != 0U) {
        LOG_E("-t and -n are mutually exclusive\r\n");
        return;
    }
    config.role = client != 0U ? BFLB_IPERF_ROLE_CLIENT : BFLB_IPERF_ROLE_SERVER;
    if (bandwidth_set != 0U && config.proto != BFLB_IPERF_PROTO_UDP) {
        LOG_E("-b is only valid for UDP\r\n");
        return;
    }
    /* Select protocol-specific sizes only when the user omitted -l and -b. */
    if (config.buffer_len == 0U) {
        config.buffer_len = config.proto == BFLB_IPERF_PROTO_UDP ?
                                BFLB_IPERF_DEFAULT_UDP_LEN :
                                BFLB_IPERF_DEFAULT_TCP_LEN;
    }
    if (config.proto == BFLB_IPERF_PROTO_UDP && config.bandwidth_bps == 0U) {
        config.bandwidth_bps = BFLB_IPERF_DEFAULT_UDP_RATE_BPS;
    }
    config.event_cb = iperf_cli_event_cb;

    /* The core supports many instances; this CLI explicitly owns one slot. */
    xSemaphoreTake(s_cli_lock, portMAX_DELAY);
    if (s_cli_state == IPERF_CLI_FINISHED &&
        iperf_cli_destroy_locked() != BFLB_IPERF_OK) {
        xSemaphoreGive(s_cli_lock);
        return;
    }
    if (s_cli_state != IPERF_CLI_EMPTY) {
        xSemaphoreGive(s_cli_lock);
        LOG_E("an iperf test is active or closing\r\n");
        return;
    }
    if (s_cli_generation == UINTPTR_MAX) {
        xSemaphoreGive(s_cli_lock);
        LOG_E("CLI generation exhausted; refusing token reuse\r\n");
        return;
    }
    config.user_data = (void *)++s_cli_generation;
    /* Output handle is published by create before worker visibility. The
     * callback cannot inspect the slot until ACTIVE is committed and unlocked. */
    if (bflb_iperf_create(&config, &s_cli_iperf) == BFLB_IPERF_OK) {
        s_cli_state = IPERF_CLI_ACTIVE;
    } else {
        s_cli_state = IPERF_CLI_EMPTY;
        LOG_E("create failed (invalid config or no resources)\r\n");
    }
    xSemaphoreGive(s_cli_lock);
    return;

invalid_value:
    LOG_E("invalid numeric value\r\n");
}

SHELL_CMD_EXPORT_ALIAS(iperf_cmd, iperf, compact iperf command);
