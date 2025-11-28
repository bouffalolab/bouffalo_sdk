
/**
 * @file hd_speed_test.c
 * @brief Host Driver SDIO Speed Test Implementation
 * @author Host Driver Development Team
 * @date 2024
 * @details This module implements performance testing functionality for SDIO
 *          communication interfaces. It provides throughput measurement,
 *          latency analysis, and stress testing capabilities for both
 *          upload and download operations.
 */

#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/jiffies.h>

#include "hd_debugfs.h"
#include "hd_msg_ctrl.h"
#include "hd_speed_test.h"

/**
 * @brief Uninitialize speed test buffers
 * @param[in] speed_test Pointer to speed test structure
 * @retval 0 Success
 * @details This function releases all allocated test buffers including
 *          both download and upload test buffers.
 */
static int hd_speed_test_buff_uninit(struct hd_speed_test *speed_test)
{
    int32_t i;

    /* Free all test buffers */
    for (i = 0; i < HD_SPEED_TEST_BUFF_CNT; i++) {
        if (speed_test->dnld_test_buff[i] != NULL) {
            kfree(speed_test->dnld_test_buff[i]);
            speed_test->dnld_test_buff[i] = NULL;
        }
        if (speed_test->upld_test_buff[i] != NULL) {
            kfree(speed_test->upld_test_buff[i]);
            speed_test->upld_test_buff[i] = NULL;
        }
    }
    return 0;
}

/**
 * @brief Initialize speed test buffers
 * @param[in] speed_test Pointer to speed test structure
 * @retval 0 Success
 * @retval -1 Error (null pointer or size constraint violation)
 * @details This function allocates and initializes test buffers for performance
 *          testing. Download buffers are formatted as message packets with
 *          test patterns. Upload buffers must be larger than download buffers
 *          to accommodate device response data.
 */
static int hd_speed_test_buff_init(struct hd_speed_test *speed_test)
{
    int32_t i, j;
    uint8_t *buff;
    struct hd_msg_packt *msg_packt;

    if (speed_test == NULL) {
        printk("hd_speed_test_buff_init: speed_test is NULL\n");
        return -1;
    }

    /* Validate buffer size constraints */
    if (speed_test->upld_max_size < speed_test->dnld_max_size) {
        printk("upld_max_size < dnld_max_size, please check!\n");
        return -1;
    }

    /* Allocate upload buffers */
    for (i = 0; i < HD_SPEED_TEST_BUFF_CNT; i++) {
        buff = kzalloc(speed_test->upld_max_size, GFP_KERNEL);
        if (buff == NULL) {
            printk("malloc error\n");
            return -1;
        }
        speed_test->upld_test_buff[i] = buff;
    }

    /* Allocate download buffers */
    for (i = 0; i < HD_SPEED_TEST_BUFF_CNT; i++) {
        buff = kzalloc(speed_test->dnld_max_size, GFP_KERNEL);
        if (buff == NULL) {
            printk("malloc error\n");
            return -1;
        }
        speed_test->dnld_test_buff[i] = buff;

#if 0
        /* Pattern fill method 1: Incremental data for timing stress test */
        for (j = 0; j < speed_test->dnld_max_size; j++) {
            speed_test->dnld_test_buff[i][j] = (j + i) & 0xff;
        }
#else
        /* Pattern fill method 2: Logic analyzer friendly waveform pattern */
        /* First and last bytes of each 512-byte block set to 0x40, others to 0 */
        for (j = 0; j < speed_test->dnld_max_size; j++) {
            if (j % 512 == 0 || j % 512 == 511) {
                speed_test->dnld_test_buff[i][j] = 0x40;
            } else {
                speed_test->dnld_test_buff[i][j] = 0x00;
            }
        }
#endif
    }

    // printk("upld_test_buff:");
    // for (i = 0; i < HD_SPEED_TEST_BUFF_CNT; i++) {
    //     printk("    upld_buff[%d] 0x%08lX\n", i, (unsigned long)speed_test->upld_test_buff[i]);
    // }

    /* Format download buffers as message packets */
    // printk("dnld_test_buff:");
    for (i = 0; i < HD_SPEED_TEST_BUFF_CNT; i++) {
        // printk("    dnld_buff[%d] 0x%8p\n", i, speed_test->dnld_test_buff[i]);

        msg_packt = (struct hd_msg_packt *)speed_test->dnld_test_buff[i];
        msg_packt->tag = speed_test->msg_tag;
        msg_packt->sub_tag = i;
        msg_packt->len = speed_test->dnld_max_size - sizeof(struct hd_msg_packt);

#if 0
        /* 测试一些特殊的长度 */
        if (i == 0) {
            msg_packt->len = 63 - sizeof(struct hd_msg_packt);
        }

        if (i == 1) {
            msg_packt->len = 100 - sizeof(struct hd_msg_packt);
        }

        if (i == 2) {
            msg_packt->len = 500 - sizeof(struct hd_msg_packt);
        }

        if (i == 3) {
            msg_packt->len = 1000 - sizeof(struct hd_msg_packt);
        }

        if (i == 4) {
            msg_packt->len = 512 * 3 - sizeof(struct hd_msg_packt);
        }

        if (i == 5) {
            msg_packt->len = 512 * 4 - sizeof(struct hd_msg_packt);
        }
#endif
    }

    return 0;
}

