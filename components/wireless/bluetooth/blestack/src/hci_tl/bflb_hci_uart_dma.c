#include "bflb_gpio.h"
#include "bflb_uart.h"
#include "bflb_dma.h"
#include "bflb_clock.h"
#include "bt_log.h"
#include "bflb_hci_uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#define UART_NAME                  "uart1"
#define UART_BAUDRATE              115200
#define UART_TXD_PIN               23
#define UART_RXD_PIN               24
#define UART_CTS_PIN               25
#define UART_RTS_PIN               26

#define DEBUG_HCI_UART              0
#define UART_FLOW_CTRL_ENABLE       0
#define UART_RX_DMA_BUF_SIZE       512
#define STREAM_BUFFER_SIZE 512 
#define DMA_RX_NAME "dma0_ch2"
#define DMA_TX_NAME "dma0_ch3"

static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION uart_rx_dma_buf[UART_RX_DMA_BUF_SIZE];


static struct bflb_rx_cycle_dma g_uart_rx_dma;
static struct bflb_dma_channel_lli_pool_s rx_llipool[20];
static struct bflb_dma_channel_lli_pool_s tx_llipool[20];

static struct bflb_device_s *uart = NULL;
static struct bflb_device_s *dma_rx = NULL;
static struct bflb_device_s *dma_tx = NULL;


#define PRI_HCI_PROCESS (configMAX_PRIORITIES - 1)

#define STKSIZE_HCI_PROCESS 1024
static uint8_t uartInitialized = 0;
static struct k_thread uart_read_thread;
static struct mdev_t uart_dev;
UART_Context uart_ctx;
StreamBufferHandle_t xStreamBufferRX = NULL;

const static bflb_hci_trans_callbacks_t *hciTranCb;

static void UartReadThread(void *args) {
  int bytesRead = 0;
  uint16_t len;

  do {
    if (uart_ctx.rxState != UART_GET_BUFFER) {
      bytesRead = xStreamBufferReceive(xStreamBufferRX, uart_ctx.ptr, uart_ctx.maxReadLen, portMAX_DELAY);
      uart_ctx.ptr += bytesRead;
      if (bytesRead != uart_ctx.maxReadLen) {
        uart_ctx.maxReadLen = (uint16_t)(uart_ctx.maxReadLen) - bytesRead;
        continue;
      }
      //bflb_dma_channel_start(dma_rx);
    }
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
      uart_ctx.rxState = UART_GET_BUFFER;
    case UART_GET_BUFFER:
      switch (uart_ctx.buffType) {
      case IND_ACL_DATA:
        len =
            (uint16_t)(LEtoHost16(&(uart_ctx.header)[2])) + uart_ctx.headerLen;
        uart_ctx.rxBuf =
            (struct net_buf *)hciTranCb->alloc_buf(IND_ACL_DATA, 0, len);
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
        uart_ctx.rxBuf = (struct net_buf *)(hciTranCb->alloc_buf(
            IND_EVENT, uart_ctx.header[0], len));
        if (uart_ctx.rxBuf) {
          uart_ctx.maxReadLen = uart_ctx.header[1];
          uart_ctx.rxBuf->len = len;
          uart_ctx.ptr = (uint8_t *)(uart_ctx.rxBuf->data);
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
      } else if (uart_ctx.buffType == IND_ACL_DATA) {
        hciTranCb->rx_handler(uart_ctx.rxBuf);
      }
      uart_ctx.buffType = 0;
      uart_ctx.ptr = &(uart_ctx.buffType);
      uart_ctx.maxReadLen = 1;
      uart_ctx.rxState = UART_GET_TYPE;
      break;
    }
  } while (bytesRead);
}

static void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uart);

    if(intstatus & UART_INTSTS_RTO){
#if UART_FLOW_CTRL_ENABLE
        bflb_uart_feature_control(uart, UART_CMD_SET_RTS_VALUE, 0);
#endif

        bflb_uart_int_clear(uart, UART_INTCLR_RTO);
        bflb_rx_cycle_dma_process(&g_uart_rx_dma, 0);
    }
}

static void dma_rx_isr(void *arg)
{
#if UART_FLOW_CTRL_ENABLE
    bflb_uart_feature_control(uart, UART_CMD_SET_RTS_VALUE, 1);
#endif

    bflb_rx_cycle_dma_process(&g_uart_rx_dma, 1);
}

static void dma_tx_isr(void *arg)
{
    
}


static void dma_rx_copy(uint8_t *data, uint32_t len)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    int ret;
    if(xStreamBufferRX != NULL) {
        size_t xAvailableSpace = xStreamBufferSpacesAvailable(xStreamBufferRX);
        if (xAvailableSpace < len) {
            BT_ERR("Stream buffer overflow: available %d, need %d\r\n", xAvailableSpace, len);
            //bflb_dma_channel_stop(dma_rx);
        }
        else
        {
            ret = xStreamBufferSendFromISR(xStreamBufferRX, data, len, &xHigherPriorityTaskWoken);
            if(ret != len){
                BT_ERR("Stream buffer send failed: ret %d, need %d\r\n", ret, len);
                configASSERT(ret == len);
            }
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


static void gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, UART_TXD_PIN, GPIO_UART_FUNC_UART0_TX + 4 * uart->idx);
    bflb_gpio_uart_init(gpio, UART_RXD_PIN, GPIO_UART_FUNC_UART0_RX + 4 * uart->idx);

#if UART_FLOW_CTRL_ENABLE
    bflb_gpio_uart_init(gpio, UART_CTS_PIN, GPIO_UART_FUNC_UART0_CTS + 4 * uart->idx);
    bflb_gpio_uart_init(gpio, UART_RTS_PIN, GPIO_UART_FUNC_UART0_RTS + 4 * uart->idx);
#endif
}

