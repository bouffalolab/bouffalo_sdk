/****************************************************************************
 * btble_test_port.c
 *
 * BLE 性能测试命令集（扫描/包接收成功率、连接成功率 Master/Slave）。
 * 适配 blestack（BL602/BL616/BL616CL 等使用的蓝牙协议栈）。
 *
 * 设计目标：自动化友好。每个测试命令为「阻塞执行 + 单行机器可解析结果」：
 *   - 命令运行期间不打印每个包/事件，避免 UART 拖慢中断导致丢包；
 *   - 测试结束统一输出一行：
 *       [BLE_TEST] RESULT type=<...> key=value key=value ...
 *     脚本只需 grep "\[BLE_TEST\] RESULT" 再按 key=value 解析。
 *
 * 配套发送端/对端使用约定的厂商 ID + 递增序列号广播包。
 ****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "conn.h"
#include "conn_internal.h"
#include "hci_core.h"
#include "bluetooth.h"
#include "shell.h"

#include "btble_test_port.h"

/* 测试约定：厂商数据格式 = [厂商ID 2字节小端][序列号 2字节大端] */
#define TEST_MFG_ID         0xB0F0
#define TEST_MFG_DATA_LEN   4

/* 序列号去重位图：支持最多 8192 个唯一包（足够 100~1000 次测试） */
#define SEQ_BITMAP_BITS     8192
#define SEQ_BITMAP_WORDS    (SEQ_BITMAP_BITS / 32)

/* 发送端默认节奏：500ms 内发 20 个包 → 每包 25ms */
#define TX_BURST_COUNT      20
#define TX_BURST_WINDOW_MS  500
#define TX_PKT_INTERVAL_MS  (TX_BURST_WINDOW_MS / TX_BURST_COUNT)  /* 25ms */
#define TX_ADV_ON_MS        5    /* 每包广播开启时长 */

/* 连接测试角色 */
enum test_conn_role {
    TEST_ROLE_NONE = 0,
    TEST_ROLE_MASTER,
    TEST_ROLE_SLAVE,
};

/* 工作任务命令：蓝牙回调上下文不能直接调用 bt_conn_disconnect/set_adv_enable，
 * 否则触发 bt_assert 崩溃。回调里只投递命令，由工作任务在任务上下文执行。 */
enum test_work_cmd {
    WORK_DISCONNECT = 1,  /* Master：连上后断开 */
    WORK_RESTART_ADV,     /* Slave：断开后重启广播 */
};

struct test_work_item {
    enum test_work_cmd cmd;
    struct bt_conn    *conn;   /* WORK_DISCONNECT 用 */
};

struct ble_test_ctx {
    /* ---- 包接收(RX)统计 ---- */
    volatile uint32_t rx_total;       /* 收到的匹配厂商ID的广播事件总数（含重复） */
    volatile uint32_t rx_unique;      /* 去重后唯一序列号数量 */
    uint32_t          rx_bitmap[SEQ_BITMAP_WORDS];
    uint16_t          rx_expect;      /* 期望总包数（用于算成功率，0 表示不算） */
    volatile bool     rx_running;

    /* ---- 连接测试 ---- */
    enum test_conn_role conn_role;
    volatile bool     conn_running;
    uint16_t          conn_target;    /* 目标次数 */
    volatile uint16_t conn_attempt;   /* Master 已发起次数 */
    volatile uint16_t conn_success;   /* 成功次数 */
    bt_addr_le_t      conn_peer;      /* Master 目标地址 */
    SemaphoreHandle_t conn_evt_sem;   /* Master 每轮结束（断开后）发信号 */

    /* ---- 工作任务（把回调里的操作派发到任务上下文执行）---- */
    QueueHandle_t     work_queue;
    TaskHandle_t      work_task;
};

static struct ble_test_ctx g_test;

/* blestack 库导出：使能/重启广播（slave 重广播用） */
extern int set_adv_enable(bool enable);

