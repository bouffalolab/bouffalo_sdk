/**
 * @file hd_sdio_manage.c
 * @brief SDIO management implementation for asynchronous data transfer
 * @details This file implements the SDIO management layer that provides unified
 *          asynchronous data transfer operations for both SDIO2 and SDIO3 hardware.
 *          It uses Linux work queues and socket buffers for efficient data handling.
 * @author mlwang
 * @date 2025-07-17
 * @version 1.0
 *
 * @note SDIO asynchronous transmission workflow:
 *       - For download: use hd_sdio_manage_dnld_send() to submit skb and trigger
 *         download work queue for transfer operation
 *       - For upload: triggered by card-to-host interrupt to process upload queue,
 *         submit skb, then use hd_sdio_manage_upld_recv() to retrieve skb
 *       - After completing download/upload operations, registered callback functions
 *         are called to facilitate asynchronous programming model
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/delay.h>

#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>

#include "hd_sdio_manage.h"
#include "hd_debugfs.h"

/**
 * @brief Lock SDIO management structure for thread safety
 * @param[in] sdio_manage Pointer to SDIO management structure
 */
static void hd_sdio_lock(struct hd_sdio_manage *sdio_manage)
{
    if (sdio_manage) {
        mutex_lock(&sdio_manage->lock);
    }
}

/**
 * @brief Unlock SDIO management structure
 * @param[in] sdio_manage Pointer to SDIO management structure
 */
static void hd_sdio_unlock(struct hd_sdio_manage *sdio_manage)
{
    if (sdio_manage) {
        mutex_unlock(&sdio_manage->lock);
    }
}

/**
 * @brief SDIO download work queue function
 * @param[in] work Pointer to work structure
 * @details This function processes download requests from the work queue.
 *          It dequeues socket buffers and performs SDIO download transfers
 *          with automatic retry mechanism on busy conditions.
 */
static int hd_sdio_dnld_polling(struct hd_sdio_manage *sdio_manage)
{
    int ret;
    struct sk_buff *skb;
    struct sdio_trans_desc dnld_desc;

    skb = skb_dequeue(&sdio_manage->dnld_skb_head);
    if (!skb) {
        return false;
    }

    /* Check buffer size constraints */
    if (skb->len > sdio_manage->sdio_card->dnld_max_size) {
        SDIO_DRV_ERR(sdio_manage->sdio_card, "dnld skb sizeover!");
        dev_kfree_skb_any(skb);
        return true;
    }

    dnld_desc.buff = skb->data;
    dnld_desc.data_len = skb->len;
    dnld_desc.buff_len = sdio_manage->sdio_card->dnld_max_size;

    /* Execute download transfer with synchronization */
    hd_sdio_lock(sdio_manage);
    ret = sdio_manage->sdio_card->sdio_dnld_transf(sdio_manage->sdio_card, &dnld_desc, 0);
    hd_sdio_unlock(sdio_manage);

    /* Handle transfer result */
    if (ret < 0) {
        if (ret == -EBUSY) {
            /* Implement exponential backoff for retry */
            skb_queue_head(&sdio_manage->dnld_skb_head, skb);

            return EBUSY;
        } else {
            /* Call download completion callback with failure status and protection */
            mutex_lock(&sdio_manage->dnld_cplt_cb_mutex);
            if (sdio_manage->dnld_cplt_cb) {
                ret = sdio_manage->dnld_cplt_cb(skb, false, sdio_manage->dnld_cplt_arg);
                mutex_unlock(&sdio_manage->dnld_cplt_cb_mutex);
                if (ret < 0) {
                    /* Callback failed, free the skb */
                    dev_kfree_skb_any(skb);
                }
            } else {
                mutex_unlock(&sdio_manage->dnld_cplt_cb_mutex);
                /* If no callback, free the skb */
                dev_kfree_skb_any(skb);
            }
            SDIO_DRV_ERR(sdio_manage->sdio_card, "dnld work error! %d", ret);

            return ret;
        }
    }

    /* Call download completion callback with protection */
    mutex_lock(&sdio_manage->dnld_cplt_cb_mutex);
    if (sdio_manage->dnld_cplt_cb) {
        ret = sdio_manage->dnld_cplt_cb(skb, true, sdio_manage->dnld_cplt_arg);
        mutex_unlock(&sdio_manage->dnld_cplt_cb_mutex);
        if (ret < 0) {
            /* Callback failed, free the skb */
            dev_kfree_skb_any(skb);
        }
    } else {
        mutex_unlock(&sdio_manage->dnld_cplt_cb_mutex);
        /* If no callback, just free the skb */
        dev_kfree_skb_any(skb);
    }

    return 1;
}