/**
 * @brief Perform single download data transfer
 * @param[in] speed_test Pointer to speed test structure
 * @param[in] dnld_test_buff Download test buffer
 * @retval 0 Success
 * @retval -ENOMEM Memory allocation failed
 * @retval <0 Error code from transfer operation
 * @details This function sends a single test packet through the message
 *          controller and updates transfer statistics.
 */
static int hd_speed_test_dnld(struct hd_speed_test *speed_test, uint8_t *dnld_test_buff)
{
    int ret = 0;
    struct hd_msg_packt *msg_packt = (struct hd_msg_packt *)dnld_test_buff;

    struct sk_buff *skb = dev_alloc_skb(speed_test->dnld_max_size);
    if (!skb) {
        SPEED_TEST_ERR(speed_test, "dnld_test alloc_skb error!\n");
        ret = -ENOMEM;
        goto return_out;
    }
    skb_put(skb, (msg_packt->len + sizeof(struct hd_msg_packt)));
    memcpy(skb->data, dnld_test_buff, (msg_packt->len + sizeof(struct hd_msg_packt)));

    ret = hd_msg_ctrl_send(speed_test->msg_ctrl, skb);
    if (ret < 0) {
        SPEED_TEST_ERR(speed_test, "hd_speed_test_dnld send error %d", ret);
        dev_kfree_skb_any(skb);
        goto return_out;
    }

return_out:
    return ret;
}

/**
 * @brief Download send completion callback function
 * @param[in] skb Socket buffer that was sent
 * @param[in] success Transfer success flag
 * @param[in] arg Speed test structure pointer
 * @retval 0 Success
 * @retval -EINVAL Invalid parameter
 * @details This callback is invoked when download data transmission completes.
 *          It updates download completion statistics for synchronization.
 */
static int hd_speed_test_dnld_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct hd_speed_test *speed_test = (struct hd_speed_test *)arg;

    if (!success) {
        SPEED_TEST_ERR(speed_test, "dnld_test transfer failed!\n");
        return -EIO;
    }

    if (!speed_test) {
        return -EINVAL;
    }

    if (!skb) {
        SPEED_TEST_ERR(speed_test, "dnld_test skb is NULL!\n");
        return -EINVAL;
    }

    /* Update download completion counter and total size */
    spin_lock(&speed_test->stats_lock);
    speed_test->dnld_cnt++;
    speed_test->dnld_total_size += skb->len;
    spin_unlock(&speed_test->stats_lock);

    SPEED_TEST_DBG(speed_test, "dnld_test completed: %d", speed_test->dnld_cnt);

    dev_kfree_skb_any(skb);
    return 0;
}

/**
 * @brief Upload data message callback function
 * @param[in] skb Socket buffer containing received data
 * @param[in] success Transfer success flag
 * @param[in] arg Speed test structure pointer
 * @retval 0 Success
 * @retval -EINVAL Invalid parameter
 * @details This callback is invoked when upload data is received from
 *          the device. It validates data integrity and updates statistics.
 */
