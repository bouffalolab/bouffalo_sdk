/**
 ****************************************************************************************
 *
 * @file uart.c
 *
 * @brief UART driver
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
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
#include "btblecontroller_port_uart.h" // uart definition

/*
 * DEFINES
 *****************************************************************************************
 */

/*
 * STRUCT DEFINITIONS
 *****************************************************************************************
 */
/* TX and RX channel class holding data used for asynchronous read and write data
 * transactions
 */
/// UART TX RX Channel
struct uart_txchannel {
    /// call back function pointer
    void (*callback)(void *, uint8_t);
    /// Dummy data pointer returned to callback when operation is over.
    void *dummy;
    uint32_t remain_size;
    const uint8_t *remain_data;
};

struct uart_rxchannel {
    /// call back function pointer
    void (*callback)(void *, uint8_t);
    /// Dummy data pointer returned to callback when operation is over.
    void *dummy;
    uint32_t remain_size;
    uint8_t *remain_data;
};

/// UART environment structure
struct uart_env_tag {
    /// tx channel
    struct uart_txchannel tx;
    /// rx channel
    struct uart_rxchannel rx;
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#define UART_GLITCH_TEST_ENABLE

/// uart environment structure
static struct uart_env_tag uart_env;
static struct bflb_device_s *btble_uart;
static volatile uint8_t uart_id;

static void uart_isr(int irq, void *arg)
{
    uint8_t *p;
    void (*callback)(void *, uint8_t) = NULL;
    void *data = NULL;

    uint32_t intstatus = bflb_uart_get_intstatus(btble_uart);

    if (intstatus & UART_INTSTS_RX_FIFO) {
        while (bflb_uart_rxavailable(btble_uart) && uart_env.rx.remain_size) {
            p = uart_env.rx.remain_data;
            *p = bflb_uart_getchar(btble_uart);
            p++;
            uart_env.rx.remain_size--;
            uart_env.rx.remain_data++;
        }

        if (uart_env.rx.remain_size == 0) {
            bflb_uart_rxint_mask(btble_uart, true);

            callback = uart_env.rx.callback;
            data = uart_env.rx.dummy;
            if (callback != NULL) {
                // Clear callback pointer
                uart_env.rx.callback = NULL;
                uart_env.rx.dummy = NULL;

                // Call handler
                callback(data, 0);
            }
        }
    }
    if (intstatus & UART_INTSTS_RTO) {
        while (bflb_uart_rxavailable(btble_uart) && uart_env.rx.remain_size) {
            p = uart_env.rx.remain_data;
            *p = bflb_uart_getchar(btble_uart);
            p++;
            uart_env.rx.remain_size--;
            uart_env.rx.remain_data++;
        }

        if (uart_env.rx.remain_size == 0) {
            bflb_uart_rxint_mask(btble_uart, true);

            callback = uart_env.rx.callback;
            data = uart_env.rx.dummy;
            if (callback != NULL) {
                // Clear callback pointer
                uart_env.rx.callback = NULL;
                uart_env.rx.dummy = NULL;

                // Call handler
                callback(data, 0);
            }
        }
        bflb_uart_int_clear(btble_uart, UART_INTCLR_RTO);
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
                // Clear callback pointer
                uart_env.tx.callback = NULL;
                uart_env.tx.dummy = NULL;

                // Call handler
                callback(data, 0);
            }
        }
    }
}

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

__attribute__((weak)) void btble_uart_pin_config(uint8_t uartid, uint8_t tx, uint8_t rx, uint8_t cts, uint8_t rts)
{
    struct bflb_device_s *gpio;
    gpio = bflb_device_get_by_name("gpio");

    if (uartid == 0) {
        bflb_gpio_uart_init(gpio, tx, GPIO_UART_FUNC_UART0_TX);
        bflb_gpio_uart_init(gpio, rx, GPIO_UART_FUNC_UART0_RX);

        if (cts != 0xff) {
            bflb_gpio_uart_init(gpio, cts, GPIO_UART_FUNC_UART0_CTS);
        }
        if (rts != 0xff) {
            bflb_gpio_uart_init(gpio, rts, GPIO_UART_FUNC_UART0_RTS);
        }
    } else if (uartid == 1) {
        bflb_gpio_uart_init(gpio, tx, GPIO_UART_FUNC_UART1_TX);
        bflb_gpio_uart_init(gpio, rx, GPIO_UART_FUNC_UART1_RX);

        if (cts != 0xff) {
            bflb_gpio_uart_init(gpio, cts, GPIO_UART_FUNC_UART1_CTS);
        }
        if (rts != 0xff) {
            bflb_gpio_uart_init(gpio, rts, GPIO_UART_FUNC_UART1_RTS);
        }
    }
}

__attribute__((weak)) void btble_uart_init(uint8_t uartid)
{
    char uart_name[64];
    struct bflb_uart_config_s cfg;

    uart_id = uartid;
    sprintf((char *)uart_name, "uart%d", uartid);
    btble_uart = bflb_device_get_by_name(uart_name);

    bflb_uart_txint_mask(btble_uart, true);
    bflb_uart_rxint_mask(btble_uart, true);

    cfg.baudrate = 115200;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    bflb_uart_init(btble_uart, &cfg);
#ifdef UART_GLITCH_TEST_ENABLE
    bflb_uart_feature_control(btble_uart, UART_CMD_SET_GLITCH_VALUE, 12);
#endif
    bflb_irq_attach(btble_uart->irq_num, uart_isr, NULL);
    bflb_irq_enable(btble_uart->irq_num);
}

__attribute__((weak)) int8_t btble_uart_reconfig(uint32_t baudrate, uint8_t flow_ctl_en, uint8_t cts_pin, uint8_t rts_pin)
{
    struct bflb_device_s *gpio;
    gpio = bflb_device_get_by_name("gpio");

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

__attribute__((weak)) void btble_uart_flow_on(void)
{
    bflb_uart_feature_control(btble_uart, UART_CMD_SET_SW_RTS_CONTROL, false);
    //bflb_uart_feature_control(btble_uart, UART_CMD_SET_CTS_EN, true);
}

__attribute__((weak)) bool btble_uart_flow_off(void)
{
    bflb_uart_feature_control(btble_uart, UART_CMD_SET_SW_RTS_CONTROL, true);
    bflb_uart_feature_control(btble_uart, UART_CMD_SET_CTS_EN, false);
    return true;
}

__attribute__((weak)) void btble_uart_write(const uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
    if (!bufptr || !size) {
        return;
    }
    uart_env.tx.remain_data = bufptr;
    uart_env.tx.remain_size = size;
    uart_env.tx.callback = callback;
    uart_env.tx.dummy = dummy;

    bflb_uart_txint_mask(btble_uart, false);
}

__attribute__((weak)) void btble_uart_read(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
    if (!bufptr || !size) {
        return;
    }
    uart_env.rx.remain_data = bufptr;
    uart_env.rx.remain_size = size;
    uart_env.rx.callback = callback;
    uart_env.rx.dummy = dummy;

    if (size < 8) {
        bflb_uart_feature_control(btble_uart, UART_CMD_SET_RX_FIFO_THREHOLD, size - 1);
    } else {
        bflb_uart_feature_control(btble_uart, UART_CMD_SET_RX_FIFO_THREHOLD, 7);
    }
    bflb_uart_rxint_mask(btble_uart, false);
}

__attribute__((weak)) void btble_uart_finish_transfers(void)
{
    bflb_uart_feature_control(btble_uart, UART_CMD_SET_SW_RTS_CONTROL, true);
    while (!bflb_uart_txempty(btble_uart)) {}
}
