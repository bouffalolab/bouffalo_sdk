/**
 ****************************************************************************************
 *
 * @file btblecontroller_port_uart_conf.c
 *
 * @brief BTBLE HCI UART transport driver, runtime-selectable IRQ or DMA mode.
 *
 * Selection is done at runtime via btble_uart_configure(): the dma_enable flag
 * picks between the interrupt-driven path and the DMA/RTO path. No compile-time
 * macro or weak-symbol override is required.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup UART
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "btblecontroller_port_uart.h"
#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_irq.h"
#include "bflb_dma.h"
#include "bflb_clock.h"
#if defined(CONFIG_IOT_SDK)
#include "bl_irq.h"
#endif
#if defined(BL616)
#include "bl616_l1c.h"
#endif
#if defined(BL618DG)
#include "bl618dg_glb.h"
#endif
#include "ring_buffer.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ll.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#if defined(BL618DG)
#define DMA_RX_NAME                "dma1_ch0"
#define DMA_TX_NAME                "dma1_ch1"
#else
#define DMA_RX_NAME                "dma0_ch2"
#define DMA_TX_NAME                "dma0_ch3"
#endif

#define UART_RX_DMA_BUF_SIZE       2048

#if defined(BL616) || defined(BL616CL)
#define BTBLE_UART_DEFAULT_ID       1
#define BTBLE_UART_DEFAULT_TX_PIN   27
#define BTBLE_UART_DEFAULT_RX_PIN   28
#define BTBLE_UART_DEFAULT_CTS_PIN  29
#define BTBLE_UART_DEFAULT_RTS_PIN  30
#define BTBLE_UART_DEFAULT_BAUDRATE 2000000
#define BTBLE_UART_DEFAULT_DMA      1
#define BTBLE_UART_DEFAULT_FLOW     1
#elif defined(BL618DG)
#define BTBLE_UART_DEFAULT_ID       1
#define BTBLE_UART_DEFAULT_TX_PIN   15
#define BTBLE_UART_DEFAULT_RX_PIN   16
#define BTBLE_UART_DEFAULT_CTS_PIN  17
#define BTBLE_UART_DEFAULT_RTS_PIN  18
#define BTBLE_UART_DEFAULT_BAUDRATE 2000000
#define BTBLE_UART_DEFAULT_DMA      1
#define BTBLE_UART_DEFAULT_FLOW     1
#else
#define BTBLE_UART_DEFAULT_ID       0
#define BTBLE_UART_DEFAULT_TX_PIN   14
#define BTBLE_UART_DEFAULT_RX_PIN   15
#define BTBLE_UART_DEFAULT_CTS_PIN  0xff
#define BTBLE_UART_DEFAULT_RTS_PIN  0xff
#define BTBLE_UART_DEFAULT_BAUDRATE 115200
#define BTBLE_UART_DEFAULT_DMA      0
#define BTBLE_UART_DEFAULT_FLOW     0
#endif

/*
 * STRUCT DEFINITIONS
 *****************************************************************************************
 */
/* TX and RX channel class holding data used for asynchronous read and write data
 * transactions.
 */
struct uart_txchannel {
    void (*callback)(void *, uint8_t);
    void *dummy;
    uint32_t remain_size;
    const uint8_t *remain_data;
};

struct uart_rxchannel {
    void (*callback)(void *, uint8_t);
    void *dummy;
    uint32_t remain_size;
    uint8_t *remain_data;
};