static int hd_speed_test_upld_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct hd_speed_test *speed_test = (struct hd_speed_test *)arg;
    struct hd_msg_packt *msg_packt = NULL;

    if (!success) {
        SPEED_TEST_ERR(speed_test, "upld_test transfer failed!\n");
        return -EIO;
    }

    if (!speed_test) {
        return -EINVAL;
    }

    if (!speed_test->msg_ctrl) {
        SPEED_TEST_ERR(speed_test, "speed_test or msg_ctrl is NULL!\n");
        return -EINVAL;
    }

    if (!skb) {
        SPEED_TEST_ERR(speed_test, "upld_test skb is NULL!\n");
        return -EINVAL;
    }

    if (speed_test->upld_cnt >= HD_SPEED_TEST_BUFF_CNT) {
        SPEED_TEST_ERR(speed_test, "upld_test cnt is over limit: %d\n", speed_test->upld_cnt);
        return -EINVAL;
    }

    if (skb->len > speed_test->upld_max_size) {
        SPEED_TEST_ERR(speed_test, "upld_test skb len %d exceeds max size %d!\n",
                       skb->len, speed_test->upld_max_size);
        return -EINVAL;
    }

    memcpy(speed_test->upld_test_buff[speed_test->upld_cnt], skb->data, skb->len);

    msg_packt = (struct hd_msg_packt *)(speed_test->upld_test_buff[speed_test->upld_cnt]);
    if (msg_packt->sub_tag != speed_test->upld_cnt) {
        SPEED_TEST_ERR(speed_test, "upld_test sub_tag mismatch: expected=%d, received=%d\n",
                       speed_test->upld_cnt, msg_packt->sub_tag);
        /* Continue processing even with mismatch - don't return error */
    }

    spin_lock(&speed_test->stats_lock);
    speed_test->upld_cnt++;
    speed_test->upld_total_size += skb->len;
    spin_unlock(&speed_test->stats_lock);

    dev_kfree_skb_any(skb);
    return 0;
}

/**
 * @brief Wait for both download and upload completion with timeout
 * @param[in] speed_test Pointer to speed test structure
 * @param[in] expected_cnt Expected number of transfers to complete for both directions
 * @param[in] timeout_ms Timeout in milliseconds
 * @retval 0 Success
 * @retval -ETIMEDOUT Timeout occurred
 * @retval -EINTR Thread interrupted
 * @details This function waits for the specified number of both download and upload
 *          operations to complete within the given timeout period.
 */
static int hd_speed_test_transfer_wait(struct hd_speed_test *speed_test, uint32_t expected_cnt, int timeout_ms)
{
    unsigned long timeout = jiffies + msecs_to_jiffies(timeout_ms);
    uint32_t current_dnld_cnt, current_upld_cnt;

    while (1) {
        /* Read counters atomically */
        spin_lock(&speed_test->stats_lock);
        current_dnld_cnt = speed_test->dnld_cnt;
        current_upld_cnt = speed_test->upld_cnt;
        spin_unlock(&speed_test->stats_lock);

        /* Check if both directions have completed */
        if (current_dnld_cnt >= expected_cnt && current_upld_cnt >= expected_cnt) {
            SPEED_TEST_DBG(speed_test, "transfer_wait completed: dnld=%d, upld=%d",
                           current_dnld_cnt, current_upld_cnt);
            break;
        }

        if (time_after(jiffies, timeout)) {
            SPEED_TEST_DBG(speed_test, "transfer_wait timeout: dnld=%d/%d, upld=%d/%d!\n",
                           current_dnld_cnt, expected_cnt, current_upld_cnt, expected_cnt);
            return -ETIMEDOUT;
        }

        /* Check if thread should be stopped to avoid invalid waiting */
        if (speed_test->thread_mode && kthread_should_stop()) {
            return -EINTR;
        }

        usleep_range(50, 100);
    }

    return 0;
}

/**
 * @brief Check and compare buffer contents for data integrity
 * @param[in] speed_test Pointer to speed test structure
 * @retval 0 Data integrity check passed
 * @retval -1 Data mismatch detected
 * @details This function compares uploaded data with the original download
 *          data to verify data integrity. Since transmission is sequential
 *          within the same msg_tag channel, we can directly compare by index.
 */
