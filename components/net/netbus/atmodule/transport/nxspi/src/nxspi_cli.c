#include <stdio.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <mem.h>

#include "bflb_clock.h"
#include "bflb_mtimer.h"
#include "bflb_timer.h"
#include <bflb_gpio.h>
#include <bflb_core.h>
#include <nxspi.h>
#include <nxspi_net.h>
#include <nxspi_log.h>

#include <shell.h>

#define DBG_TAG "NXSPI_CLI"
#include <log.h>

extern char *dn_buf;
extern char *up_buf;
extern spi_header_t dn_header;
extern spi_header_t up_header;
extern nxspi_desc_t g_nxspi;

extern struct bflb_device_s *sgpio;

volatile uint32_t g_real_len = 0;

int cmd_nxspicli_test(int argc, char **argv)
{
    LOG_I("Starting cmd_nxspicli_test ...\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_nxspicli_test, nxspicli, nxspicli test.);

int cmd_nxspiwrite_test(int argc, char **argv)
{
    int res;
    uint8_t *buf;

    buf = malloc(NXBD_MTU);
    if (!buf) {
        NX_LOGE("mem err\r\n");
        return -1;
    }

    memset(buf, 0x5F, NXBD_MTU);

    res = nxspi_write(NXSPI_TYPE_AT, buf, NXBD_MTU, portMAX_DELAY);
    if (res != NXBD_MTU) {
        NX_LOGE("res:%d, real:%d\r\n", res, NXBD_MTU);
    }
    free(buf);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_nxspiwrite_test, nxw, nxspi write.);

int nxspiwrite_variable_entry(void *arg)
{
    int res;
    uint8_t *buf;

    buf = malloc(NXBD_MTU);
    if (!buf) {
        NX_LOGE("mem err\r\n");
        return -1;
    }

    for (int m = 0; m < 600; m++) {
        for (int i = 1; i <= NXBD_MTU; i++) {
            memset(buf, i%256, i);
            res = nxspi_write(NXSPI_TYPE_AT, buf, i, portMAX_DELAY);
            if (res != i) {
                NX_LOGE("res:%d, real:%d\r\n", res, i);
            }
        }
    }
    free(buf);

    vTaskDelete(NULL);
}
int cmd_nxspiwrite_variable_test(int argc, char **argv)
{
    int ret;
    ret = xTaskCreate(nxspiwrite_variable_entry,
                (char*)"nxr_variable",
                4096/sizeof(StackType_t), NULL,
                30,
                NULL);
    if (ret != pdPASS) {
        NX_LOGE("failed to create nx write variable task, %ld\r\n", ret);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_nxspiwrite_variable_test, nxw_variable, nxspi write.);

void nx_wm_task(void *arg)
{
    int res;
    uint8_t *buf;

    NX_LOGI("Task start\r\n");
    buf = malloc(NXBD_MTU);
    if (!buf) {
        NX_LOGE("mem err\r\n");
        return -1;
    }

    memset(buf, 0xF, NXBD_MTU);
    while (1) {
        //vTaskDelay(3);
        res = nxspi_write(NXSPI_TYPE_AT, buf, NXBD_MTU, portMAX_DELAY);
        NX_LOGD("app write :%d Bytes\r\n", res);
    }
    free(buf);
}

int cmd_nxspiwritem_test(int argc, char **argv)
{
    int ret;
    ret = xTaskCreate(nx_wm_task,
                (char*)"nxwm",
                2048/sizeof(StackType_t), NULL,
                30,
                NULL);
    if (ret != pdPASS) {
        NX_LOGE("failed to create nx read task, %ld\r\n", ret);
    }
}

SHELL_CMD_EXPORT_ALIAS(cmd_nxspiwritem_test, nxwm, nxspi write mul.);

int nxspiread_variable_entry(void *arg)
{
    int res;
    uint8_t *buf;
    uint8_t flag = 0;

    buf = malloc(NXBD_MTU);
    if (!buf) {
        NX_LOGE("mem err\r\n");
        return -1;
    }

    NX_LOGI("read variable start.\r\n");
    for (int m = 0; m < 600; m++) {
        for (int i = 1; i <= NXBD_MTU; i++) {
            g_real_len =  i;
            res = nxspi_read(NXSPI_TYPE_AT, buf, NXBD_MTU, portMAX_DELAY);

            if (res != i) {
                NX_LOGE("res:%d, error\r\n", res);
                break;
            }
            // add here
            flag = 0;
            for (int j = 0; j < res; j++) {
                if (buf[j] != (uint8_t)(i % 256)) {
                    NX_LOGE("m:%d, i:%d, buf[%d]:%d, expected:%d\r\n",
                            m, i, j, buf[j], (uint8_t)(i % 256));
                    flag = 1;
                    break;
                }
            }
            if (flag) {
                vTaskDelay(1000);
                printf("recv:\r\n");
                for (int j = 0; j < res; j++) {
                    if (j%64 == 0) {
                        printf("\r\n");
                    }
                    printf(" %02X", buf[j]);
                }
                printf("\r\n");
                vTaskDelay(1000);
                break;
            }
        }
        if (flag) {
            break;
        }
    }
    NX_LOGI("read variable end.\r\n");

    free(buf);

    vTaskDelete(NULL);
}
int cmd_nxspiread_variable_test(int argc, char **argv)
{
    int ret;
    ret = xTaskCreate(nxspiread_variable_entry,
                (char*)"nxr_variable",
                4096/sizeof(StackType_t), NULL,
                30,
                NULL);
    if (ret != pdPASS) {
        NX_LOGE("failed to create nx read variable task, %ld\r\n", ret);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_nxspiread_variable_test, nxr_variable, nxspi read.);

int cmd_nxspiread_test(int argc, char **argv)
{
    int res;
    uint8_t *buf;

    buf = malloc(NXBD_MTU);
    if (!buf) {
        NX_LOGE("mem err\r\n");
        return -1;
    }

    res = nxspi_read(NXSPI_TYPE_AT, buf, NXBD_MTU, 10000);

    if (res > 0) {
        printf("res[%d]: ", res);
        for (int i = 0; i < res; i++) {
            printf(" %02X", buf[i]);
            if (0x55 != buf[i]) {
                NX_LOGE("err[%d]:0x%02X\r\n", i, buf[i]);
            }
        }
        printf("\r\n");
    } else {
        printf("read NULL, res:%d\r\n", res);
    }

    free(buf);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_nxspiread_test, nxr, nxspi read.);

void nx_rm_task(void *arg)
{
    int res;
    uint8_t *buf;

    NX_LOGI("Task start\r\n");
    buf = malloc(NXBD_MTU);
    if (!buf) {
        NX_LOGE("mem err\r\n");
        return -1;
    }

    while (1) {
        res = nxspi_read(NXSPI_TYPE_AT, buf, NXBD_MTU, portMAX_DELAY);

        if (res > 0) {
            //NX_LOGI("recv[%d]: \r\n", res);
#if 0
            for (int i = 0; i < res; i++) {
                //printf(" %02X", buf[i]);
                if (0x55 != buf[i]) {
                    NX_LOGE("err[%d]:0x%02X\r\n", i, buf[i]);
                    break;
                }
            }
#endif
        } else {
            NX_LOGE("read NULL, res:%d\r\n", res);
        }
    }

    free(buf);
    vTaskDelete(NULL);
}

int cmd_nxspireadm_test(int argc, char **argv)
{
    int ret;
    ret = xTaskCreate(nx_rm_task,
                (char*)"nxrm",
                2048/sizeof(StackType_t), NULL,
                30,
                NULL);
    if (ret != pdPASS) {
        NX_LOGE("failed to create nx read task, %ld\r\n", ret);
    }
}

SHELL_CMD_EXPORT_ALIAS(cmd_nxspireadm_test, nxrm, nxspi read mul.);
int cmd_nxspiinit_test(int argc, char **argv)
{
    nxspi_init();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_nxspiinit_test, nxi, nxspi init.);

int cmd_nxgpio(int argc, char *argv[])
{
    int pin;
    int val;
    if (argc != 3) {
        printf("Usage: nxgpio [pin] [1/0]\n");
        return -1;
    }

    pin = atoi(argv[1]);
    val = atoi(argv[2]);

    if (1 == val) {
        nxspi_hwgpio_high(pin);
    } else if (0 == val) {
        nxspi_hwgpio_low(pin);
    } else {
        printf("arg err\r\n");
        return -1;
    }
    printf("gpio set pin:%d, %s\r\n", pin, (val==0)?"low":"high");
    printf("gpio get pin:%d, %s\r\n", pin, (nxspi_hwgpio_status(pin)==0)?"low":"high");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_nxgpio, nxg, Control GPIO pin to high or low);

#define INTERVAL_MS 1000
int nx_iperf_task(void *arg)
{
    int res;
    uint8_t *buf;
    TickType_t current_time;
    TickType_t last_time;
    uint32_t total_bytes = 0;

    buf = malloc(NXBD_MTU);
    if (!buf) {
        NX_LOGE("mem err\r\n");
        return -1;
    }

    last_time = xTaskGetTickCount();
    while (1) {
        //xTaskGetTickCount();
        res = nxspi_read(NXSPI_TYPE_AT, buf, NXBD_MTU, 100);
        //NX_LOGD("res:%d\r\n", res);
        if (res > 0) {
            NX_LOGD("res:%d\r\n", res);
            total_bytes += res;
            // Get the current time
            current_time = xTaskGetTickCount();

            // Check if 1 second has passed (INTERVAL_MS / portTICK_PERIOD_MS converts milliseconds to ticks)
            if ((current_time - last_time) * portTICK_PERIOD_MS >= INTERVAL_MS) {
                // Calculate throughput: total bytes in 1 second
                uint32_t throughput = total_bytes / (INTERVAL_MS / 1000);  // bytes per second
                NX_LOGI("Throughput: %u bytes/sec\r\n", throughput);

                // Reset for next interval
                total_bytes = 0;
                last_time = current_time;
            }
        }
    }

    return 0;
}
int cmd_nx_perf(int argc, char **argv)
{
    int ret;
    ret = xTaskCreate(nx_iperf_task,
                (char*)"nxperf",
                2048/sizeof(StackType_t), NULL,
                30,
                NULL);
    if (ret != pdPASS) {
        NX_LOGE("failed to create nxperf task, %ld\r\n", ret);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_nx_perf, nx_perf, nxperf...);


int cmd_nxperf(int argc, char **argv)
{
    uint32_t timeout = 1000; // 100ms
    if (argc < 3) {
        printf("Usage: nx_perf [-t duration] [-R for read] [-i interval] [-s packetsize]\r\n"
               "eg: nxperf -t 10 -i 1 -s 1536\r\n"
               "eg: nxperf -t 10 -i 1 -R\r\n");
        return -1;
    }

    uint32_t duration = 10; // Default duration of 10 seconds
    int is_read = 0;
    uint32_t interval = 1; // Default interval of 1 second for printing throughput
    uint32_t packet_size = 1536; // Default packet size for writing is 1K
    uint32_t read_length = 1536; // Default read length is 2K

    // Parse additional arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            duration = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-R") == 0) {
            is_read = 1;
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            interval = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            packet_size = atoi(argv[++i]);
        }
    }

    uint8_t *buf = malloc(is_read ? read_length : packet_size);
    if (!buf) {
        printf("Memory allocation error.\r\n");
        return -1;
    }

    // Initialize buffer with dummy data for write
    if (!is_read) {
        memset(buf, 'A', packet_size);
    }

    TickType_t start_time = xTaskGetTickCount();
    uint32_t total_bytes = 0;

    while ((xTaskGetTickCount() - start_time) / configTICK_RATE_HZ < duration) {
        TickType_t current_time = xTaskGetTickCount();
        uint32_t interval_bytes = 0;
        TickType_t interval_start_time = current_time;

        while ((xTaskGetTickCount() - interval_start_time) / configTICK_RATE_HZ < interval) {
            int result;

            if (is_read) {
                // Perform a read operation
                result = nxspi_read(NXSPI_TYPE_AT, buf, read_length, timeout);
            } else {
                // Perform a write operation
                result = nxspi_write(NXSPI_TYPE_AT, buf, packet_size, timeout);
            }

            // Log throughput information for debugging
            NX_LOGI("[%d] packet_size:%d, interval_bytes:%d, total_bytes:%d\r\n",
                          xTaskGetTickCount(), packet_size, interval_bytes, total_bytes);

            if (result < 0) {
                printf("Error %s to SPI sync result:%d.\r\n", is_read ? "reading" : "writing", result);
                free(buf);
                return -1;
            }

            if (result > 0) {
                interval_bytes += is_read ? read_length : packet_size;
                total_bytes += is_read ? read_length : packet_size;
            }

            current_time = xTaskGetTickCount();
        }

        double elapsed_interval = (double)(xTaskGetTickCount() - interval_start_time) / configTICK_RATE_HZ;
        uint32_t end_sec = (xTaskGetTickCount() - start_time) / configTICK_RATE_HZ;
        uint32_t start_sec = end_sec - interval;
        printf("Interval [%u-%u] sec: %.2f bytes/sec, %.2f bps\r\n", start_sec, end_sec,
               interval_bytes / elapsed_interval, (interval_bytes / elapsed_interval) * 8);
    }

    double elapsed_time = (double)(xTaskGetTickCount() - start_time) / configTICK_RATE_HZ;
    printf("Test completed in %.2f seconds.\r\n", elapsed_time);
    printf("Total bytes %s: %u\r\n", is_read ? "read" : "written", total_bytes);
    printf("Average throughput: %.2f bytes/sec, %.2f bps\r\n", total_bytes / elapsed_time,
           ((total_bytes / elapsed_time) * 8));

    free(buf);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_nxperf, nxperf, nxperf [-t duration] [-R for read] [-i interval] [-s packetsize]);

#if GPIO_TIME_ENABLE
// Function to print the current statistics
void nx_print_stats(const nx_stats_t *stats)
{
    printf("Total Count: %d\r\n", stats->count);
    printf("Average: %.2f\r\n", stats->average);

    printf("Top %d Values: ", NX_MAX_TOP);
    for (int i = 0; i < NX_MAX_TOP; i++) {
        printf(" %lld", stats->top[i]);
    }
    printf("\r\n");

    printf("Bottom %d Values: ", NX_MAX_BOTTOM);
    for (int i = 0; i < NX_MAX_BOTTOM; i++) {
        printf(" %lld", stats->bottom[i]);
    }
    printf("\r\n");
}
#endif

extern uint8_t nx_gpio_trigmode_get(struct bflb_device_s *dev, uint8_t pin);
int cmd_nx(int argc, char *argv[])
{
    /* reg cfg */
    NX_LOGP("[%ld] flash clock:%d\r\n", nx_tick(), bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_FLASH, 0));

    NX_LOGP("CLK-%d,MISO-%d,MOSI-%d,CS-%d,IRQ:%d\r\n",
            NXSPI_GPIO_CLK, NXSPI_GPIO_MISO, NXSPI_GPIO_MOSI,
            NXSPI_GPIO_CS, NXSPI_GPIO_IRQ);
    NX_LOGP("IRQ pin-%3d, level-%d, reg:%p\r\n",
            NXSPI_GPIO_IRQ, nxspi_hwgpio_status(NXSPI_GPIO_IRQ),
            *(volatile uint32_t *)(0x200008C4 + NXSPI_GPIO_IRQ*4));
    NX_LOGP("CS  pin-%3d, level-%d, reg:%p\r\n",
            NXSPI_GPIO_CS, nxspi_hwgpio_status(NXSPI_GPIO_CS),
            *(volatile uint32_t *)(0x200008C4 + NXSPI_GPIO_CS*4));

    NX_LOGP("DMA Tx ch0 S:0x%08X, D:0x%08X, tfsize:0x%08X-%d, C0CFG:%p\r\n",
            *(volatile uint32_t *)0x2000C100, *(volatile uint32_t *)0x2000C104, *(volatile uint32_t *)0x2000C10C, ((*(volatile uint32_t *)0x2000C10C)&4095),
            *(volatile uint32_t *)0x2000C110);
    NX_LOGP("DMA Rx ch1 S:0x%08X, D:0x%08X, tfsize:0x%08X-%d, C0CFG:%p\r\n",
            *(volatile uint32_t *)0x2000C200, *(volatile uint32_t *)0x2000C204, *(volatile uint32_t *)0x2000C20C, ((*(volatile uint32_t *)0x2000C20C)&4095),
            *(volatile uint32_t *)0x2000C210);
    NX_LOGP("SPI cfg0:0x%08X, cfg1:0x%08X fifo_avail_B rx:%d-tx:%d\r\n",
            *(volatile uint32_t *)0x2000A280,
            *(volatile uint32_t *)0x2000A284, (*(volatile uint32_t *)0x2000A284)&0x3F00,  (*(volatile uint32_t *)0x2000A284)&0x3F);

    /* val */
#if 1
    NX_LOGP("dn_header:%p,magic:%04X,l:0x%04X(%d),v:%d,f-%d,t:%d,rsvd:0x%04x\r\n",
            &dn_header, dn_header.magic, dn_header.len, dn_header.len, dn_header.version,
            dn_header.flags, dn_header.type, dn_header.rsvd);
    NX_LOGP("up_header:%p,magic:%04X,l:0x%04X(%d),v:%d,f-%d,t:%d,rsvd:0x%04x\r\n",
            &up_header, up_header.magic, up_header.len, up_header.len, up_header.version,
            up_header.flags, up_header.type, up_header.rsvd);
#else
    NX_LOGP("dn_header:%p,l:0x%04X(%d),inv:0x%04X,v:%d,r-%d,f-%d,t:%d,rsvd:0x%04x\r\n",
            &dn_header, dn_header.len, dn_header.len, dn_header.len_inv, dn_header.version,
            dn_header.reset, dn_header.flags, dn_header.type, dn_header.rsvd);
    NX_LOGP("up_header:%p,l:0x%04X(%d),inv:0x%04X,v:%d,r-%d,f-%d,t:%d,rsvd:0x%04x\r\n",
            &up_header, up_header.len, up_header.len, up_header.len_inv, up_header.version,
            up_header.reset, up_header.flags, up_header.type, up_header.rsvd);
#endif

    NX_LOGP("dn (at+net):(%d+%d), fq:%d, dnmsg:%p, buf:%d*%d",
            uxQueueMessagesWaiting(g_nxspi.dnat),
            uxQueueMessagesWaiting(g_nxspi.dnnet),
            uxQueueMessagesWaiting(g_nxspi.dnfq),
            g_nxspi.dnmsg,
            NXBD_ITEMS, NXBD_MTU);
    for (int i= 0; i < NXBD_ITEMS; i++) {
        NX_LOGP(" %p", dn_buf+i*NXBD_MTU);
    }
    NX_LOGP("\r\n");

    NX_LOGP("up vq:%d, fq:%d, upmsg:%p, buf:%d*%d",
            uxQueueMessagesWaiting(g_nxspi.upvq),
            uxQueueMessagesWaiting(g_nxspi.upfq),
            g_nxspi.upmsg,
            NXBD_ITEMS, NXBD_MTU);
    for (int i= 0; i < NXBD_ITEMS; i++) {
        NX_LOGP(" %p", up_buf+i*NXBD_MTU);
    }
    NX_LOGP("\r\n");

    uint8_t mode = nx_gpio_trigmode_get(sgpio, NXSPI_GPIO_CS);
    NX_LOGP("statemchine:%s (Config->started->hdreceived->complete)\r\n", get_smstr(g_nxspi.sm));
    NX_LOGP("cs mode :%d:%s\r\n", mode, get_gpiomodestr(mode));

    NX_LOGP("cs_enter_cnt         :%d\r\n", g_nxspi.cs_enter_cnt);
    NX_LOGP("ts_start_cnt         :%d\r\n", g_nxspi.ts_start_cnt);
    NX_LOGP("ts_complete_cnt      :%d\r\n", g_nxspi.ts_complete_cnt);

    /* debug */
#if NXSPI_DEBUG
    NX_LOGP("nxspi write cnt:%d, bytes:%d\r\n", g_nxspi.write_cnt, g_nxspi.write_bytes);
    NX_LOGP("nxspi read  cnt:%d, bytes:%d\r\n", g_nxspi.read_cnt, g_nxspi.read_bytes);
    NX_LOGP("ps entercnt:%d, exitcnt:%d\r\n", g_nxspi.ps_entercnt, g_nxspi.ps_exitcnt);
    NX_LOGP("dn rx_stall_cnt:%d\r\n", g_nxspi.rx_stall_cnt);
    NX_LOGP("dn discard cnt:%d, bytes:%d\r\n", g_nxspi.discard_cnt, g_nxspi.discard_bytes);
    NX_LOGP("state machine start2complete_cnt:%d\r\n", g_nxspi.start2complete_cnt);
#endif

    int _bdreceived(void);
    printf("bdreceived:%d\r\n", _bdreceived());

    printf("use:%lu us, start:%llu, end:%llu\r\n",
            g_nxspi.cfg_usetime, g_nxspi.cfg_starttime, g_nxspi.cfg_endtime);
    printf("time start:%d, isr:%d\r\n", g_nxspi.time_start_cnt, g_nxspi.time_isr_cnt);
#if 0
    NX_LOGP("tfsize_after_start   :%d\r\n", g_nxspi.tfsize_after_start);
    NX_LOGP("tfsize_received_hd   :%d\r\n", g_nxspi.tfsize_received_hd);
    NX_LOGP("tfsize_complete      :%d\r\n", g_nxspi.tfsize_complete);
    NX_LOGP("dst_after_start      :%p\r\n", g_nxspi.dst_after_start);
    NX_LOGP("dst_received_hd      :%p\r\n", g_nxspi.dst_received_hd);
    NX_LOGP("dst_complete         :%p\r\n", g_nxspi.dst_complete);
#endif
#if GPIO_TIME_ENABLE
    nx_print_stats(&g_nxspi.stats);
#endif

#ifdef NXSPI_NET
    {
    extern spinet_t g_spinet;
    NX_LOGP("net upld vq:%d  total:%d\r\n",
            uxQueueMessagesWaiting(g_spinet.upvq),
            NXBD_UPLD_ITEMS);
    NX_LOGP("net dnld fq:%d  total:%d\r\n",
            uxQueueMessagesWaiting(g_spinet.dnfq),
            NXBD_DNLD_ITEMS);
    NX_LOGP("spinet write cnt:%d, bytes:%d\r\n", g_spinet.write_cnt, g_spinet.write_bytes);
    NX_LOGP("spinet read  cnt:%d, bytes:%d\r\n", g_spinet.read_cnt, g_spinet.read_bytes);
    NX_LOGP("spinet net stream: to %s\r\n", (g_spinet.netstream==SPINET_NETSTREAM_TO_LOCAL)?"local":"host");
    NX_LOGP("spinet pktcnt local:%d, host:%d, oth:%d\r\n",
            g_spinet.local_pktcnt, g_spinet.host_pktcnt, g_spinet.oth_pktcnt);
    }
#endif

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_nx, nx, nx status);

int cmd_nxsm(int argc, char *argv[])
{
    void nxspi_state_machine(void);
    nxspi_state_machine();
}
SHELL_CMD_EXPORT_ALIAS(cmd_nxsm, nxsm, nxsm status);

void nx_app_task(void *arg)
{
    vTaskDelay(100);

    //cmd_nxspiinit_test(0, NULL);
    //cmd_nxspicli_start(0, NULL);
    //cmd_nx_perf(0, NULL);
    while(1) {
        vTaskDelay(100);
    }
}

void app_nxspicli_init(void)
{
    nxspi_init();
#if 0
    int ret;

    ret = xTaskCreate(nx_app_task,
                (char*)"nxapp",
                2048/sizeof(StackType_t), NULL,
                30,
                NULL);
    if (ret != pdPASS) {
        NX_LOGE("failed to create nxapp task, %ld\r\n", ret);
    }
#endif
}

#ifdef NXSPI_NET
int cmd_netstream(int argc, char **argv)
{
    extern spinet_t g_spinet;

    if (argc != 2) {
        printf("Usage: netstream [1|2]\n");
        return -1;
    }

    if (strcmp(argv[1], "local") == 0) {
        g_spinet.netstream = SPINET_NETSTREAM_TO_LOCAL;
        printf("to local\r\n");
    } else if (strcmp(argv[1], "host") == 0) {
        g_spinet.netstream = SPINET_NETSTREAM_TO_HOST;
        printf("to host\r\n");
    } else {
        printf("Invalid argument. Use 1 or 2.\n");
        return -1;
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_netstream, netstream, netstream command to ctrl to host or local);
#endif