struct uart_env_tag {
    struct uart_txchannel tx;
    struct uart_rxchannel rx;
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
static struct uart_env_tag uart_env;
static struct bflb_device_s *btble_uart;
static volatile uint8_t uart_id;

static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION uart_rx_dma_buf[UART_RX_DMA_BUF_SIZE];
static Ring_Buffer_Type uartRB;
static uint8_t uartBuf[UART_RX_DMA_BUF_SIZE];
static struct bflb_rx_cycle_dma g_uart_rx_dma;
static struct bflb_dma_channel_lli_pool_s rx_llipool[20];
static struct bflb_dma_channel_lli_pool_s tx_llipool[20];
static struct bflb_device_s *dma_rx = NULL;
static struct bflb_device_s *dma_tx = NULL;

static btble_uart_config_t uart_config = {
    .uart_id = BTBLE_UART_DEFAULT_ID,
    .tx_pin = BTBLE_UART_DEFAULT_TX_PIN,
    .rx_pin = BTBLE_UART_DEFAULT_RX_PIN,
    .cts_pin = BTBLE_UART_DEFAULT_CTS_PIN,
    .rts_pin = BTBLE_UART_DEFAULT_RTS_PIN,
    .baudrate = BTBLE_UART_DEFAULT_BAUDRATE,
    .dma_enable = BTBLE_UART_DEFAULT_DMA,
    .flow_ctrl_enable = BTBLE_UART_DEFAULT_FLOW,
};
static bool uart_configured;
static bool uart_initialized;

static void btble_dma_uart_rx_event(void);
static void btble_dma_uart_tx_event(void);
void btble_uart_read_data_from_dma(void);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static void dma_rx_isr(void *arg)
{
    (void)arg;

    if (uart_config.flow_ctrl_enable) {
        bflb_uart_feature_control(btble_uart, UART_CMD_SET_RTS_VALUE, 1);
    }

    bflb_rx_cycle_dma_process(&g_uart_rx_dma, 1);
    btble_dma_uart_rx_event();
}

static void dma_tx_isr(void *arg)
{
    (void)arg;
    btble_dma_uart_tx_event();
}

static void dma_rx_copy(uint8_t *data, uint32_t len)
{
    Ring_Buffer_Write(&uartRB, data, len);
}

static void dma_rx_init(void)
{
    struct bflb_dma_channel_config_s rx_cfg = {
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_UART0_RX + 2 * btble_uart->idx,
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
                           sizeof(rx_llipool) / sizeof(rx_llipool[0]),
                           btble_uart->reg_base + 0x8C,
                           uart_rx_dma_buf,
                           UART_RX_DMA_BUF_SIZE,
                           dma_rx_copy);

    bflb_dma_channel_start(dma_rx);
}

static void dma_tx_init(void)
{
    struct bflb_dma_channel_config_s tx_cfg = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_UART0_TX + 2 * btble_uart->idx,
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
#if defined(BL618DG)
    GLB_Set_Peripheral_DMA_CN(GLB_PERI_DMA_UART1_TX, GLB_PERI_DMA_CN_SEL_DMA1);
    GLB_Set_Peripheral_DMA_CN(GLB_PERI_DMA_UART1_RX, GLB_PERI_DMA_CN_SEL_DMA1);
#else
    PERIPHERAL_CLOCK_DMA0_ENABLE();
#endif
    dma_rx_init();
    dma_tx_init();
}

static void btble_dma_uart_write(uint8_t *data, uint32_t len)
{
    struct bflb_dma_channel_lli_transfer_s transfer = {
        .src_addr = (uint32_t)data,
        .dst_addr = btble_uart->reg_base + 0x88,
        .nbytes = len,
    };

    while (bflb_dma_channel_isbusy(dma_tx)) {
    }

#if defined(BL616) || defined(BL618DG)
    bflb_l1c_dcache_clean_all();
#endif

    bflb_dma_channel_lli_reload(dma_tx, tx_llipool,
                                sizeof(tx_llipool) / sizeof(tx_llipool[0]),
                                &transfer, 1);
    bflb_dma_channel_start(dma_tx);
}

static uint32_t btble_dma_uart_get_rx_count(void)
{
    return Ring_Buffer_Get_Length(&uartRB);
}

static uint32_t btble_dma_uart_read(uint8_t *data, uint32_t len)
{
    uint32_t cnt = Ring_Buffer_Get_Length(&uartRB);

    if (cnt < len) {
        len = cnt;
    }

    Ring_Buffer_Read(&uartRB, data, len);
    return len;
}

static void uart_dma_isr(int irq, void *arg)
{
    (void)irq;
    (void)arg;

    uint32_t intstatus = bflb_uart_get_intstatus(btble_uart);

    if (intstatus & UART_INTSTS_RTO) {
        if (uart_config.flow_ctrl_enable) {
            bflb_uart_feature_control(btble_uart, UART_CMD_SET_RTS_VALUE, 0);
        }

        bflb_uart_int_clear(btble_uart, UART_INTCLR_RTO);
        bflb_rx_cycle_dma_process(&g_uart_rx_dma, 0);
        btble_dma_uart_rx_event();
    }
}

static void uart_isr(int irq, void *arg)
{
    (void)irq;
    (void)arg;

    uint8_t *p;
    void (*callback)(void *, uint8_t) = NULL;
    void *data = NULL;

    uint32_t intstatus = bflb_uart_get_intstatus(btble_uart);

    if (intstatus & (UART_INTSTS_RX_FIFO | UART_INTSTS_RTO)) {
        while (bflb_uart_rxavailable(btble_uart) && uart_env.rx.remain_size) {
            p = uart_env.rx.remain_data;
            *p = bflb_uart_getchar(btble_uart);
            p++;
            uart_env.rx.remain_size--;
            uart_env.rx.remain_data++;
        }

        if (intstatus & UART_INTSTS_RTO) {
            bflb_uart_int_clear(btble_uart, UART_INTCLR_RTO);
        }

        if (uart_env.rx.remain_size == 0) {
            bflb_uart_rxint_mask(btble_uart, true);

            callback = uart_env.rx.callback;
            data = uart_env.rx.dummy;
            if (callback != NULL) {
                uart_env.rx.callback = NULL;
                uart_env.rx.dummy = NULL;
                callback(data, 0);
            }
        }
    }
    if (intstatus & UART_INTSTS_TX_FIFO) {
        while (bflb_uart_txready(btble_uart) && uart_env.tx.remain_size) {
            p = (uint8_t *)uart_env.tx.remain_data;
            bflb_uart_putchar(btble_uart, *p);
            p++;
            uart_env.tx.remain_size--;
            uart_env.tx.remain_data++;
        }
        if (uart_env.tx.remain_size == 0) {
            bflb_uart_txint_mask(btble_uart, true);

            callback = uart_env.tx.callback;
            data = uart_env.tx.dummy;
            if (callback != NULL) {
                uart_env.tx.callback = NULL;
                uart_env.tx.dummy = NULL;
                callback(data, 0);
            }
        }
    }
}

static void btble_dma_uart_rx_event(void)
{
    if (btble_dma_uart_get_rx_count() == 0) {
        return;
    }
    btble_uart_read_data_from_dma();
}

static void btble_dma_uart_tx_event(void)
{
    void (*callback)(void *, uint8_t) = uart_env.tx.callback;
    void *data = uart_env.tx.dummy;

    if (callback != NULL) {
        uart_env.tx.callback = NULL;
        uart_env.tx.dummy = NULL;
        callback(data, 0);
    }
}

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

int btble_uart_configure(const btble_uart_config_t *config)
{
    if (config == NULL || uart_initialized || config->baudrate == 0 ||
        config->dma_enable > 1 || config->flow_ctrl_enable > 1) {
        return -1;
    }

    uart_config = *config;
    uart_configured = true;
    return 0;
}

void btble_uart_pin_config(uint8_t uartid, uint8_t tx, uint8_t rx, uint8_t cts, uint8_t rts)
{
    if (uart_initialized) {
        return;
    }

    uart_config.uart_id = uartid;
    uart_config.tx_pin = tx;
    uart_config.rx_pin = rx;
    uart_config.cts_pin = cts;
    uart_config.rts_pin = rts;
    uart_configured = true;
}

void btble_uart_init(uint8_t uartid)
{
    char uart_name[16];
    struct bflb_uart_config_s cfg = {0};
    struct bflb_device_s *gpio;

    if (!uart_configured) {
        uart_config.uart_id = uartid;
    }
    uart_id = uart_config.uart_id;

    snprintf(uart_name, sizeof(uart_name), "uart%d", uart_id);
    btble_uart = bflb_device_get_by_name(uart_name);
    if (btble_uart == NULL) {
        return;
    }

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, uart_config.tx_pin, GPIO_UART_FUNC_UART0_TX + 4 * uart_id);
    bflb_gpio_uart_init(gpio, uart_config.rx_pin, GPIO_UART_FUNC_UART0_RX + 4 * uart_id);
    if (uart_config.flow_ctrl_enable) {
        bflb_gpio_uart_init(gpio, uart_config.cts_pin, GPIO_UART_FUNC_UART0_CTS + 4 * uart_id);
        bflb_gpio_uart_init(gpio, uart_config.rts_pin, GPIO_UART_FUNC_UART0_RTS + 4 * uart_id);
    }