/**
 * @brief SDIO upload work queue function (card-to-host interrupt handler)
 * @param[in] work Pointer to work structure
 * @details This function processes card-to-host interrupts by reading data
 *          from the SDIO card. It manages buffer allocation, queue depth
 *          control, and provides completion callback notifications.
 *          Implements busy waiting with exponential backoff for congestion control.
 */
static int hd_sdio_upld_polling(struct hd_sdio_manage *sdio_manage)
{
    int ret;
    struct sk_buff *skb;
    struct sdio_trans_desc upld_desc;

    /* Prepare upload transfer descriptor */
    upld_desc.buff = sdio_manage->upld_trans_buf;
    upld_desc.buff_len = sdio_manage->sdio_card->upld_max_size;
    upld_desc.data_len = 0;

    /* Execute upload transfer with synchronization */
    hd_sdio_lock(sdio_manage);
    ret = sdio_manage->sdio_card->sdio_upld_transf(sdio_manage->sdio_card, &upld_desc, 0);
    hd_sdio_unlock(sdio_manage);

    /* Handle transfer result */
    if (ret < 0) {
        if (ret == -EBUSY) {
            /* Device busy, wait for next interrupt */
            SDIO_DRV_DBG(sdio_manage->sdio_card, "upld work busy!");
        } else {
            SDIO_DRV_ERR(sdio_manage->sdio_card, "upld work error!");
        }

        return ret;
    }

    /* Implement queue depth flow control */
    if (skb_queue_len(&sdio_manage->upld_skb_head) >= SDIO_MANAGE_UPLD_SKB_QUEUE_DEPTH) {
        /* Queue overflow, drop packet */
        SDIO_DRV_ERR(sdio_manage->sdio_card, "upld skb queue full !!!, drop skb");
        return true;
    }

    /* Allocate socket buffer and copy received data */
    skb = dev_alloc_skb(sdio_manage->sdio_card->upld_max_size);
    if (!skb) {
        SDIO_DRV_ERR(sdio_manage->sdio_card, "upld alloc_skb error!");
        return true;
    }
    skb_put(skb, upld_desc.data_len);
    memcpy(skb->data, sdio_manage->upld_trans_buf, upld_desc.data_len);

    /* Call upload completion callback with protection */
    mutex_lock(&sdio_manage->upld_cplt_cb_mutex);
    if (sdio_manage->upld_cplt_cb) {
        ret = sdio_manage->upld_cplt_cb(skb, true, sdio_manage->upld_cplt_arg);
        mutex_unlock(&sdio_manage->upld_cplt_cb_mutex);
        if (ret < 0) {
            /* Callback rejected SKB, we need to free it */
            dev_kfree_skb_any(skb);
            SDIO_DRV_ERR(sdio_manage->sdio_card, "upld callback rejected SKB, freed");
        }
        /* If callback returns 0, it took ownership of the SKB */
    } else {
        mutex_unlock(&sdio_manage->upld_cplt_cb_mutex);
        /* If no callback, add to upload queue for later processing with queue depth check */
        if (skb_queue_len(&sdio_manage->upld_skb_head) >= SDIO_MANAGE_UPLD_SKB_QUEUE_DEPTH) {
            SDIO_DRV_ERR(sdio_manage->sdio_card, "upld skb queue full, dropping SKB");
            dev_kfree_skb_any(skb);
        } else {
            skb_queue_tail(&sdio_manage->upld_skb_head, skb);
        }
    }

    return true;
}

/**
 * @brief Shared workqueue function for both download and upload operations
 * @param[in] work Work structure containing SDIO management context
 * @details This function balances download and upload operations to optimize
 *          bandwidth utilization and prevent starvation of either direction.
 *          Implements adaptive scheduling with backoff mechanisms.
 */
