#include "bflb_platform.h"
#include "hal_uart.h"
#include "at.h"
#include "ring_buffer.h"

#define UART_SEND_BUF_SZIE 1024
char send_buf[UART_SEND_BUF_SZIE] = { 0 };

#define UART_RX_RINGBUFFER_SIZE (1 * 1024)

uint8_t uart_rx_mem[UART_RX_RINGBUFFER_SIZE];

struct device *uart1;
Ring_Buffer_Type uart1_rx_rb;

static void uart1_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    Ring_Buffer_Write(&uart1_rx_rb, (uint8_t *)args, size);
}

uint32_t at_vprintfln(const char *format, va_list args)
{
    uint32_t len;

    len = vsnprintf(send_buf, sizeof(send_buf) - 2, format, args);
    if (len > sizeof(send_buf) - 2)
        len = sizeof(send_buf) - 2;
    memcpy(send_buf + len, "\r\n", 2);

    len = len + 2;
    MSG("%s\r\n", send_buf);
    device_control(uart1, DEVICE_CTRL_CLR_INT, (void *)(UART_RX_FIFO_IT | UART_RTO_IT));
    uart1->oflag &= ~DEVICE_OFLAG_INT_RX;
    return device_write(uart1, 0, (uint8_t *)send_buf, len);
}

int at_read_line(struct at_client *client)
{
    uint32_t read_len = 0;
    uint32_t start_time = 0;
    char ch = 0, last_ch = 0;

    memset(client->recv_line_buf, 0, client->recv_bufsz);
    client->recv_line_len = 0;

    start_time = bflb_platform_get_time_ms();
    while (1) {
        while (device_read(uart1, 0, &ch, 1) == 0) {
            if ((bflb_platform_get_time_ms() - start_time) > client->timeout) {
                client->resp_status = AT_RESP_TIMEOUT;
                return AT_RESP_TIMEOUT;
            }
        }

        if (read_len < client->recv_bufsz) {
            client->recv_line_buf[read_len++] = ch;
            client->recv_line_len = read_len;
        } else {
            MSG("buf full\r\n");
            client->resp_status = AT_RESP_BUFF_FULL;
            return AT_RESP_BUFF_FULL;
        }

        if (ch == '\n' && last_ch == '\r') {
            if (strstr((const char *)client->recv_line_buf, (const char *)client->resp_succ)) {
                client->resp_status = AT_RESP_OK;
                goto match_out;
            } else if (client->resp_err && strstr((const char *)client->recv_line_buf, (const char *)client->resp_err)) {
                client->resp_status = AT_RESP_ERROR;
                goto match_out;
            }
        }
        last_ch = ch;
    }

match_out:
    MSG("%s", client->recv_line_buf);
    return read_len;
}

int at_exe_cmd(struct at_client *client, char *cmd_expr, ...)
{
    va_list ap;

    va_start(ap, cmd_expr);
    at_vprintfln(cmd_expr, ap);
    va_end(ap);

    return at_read_line(client);
}

int at_write_recv(uint8_t *send_buffer, uint32_t send_len, uint8_t *recv_buffer, uint32_t *recv_len, uint32_t timeout)
{
    int ret = 0;
    uint8_t found = 0;
    uint32_t read_len = 0;
    uint32_t start_time = 0;
    uint8_t ch = 0, last_ch = 0;
    uint8_t tmp_buf[32] = { 0 };
    const char *found_string = "+IPD:0,";
    uint8_t found_index = 0;

    struct at_client static_client;

    static_client.recv_line_buf = tmp_buf;
    static_client.recv_bufsz = 32;
    static_client.resp_succ = ">";
    static_client.resp_err = "FAIL";
    static_client.timeout = 1000;
    ret = at_exe_cmd(&static_client, "AT+CIPSEND=0,%d", 114);
    if (ret < 0) {
        return -1;
    }

    Ring_Buffer_Reset(&uart1_rx_rb);
    device_control(uart1, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT | UART_RTO_IT));
    device_write(uart1, 0, (uint8_t *)send_buffer, send_len);

    if (recv_buffer == NULL) {
        return 0;
    }

    uint8_t *p = recv_buffer;
    start_time = bflb_platform_get_time_ms();
    while (1) {
        while (Ring_Buffer_Read_Byte(&uart1_rx_rb, &ch) == 0) {
            if ((bflb_platform_get_time_ms() - start_time) > timeout) {
                return AT_RESP_TIMEOUT;
            }
        }

        if (found == 0) {
            tmp_buf[found_index++] = ch;
            if (strstr((const char *)tmp_buf, found_string)) {
                found = 1;
                found_index = 0;
            }
        } else if (found == 1) {
            tmp_buf[found_index++] = ch;
            if (ch == '\n' && last_ch == '\r') {
                if (found_index == 3) {
                    read_len = (tmp_buf[0] - '0');
                } else if (found_index == 4) {
                    read_len = (tmp_buf[0] - '0') * 10 + (tmp_buf[1] - '0');
                } else if (found_index == 5) {
                    read_len = (tmp_buf[0] - '0') * 100 + (tmp_buf[1] - '0') * 10 + (tmp_buf[2] - '0');
                }
                *recv_len = read_len;
                found = 2;
            }
            last_ch = ch;
        } else {
            *p++ = ch;
            read_len--;
            if (read_len == 0) {
                return 0;
            }
        }
    }
    return 0;
}

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
void at_dump_hex(const void *ptr, uint32_t buflen)
{
    unsigned char *buf = (unsigned char *)ptr;
    int i, j;

    for (i = 0; i < buflen; i += 16) {
        MSG("%08X:", i);

        for (j = 0; j < 16; j++)
            if (i + j < buflen) {
                if ((j % 8) == 0) {
                    MSG("  ");
                }

                MSG("%02X ", buf[i + j]);
            } else
                MSG("   ");
        MSG(" ");

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                MSG("%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
        MSG("\n");
    }
}

void at_client_init()
{
    Ring_Buffer_Init(&uart1_rx_rb, uart_rx_mem, UART_RX_RINGBUFFER_SIZE, cpu_global_irq_disable, cpu_global_irq_enable);

    uart_register(UART1_INDEX, "uart1");
    uart1 = device_find("uart1");

    if (uart1) {
        UART_DEV(uart1)->baudrate = 115200;
        device_open(uart1, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_INT_RX);
        device_set_callback(uart1, uart1_irq_callback);
        device_control(uart1, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT | UART_RTO_IT));
        Ring_Buffer_Reset(&uart1_rx_rb);
    }
}