static int hd_speed_test_check(struct hd_speed_test *speed_test)
{
    int i, j;
    int check_size;
    struct hd_msg_packt *dnld_msg_packt, *upld_msg_packt;
    uint8_t *dnld_buff, *upld_buff;
    int error_count = 0;

    SPEED_TEST_DBG(speed_test, "data integrity check start");

    /* Check each buffer pair (sequential order) */
    for (i = 0; i < HD_SPEED_TEST_BUFF_CNT; i++) {
        dnld_buff = speed_test->dnld_test_buff[i];
        upld_buff = speed_test->upld_test_buff[i];

        dnld_msg_packt = (struct hd_msg_packt *)dnld_buff;
        upld_msg_packt = (struct hd_msg_packt *)upld_buff;

        check_size = dnld_msg_packt->len + sizeof(struct hd_msg_packt);

        SPEED_TEST_DBG(speed_test, "checking packet %d, size %d", i, check_size);

        /* Compare packet contents byte by byte */
        for (j = 0; j < check_size; j++) {
            if (upld_buff[j] != dnld_buff[j]) {
                SPEED_TEST_ERR(speed_test, "data mismatch in packet %d at byte %d: %02X->%02X",
                               i, j, dnld_buff[j], upld_buff[j]);
                error_count++;
                break; /* Only report first mismatch per packet */
            }
        }

        /* Clear the upload buffer after checking */
        memset(upld_buff, 0, speed_test->upld_max_size);
    }

    if (error_count > 0) {
        SPEED_TEST_ERR(speed_test, "data integrity check failed with %d errors", error_count);
        return -1;
    }

    SPEED_TEST_DBG(speed_test, "data integrity check passed");
    return 0;
}

/**
 * @brief Execute data transfer speed test
 * @param[in] speed_test Pointer to speed test structure
 * @retval 0 Test completed successfully
 * @retval -EINVAL Invalid parameter
 * @retval <0 Error code from test operations
 * @details This function performs comprehensive speed testing including
 *          buffer allocation, data transfer, integrity verification,
 *          and performance measurement with detailed statistics reporting.
 */