static void uart_init(void)
{
    struct bflb_uart_config_s cfg = {
        .baudrate = UART_BAUDRATE,
        .data_bits = UART_DATA_BITS_8,
        .stop_bits = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .bit_order = UART_LSB_FIRST,
#if UART_FLOW_CTRL_ENABLE
        .flow_ctrl = 1,
#else
        .flow_ctrl = 0,
#endif
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };

    bflb_uart_init(uart, &cfg);
    bflb_uart_feature_control(uart, UART_CMD_SET_RTO_VALUE, 0x80);
    bflb_uart_link_txdma(uart, true);
    bflb_uart_link_rxdma(uart, true);
    bflb_irq_attach(uart->irq_num, uart_isr, NULL);
    bflb_irq_enable(uart->irq_num);
}

static void dma_rx_init(void)
{
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

    bflb_dma_channel_irq_attach(dma_rx, dma_rx_isr, NULL);

    bflb_rx_cycle_dma_init(&g_uart_rx_dma,
                           dma_rx,
                           rx_llipool,
                           sizeof(rx_llipool)/sizeof(rx_llipool[0]),
                           uart->reg_base + 0x8C,
                           uart_rx_dma_buf,
                           UART_RX_DMA_BUF_SIZE,
                           dma_rx_copy);

}

static void dma_tx_init(void)
{
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

    bflb_dma_channel_irq_attach(dma_tx, dma_tx_isr, NULL);
}

static void dma_init(void)
{
    PERIPHERAL_CLOCK_DMA0_ENABLE();

    dma_rx_init();
    dma_tx_init();
}


static int32_t hci_uart_open(void)
{
    if (uartInitialized == true) {
        return 0;
    }
    uartInitialized = true;
  
    uart = bflb_device_get_by_name(UART_NAME);
    dma_rx = bflb_device_get_by_name(DMA_RX_NAME);
    dma_tx = bflb_device_get_by_name(DMA_TX_NAME);

    
    xStreamBufferRX = xStreamBufferCreate(STREAM_BUFFER_SIZE, 1);
    if(xStreamBufferRX == NULL) {
        BT_ERR("create stream buffer failed\r\n");
        return -1;
    }
    uart_ctx.maxReadLen = 1;
    uart_ctx.buffType = 0;
    uart_ctx.ptr = &uart_ctx.buffType;
    uart_ctx.rxState = UART_GET_TYPE;
    gpio_init();
    uart_init();
    dma_init();
    /* Create thread for read */
  if (k_thread_create(&uart_read_thread, "uart_read_thread",
                      STKSIZE_HCI_PROCESS, UartReadThread, PRI_HCI_PROCESS)) {
    BT_ERR("Fail to Create UartReadThread\r\n");
    return -1;
  } else
  {
    BT_DBG("Create UartReadThread successfully\r\n");
    bflb_dma_channel_start(dma_rx);
  }
  return 0;
}


static int32_t hci_uart_write(uint8_t *buf, uint32_t nbytes)
{
  taskENTER_CRITICAL();
  struct bflb_dma_channel_lli_transfer_s transfer = {
      .src_addr = (uint32_t)buf,
      .dst_addr = uart->reg_base + 0x88,
      .nbytes = nbytes,
  };

  while(bflb_dma_channel_isbusy(dma_tx));

  #if defined(BL616) || defined(BL616D)
  bflb_l1c_dcache_clean_all();
  #endif

  bflb_dma_channel_lli_reload(dma_tx, tx_llipool, sizeof(tx_llipool)/sizeof(tx_llipool[0]), &transfer, 1);

  bflb_dma_channel_start(dma_tx);
  taskEXIT_CRITICAL();
  #if DEBUG_HCI_UART
  for (int i = 0; i < nbytes; i++) {
      printf("%02x ", buf[i]);
  }
  printf("\r\n");
  #endif
  return nbytes;
}

static int32_t hci_uart_close(void) {
  
  int ret = 0;

  if (!uartInitialized) {
    BT_ERR("ERROR!!!UART is not open");
    return -1;
  }

  k_thread_delete(&uart_read_thread);
  vStreamBufferDelete(xStreamBufferRX);
  uartInitialized = false;

  return ret;
}

struct mdev_t *bflb_hci_uart_init(const bflb_hci_trans_callbacks_t *cb) {
  uart_dev.name = "uart";
  uart_dev.open = hci_uart_open;
  uart_dev.close = hci_uart_close;
  uart_dev.read = NULL;
  uart_dev.write = hci_uart_write;

  hciTranCb = cb;
  BT_DBG("%s, hciTranCb:0x%p, hciTranCb->alloc_buf:0x%p\r\n", __func__, hciTranCb,
         hciTranCb->alloc_buf);
  return &uart_dev;
}