    cfg.baudrate = uart_config.baudrate;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.bit_order = UART_LSB_FIRST;
    cfg.flow_ctrl = uart_config.flow_ctrl_enable ? UART_FLOWCTRL_CTS : UART_FLOWCTRL_NONE;
    if (uart_config.dma_enable) {
        cfg.tx_fifo_threshold = 0;
        cfg.rx_fifo_threshold = 0;
    } else {
        cfg.tx_fifo_threshold = 7;
        cfg.rx_fifo_threshold = 7;
    }
    bflb_uart_init(btble_uart, &cfg);

    if (!uart_config.dma_enable) {
        bflb_uart_txint_mask(btble_uart, true);
        bflb_uart_rxint_mask(btble_uart, true);
#if defined(CONFIG_IOT_SDK)
        bl_irq_register_with_ctx(btble_uart->irq_num, uart_isr, NULL);
        bl_irq_enable(btble_uart->irq_num);
#else
        bflb_irq_attach(btble_uart->irq_num, uart_isr, NULL);
        bflb_irq_enable(btble_uart->irq_num);
#endif
    } else {
        bflb_uart_feature_control(btble_uart, UART_CMD_SET_RTO_VALUE, 0x80);
        bflb_uart_link_txdma(btble_uart, true);
        bflb_uart_link_rxdma(btble_uart, true);
        dma_rx = bflb_device_get_by_name(DMA_RX_NAME);
        dma_tx = bflb_device_get_by_name(DMA_TX_NAME);

        Ring_Buffer_Init(&uartRB, uartBuf, sizeof(uartBuf), NULL, NULL);
        dma_init();
#if defined(CONFIG_IOT_SDK)
        bl_irq_register_with_ctx(btble_uart->irq_num, uart_dma_isr, NULL);
        bl_irq_enable(btble_uart->irq_num);
#else
        bflb_irq_attach(btble_uart->irq_num, uart_dma_isr, NULL);
        bflb_irq_enable(btble_uart->irq_num);
#endif
    }