static void hd_sdio_workqueue_func(struct work_struct *work)
{
    struct hd_sdio_manage *sdio_manage = container_of(work, struct hd_sdio_manage, sdio_work);
    int processed = 0;

    int dnld_retry_cnt = 0;
    int dnld_proc_flag = false;

    int upld_proc_flag = false;

    upld_proc_flag = atomic_xchg(&sdio_manage->upld_int_flag, false);

    while (1) {
        /* Attempt to initiate upload processing */
        if (upld_proc_flag) {
            upld_proc_flag = hd_sdio_upld_polling(sdio_manage);
        }
        /* If no upload pending, update from interrupt flag */
        if (upld_proc_flag == false) {
            upld_proc_flag = atomic_xchg(&sdio_manage->upld_int_flag, false);
        }

        /* When download is busy, balance with upload operations using adaptive retry logic */
        if (dnld_proc_flag == EBUSY) {
            dnld_retry_cnt++;

            if (upld_proc_flag == false) {
                dnld_proc_flag = hd_sdio_dnld_polling(sdio_manage);

                if (dnld_proc_flag == EBUSY) {
                    /* Download is still busy, wait and retry */
                    if (dnld_retry_cnt < 30) {
                        usleep_range(dnld_retry_cnt * 10, dnld_retry_cnt * 20);
                    } else {
                        usleep_range(1000, 1100);
                    }
                } else {
                    dnld_retry_cnt = 0;
                }
            } else if ((dnld_retry_cnt < 10 && dnld_retry_cnt % 3 == 0) ||
                       (dnld_retry_cnt > 10 && dnld_retry_cnt % 4 == 0)) {
                /* If upload has packets, yield bandwidth to upload and periodically query download */
                dnld_proc_flag = hd_sdio_dnld_polling(sdio_manage);
            }

        } else {
            /* When download is not busy, continue attempting */
            dnld_proc_flag = hd_sdio_dnld_polling(sdio_manage);
            dnld_retry_cnt = 0;
        }

        /* Both idle, finish current work and wait for upload interrupt or download send wakeup */
        if ((dnld_proc_flag == false && upld_proc_flag == false) || dnld_proc_flag < 0 || upld_proc_flag < 0) {
            /* Both download and upload work completed successfully */
            break;
        }

        /* 连续循环后, 重新调度 workqueue, 避免性能和资源占用问题 */
        processed += 1;
        if (processed >= 256) {
            SDIO_DRV_DBG(sdio_manage->sdio_card, "Processed %d packets in this workqueue run, dnld:%d, upld:%d", processed, dnld_proc_flag, upld_proc_flag);
            if (upld_proc_flag) {
                /* NOTE! If upload has packets, must set flag or next schedule won't trigger upload transfer */
                atomic_set(&sdio_manage->upld_int_flag, true);
            }
            queue_work(sdio_manage->sdio_workqueue, &sdio_manage->sdio_work);
            break;
        }
    }
}

/**
 * @brief SDIO card-to-host interrupt callback (upload event handler)
 * @param[in] sdio_manage Pointer to SDIO management structure
 * @details This function handles card-to-host interrupts by scheduling
 *          upload work to the work queue for deferred processing.
 */
static void hd_sdio_upld_irq_cb(struct hd_sdio_manage *sdio_manage)
{
    if (!sdio_manage) {
        return;
    }

    SDIO_DRV_DBG(sdio_manage->sdio_card, "upld_irq come");

    atomic_set(&sdio_manage->upld_int_flag, true);
    /* Schedule upload work for deferred processing with safety check */
    queue_work(sdio_manage->sdio_workqueue, &sdio_manage->sdio_work);
}

/**
 * @brief Send socket buffer through download (host to device) interface
 * @param[in] sdio_manage Pointer to SDIO management structure
 * @param[in] skb Socket buffer to send
 * @retval 0 Success
 * @retval -ENOMEM Queue is full
 * @details This function adds a socket buffer to the download queue
 *          and triggers download work processing. Implements queue
 *          depth control to prevent memory exhaustion.
 */
int hd_sdio_manage_dnld_send(struct hd_sdio_manage *sdio_manage, struct sk_buff *skb)
{
    /* Check queue depth limit */
    if (skb_queue_len(&sdio_manage->dnld_skb_head) >= SDIO_MANAGE_DNLD_SKB_QUEUE_DEPTH) {
        SDIO_DRV_ERR(sdio_manage->sdio_card, "dnld skb queue full !!!, drop skb");
        return -ENOMEM;
    }

    /* Add to queue and trigger work */
    skb_queue_tail(&sdio_manage->dnld_skb_head, skb);
    queue_work(sdio_manage->sdio_workqueue, &sdio_manage->sdio_work);

    return 0;
}

/**
 * @brief Receive socket buffer from upload (device to host) interface
 * @param[in] sdio_manage Pointer to SDIO management structure
 * @param[out] skb Pointer to receive socket buffer
 * @retval 0 Socket buffer retrieved successfully
 * @retval -ENODATA No data available in queue
 * @details This function removes and returns the first socket buffer
 *          from the upload queue. Returns NULL if queue is empty.
 */
int hd_sdio_manage_upld_recv(struct hd_sdio_manage *sdio_manage, struct sk_buff **skb)
{
    *skb = skb_dequeue(&sdio_manage->upld_skb_head);

    if (!*skb) {
        return -ENOENT;
    }

    return 0;
}

