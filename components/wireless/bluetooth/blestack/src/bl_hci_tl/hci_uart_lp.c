#include "hci_uart_lp.h"
#include "bl_hci_uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "stream_buffer.h"
#include "semphr.h"
#include "bt_log.h"
#include "softcrc.h"
#include "bl_lp.h"
#include <inttypes.h>
#include "bflb_irq.h"
#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "bflb_uart.h"
#include "bflb_clock.h"
#include "bl_timer.h"
typedef struct {
  uint8_t *data;
  uint32_t len;
} uart_msg_t;

#define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)                                                     \
  printf("[%u] " fmt, bl_timer_now_us(), ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)                                                    \
  printf("[ERROR: %u] " fmt, bl_timer_now_us(), ##__VA_ARGS__)

#define SHOW_TX_DATA 1
#define SHOW_RX_DATA 1

#define UART_RING_BUF_ENABLE 0
#define UART_RING_BUF_SIZE 2048

#define UART_EVENT_DMA_TX_DONE (1 << 0)
#define UART_EVENT_TX_DONE (1 << 1)
#define UART_EVENT_ACK (1 << 2)

#define UART_NAME "uart1"
#define UART_BAUDRATE 115200 // ~11.5 bytes per millisecond
#define UART_TX_QUEUE_DEPTH 5
#define UART_RX_BUF_SIZE 512
//#define UART_PREAMBLE_PATTERN 0x55
//#define UART_TX_INTERVAL_MS 2 // ~23 bytes @115200bps
#define UART_TX_TIMEOUT_MS 500 // ~5750 bytes (ok if great than max dma transfer size 4095) @115200bps
//#define UART_WAKEUP_TIMEOUT_MS 2000
#define UART_ACK_TIMEOUT_MS 10

#define DMA_RX_NAME "dma0_ch2"
#define DMA_TX_NAME "dma0_ch3"

#if UART_RING_BUF_ENABLE != 0
#include "ring_buffer.h"
static uint8_t uartBuf[UART_RING_BUF_SIZE];
static Ring_Buffer_Type uartRB;
#endif

static QueueHandle_t uart_tx_queue = NULL;
static EventGroupHandle_t uart_event_group = NULL;
static TaskHandle_t uart_task_handle = NULL;
static uint8_t uart_tx_pin;
static uint8_t uart_rx_pin;
//static uint8_t uart_preamble[23]; // ~2ms @115200bps
//static uint8_t uart_ack[] = {0x4F, 0x4B};
//static uint32_t uart_last_tx_end = 0;
static uint8_t uart_rx_buf[2][UART_RX_BUF_SIZE] __attribute__((section(".noncacheable")));
static uint32_t uart_rx_buf_idx = 0;
static struct bflb_dma_channel_lli_pool_s rx_llipool[20];
static struct bflb_dma_channel_lli_pool_s tx_llipool[20];
static struct bflb_device_s *uart = NULL;
static struct bflb_device_s *dma_rx = NULL;
static struct bflb_device_s *dma_tx = NULL;
//////////////////////////////////////////////////////////////////////////////
static struct mdev_t uart_dev;
static uint8_t uartInitialized = false;
UART_Context uart_ctx;
const static bl_hci_trans_callbacks_t *hciTranCb;
StreamBufferHandle_t xStreamBufferRX = NULL;
static TaskHandle_t uart_read_task_handle = NULL;
TickType_t hci_wake_tick = 0;
static SemaphoreHandle_t uart_send_sem = NULL;
static SemaphoreHandle_t uart_ack_sem = NULL;
static TaskHandle_t uart_ack_task_handle = NULL;
//////////////////////////////////////////////////////////////////////////////

static uint8_t uart_proc_buf[UART_RX_BUF_SIZE];
static uint32_t uart_proc_buf_len = 0;
static uint16_t uart_proc_send_sn = 0;
static uint16_t uart_proc_send_wait_ack_sn = 0;
static uint8_t uart_proc_recv_sn_valid = 0;
static uint16_t uart_proc_recv_sn = 0;
static uint16_t uart_proc_recv_next_sn = 0;

#define UART_START "\x80\x55"
#define UART_START_SIZE 2
#define UART_DATA 0x01
#define UART_ACK 0x02

#pragma pack(push, 1)  // 设置结构体对齐为1字节
typedef struct _UARTCMD_HEAD {
    uint8_t start[UART_START_SIZE];
    uint16_t sn;
    uint16_t type_and_len;
    uint16_t crc16;
} UARTCMD_HEAD;
#pragma pack(pop)  // 恢复默认对齐

static int uart_send(uint8_t *data, uint32_t len) {
  //taskENTER_CRITICAL();

  if (pdTRUE != xSemaphoreTake(uart_send_sem, UART_TX_TIMEOUT_MS * 2)) {
        return -1;
  }

  xEventGroupClearBits(uart_event_group, UART_EVENT_DMA_TX_DONE);

  csi_dcache_clean_range((uint32_t*)data, len);

  struct bflb_dma_channel_lli_transfer_s transfer;
  transfer.src_addr = (uint32_t)data;
  transfer.dst_addr = uart->reg_base + 0x88;
  transfer.nbytes = len;
    
  bflb_dma_channel_stop(dma_tx);
  bflb_dma_channel_lli_reload(dma_tx, tx_llipool, sizeof(tx_llipool)/sizeof(tx_llipool[0]), &transfer, 1);
  bflb_dma_channel_start(dma_tx);

  //taskEXIT_CRITICAL();

  EventBits_t bits = xEventGroupWaitBits(uart_event_group, UART_EVENT_DMA_TX_DONE, pdTRUE, pdTRUE, UART_TX_TIMEOUT_MS);
  if (!(bits & UART_EVENT_DMA_TX_DONE)) {
    xSemaphoreGive(uart_send_sem);
    return -1;
  }

  //while (UART_GetTxBusBusyStatus(UART_ID))
  //  ;
  bflb_uart_wait_tx_done(uart);

  xSemaphoreGive(uart_send_sem);
  return 0;
}

/*static int uart_send_from_isr(uint8_t *data, uint32_t len) {
  uint32_t start = bl_timer_now_us();

  csi_dcache_clean_range((uint32_t*)data, len);

  DMA_Channel_Disable(DMA0_ID, uart_tx_dma_ch);
  DMA_Channel_Update_SrcMemcfg(DMA0_ID, uart_tx_dma_ch,
                               (uint32_t)(uintptr_t)data, len);
  DMA_Channel_Enable(DMA0_ID, uart_tx_dma_ch);

  while (DMA_Channel_Is_Busy(DMA0_ID, uart_tx_dma_ch)) {
    if (bl_timer_now_us() - start >= UART_TX_TIMEOUT_MS * 1000) {
      return -1;
    }
  }

  while (UART_GetTxBusBusyStatus(UART_ID))
    ;

  return 0;
}*/

/*static int uart_send_preamble(void) {
  return uart_send(uart_preamble, sizeof(uart_preamble));
}*/

static int uart_send_data(uint8_t *data, uint32_t len) {
  // wait for remote uart rx timeout
  //while (bl_timer_now_us() - uart_last_tx_end < UART_TX_INTERVAL_MS * 1000)
  //  ;

  if (uart_send(data, len) != 0) {
    return -1;
  }

  //uart_last_tx_end = bl_timer_now_us();

  return 0;
}

static int uart_send_ack(void) {
#if 0
  uint8_t ack_data[11] = {0};
  uint32_t crc32 = 0;

  // wait for remote uart rx timeout
  while (bl_timer_now_us() - uart_last_tx_end < UART_TX_INTERVAL_MS * 1000);

  memcpy(ack_data, UART_START, UART_START_SIZE);
  ack_data[UART_START_SIZE] = 0;
  ack_data[UART_START_SIZE + 1] = 0;
  ack_data[UART_START_SIZE + 2] = UART_ACK;
  ack_data[UART_START_SIZE + 3] = 0;
  ack_data[UART_START_SIZE + 4] = 0;
  crc32 = BFLB_Soft_CRC32_Table(ack_data, sizeof(UARTCMD_HEAD));
  memcpy(ack_data + sizeof(UARTCMD_HEAD), &crc32, sizeof(crc32));

  if (uart_send_from_isr(ack_data, sizeof(UARTCMD_HEAD) + sizeof(crc32)) != 0) {
    return -1;
  }

  uart_last_tx_end = bl_timer_now_us();
#else
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  int ret;

  if (uart_ack_sem) {
    if (xPortIsInsideInterrupt()) {
      ret = xSemaphoreGiveFromISR((SemaphoreHandle_t)uart_ack_sem, &xHigherPriorityTaskWoken);
      if (ret == pdPASS) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      } 
    }else {
      ret = xSemaphoreGive((SemaphoreHandle_t)uart_ack_sem);
    }
  }
#endif

  return 0;
}

