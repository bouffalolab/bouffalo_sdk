#include "board.h"
#include "bflb_canfd.h"

struct bflb_device_s *canfd;

uint8_t tx_buffer[64] = { 0 };
uint8_t rx_buffer[64] = { 0 };

struct bflb_canfd_config_s config = {
    /* fast speed = 40MHz / 2 / (1 + 31 + 8) = 500KHz */
    .timing_slow.prescaler = 1,
    .timing_slow.segment_1 = 30,
    .timing_slow.segment_2 = 7,
    .timing_slow.sync_jump_width = 3,
    /* fast speed = 40MHz / 1 / (1 + 5 + 2) = 5MHz */
    .timing_fast.prescaler = 0,
    .timing_fast.segment_1 = 4,
    .timing_fast.segment_2 = 1,
    .timing_fast.sync_jump_width = 1,
    .acf = (void *)0,
    .acf_count = 0,
    .mode = CANFD_MODE_EXTERNAL_LOOPBACK,
    .bosch_mode = 0,
};

struct bflb_canfd_message_header_s tx_head = {
    .identifier = 0x11225533,
    .id_type = CANFD_ID_EXTENDED,
    .frame_type = CANFD_FRAME_DATA,
    .data_length = sizeof(tx_buffer),
    .bit_rate_switch = CANFD_BRS_ON,
    .fd_format = CANFD_FORMAT_FD_CAN,
};
struct bflb_canfd_message_header_s rx_head;

int main(void)
{
    board_init();
    canfd = bflb_device_get_by_name(BFLB_NAME_CAN0);
    printf("canfd_%d = 0x%08lX\r\n", canfd->idx, canfd);
    for (int i = 0; i < sizeof(tx_buffer); i++) {
        tx_buffer[i] = i;
    }
    bflb_canfd_init(canfd, &config);
    if (config.mode == CANFD_MODE_EXTERNAL_LOOPBACK) {
        board_canfd_gpio_init(canfd->idx);
        bflb_canfd_feature_control(canfd, CANFD_CMD_SET_SELF_ACK_EXTERNAL_LOOPBACK, true);
        printf("You need to connect the TX pin and the RX pin of CANFD using wires.\r\n");
    }

    while (1) {
        /* send data */
        bflb_canfd_write_stb(canfd, &tx_head, tx_buffer);
        bflb_canfd_exe_tx_cmd(canfd, CANFD_TX_CMD_STB_ONE_START);
        bflb_mtimer_delay_ms(1000);
        /* receive data */
        while (bflb_canfd_get_rx_buffer_state(canfd) != CANFD_RB_STATE_EMPTY) {
            bflb_canfd_read_rx_buffer(canfd, &rx_head, rx_buffer);
            printf("\r\nreceived message: id=0x%X, dlc=%d, data:", rx_head.identifier, rx_head.data_length);
            for (int i = 0; i < rx_head.data_length; i++) {
                printf(" %02X", i, rx_buffer[i]);
            }
        }
        /* modify tx data */
        tx_head.identifier++;
        tx_head.data_length++;
        if (tx_head.data_length > sizeof(tx_buffer)) {
            tx_head.data_length = 1;
        }
        for (int i = 0; i < sizeof(tx_buffer); i++) {
            tx_buffer[i] += 1;
        }
    }
}