    uart_initialized = true;
}

int8_t btble_uart_reconfig(uint32_t baudrate, uint8_t flow_ctl_en, uint8_t cts_pin, uint8_t rts_pin)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_uart_disable(btble_uart);
    bflb_uart_feature_control(btble_uart, UART_CMD_SET_BAUD_RATE, baudrate);
    bflb_uart_enable(btble_uart);

    if (flow_ctl_en) {
        if (uart_id == 0) {
            bflb_gpio_uart_init(gpio, cts_pin, GPIO_UART_FUNC_UART0_CTS);
            bflb_gpio_uart_init(gpio, rts_pin, GPIO_UART_FUNC_UART0_RTS);
        } else if (uart_id == 1) {
            bflb_gpio_uart_init(gpio, cts_pin, GPIO_UART_FUNC_UART1_CTS);
            bflb_gpio_uart_init(gpio, rts_pin, GPIO_UART_FUNC_UART1_RTS);
        }
        btble_uart_flow_on();
    } else {
        btble_uart_flow_off();
    }
    return 0;
}

void btble_uart_flow_on(void)
{
    if (uart_config.flow_ctrl_enable) {
        bflb_uart_feature_control(btble_uart, UART_CMD_SET_SW_RTS_CONTROL, false);
    }
}

bool btble_uart_flow_off(void)
{
    if (uart_config.flow_ctrl_enable) {
        bflb_uart_feature_control(btble_uart, UART_CMD_SET_SW_RTS_CONTROL, true);
        bflb_uart_feature_control(btble_uart, UART_CMD_SET_CTS_EN, false);
    }
    return true;
}