static int uart_data_process(uint8_t *data, uint32_t len)
{
    uint32_t crc32 = 0;
    uint8_t cmd_type = 0;
    UARTCMD_HEAD *head = NULL;

    crc32 = BFLB_Soft_CRC32_Table(data, len - sizeof(crc32));
    if(memcmp(&crc32, data + len - sizeof(crc32), sizeof(crc32)) == 0) {
        head = (UARTCMD_HEAD *)data;
        cmd_type = (head->type_and_len >> 12) & 0x0F;
        if (cmd_type == UART_DATA) {
            // check and update sn
            if (uart_proc_recv_sn_valid) {
                if (head->sn == uart_proc_recv_sn) {
                    // send ack
                    uart_send_ack();

                    LOG_ERROR("duplicate packet\r\n");
                    return 0;
                }

                if (head->sn != uart_proc_recv_next_sn) {
                    LOG_ERROR("sn error\r\n");
                    return 0;
                }
            }
            uart_proc_recv_sn_valid = 1;
            uart_proc_recv_sn = head->sn;
            uart_proc_recv_next_sn = head->sn + ((head->type_and_len) & 0x0FFF);

            // send ack
            uart_send_ack();

#if UART_RING_BUF_ENABLE != 0
            // put data to ring buffer
            Ring_Buffer_Write(&uartRB, data + sizeof(UARTCMD_HEAD), len - sizeof(UARTCMD_HEAD) - sizeof(crc32));
#endif

            // report data
            hci_uart_rx_done_callback(data + sizeof(UARTCMD_HEAD), len - sizeof(UARTCMD_HEAD) - sizeof(crc32));
        } else if (cmd_type == UART_ACK) {
            EventBits_t bits = xEventGroupGetBitsFromISR(uart_event_group);
            if ((bits & UART_EVENT_TX_DONE) && (head->sn == uart_proc_send_wait_ack_sn)) {
                LOG_INFO("r ack\r\n");
                xEventGroupClearBitsFromISR(uart_event_group, UART_EVENT_TX_DONE);

                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xEventGroupSetBitsFromISR(uart_event_group, UART_EVENT_ACK, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        }
        return 0;
    } else {
        LOG_ERROR("crc32 error\r\n");
        return -1;
    }
}

static int uart_find_data(uint8_t *data, int len, int *scan)
{
    int pos = 0;
    int head_pos = -1;
    int data_len = 0;
    int cmd_len = 0;
    uint16_t crc16 = 0;

    while (pos + UART_START_SIZE <= len) {
        if (memcmp(data + pos, UART_START, UART_START_SIZE) == 0) {
            head_pos = pos;
            break;
        }
        pos++;
    }

    if (head_pos >= 0) { // find valid head
        if (len - head_pos < sizeof(UARTCMD_HEAD)) { // not receiving enough data
            *scan = head_pos;
            return 1;
        }

        crc16 = BFLB_Soft_CRC16(data + head_pos, sizeof(UARTCMD_HEAD) - 2);
        if (crc16 != (data[head_pos + 6] + data[head_pos + 7]*256)) {
            LOG_ERROR("head crc16 error\r\n");
            *scan = head_pos + UART_START_SIZE;
            return 0;
        }

        data_len = (data[head_pos + 4] + data[head_pos + 5]*256) & 0x0FFF;
        if (data_len > UART_RX_BUF_SIZE) { // data len is error, discard all data
            LOG_ERROR("data len error\r\n");
            *scan = head_pos + UART_START_SIZE;
            return 0;
        }

        cmd_len = sizeof(UARTCMD_HEAD) + data_len + 4;
        if (len - head_pos < cmd_len) { // not receiving enough data
            *scan = head_pos;
            return 1;
        }

        if (uart_data_process(data + head_pos, cmd_len) == 0) {
            *scan = head_pos + cmd_len;
        } else {
            *scan = head_pos + UART_START_SIZE;
        }
    }
    else { // not find
        if (len > UART_START_SIZE) { // retain uart head start
            *scan = len - UART_START_SIZE;
        }
    }

    return 0;
}

static void uart_recv_data(uint8_t *data, uint32_t len)
{
    int scan_bytes = 0;
    int data_incomplete = 0;

    if (uart_proc_buf_len + len > UART_RX_BUF_SIZE) {
        LOG_ERROR("uart process buffer oveflow\r\n");
        uart_proc_buf_len = 0; // discard old data
    }

    memcpy(uart_proc_buf + uart_proc_buf_len, data, len);
    uart_proc_buf_len += len;
    while (uart_proc_buf_len >= sizeof(UARTCMD_HEAD)) {
        scan_bytes = 0;
        data_incomplete = uart_find_data(uart_proc_buf, uart_proc_buf_len, &scan_bytes);

        if (scan_bytes > 0) {
            memcpy(uart_proc_buf, uart_proc_buf + scan_bytes, uart_proc_buf_len - scan_bytes);
            uart_proc_buf_len -= scan_bytes;
        }

        if (data_incomplete)
            break;
    }
}

static void UART_RTO_Callback(int irq, void *arg) {
  uint8_t *data = uart_rx_buf[uart_rx_buf_idx];
  uint32_t len = bflb_dma_feature_control(dma_rx, DMA_CMD_GET_LLI_DSTADDR, 0) - (uint32_t)uart_rx_buf[uart_rx_buf_idx];
  //uint32_t crc32;

  bflb_uart_int_clear(uart, UART_INTCLR_RTO);

  uart_rx_buf_idx = !uart_rx_buf_idx;

  struct bflb_dma_channel_lli_transfer_s transfer;
  transfer.src_addr = uart->reg_base + 0x8C;
  transfer.dst_addr = (uint32_t)uart_rx_buf[uart_rx_buf_idx];
  transfer.nbytes = UART_RX_BUF_SIZE;
  bflb_dma_channel_stop(dma_rx);
  bflb_dma_channel_lli_reload(dma_rx, rx_llipool, sizeof(rx_llipool)/sizeof(rx_llipool[0]), &transfer, 1);
  bflb_dma_channel_start(dma_rx);

#if SHOW_RX_DATA != 0
  LOG("[%u] uart rx %" PRIu32 ":", bl_timer_now_us(), len);
  for (uint32_t i = 0; i < len; i++) {
    LOG(" %02x", data[i]);
  }
  LOG("\r\n");
#endif

#if 0
  // check ack
  if (len == sizeof(uart_ack)) {
    if (memcmp(data, uart_ack, sizeof(uart_ack)) == 0) {
      EventBits_t bits = xEventGroupGetBitsFromISR(uart_event_group);
      if (bits & UART_EVENT_TX_DONE) {
        xEventGroupClearBitsFromISR(uart_event_group, UART_EVENT_TX_DONE);

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xEventGroupSetBitsFromISR(uart_event_group, UART_EVENT_ACK,
                                  &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
      return;
    }
  }

  // check crc32
  if (len > sizeof(crc32)) {
    crc32 = BFLB_Soft_CRC32_Table(data, len - sizeof(crc32));
    if (memcmp(&crc32, data + len - sizeof(crc32), sizeof(crc32)) == 0) {
      // send ack
      uart_send_ack();

#if UART_RING_BUF_ENABLE != 0
      // put data to ring buffer
      Ring_Buffer_Write(&uartRB, data, len - sizeof(crc32));
#endif

      // report data
      hci_uart_rx_done_callback(data, len - sizeof(crc32));
    }
  }
#endif

    /* 更新HCI保活时间 */
    hci_wake_tick = xTaskGetTickCountFromISR();
    bl_pm_event_bit_set(PSM_EVENT_BLE_HCI);

    uart_recv_data(data, len);
}

static void UART_RX_DMA_Callback(void *arg) {
    // empty
}

static void UART_TX_DMA_Callback(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(uart_event_group, UART_EVENT_DMA_TX_DONE, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void gpio_init(void) {
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, uart_tx_pin, GPIO_UART_FUNC_UART0_TX + 4 * uart->idx);
    bflb_gpio_uart_init(gpio, uart_rx_pin, GPIO_UART_FUNC_UART0_RX + 4 * uart->idx);
}

static void uart_init(void) {
    struct bflb_uart_config_s cfg = {
        .baudrate = UART_BAUDRATE,
        .data_bits = UART_DATA_BITS_8,
        .stop_bits = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .bit_order = UART_LSB_FIRST,
        .flow_ctrl = 0,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };

    bflb_uart_init(uart, &cfg);
    bflb_uart_disable(uart);
    bflb_uart_feature_control(uart, UART_CMD_SET_RTO_VALUE, 0x80);
    bflb_uart_link_txdma(uart, true);
    bflb_uart_link_rxdma(uart, true);
    bflb_irq_attach(uart->irq_num, UART_RTO_Callback, NULL);
    bflb_irq_enable(uart->irq_num);
}

static void dma_rx_init(void) {
    struct bflb_dma_channel_config_s rx_cfg = {
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_UART0_RX + 2 * uart->idx,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_8BIT,
        .dst_width = DMA_DATA_WIDTH_8BIT,
    };

    bflb_dma_channel_init(dma_rx, &rx_cfg);
    bflb_dma_channel_irq_attach(dma_rx, UART_RX_DMA_Callback, NULL);
}

static void dma_tx_init(void) {
    struct bflb_dma_channel_config_s tx_cfg = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_UART0_TX + 2 * uart->idx,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_8BIT,
        .dst_width = DMA_DATA_WIDTH_8BIT,
    };

    bflb_dma_channel_init(dma_tx, &tx_cfg);
    bflb_dma_channel_irq_attach(dma_tx, UART_TX_DMA_Callback, NULL);
}

static void dma_init(void) {
    PERIPHERAL_CLOCK_DMA0_ENABLE();

    dma_rx_init();
    dma_tx_init();
}

static void uart_enable(void) {
    uart_rx_buf_idx = 0;

    struct bflb_dma_channel_lli_transfer_s transfer;
    transfer.src_addr = uart->reg_base + 0x8C;
    transfer.dst_addr = (uint32_t)uart_rx_buf[uart_rx_buf_idx];
    transfer.nbytes = UART_RX_BUF_SIZE;

    bflb_dma_channel_stop(dma_rx);
    bflb_dma_channel_lli_reload(dma_rx, rx_llipool, sizeof(rx_llipool)/sizeof(rx_llipool[0]), &transfer, 1);
    bflb_dma_channel_start(dma_rx);

    bflb_uart_enable(uart);
}

/*static void enable_uart_tx_pin_func(void) {
  GLB_GPIO_Type pin = uart_tx_pin;
  GLB_GPIO_Func_Init(GPIO_FUN_UART, &pin, 1);
}

static void disable_uart_tx_pin_func(void) {
  bl_gpio_enable_input(uart_tx_pin, 0, 0);
  arch_delay_us(20);
}

static uint32_t get_uart_tx_pin_level(void) {
  GLB_GPIO_Type pin = uart_tx_pin;
  return GLB_GPIO_Read(pin);
}

static bool is_remote_sleep(void) {
  uint32_t level;

  disable_uart_tx_pin_func();

  level = get_uart_tx_pin_level();

  enable_uart_tx_pin_func();

  return level == 0;
}*/

static int uart_write_with_wakeup(uint8_t *data, uint32_t len) {
#if 0
  bool sleep;
  uint32_t start;

  sleep = is_remote_sleep();
  LOG_INFO("remote %s\r\n", sleep ? "sleep" : "active");

  if (sleep) {
    start = bl_timer_now_us();

    do {
      if (bl_timer_now_us() - start >= UART_WAKEUP_TIMEOUT_MS * 1000) {
        LOG_ERROR("remote wakeup timeout\r\n");
        return HCI_UART_WAKEUP_TIMEOUT;
      }

      if (uart_send_preamble() != 0) {
        LOG_ERROR("remote wakeup fault\r\n");
        return HCI_UART_WAKEUP_FAULT;
      }
    } while (is_remote_sleep());
  }
#endif

#if SHOW_TX_DATA != 0
  LOG("[%u] uart tx %" PRIu32 ":", bl_timer_now_us(), len);
  for (uint32_t i = 0; i < len; i++) {
    LOG(" %02x", data[i]);
  }
  LOG("\r\n");
#endif

#if 0
  // wait for remote uart rx timeout
  if (sleep) {
    arch_delay_ms(UART_TX_INTERVAL_MS);
  }
#endif

  if (uart_send_data(data, len) != 0) {
    LOG_ERROR("uart tx fault\r\n");
    return HCI_UART_TX_FAULT;
  }

  return HCI_UART_OK;
}

static void hci_uart_task(void *pvParameters) {
  uart_msg_t msg;
  int retry_count = 0;
  UARTCMD_HEAD *head = NULL;

  //memset(uart_preamble, UART_PREAMBLE_PATTERN, sizeof(uart_preamble));

  while (1) {
    xQueuePeek(uart_tx_queue, &msg, portMAX_DELAY);

    while (1) {
      // update wait sn
      head = (UARTCMD_HEAD *)msg.data;
      uart_proc_send_wait_ack_sn = head->sn + ((head->type_and_len) & 0x0FFF);

      if (uart_write_with_wakeup(msg.data, msg.len) == HCI_UART_OK) {
        xEventGroupSetBits(uart_event_group, UART_EVENT_TX_DONE);
        EventBits_t bits =
            xEventGroupWaitBits(uart_event_group, UART_EVENT_ACK, pdTRUE,
                                pdTRUE, UART_ACK_TIMEOUT_MS);
        if (bits & UART_EVENT_ACK) {
          // report ack
          hci_uart_ack_received_callback();

          // pop
          xQueueReceive(uart_tx_queue, &msg, 0);
          free(msg.data);
          retry_count = 0;
          break;
        }
        LOG_INFO("no ack\r\n");
        retry_count += 1;
      }
      LOG_INFO("retry %d\r\n", retry_count);
    }
    xEventGroupClearBits(uart_event_group, UART_EVENT_TX_DONE);
    xEventGroupClearBits(uart_event_group, UART_EVENT_ACK);
  }
}

static void hci_uart_ack_task(void *pvParameters) {
  uint8_t ack_data[12] = {0};
  uint16_t type_and_len;
  uint32_t crc16 = 0;
  uint32_t crc32 = 0;

  while(1) {
    if (xSemaphoreTake(uart_ack_sem, portMAX_DELAY) == pdTRUE) {
      // creat ack data
      memcpy(ack_data, UART_START, UART_START_SIZE);
      ack_data[UART_START_SIZE] = (uart_proc_recv_next_sn & 0xFF);
      ack_data[UART_START_SIZE + 1] = (uart_proc_recv_next_sn & 0xFF00) >> 8;
      type_and_len = (UART_ACK << 12) | 0;
      memcpy(ack_data + UART_START_SIZE + 2, &type_and_len, sizeof(type_and_len));
      crc16 = BFLB_Soft_CRC16(ack_data, sizeof(UARTCMD_HEAD) - 2); // calc head crc16
      memcpy(ack_data + sizeof(UARTCMD_HEAD) - 2, &crc16, sizeof(crc16));
      crc32 = BFLB_Soft_CRC32_Table(ack_data, sizeof(UARTCMD_HEAD));
      memcpy(ack_data + sizeof(UARTCMD_HEAD), &crc32, sizeof(crc32));

      LOG_INFO("s ack\r\n");
      uart_send_data(ack_data, sizeof(UARTCMD_HEAD) + sizeof(crc32));    
    }
  }
}


int hci_uart_init(void) {
  uart = bflb_device_get_by_name(UART_NAME);
  dma_rx = bflb_device_get_by_name(DMA_RX_NAME);
  dma_tx = bflb_device_get_by_name(DMA_TX_NAME);

  uart_tx_pin = CONFIG_HCI_UART_TX_PIN;
  uart_rx_pin = CONFIG_HCI_UART_RX_PIN;

  gpio_init();
  uart_init();
  dma_init();

#if UART_RING_BUF_ENABLE != 0
  Ring_Buffer_Init(&uartRB, uartBuf, sizeof(uartBuf), NULL, NULL);
#endif

  if (!uart_tx_queue) {
    uart_tx_queue = xQueueCreate(UART_TX_QUEUE_DEPTH, sizeof(uart_msg_t));
  }

  if (!uart_event_group) {
    uart_event_group = xEventGroupCreate();
  }

  if (!uart_send_sem) {
    uart_send_sem = xSemaphoreCreateMutex();
  }

  if (!uart_task_handle) {
    xTaskCreate(hci_uart_task, "hci_uart", 256, NULL, configMAX_PRIORITIES - 2,
                &uart_task_handle);
  }

  if (!uart_ack_sem) {
    uart_ack_sem = xSemaphoreCreateCounting(32, 0);
  }

  if (!uart_ack_task_handle) {
    xTaskCreate(hci_uart_ack_task, "hci_uart_ack", 256, NULL, configMAX_PRIORITIES - 1, &uart_ack_task_handle);
  }

  uart_enable();

  return HCI_UART_OK;
}

int hci_uart_write(uint8_t *data, uint32_t len) {
  uint16_t type_and_len;
  uint32_t crc16;
  uint32_t crc32;
  uart_msg_t msg;

  if (uxQueueSpacesAvailable(uart_tx_queue) == 0) {
    LOG_ERROR("uart tx queue full\r\n");
    return HCI_UART_TX_QUEUE_FULL;
  }

  msg.data = malloc(sizeof(UARTCMD_HEAD) + len + sizeof(crc32));
  memcpy(msg.data, UART_START, UART_START_SIZE);
  msg.data[UART_START_SIZE] = (uart_proc_send_sn & 0xFF);
  msg.data[UART_START_SIZE + 1] = (uart_proc_send_sn & 0xFF00) >> 8;
  uart_proc_send_sn += len;
  type_and_len = (UART_DATA << 12) | len;
  memcpy(msg.data + UART_START_SIZE + 2, &type_and_len, sizeof(type_and_len));
  crc16 = BFLB_Soft_CRC16(msg.data, sizeof(UARTCMD_HEAD) - 2); // calc head crc16
  memcpy(msg.data + sizeof(UARTCMD_HEAD) - 2, &crc16, sizeof(crc16));
  memcpy(msg.data + sizeof(UARTCMD_HEAD), data, len);
  crc32 = BFLB_Soft_CRC32_Table(msg.data, sizeof(UARTCMD_HEAD) + len);
  memcpy(msg.data + sizeof(UARTCMD_HEAD) + len, &crc32, sizeof(crc32));
  msg.len = sizeof(UARTCMD_HEAD) + len + sizeof(crc32);

  if (!xPortIsInsideInterrupt()) {
    xQueueSend(uart_tx_queue, &msg, 0);
  } else {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(uart_tx_queue, &msg, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  // return HCI_UART_OK;
  return len;
}

#if UART_RING_BUF_ENABLE != 0
uint32_t hci_uart_get_rx_count(void) { return Ring_Buffer_Get_Length(&uartRB); }

uint32_t hci_uart_read(uint8_t *data, uint32_t len) {
  uint32_t cnt;

  cnt = Ring_Buffer_Get_Length(&uartRB);
  if (cnt < len) {
    len = cnt;
  }

  Ring_Buffer_Read(&uartRB, data, len);
  return len;
}
#endif

int hci_uart_is_busy(void) {
  // tx busy
  if (uxQueueSpacesAvailable(uart_tx_queue) != UART_TX_QUEUE_DEPTH) {
    return 1;
  }

  // rx busy
  if (bflb_dma_feature_control(dma_rx, DMA_CMD_GET_LLI_DSTADDR, 0) != (uint32_t)uart_rx_buf[uart_rx_buf_idx]) {
    return 2;
  }

  // ring buffer not empty
#if UART_RING_BUF_ENABLE != 0
  if (Ring_Buffer_Get_Length(&uartRB) != 0) {
    return 3;
  }
#endif

  // 等待2 x UART_ACK_TIMEOUT_MS
  if ((xTaskGetTickCountFromISR() - hci_wake_tick) < pdMS_TO_TICKS(2*UART_ACK_TIMEOUT_MS)) {
    return 4;
  } else {
    bl_pm_event_bit_clear(PSM_EVENT_BLE_HCI);
  }

  return 0;
}

void hci_uart_init_after_lp(int wakeup_by_hci) {
  if (wakeup_by_hci) {
    /* 当702L HCI 唤醒之后，设置保活时间 */
    hci_wake_tick = xTaskGetTickCountFromISR();
    bl_pm_event_bit_set(PSM_EVENT_BLE_HCI);
  } else {
    hci_uart_init();
  }
}

__attribute__((weak)) void hci_uart_ack_received_callback(void) {
  LOG_INFO("ack received\r\n");
}

__attribute__((weak)) void hci_uart_rx_done_callback(uint8_t *data,
                                                     uint32_t data_len) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  int ret;

  LOG_INFO("rx done\r\n");
  ret = xStreamBufferSendFromISR(xStreamBufferRX, data, data_len, &xHigherPriorityTaskWoken);
  configASSERT(ret == data_len);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void hci_uart_reset(void) {
    uart_proc_recv_sn_valid = 0;
}

//////////////////////////////////////////////////////////////////////////////

static void UartReadThread(void *args) {
  int bytesRead = 0;
  uint16_t len;

  do {
    if (uart_ctx.rxState != UART_GET_BUFFER) {
      // printf("uart_ctx.maxReadLen:%d, uart_ctx.ptr:0x%x,
      // uart_ctx.buffType:%d\r\n",
      //         uart_ctx.maxReadLen, uart_ctx.ptr, uart_ctx.buffType);
      // int_status = uart2_checkIntStatus();
      // printf("int_status=0x%lx\r\n", int_status);
      //bytesRead = uart2_read(uart_ctx.ptr, uart_ctx.maxReadLen);
      bytesRead = xStreamBufferReceive(xStreamBufferRX, uart_ctx.ptr, uart_ctx.maxReadLen, portMAX_DELAY);

      // printf("bytesRead=%d\r\n",bytesRead);
      // printf("urlen=%d\r\n",bytesRead);

      uart_ctx.ptr += bytesRead;
      if (bytesRead != uart_ctx.maxReadLen) {
        uart_ctx.maxReadLen = (uint16_t)(uart_ctx.maxReadLen) - bytesRead;
        continue;
      }
    }

    // printf("uart_ctx.buffType:%d, bytesRead:%d\n", uart_ctx.buffType,
    // bytesRead); printf("uart_ctx.rxState:%d, read out %d bytes,
    // uart_ctx.ptr:0x%x, data:%s\n",
    //        uart_ctx.rxState, bytesRead, uart_ctx.ptr,
    //        bt_hex((uart_ctx.ptr-bytesRead), bytesRead);

    switch (uart_ctx.rxState) {
    case UART_GET_TYPE:
      /* Get the type of buffer */
      if (uart_ctx.buffType == IND_EVENT) {
        uart_ctx.headerLen = 2;
      } else if (uart_ctx.buffType == IND_ACL_DATA) {
        uart_ctx.headerLen = 4;
      } else {
        BT_ERR("Invalid buffType(%d)", uart_ctx.buffType);
        uart_ctx.buffType = 0;
        uart_ctx.ptr = &(uart_ctx.buffType);
        uart_ctx.maxReadLen = 1;
        uart_ctx.rxState = UART_GET_TYPE;
        break;
      }

      /* Set up for the next state */
      uart_ctx.ptr = &uart_ctx.header[0];
      uart_ctx.maxReadLen = uart_ctx.headerLen;
      uart_ctx.rxState = UART_GET_HEADER;
      break;

    case UART_GET_HEADER:
      /*Get buffer and store header info */
      uart_ctx.rxState = UART_GET_BUFFER;
      /* fall through */
    case UART_GET_BUFFER:
      /* Calculate buffer length based on type */
      switch (uart_ctx.buffType) {
      case IND_ACL_DATA:
        len =
            (uint16_t)(LEtoHost16(&(uart_ctx.header)[2])) + uart_ctx.headerLen;
        uart_ctx.rxBuf =
            (struct net_buf *)hciTranCb->alloc_buf(IND_ACL_DATA, 0, len);
        // uart_ctx.rxBuf = (struct net_buf*)bl_hci_alloc_buf(IND_ACL_DATA, 0,
        // len);

        if (uart_ctx.rxBuf) {
          uart_ctx.maxReadLen = LEtoHost16(&(uart_ctx.header)[2]);
          uart_ctx.rxBuf->len = len;
          uart_ctx.ptr = (uint8_t *)(uart_ctx.rxBuf->data);
        } else {
          BT_ERR("Fail to allocate rx buffer for rx acl data\r\n");
          uart_ctx.buffType = 0;
          uart_ctx.ptr = &(uart_ctx.buffType);
          uart_ctx.maxReadLen = 1;
          uart_ctx.rxState = UART_GET_TYPE;
        }
        break;
      case IND_EVENT:
        len = (uint16_t)(uart_ctx.header[1] + uart_ctx.headerLen);

        // printf("uart_ctx.header[1]:0x%x, hciTranCb:0x%x,
        // hciTranCb->alloc_buf:0x%x\n",
        //        uart_ctx.header[1], hciTranCb, hciTranCb->alloc_buf);

        uart_ctx.rxBuf = (struct net_buf *)(hciTranCb->alloc_buf(
            IND_EVENT, uart_ctx.header[0], len));
        // uart_ctx.rxBuf = (struct net_buf*)bl_hci_alloc_buf(IND_EVENT,
        // uart_ctx.header[0], len);
        if (uart_ctx.rxBuf) {
          uart_ctx.maxReadLen = uart_ctx.header[1];
          uart_ctx.rxBuf->len = len;
          uart_ctx.ptr = (uint8_t *)(uart_ctx.rxBuf->data);
          // printf("ind_event, hdrlen:%d, len:%d, ptr:0x%x, maxRdlen:%d\n",
          //         uart_ctx.headerLen, len, uart_ctx.ptr,
          //         uart_ctx.maxReadLen);
        } else {
          BT_ERR("%s,Fail to allocate rx buffer for event\r\n", __func__);
          uart_ctx.buffType = 0;
          uart_ctx.ptr = &(uart_ctx.buffType);
          uart_ctx.maxReadLen = 1;
          uart_ctx.rxState = UART_GET_TYPE;
          break;
        }
        break;

      default: {
        BT_ERR("wrong type (%d)\r\n", uart_ctx.buffType);
        uart_ctx.buffType = 0;
        uart_ctx.ptr = &(uart_ctx.buffType);
        uart_ctx.maxReadLen = 1;
        uart_ctx.rxState = UART_GET_TYPE;
        break;
      }
      }

      memcpy(uart_ctx.ptr, &uart_ctx.header, uart_ctx.headerLen);

      uart_ctx.ptr += uart_ctx.headerLen;
      uart_ctx.rxState = UART_GET_DATA;
      bytesRead = 1;
      break;

    case UART_GET_DATA:
      if (uart_ctx.buffType == IND_EVENT) {
        hciTranCb->rx_handler(uart_ctx.rxBuf);
        // bl_hci_recv(uart_ctx.rxBuf);
      } else if (uart_ctx.buffType == IND_ACL_DATA) {
        hciTranCb->rx_handler(uart_ctx.rxBuf);
        // bl_hci_recv(uart_ctx.rxBuf);
      }
      /* Reset for next read*/
      uart_ctx.buffType = 0;
      uart_ctx.ptr = &(uart_ctx.buffType);
      uart_ctx.maxReadLen = 1;
      uart_ctx.rxState = UART_GET_TYPE;
      break;
    }
  } while (bytesRead);
}

static int32_t hci_uart_open(void) {
  if (uartInitialized == true) {
    return 0;
  }
  uartInitialized = true;

  xStreamBufferRX = xStreamBufferCreate(512, 1);
  if (xStreamBufferRX == NULL) {
    printf("create stream buffer failed\r\n");
    return -1;
  }

  uart_ctx.maxReadLen = 1;
  uart_ctx.buffType = 0;
  uart_ctx.ptr = &uart_ctx.buffType;
  uart_ctx.rxState = UART_GET_TYPE;

  hci_uart_init();

  xTaskCreate(UartReadThread, "hci_read", 1024, NULL, configMAX_PRIORITIES - 1,
                &uart_read_task_handle);

  return 0;
}
static int32_t bl_uart_close(void) {
  int ret = 0;

  if (!uartInitialized) {
    return -1;
  }
  uartInitialized = false;

  vStreamBufferDelete(xStreamBufferRX);

  return ret;
}

struct mdev_t *bl_hci_uart_init(const bl_hci_trans_callbacks_t *cb) {
  uart_dev.name = "uart";
  uart_dev.open = hci_uart_open;
  uart_dev.close = bl_uart_close;
  uart_dev.read = NULL;
  uart_dev.write = hci_uart_write;

  hciTranCb = cb;

  return &uart_dev;
}
