#ifndef IPC_THROUGHPUT_TEST_H
#define IPC_THROUGHPUT_TEST_H

#include <stdbool.h>
#include <stdint.h>

#define DEFAULT_BUFFER_SIZE       1400U
#define MAX_BUFFER_SIZE           1500U
#define DEFAULT_DURATION_SEC      10U
#define PHASE_GAP_SEC             2U
#define APP_TASK_STACK_SIZE       4096U
#define APP_TASK_PRIORITY         6U
#define REPORT_INTERVAL_MS        1000U

#define PACKET_FLAG_NONE   0x00U
#define PACKET_FLAG_SYNC   0x04U
#define PACKET_FLAG_START  0x08U
#define PACKET_FLAG_END    0x10U

typedef struct {
    uint32_t seq_num;
    uint32_t flags;
    uint32_t data_size;
    uint32_t crc32;
    uint8_t data[];
} throughput_packet_t;

typedef struct {
    uint64_t last_time_ms;
    uint32_t total_bytes_sent;
    uint32_t total_packets_sent;
    uint32_t total_bytes_recv;
    uint32_t total_packets_recv;
    uint32_t crc_errors;
} throughput_stats_t;

extern const bool ipc_throughput_crc_enabled;
int ipc_throughput_init(void);
int ipc_throughput_is_ready(void);
int cmd_ipc_throughput_test(int argc, char **argv);

#endif /* IPC_THROUGHPUT_TEST_H */