/* 工作任务：在任务上下文执行蓝牙操作（回调上下文禁止直接调用） */
static void test_work_task(void *arg)
{
    struct test_work_item item;
    for (;;) {
        if (xQueueReceive(g_test.work_queue, &item, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        switch (item.cmd) {
        case WORK_DISCONNECT:
            if (item.conn) {
                bt_conn_disconnect(item.conn,
                                   BT_HCI_ERR_REMOTE_USER_TERM_CONN);
            }
            break;
        case WORK_RESTART_ADV:
            set_adv_enable(true);
            break;
        default:
            break;
        }
    }
}

/* 回调上下文安全地投递工作命令 */
static void test_work_post(enum test_work_cmd cmd, struct bt_conn *conn)
{
    struct test_work_item item = { .cmd = cmd, .conn = conn };
    if (g_test.work_queue) {
        xQueueSend(g_test.work_queue, &item, 0);
    }
}

/****************************************************************************
 * 工具函数
 ****************************************************************************/

/* 解析 "112233AABBCC" 形式的 MAC，输出为广播序（小端，即 addr.a.val） */
static int parse_mac(const char *str, uint8_t out_le[6])
{
    uint8_t be[6];
    if (strlen(str) != 12) {
        return -1;
    }
    for (int i = 0; i < 6; i++) {
        char b[3] = { str[i * 2], str[i * 2 + 1], 0 };
        char *end = NULL;
        long v = strtol(b, &end, 16);
        if (end != b + 2) {
            return -1;
        }
        be[i] = (uint8_t)v;
    }
    /* 输入按可读顺序（高字节在前），地址 val 是小端，需反转 */
    for (int i = 0; i < 6; i++) {
        out_le[i] = be[5 - i];
    }
    return 0;
}

static inline bool seq_bitmap_test_and_set(uint32_t *bitmap, uint16_t seq)
{
    uint16_t idx = seq % SEQ_BITMAP_BITS;
    uint32_t mask = 1u << (idx & 31);
    if (bitmap[idx >> 5] & mask) {
        return true; /* 已存在 */
    }
    bitmap[idx >> 5] |= mask;
    return false;
}

/****************************************************************************
 * RX：包接收成功率测试
 ****************************************************************************/

/* 广播数据解析回调：仅在厂商数据匹配时静默计数 */
static bool rx_data_cb(struct bt_data *data, void *user_data)
{
    if (data->type == BT_DATA_MANUFACTURER_DATA &&
        data->data_len >= TEST_MFG_DATA_LEN) {
        uint16_t mfg = data->data[0] | (data->data[1] << 8);
        if (mfg == TEST_MFG_ID) {
            uint16_t seq = (data->data[2] << 8) | data->data[3];
            g_test.rx_total++;
            if (!seq_bitmap_test_and_set(g_test.rx_bitmap, seq)) {
                g_test.rx_unique++;
            }
            return false; /* 命中即停止解析该包 */
        }
    }
    return true;
}

/* 扫描回调：静默，仅做内容匹配，绝不打印 */
static void rx_scan_cb(const bt_addr_le_t *addr, s8_t rssi, u8_t adv_type,
                       struct net_buf_simple *buf)
{
    if (!g_test.rx_running) {
        return;
    }
    bt_data_parse(buf, rx_data_cb, NULL);
}

/* ble_test_rx <duration_ms> [expect_count] [interval] [window]
 *   阻塞扫描 duration_ms，结束输出统计。
 *   expect_count 给定时额外输出成功率百分比。
 *   interval/window 单位 0.625ms（协议原生单位，十进制或 0x 十六进制均可），
 *   不填默认 0x40/0x20（40ms/20ms，占空比 50%，满足用例「不高于 50%」要求）。
 */
int cmd_ble_test_rx(int argc, char **argv)
{
    struct bt_le_scan_param scan_param;
    uint32_t duration_ms;
    uint16_t interval = 0x40;  /* 64 × 0.625ms = 40ms */
    uint16_t window   = 0x20;  /* 32 × 0.625ms = 20ms → 占空比 50% */

    if (argc < 2) {
        printf("Usage: ble_test_rx <duration_ms> [expect_count] [interval] [window]\r\n"
               "  interval/window in 0.625ms units, default 0x40/0x20 (50%% duty)\r\n");
        return -1;
    }
    duration_ms = (uint32_t)strtoul(argv[1], NULL, 0);
    g_test.rx_expect = (argc >= 3) ? (uint16_t)strtoul(argv[2], NULL, 0) : 0;
    if (argc >= 4) {
        interval = (uint16_t)strtoul(argv[3], NULL, 0);
    }
    if (argc >= 5) {
        window = (uint16_t)strtoul(argv[4], NULL, 0);
    }
    if (window > interval || interval == 0) {
        printf("[BLE_TEST] RESULT type=rx status=error err=-22 "
               "(need 0 < window <= interval)\r\n");
        return -1;
    }

    /* 复位统计 */
    g_test.rx_total = 0;
    g_test.rx_unique = 0;
    memset(g_test.rx_bitmap, 0, sizeof(g_test.rx_bitmap));

    /* 关键：关闭协议层去重(filter_dup=0)，否则同一发送端只上报一次会漏统计。 */
    memset(&scan_param, 0, sizeof(scan_param));
    scan_param.type       = BT_LE_SCAN_TYPE_PASSIVE; /* 被动扫描即可统计广播 */
    scan_param.filter_dup = 0;
    scan_param.interval   = interval;
    scan_param.window     = window;

    /* 占空比可追溯：INFO 行输出本次实际扫描参数 */
    printf("[BLE_TEST] INFO type=rx interval=%u window=%u duty=%u%%\r\n",
           interval, window, (unsigned)(window * 100 / interval));

    /* 先停掉可能残留的扫描，命令自洽不依赖板子初始状态 */
    bt_le_scan_stop();

    g_test.rx_running = true;
    int err = bt_le_scan_start(&scan_param, rx_scan_cb);
    if (err) {
        g_test.rx_running = false;
        printf("[BLE_TEST] RESULT type=rx status=error err=%d\r\n", err);
        return -1;
    }

    vTaskDelay(pdMS_TO_TICKS(duration_ms));

    g_test.rx_running = false;
    bt_le_scan_stop();

    /* 单行机器可解析结果 */
    if (g_test.rx_expect > 0) {
        uint32_t rate = (g_test.rx_unique * 100) / g_test.rx_expect;
        printf("[BLE_TEST] RESULT type=rx status=ok total=%lu unique=%lu "
               "expect=%u rate=%lu\r\n",
               (unsigned long)g_test.rx_total, (unsigned long)g_test.rx_unique,
               g_test.rx_expect, (unsigned long)rate);
    } else {
        printf("[BLE_TEST] RESULT type=rx status=ok total=%lu unique=%lu\r\n",
               (unsigned long)g_test.rx_total, (unsigned long)g_test.rx_unique);
    }
    return 0;
}

/****************************************************************************
 * RX 异步版：ble_test_rx_start / _stat / _stop
 *
 * 供 coex 等需要"扫描期间交错发其他命令"的场景使用。原 ble_test_rx 是阻塞
 * 同步扫描（vTaskDelay 等 duration），占用 shell task 整段时长，无法在扫描
 * 期间发 iperf 等命令。异步版拆成三步：
 *   ble_test_rx_start [interval] [window]  清零+启动后台扫描，立即返回
 *   ble_test_rx_stat                       读当前累计 total/unique（不打断扫描）
 *   ble_test_rx_stop [expect]              停扫描+输出最终（expect 给定则算 rate）
 * 复用同一套 rx_data_cb + seq bitmap 去重，统计口径与 ble_test_rx 完全一致。
 * ble only 的 test_rx 继续用原 ble_test_rx，不受影响。
 ****************************************************************************/

/* ble_test_rx_start [interval] [window]
 *   interval/window 单位 0.625ms，默认 0x40/0x20（40ms/20ms，占空比 50%）。 */
int cmd_ble_test_rx_start(int argc, char **argv)
{
    struct bt_le_scan_param scan_param;
    uint16_t interval = 0x40;
    uint16_t window   = 0x20;

    if (argc >= 2) {
        interval = (uint16_t)strtoul(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        window = (uint16_t)strtoul(argv[2], NULL, 0);
    }
    if (window > interval || interval == 0) {
        printf("[BLE_TEST] RESULT type=rx_start status=error err=-22 "
               "(need 0 < window <= interval)\r\n");
        return -1;
    }

    /* 复位统计 */
    g_test.rx_total = 0;
    g_test.rx_unique = 0;
    memset(g_test.rx_bitmap, 0, sizeof(g_test.rx_bitmap));

    memset(&scan_param, 0, sizeof(scan_param));
    scan_param.type       = BT_LE_SCAN_TYPE_PASSIVE;
    scan_param.filter_dup = 0;   /* 关协议层去重，靠 seq bitmap 自去重 */
    scan_param.interval   = interval;
    scan_param.window     = window;

    bt_le_scan_stop();   /* 命令自洽：先停可能残留的扫描 */

    g_test.rx_running = true;
    int err = bt_le_scan_start(&scan_param, rx_scan_cb);
    if (err) {
        g_test.rx_running = false;
        printf("[BLE_TEST] RESULT type=rx_start status=error err=%d\r\n", err);
        return -1;
    }

    printf("[BLE_TEST] INFO type=rx_start interval=%u window=%u duty=%u%%\r\n",
           interval, window, (unsigned)(window * 100 / interval));
    return 0;
}

/* ble_test_rx_stat —— 读当前累计统计（不打断扫描）。供 coex 每方向算 delta。 */
int cmd_ble_test_rx_stat(int argc, char **argv)
{
    printf("[BLE_TEST] RESULT type=rx_stat status=ok total=%lu unique=%lu\r\n",
           (unsigned long)g_test.rx_total, (unsigned long)g_test.rx_unique);
    return 0;
}

/* ble_test_rx_stop [expect]
 *   停止扫描 + 输出最终。expect>0 时额外算 rate=unique*100/expect。 */
int cmd_ble_test_rx_stop(int argc, char **argv)
{
    g_test.rx_running = false;
    bt_le_scan_stop();

    g_test.rx_expect = (argc >= 2) ? (uint16_t)strtoul(argv[1], NULL, 0) : 0;
    if (g_test.rx_expect > 0) {
        uint32_t rate = (g_test.rx_unique * 100) / g_test.rx_expect;
        printf("[BLE_TEST] RESULT type=rx_stop status=ok total=%lu unique=%lu "
               "expect=%u rate=%lu\r\n",
               (unsigned long)g_test.rx_total, (unsigned long)g_test.rx_unique,
               g_test.rx_expect, (unsigned long)rate);
    } else {
        printf("[BLE_TEST] RESULT type=rx_stop status=ok total=%lu unique=%lu\r\n",
               (unsigned long)g_test.rx_total, (unsigned long)g_test.rx_unique);
    }
    return 0;
}

/****************************************************************************
 * TX：发包测试（作为对端发送约定格式广播）
 ****************************************************************************/

/* ble_test_tx <total_count>
 *   按 500ms/20包(25ms间隔) 的节奏发 total_count 个包，序列号从 0 递增。
 */
int cmd_ble_test_tx(int argc, char **argv)
{
    struct bt_le_adv_param param;
    uint16_t total;
    uint8_t  mfg_data[TEST_MFG_DATA_LEN];

    if (argc < 2) {
        printf("Usage: ble_test_tx <total_count>\r\n");
        return -1;
    }
    total = (uint16_t)strtoul(argv[1], NULL, 0);

    /* 不可连接、不可扫描的纯广播(ADV_NONCONN_IND)，间隔取最小 20ms */
    memset(&param, 0, sizeof(param));
    param.id           = BT_ID_DEFAULT;
    param.interval_min = 0x0020;
    param.interval_max = 0x0020;
    param.options      = BT_LE_ADV_OPT_NONE;

    printf("[BLE_TEST] INFO type=tx start total=%u mfg_id=0x%04X\r\n",
           total, TEST_MFG_ID);

    /* 先停掉可能残留的广播（如上次测试断开后自动重广播留下的状态），
     * 否则第一包 bt_le_adv_start 会因广播已开返回 -EALREADY 直接失败。
     * 命令自洽：不依赖板子初始状态。 */
    bt_le_adv_stop();

    for (uint16_t seq = 0; seq < total; seq++) {
        mfg_data[0] = TEST_MFG_ID & 0xFF;        /* 厂商ID低字节 */
        mfg_data[1] = (TEST_MFG_ID >> 8) & 0xFF; /* 厂商ID高字节 */
        mfg_data[2] = (seq >> 8) & 0xFF;         /* 序列号高字节 */
        mfg_data[3] = seq & 0xFF;                /* 序列号低字节 */

        struct bt_data ad[] = {
            BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
            BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, sizeof(mfg_data)),
        };

        int err = bt_le_adv_start(&param, ad, ARRAY_SIZE(ad), NULL, 0);
        if (err) {
            printf("[BLE_TEST] RESULT type=tx status=error seq=%u err=%d\r\n",
                   seq, err);
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(TX_ADV_ON_MS));
        bt_le_adv_stop();

        /* 控制节奏：每包 25ms（已广播 5ms，再等 20ms） */
        if (seq + 1 < total) {
            vTaskDelay(pdMS_TO_TICKS(TX_PKT_INTERVAL_MS - TX_ADV_ON_MS));
        }
    }

    printf("[BLE_TEST] RESULT type=tx status=ok sent=%u\r\n", total);
    return 0;
}

/****************************************************************************
 * 连接测试：Master / Slave
 *
 * 由 port 文件的 connected/disconnected 回调调用下面的 hook 函数。
 ****************************************************************************/

/* 由 ble_connected 回调调用：判断成功/失败并推进测试 */
void ble_test_on_connected(struct bt_conn *conn, uint8_t err)
{
    if (!g_test.conn_running) {
        return;
    }

    if (err) {
        /* 连接失败（通常是超时 0x3E），Master 端记一次失败并触发下一轮 */
        if (g_test.conn_role == TEST_ROLE_MASTER) {
            xSemaphoreGive(g_test.conn_evt_sem);
        }
        return;
    }

    g_test.conn_success++;

    if (g_test.conn_role == TEST_ROLE_MASTER) {
        /* Master 特殊设置：连上立刻断开，腾出连接给下一次。
         * 不能在回调上下文直接调 bt_conn_disconnect（会 bt_assert 崩溃），
         * 派发到工作任务执行。推进下一轮放在 disconnected 回调。 */
        test_work_post(WORK_DISCONNECT, conn);
    }
    /* Slave：成功被连，等对端断开；统计已在上面累加 */
}

/* 由 ble_disconnected 回调调用。
 * 返回 true 表示测试已接管广播控制，port 应跳过默认重广播。 */
bool ble_test_on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    if (!g_test.conn_running) {
        return false; /* 非测试态，走 port 默认重广播 */
    }

    if (g_test.conn_role == TEST_ROLE_MASTER) {
        /* Master：本轮结束，触发下一轮发起连接（信号量在回调上下文 give 是安全的）。
         * 不重启广播（DUT 作主机，开广播无意义且可能干扰扫描）。 */
        xSemaphoreGive(g_test.conn_evt_sem);
        return true;
    } else if (g_test.conn_role == TEST_ROLE_SLAVE) {
        /* Slave 特殊设置：断开后自动重启广播，等待对端再连。
         * 不能在回调上下文直接调 set_adv_enable（会崩溃），派发到工作任务。 */
        test_work_post(WORK_RESTART_ADV, NULL);
        return true;
    }
    return false;
}

/* Master 发起一次连接。返回 0 成功发起，-1 失败。 */
static int master_create_one(void)
{
    struct bt_le_conn_param param = {
        .interval_min = BT_GAP_INIT_CONN_INT_MIN,
        .interval_max = BT_GAP_INIT_CONN_INT_MAX,
        .latency      = 0,
        .timeout      = 400,
    };

    struct bt_conn *conn = bt_conn_create_le(&g_test.conn_peer, &param);
    if (!conn) {
        return -1;
    }
    /* 注意：master 连接的引用由协议栈在 notify_disconnected 时自动 unref，
     * 这里绝不能再 bt_conn_unref，否则引用计数 underflow 触发 bt_assert 崩溃。
     * 与标准 blecli_connect 的做法保持一致（create 后不 unref）。 */
    return 0;
}

/* ble_test_conn_master <addr_type> <mac> <count>
 *   循环 count 次发起连接，统计成功率。阻塞至全部完成。
 */
int cmd_ble_test_conn_master(int argc, char **argv)
{
    uint8_t addr_type;
    uint8_t mac_le[6];

    if (argc < 4) {
        printf("Usage: ble_test_conn_master <addr_type> <mac> <count>\r\n");
        printf("  addr_type: 0=public 1=random\r\n");
        printf("  mac: e.g. 112233AABBCC\r\n");
        return -1;
    }
    addr_type = (uint8_t)strtoul(argv[1], NULL, 0);
    if (parse_mac(argv[2], mac_le) != 0) {
        printf("[BLE_TEST] RESULT type=conn_master status=error reason=bad_mac\r\n");
        return -1;
    }

    g_test.conn_peer.type = addr_type;
    memcpy(g_test.conn_peer.a.val, mac_le, 6);
    g_test.conn_target  = (uint16_t)strtoul(argv[3], NULL, 0);
    g_test.conn_attempt = 0;
    g_test.conn_success = 0;
    g_test.conn_role    = TEST_ROLE_MASTER;
    g_test.conn_running = true;

    /* 清空可能残留的信号 */
    while (xSemaphoreTake(g_test.conn_evt_sem, 0) == pdTRUE) {
    }

    printf("[BLE_TEST] INFO type=conn_master start target=%u\r\n",
           g_test.conn_target);

    for (uint16_t i = 0; i < g_test.conn_target; i++) {
        g_test.conn_attempt++;

        if (master_create_one() != 0) {
            /* 发起失败（例如上次连接未释放），等一会继续下一轮 */
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        /* 等待本轮结束事件：成功(连上→断开) 或 失败(超时)。
         * 连接超时 400*10ms=4s，留 6s 余量。 */
        if (xSemaphoreTake(g_test.conn_evt_sem, pdMS_TO_TICKS(6000)) != pdTRUE) {
            /* 兜底：本轮卡死（连接事件始终没来），间隔后继续下一轮 */
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

    g_test.conn_running = false;
    g_test.conn_role    = TEST_ROLE_NONE;

    uint32_t rate = g_test.conn_target ?
        (g_test.conn_success * 100u) / g_test.conn_target : 0;
    printf("[BLE_TEST] RESULT type=conn_master status=ok attempt=%u success=%u "
           "target=%u rate=%lu\r\n",
           g_test.conn_attempt, g_test.conn_success, g_test.conn_target,
           (unsigned long)rate);
    return 0;
}

/* ble_test_conn_slave <count>
 *   作为从机广播等待被连接，断开自动重广播，统计被连成功次数。
 *   非阻塞启动；用 ble_test_conn_stats 查看进度，ble_test_conn_stop 结束。
 */
int cmd_ble_test_conn_slave(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: ble_test_conn_slave <count>\r\n");
        return -1;
    }
    g_test.conn_target  = (uint16_t)strtoul(argv[1], NULL, 0);
    g_test.conn_attempt = 0;
    g_test.conn_success = 0;
    g_test.conn_role    = TEST_ROLE_SLAVE;
    g_test.conn_running = true;

    /* 确保可连接广播在运行。被连断开后由 ble_test_on_disconnected 派发
     * 工作任务调 set_adv_enable(true) 续广播。
     * 前置要求：先用 ble_start_adv 0 0 配好可连接广播数据。
     * 注意：blestack 用自带 bt_errno.h，其中 EALREADY=69（非标准 libc 的 120）。
     * 返回 -69(-EALREADY) 表示广播已在运行，属正常，忽略。 */
    #define BT_ERR_EALREADY (-69)
    int err = set_adv_enable(true);
    if (err && err != BT_ERR_EALREADY) {
        g_test.conn_running = false;
        printf("[BLE_TEST] RESULT type=conn_slave status=error err=%d "
               "(run 'ble_start_adv 0 0' first)\r\n", err);
        return -1;
    }

    printf("[BLE_TEST] INFO type=conn_slave start target=%u\r\n",
           g_test.conn_target);
    return 0;
}

/* ble_test_conn_stats —— 查看连接测试当前统计 */
int cmd_ble_test_conn_stats(int argc, char **argv)
{
    const char *role = (g_test.conn_role == TEST_ROLE_MASTER) ? "master" :
                       (g_test.conn_role == TEST_ROLE_SLAVE)  ? "slave" : "none";
    uint32_t rate = g_test.conn_target ?
        (g_test.conn_success * 100u) / g_test.conn_target : 0;
    printf("[BLE_TEST] RESULT type=conn_stats role=%s success=%u target=%u "
           "rate=%lu running=%d\r\n",
           role, g_test.conn_success, g_test.conn_target,
           (unsigned long)rate, g_test.conn_running ? 1 : 0);
    return 0;
}

/* ble_test_conn_stop —— 结束 Slave 连接测试并输出最终结果 */
int cmd_ble_test_conn_stop(int argc, char **argv)
{
    g_test.conn_running = false;
    g_test.conn_role    = TEST_ROLE_NONE;
    bt_le_adv_stop();

    uint32_t rate = g_test.conn_target ?
        (g_test.conn_success * 100u) / g_test.conn_target : 0;
    printf("[BLE_TEST] RESULT type=conn_slave status=ok success=%u target=%u "
           "rate=%lu\r\n",
           g_test.conn_success, g_test.conn_target, (unsigned long)rate);
    return 0;
}

/****************************************************************************
 * 初始化 & 命令注册
 ****************************************************************************/

void ble_test_port_init(void)
{
    if (g_test.conn_evt_sem == NULL) {
        g_test.conn_evt_sem = xSemaphoreCreateBinary();
    }
    if (g_test.work_queue == NULL) {
        g_test.work_queue = xQueueCreate(8, sizeof(struct test_work_item));
    }
    if (g_test.work_task == NULL) {
        xTaskCreate(test_work_task, "ble_test_work", 512, NULL,
                    configMAX_PRIORITIES - 3, &g_test.work_task);
    }
}

SHELL_CMD_EXPORT_ALIAS(cmd_ble_test_rx, ble_test_rx,
    ble_test_rx duration_ms expect [interval] [window] : scan and count matched adv packets. units 0.625ms def 0x40/0x20);
SHELL_CMD_EXPORT_ALIAS(cmd_ble_test_tx, ble_test_tx,
    ble_test_tx total : send test adv packets 20pkt per 500ms);
SHELL_CMD_EXPORT_ALIAS(cmd_ble_test_conn_master, ble_test_conn_master,
    ble_test_conn_master addr_type mac count : master connect success rate);
SHELL_CMD_EXPORT_ALIAS(cmd_ble_test_conn_slave, ble_test_conn_slave,
    ble_test_conn_slave count : slave connect success rate);
SHELL_CMD_EXPORT_ALIAS(cmd_ble_test_conn_stats, ble_test_conn_stats,
    ble_test_conn_stats : show connect test stats);
SHELL_CMD_EXPORT_ALIAS(cmd_ble_test_conn_stop, ble_test_conn_stop,
    ble_test_conn_stop : stop slave connect test and print result);
SHELL_CMD_EXPORT_ALIAS(cmd_ble_test_rx_start, ble_test_rx_start,
    ble_test_rx_start [interval] [window] : start async bg scan and return at once. units 0.625ms def 0x40/0x20);
SHELL_CMD_EXPORT_ALIAS(cmd_ble_test_rx_stat, ble_test_rx_stat,
    ble_test_rx_stat : print current rx total/unique without stopping scan);
SHELL_CMD_EXPORT_ALIAS(cmd_ble_test_rx_stop, ble_test_rx_stop,
    ble_test_rx_stop [expect] : stop async scan and print result);