/**
 * @brief Deinitialize SDIO management subsystem
 * @param[in] sdio_manage Pointer to SDIO management structure
 * @retval 0 Success
 * @retval -EINVAL Invalid parameter
 * @details This function performs cleanup of work queues, socket buffer queues,
 *          and disables card-to-host interrupts. Should be called during
 *          driver shutdown or error recovery.
 */
int hd_sdio_manage_deinit(struct hd_sdio_manage *sdio_manage)
{
    if (!sdio_manage) {
        return -1;
    }

    /* Step 1: Disable card-to-host interrupt FIRST to prevent new work scheduling */
    if (sdio_manage->sdio_card && sdio_manage->sdio_card->sdio_upld_irq_register) {
        sdio_manage->sdio_card->sdio_upld_irq_register(sdio_manage->sdio_card, NULL);
    }

    /* Step 2: Clear completion callbacks to prevent callback execution during cleanup */
    mutex_lock(&sdio_manage->upld_cplt_cb_mutex);
    sdio_manage->upld_cplt_cb = NULL;
    sdio_manage->upld_cplt_arg = NULL;
    mutex_unlock(&sdio_manage->upld_cplt_cb_mutex);

    mutex_lock(&sdio_manage->dnld_cplt_cb_mutex);
    sdio_manage->dnld_cplt_cb = NULL;
    sdio_manage->dnld_cplt_arg = NULL;
    mutex_unlock(&sdio_manage->dnld_cplt_cb_mutex);

    /* Step 3: Destroy work queues with proper cleanup (this will flush pending work) */
    if (sdio_manage->sdio_workqueue) {
        flush_workqueue(sdio_manage->sdio_workqueue);
        destroy_workqueue(sdio_manage->sdio_workqueue);
        sdio_manage->sdio_workqueue = NULL;
    }

    /* Step 4: Clean up socket buffer queues (after workqueues are destroyed) */
    skb_queue_purge(&sdio_manage->dnld_skb_head);
    skb_queue_purge(&sdio_manage->upld_skb_head);

    /* Step 5: Free transfer buffers */
    if (sdio_manage->dnld_trans_buf) {
        kfree(sdio_manage->dnld_trans_buf);
        sdio_manage->dnld_trans_buf = NULL;
    }

    if (sdio_manage->upld_trans_buf) {
        kfree(sdio_manage->upld_trans_buf);
        sdio_manage->upld_trans_buf = NULL;
    }

    /* Step 6: Destroy synchronization mutexes */
    mutex_destroy(&sdio_manage->lock);
    mutex_destroy(&sdio_manage->upld_cplt_cb_mutex);
    mutex_destroy(&sdio_manage->dnld_cplt_cb_mutex);

    SDIO_DRV_INFO(sdio_manage->sdio_card, "hd_sdio_manage_deinit");

    /* Step 7: Clear back-reference and free memory */
    if (sdio_manage->sdio_card) {
        sdio_manage->sdio_card->sdio_manage = NULL;
    }

    kfree(sdio_manage);
    return 0;
}

/**
 * @brief Initialize SDIO management subsystem
 * @param[out] hd_sdio_manage Pointer to receive allocated SDIO management structure
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success
 * @retval -ENOMEM Memory allocation failed
 * @retval -EINVAL Invalid parameter or work queue creation failed
 * @details This function initializes the SDIO management layer including
 *          work queues, socket buffer queues, transfer buffers, and enables
 *          card-to-host interrupts. Must be called before any SDIO operations.
 */
