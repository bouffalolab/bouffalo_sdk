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
    /* fast speed = 40MHz / 1 / (1 + 7 + 2) = 4MHz */
    .timing_fast.prescaler = 0,
    .timing_fast.segment_1 = 6,
    .timing_fast.segment_2 = 1,
    .timing_fast.sync_jump_width = 1,
    .acf = (void *)0,
    .acf_count = 0,
    .mode = CANFD_MODE_NORMAL,
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

void print_rx_message(struct bflb_canfd_message_header_s *head)
{
    printf("received message:\r\n");
    printf("id = 0x%08lX\r\n", head->identifier);
    if (head->id_type == CANFD_ID_STANDARD) {
        printf("id_type = standard ID\r\n");
    } else if (head->id_type == CANFD_ID_EXTENDED) {
        printf("id_type = extended ID\r\n");
    } else {
        printf("id_type = unknown\r\n");
    }
    if (head->frame_type == CANFD_FRAME_DATA) {
        printf("frame_type = data frame\r\n");
    } else if (head->frame_type == CANFD_FRAME_REMOTE) {
        printf("frame_type = remote frame\r\n");
    } else {
        printf("frame_type = unknown\r\n");
    }
    if (head->error_state_indicator == CANFD_ESI_ACTIVE) {
        printf("error_state_indicator = active\r\n");
    } else if (head->error_state_indicator == CANFD_ESI_PASSIVE) {
        printf("error_state_indicator = passive\r\n");
    } else {
        printf("error_state_indicator = unknown\r\n");
    }
    if (head->bit_rate_switch == CANFD_BRS_OFF) {
        printf("bit_rate_switch = off\r\n");
    } else if (head->bit_rate_switch == CANFD_BRS_ON) {
        printf("bit_rate_switch = on\r\n");
    } else {
        printf("bit_rate_switch = unknown\r\n");
    }
    if (head->fd_format == CANFD_FORMAT_CLASSIC_CAN) {
        printf("fd_format = classic CAN\r\n");
    } else if (head->fd_format == CANFD_FORMAT_FD_CAN) {
        printf("fd_format = FD CAN\r\n");
    } else {
        printf("fd_format = unknown\r\n");
    }
    if (head->time_stamping_enable) {
        printf("time_stamping = %lld\r\n", head->time_stamping);
    } else {
        printf("time_stamping = not enabled\r\n");
    }
    printf("dlc=%d:\r\n", rx_head.data_length);
    for (int i = 0; i < rx_head.data_length; i++) {
        printf("%02X ", rx_buffer[i]);
    }
    printf("\r\n\r\n");
}

int main(void)
{
    board_init();
    canfd = bflb_device_get_by_name(BFLB_NAME_CAN1);
    printf("canfd_%d = 0x%08lX\r\n", canfd->idx, canfd);
    board_canfd_gpio_init(canfd->idx);
    for (int i = 0; i < sizeof(tx_buffer); i++) {
        tx_buffer[i] = i;
    }
    bflb_canfd_init(canfd, &config);

    while (1) {
        /* send data */
        bflb_canfd_write_stb(canfd, &tx_head, tx_buffer);
        bflb_canfd_exe_tx_cmd(canfd, CANFD_TX_CMD_STB_ONE_START);
        bflb_mtimer_delay_ms(1000);
        /* receive data */
        while (bflb_canfd_get_rx_buffer_state(canfd) != CANFD_RB_STATE_EMPTY) {
            bflb_canfd_read_rx_buffer(canfd, &rx_head, rx_buffer);
            print_rx_message(&rx_head);
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