static int hd_speed_test(struct hd_speed_test *speed_test)
{
    int ret = 0, i = 0, j = 0;

    struct timespec64 ts_start, ts_end;
    struct timespec64 ts_delta;
    uint64_t time_delta_ns = 0;

    if (speed_test == NULL) {
        pr_err("[hd_speed_test] speed_test is NULL!\n");
        return -EINVAL;
    }

    if (speed_test->msg_ctrl == NULL) {
        SPEED_TEST_ERR(speed_test, "msg_ctrl is NULL!\n");
        return -EINVAL;
    }

    /* Allocate and initialize test buffers */
    ret = hd_speed_test_buff_init(speed_test);
    if (ret < 0) {
        SPEED_TEST_ERR(speed_test, "test buff init error!\n");
        goto return_out;
    }

    /* Register message receive and send completion callback functions */
    ret = hd_msg_cb_register(speed_test->msg_ctrl, speed_test->msg_tag, hd_speed_test_upld_cb, speed_test, hd_speed_test_dnld_cb, speed_test);
    if (ret < 0) {
        SPEED_TEST_ERR(speed_test, "hd_msg_cb_register error %d!\n", ret);
        goto return_out;
    }

    if (speed_test->thread_mode) {
        hd_speed_test_transfer_wait(speed_test, HD_SPEED_TEST_BUFF_CNT, HD_SPEED_TEST_BUFF_CNT);
        spin_lock(&speed_test->stats_lock);
        speed_test->dnld_cnt = 0;
        speed_test->upld_cnt = 0;
        spin_unlock(&speed_test->stats_lock);
        /* try: */
        ret = hd_speed_test_dnld(speed_test, speed_test->dnld_test_buff[0]);
        if (ret < 0) {
            goto try_exit;
        }
        ret = hd_speed_test_transfer_wait(speed_test, 1, 100);
        if (ret < 0) {
            goto try_exit;
        }
    }

    SPEED_TEST_INFO(speed_test, "test cnt: %d*%d, speedtest start ...\n", HD_SPEED_TEST_BUFF_CNT, HD_SPEED_TEST_CNT);

    spin_lock(&speed_test->stats_lock);
    speed_test->dnld_total_size = 0;
    speed_test->upld_total_size = 0;
    spin_unlock(&speed_test->stats_lock);

    for (i = 0; i < HD_SPEED_TEST_CNT; i++) {
        SPEED_TEST_INFO(speed_test, "test: %d/%d\r\n", i + 1, HD_SPEED_TEST_CNT);

        /* Clear round counters */
        spin_lock(&speed_test->stats_lock);
        speed_test->dnld_cnt = 0;
        speed_test->upld_cnt = 0;
        spin_unlock(&speed_test->stats_lock);

        /* Clear upload buffers */
        for (j = 0; j < HD_SPEED_TEST_BUFF_CNT; j++) {
            memset(speed_test->upld_test_buff[j], 0x55, speed_test->upld_max_size);
        }

        /* Record start time for this round */
        ktime_get_boottime_ts64(&ts_start);

        /* Download (asynchronous) */
        for (j = 0; j < HD_SPEED_TEST_BUFF_CNT; j++) {
            ret = hd_speed_test_dnld(speed_test, speed_test->dnld_test_buff[j]);
            if (ret < 0) {
                goto return_out;
            }
        }

        /* Wait for both download and upload completion */
        ret = hd_speed_test_transfer_wait(speed_test, HD_SPEED_TEST_BUFF_CNT, HD_SPEED_TEST_BUFF_CNT * 20);
        if (ret < 0) {
            goto return_out;
        }

        /* Add this round's transfer time to total */
        ktime_get_boottime_ts64(&ts_end);
        ts_delta = timespec64_sub(ts_end, ts_start);
        time_delta_ns += timespec64_to_ns(&ts_delta);

        /* Check data integrity for all packets in this round */
        ret = hd_speed_test_check(speed_test);
        if (ret < 0) {
            goto return_out;
        }
    }

    SPEED_TEST_INFO(speed_test, "total data: %d KB, dnld: %d KB, upld: %d KB\r\n",
                    ((speed_test->dnld_total_size + speed_test->upld_total_size) / 1024), (speed_test->dnld_total_size / 1024), (speed_test->upld_total_size / 1024));
    SPEED_TEST_INFO(speed_test, "total time: %lld ms \r\n", time_delta_ns / 1000 / 1000);
    SPEED_TEST_INFO(speed_test, "speed: %lld KByte/s\r\n", (speed_test->dnld_total_size + speed_test->upld_total_size) / (time_delta_ns / 1000 / 1000));

return_out:

    if (ret < 0) {
        SPEED_TEST_INFO(speed_test, "hd_hd_speedtest fail: %d/%d!!! \n", i, HD_SPEED_TEST_CNT);
    } else {
        SPEED_TEST_INFO(speed_test, "hd_hd_speedtest success: %d/%d!!! \n", i, HD_SPEED_TEST_CNT);
    }

try_exit:

    /* Unregister message receive callback */
    hd_msg_cb_unregister(speed_test->msg_ctrl, speed_test->msg_tag);

    /* Release test buffers */
    hd_speed_test_buff_uninit(speed_test);

    return ret;
}

/**
 * @brief Independent thread testing for multi-threaded stress testing
 * @details This section provides threaded testing capabilities to stress test
 *          the message controller module's data distribution functionality
 *          through concurrent execution of multiple speed test instances.
 */

static struct hd_speed_test *g_speed_test_ptr[HD_MSG_TAG_MAX] = { NULL };

/**
 * @brief Speed test kernel thread function
 * @param[in] data Speed test structure pointer
 * @retval 0 Success
 * @retval -EINVAL Invalid parameter
 * @details This function runs as a kernel thread to execute speed tests
 *          continuously until stopped. It provides isolation for concurrent
 *          testing scenarios.
 */
static int hd_speed_test_thread(void *data)
{
    int ret;
    int delay;
    struct hd_speed_test *speed_test = (struct hd_speed_test *)data;

    if (!speed_test) {
        pr_err("[hd_speed_test] speed_test is NULL!\n");
        goto exit_thread;
    }

    SPEED_TEST_INFO(speed_test, "hd_speed_test_thread run\n");

    while (!kthread_should_stop()) {
        ret = hd_speed_test(speed_test);

        if (ret < 0) {
            delay = 1000;
        } else {
            delay = 3000;
        }

        for (; delay > 0 && !kthread_should_stop(); delay -= 10) {
            usleep_range(10000, 11000);
        }
    }

    SPEED_TEST_INFO(speed_test, "hd_speed_test_thread stop\n\n");

exit_thread:
    while (!kthread_should_stop()) {
        usleep_range(10000, 11000);
    }

    return 0;
}