int hd_sdio_manage_init(struct hd_sdio_manage **hd_sdio_manage, struct hd_sdio_card *card)
{
    int ret = 0;
    struct hd_sdio_manage *sdio_manage = NULL;

    /* Allocate SDIO management structure */
    sdio_manage = kzalloc(sizeof(struct hd_sdio_manage), GFP_KERNEL);
    if (!sdio_manage) {
        ret = -ENOMEM;
        goto err_exit;
    }

    /* Initialize socket buffer queues */
    skb_queue_head_init(&sdio_manage->dnld_skb_head);
    skb_queue_head_init(&sdio_manage->upld_skb_head);

    /* Allocate transfer buffers */
    sdio_manage->dnld_trans_buf = kzalloc(card->dnld_max_size, GFP_KERNEL);
    if (!sdio_manage->dnld_trans_buf) {
        ret = -ENOMEM;
        goto err_exit;
    }
    sdio_manage->upld_trans_buf = kzalloc(card->upld_max_size, GFP_KERNEL);
    if (!sdio_manage->upld_trans_buf) {
        ret = -ENOMEM;
        goto err_exit;
    }

    /* Create download work queue */
    sdio_manage->sdio_workqueue = alloc_workqueue("HD_SDIO_MANAGE_WQ", WQ_HIGHPRI | WQ_UNBOUND | WQ_MEM_RECLAIM, 1);
    if (!sdio_manage->sdio_workqueue) {
        SDIO_DRV_ERR(card, "creat dnld workqueue failed");
        ret = -EINVAL;
        goto err_exit;
    }
    INIT_WORK(&sdio_manage->sdio_work, hd_sdio_workqueue_func);

    /* Enable card-to-host interrupt */
    ret = card->sdio_upld_irq_register(card, hd_sdio_upld_irq_cb);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "sdio_upld_irq_register failed: %d\n", ret);
        goto err_exit;
    }

    /* Initialize synchronization mutexes */
    mutex_init(&sdio_manage->lock);
    mutex_init(&sdio_manage->upld_cplt_cb_mutex);
    mutex_init(&sdio_manage->dnld_cplt_cb_mutex);

    /* Establish bidirectional references */
    sdio_manage->sdio_card = card;
    card->sdio_manage = sdio_manage;
    if (hd_sdio_manage) {
        *hd_sdio_manage = sdio_manage;
    }

    /* first trigger */
    hd_sdio_upld_irq_cb(sdio_manage);

err_exit:
    if (ret < 0) {
        if (hd_sdio_manage) {
            *hd_sdio_manage = NULL;
        }
        hd_sdio_manage_deinit(sdio_manage);
    }
    return ret;
}

/**
 * @brief Unified callback registration for both upload receive and download send directions
 * @param[in] sdio_manage SDIO management structure
 * @param[in] upld_recv_cb Upload receive callback function (can be NULL to unregister)
 * @param[in] upld_recv_arg User argument to pass to upload receive callback
 * @param[in] dnld_send_cb Download send completion callback function (can be NULL to unregister)
 * @param[in] dnld_send_arg User argument to pass to download send callback
 * @details This function provides a unified interface for registering both upload and download
 *          completion callbacks simultaneously, simplifying callback management between
 *          msg_ctrl and sdio_manage layers. Pass NULL for callback functions to unregister.
 */
void hd_sdio_manage_cplt_cb_register(struct hd_sdio_manage *sdio_manage, trans_cplt_cb upld_recv_cb, void *upld_recv_arg, trans_cplt_cb dnld_send_cb, void *dnld_send_arg)
{
    if (!sdio_manage) {
        return;
    }

    /* Register upload receive completion callback with dedicated mutex */
    mutex_lock(&sdio_manage->upld_cplt_cb_mutex);
    sdio_manage->upld_cplt_cb = upld_recv_cb;
    sdio_manage->upld_cplt_arg = upld_recv_arg;
    mutex_unlock(&sdio_manage->upld_cplt_cb_mutex);

    /* Register download send completion callback with dedicated mutex */
    mutex_lock(&sdio_manage->dnld_cplt_cb_mutex);
    sdio_manage->dnld_cplt_cb = dnld_send_cb;
    sdio_manage->dnld_cplt_arg = dnld_send_arg;
    mutex_unlock(&sdio_manage->dnld_cplt_cb_mutex);
}

/* External SDIO driver declarations */
extern struct sdio_driver hd_sdio2;
extern struct sdio_driver hd_sdio3;

/**
 * @brief Module initialization function
 * @retval 0 Success
 * @retval <0 Error code from driver registration
 * @details This function registers both SDIO2 and SDIO3 drivers
 *          with the Linux SDIO subsystem during module loading.
 */
static int hd_sdio_module_init(void)
{
    int ret;
    printk("hd sdio module init\n\n");

    printk("register hd sdio2 driver\n");
    ret = sdio_register_driver(&hd_sdio2);
    if (ret < 0) {
        pr_err("register sdio2 failed\n");
        return ret;
    }

    printk("register hd sdio3 driver\n");
    ret = sdio_register_driver(&hd_sdio3);
    if (ret < 0) {
        pr_err("register sdio3 failed\n");
        return ret;
    }

    return 0;
}

/**
 * @brief Module exit function
 * @details This function unregisters both SDIO drivers during
 *          module unloading and performs cleanup operations.
 */
static void hd_sdio_module_exit(void)
{
    printk("unregister hd sdio2 driver\n");
    sdio_unregister_driver(&hd_sdio2);

    printk("unregister hd sdio3 driver\n");
    sdio_unregister_driver(&hd_sdio3);

    printk("hd sdio module exit\n\n");
}

module_init(hd_sdio_module_init);
module_exit(hd_sdio_module_exit);
MODULE_LICENSE("GPL");