void btble_uart_read_data_from_dma(void)
{
    void (*callback)(void *, uint8_t) = NULL;
    void *data = NULL;

    if (uart_env.rx.remain_size > 0) {
        uint16_t data_len = btble_dma_uart_read(uart_env.rx.remain_data,
                                                 (uint16_t)uart_env.rx.remain_size);
        uart_env.rx.remain_data += data_len;
        uart_env.rx.remain_size -= data_len;
        if (uart_env.rx.remain_size == 0) {
            callback = uart_env.rx.callback;
            data = uart_env.rx.dummy;
            if (callback != NULL) {
                uart_env.rx.callback = NULL;
                uart_env.rx.dummy = NULL;
                callback(data, 0);
            }
        }
    }
}

void btble_uart_write(const uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
    if (!bufptr || !size) {
        return;
    }

    uart_env.tx.remain_data = bufptr;
    uart_env.tx.remain_size = size;
    uart_env.tx.callback = callback;
    uart_env.tx.dummy = dummy;

    if (uart_config.dma_enable) {
        btble_dma_uart_write((uint8_t *)bufptr, size);
    } else {
        bflb_uart_txint_mask(btble_uart, false);
    }
}

void btble_uart_read(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
    void (*read_callback)(void *, uint8_t);
    void *read_dummy;
    uintptr_t irq_flags;

    if (!bufptr || !size) {
        return;
    }

    uart_env.rx.remain_data = bufptr;
    uart_env.rx.remain_size = size;
    uart_env.rx.callback = callback;
    uart_env.rx.dummy = dummy;

    if (uart_config.dma_enable) {
        GLOBAL_INT_DISABLE();
        btble_uart_read_data_from_dma();
        GLOBAL_INT_RESTORE();
        return;
    }

    if (size < 8) {
        bflb_uart_feature_control(btble_uart, UART_CMD_SET_RX_FIFO_THREHOLD, size - 1);
    } else {
        bflb_uart_feature_control(btble_uart, UART_CMD_SET_RX_FIFO_THREHOLD, 7);
    }

    irq_flags = bflb_irq_save();
    while (bflb_uart_rxavailable(btble_uart) && uart_env.rx.remain_size) {
        *uart_env.rx.remain_data++ = bflb_uart_getchar(btble_uart);
        uart_env.rx.remain_size--;
    }
    if (uart_env.rx.remain_size == 0) {
        bflb_uart_rxint_mask(btble_uart, true);
        read_callback = uart_env.rx.callback;
        read_dummy = uart_env.rx.dummy;
        uart_env.rx.callback = NULL;
        uart_env.rx.dummy = NULL;
        if (read_callback != NULL) {
            read_callback(read_dummy, 0);
        }
    } else {
        bflb_uart_rxint_mask(btble_uart, false);
    }
    bflb_irq_restore(irq_flags);
}

void btble_uart_finish_transfers(void)
{
    while (!bflb_uart_txempty(btble_uart)) {
    }
}