/**
 * @brief Run speed test with specified parameters
 * @param[in] msg_ctrl Pointer to message controller
 * @param[in] msg_tag Message tag identifier
 * @param[in] thread_mode True for threaded mode, false for direct execution
 * @retval 0 Success
 * @retval -EINVAL Invalid message tag
 * @retval -EEXIST Task already exists (threaded mode)
 * @details This function initiates speed test operations either in direct
 *          mode or as a kernel thread for continuous testing.
 */
int hd_speed_test_run(struct hd_msg_ctrl *msg_ctrl, uint8_t msg_tag, bool thread_mode)
{
    int ret;
    struct hd_speed_test *speed_test;

    if (msg_tag >= HD_MSG_TAG_MAX) {
        pr_err("[hd_speed_test %d] hd_speed_test_run invalid msg_tag\n", msg_tag);
        return -EINVAL;
    }

    if (thread_mode && g_speed_test_ptr[msg_tag]) {
        pr_notice("[hd_speed_test %d] hd_speed_test_run task already exists\n", msg_tag);
        return -EINVAL;
    }

    speed_test = kzalloc(sizeof(struct hd_speed_test), GFP_KERNEL);
    if (!speed_test) {
        return -ENOMEM;
    }

    speed_test->msg_tag = msg_tag;
    speed_test->msg_ctrl = msg_ctrl;
    speed_test->dnld_max_size = msg_ctrl->msg_dnld_max_size;
    speed_test->upld_max_size = msg_ctrl->msg_upld_max_size;
    speed_test->thread_mode = thread_mode;

    /* Initialize statistics spinlock */
    spin_lock_init(&speed_test->stats_lock);

    /* Execute direct test if not in threaded mode */
    if (thread_mode == false) {
        ret = hd_speed_test(speed_test);
        kfree(speed_test);
        return ret;
    }

    SPEED_TEST_INFO(speed_test, "hd_speed_test_thread create: %d\n", msg_tag);

    speed_test->thread = kthread_create(hd_speed_test_thread, speed_test, "hd_speed_test_%d", msg_tag);
    if (IS_ERR(speed_test->thread)) {
        SPEED_TEST_INFO(speed_test, "hd_speed_test_thread create failed!\n");
        kfree(speed_test);
        return PTR_ERR(speed_test->thread);
    }

    g_speed_test_ptr[msg_tag] = speed_test;
    wake_up_process(speed_test->thread);

    SPEED_TEST_INFO(speed_test, "hd_speed_test_thread_create success!\n");
    return 0;
}

/**
 * @brief Destroy speed test thread for specific message tag
 * @param[in] msg_tag Message tag identifier
 * @retval 0 Success
 * @retval -EINVAL Invalid message tag
 * @retval -ENOENT No task found for the specified tag
 * @details This function stops and destroys the speed test thread
 *          associated with the given message tag.
 */
int hd_speed_test_thread_destroy(uint8_t msg_tag)
{
    if (msg_tag >= HD_MSG_TAG_MAX) {
        pr_notice("[hd_speed_test %d] hd_speed_test_thread_destroy invalid msg_tag\n", msg_tag);
        return -EINVAL;
    }

    if (!g_speed_test_ptr[msg_tag]) {
        pr_notice("[hd_speed_test %d] hd_speed_test_thread_destroy no task\n", msg_tag);
        return -ENOENT;
    }

    kthread_stop(g_speed_test_ptr[msg_tag]->thread);
    kfree(g_speed_test_ptr[msg_tag]);
    g_speed_test_ptr[msg_tag] = NULL;

    pr_notice("[hd_speed_test %d] hd_speed_test_thread_destroy success \n", msg_tag);
    return 0;
}

/**
 * @brief Destroy all active speed test threads
 * @retval 0 Success
 * @details This function iterates through all message tags and destroys
 *          any active speed test threads. Used for cleanup during
 *          module unloading or system shutdown.
 */
int hd_speed_test_thread_destroy_all(void)
{
    int i;

    for (i = 0; i < HD_MSG_TAG_MAX; i++) {
        if (g_speed_test_ptr[i]) {
            hd_speed_test_thread_destroy(i);
        }
    }

    return 0;
}
