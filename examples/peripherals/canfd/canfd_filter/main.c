#include "board.h"
#include "bflb_canfd.h"

struct bflb_device_s *canfd;

uint8_t tx_buffer[64] = { 0 };
uint8_t rx_buffer[64] = { 0 };

struct bflb_canfd_acf_s acf[] = {
{   /* accept all ID type and ID must be 0x1 */
    .index = 0,
    .enable = true,
    .id_type = CANFD_ACF_STANDARD_AND_EXTENDED,
    .mask = 0x0,
    .code = 0x1,
}, {/* only accept extended ID type and ID must be 0x2 */
    .index = 1,
    .enable = true,
    .id_type = CANFD_ACF_EXTENDED_ONLY,
    .mask = 0x0,
    .code = 0x2,
}, {/* only accept standard ID type and ID must be 0x4 */
    .index = 2,
    .enable = true,
    .id_type = CANFD_ACF_STANDARD_ONLY,
    .mask = 0x0,
    .code = 0x4,
}, {/* accept all ID type and ID must be in 0x8~0x9 */
    .index = 3,
    .enable = true,
    .id_type = CANFD_ACF_STANDARD_AND_EXTENDED,
    .mask = 0x1,
    .code = 0x8,
}, {/* only accept extended ID type and ID must be 0x10 */
    .index = 4,
    .enable = true,
    .id_type = CANFD_ACF_EXTENDED_ONLY,
    .mask = 0x0,
    .code = 0x10,
}, {/* only accept standard ID type and ID must be 0x20 */
    .index = 5,
    .enable = true,
    .id_type = CANFD_ACF_STANDARD_ONLY,
    .mask = 0x0,
    .code = 0x20,
}, {/* accept all ID type and ID must be in 0x40~0x41 */
    .index = 6,
    .enable = true,
    .id_type = CANFD_ACF_STANDARD_AND_EXTENDED,
    .mask = 0x1,
    .code = 0x40,
}, {/* only accept extended ID type and ID must be 0x80/0x82 */
    .index = 7,
    .enable = true,
    .id_type = CANFD_ACF_EXTENDED_ONLY,
    .mask = 0x2,
    .code = 0x80,
}, {/* only accept standard ID type and ID must be in 0x100~0x103 */
    .index = 8,
    .enable = true,
    .id_type = CANFD_ACF_STANDARD_ONLY,
    .mask = 0x3,
    .code = 0x100,
}, {/* accept all ID type and ID must be 0x200 */
    .index = 9,
    .enable = true,
    .id_type = CANFD_ACF_STANDARD_AND_EXTENDED,
    .mask = 0x0,
    .code = 0x200,
}, {/* only accept extended ID type and ID must be in 0x400~0x403 */
    .index = 10,
    .enable = true,
    .id_type = CANFD_ACF_EXTENDED_ONLY,
    .mask = 0x3,
    .code = 0x400,
}, {/* only accept extended ID type and ID must be 0x800/0x802 */
    .index = 11,
    .enable = true,
    .id_type = CANFD_ACF_EXTENDED_ONLY,
    .mask = 0x2,
    .code = 0x800,
}, {/* only accept extended ID type and ID must be in 0x1000~0x1001 */
    .index = 12,
    .enable = true,
    .id_type = CANFD_ACF_EXTENDED_ONLY,
    .mask = 0x1,
    .code = 0x1000,
}, {/* only accept extended ID type and ID must be 0x2000/0x2004 */
    .index = 13,
    .enable = true,
    .id_type = CANFD_ACF_EXTENDED_ONLY,
    .mask = 0x4,
    .code = 0x2000,
}, {/* only accept extended ID type and ID must be 0x4000/0x4100 */
    .index = 14,
    .enable = true,
    .id_type = CANFD_ACF_EXTENDED_ONLY,
    .mask = 0x100,
    .code = 0x4000,
}, {/* only accept extended ID type and ID must be in 0x8000~0x80FF */
    .index = 15,
    .enable = true,
    .id_type = CANFD_ACF_EXTENDED_ONLY,
    .mask = 0xFF,
    .code = 0x8000,
},
};

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
    .acf = acf,
    .acf_count = sizeof(acf) / sizeof(acf[0]),
    .mode = CANFD_MODE_NORMAL,
    .bosch_mode = 0,
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

void canfd_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_canfd_get_intstatus(canfd);
    printf("intstatus = 0x%08lX\r\n", intstatus);

    if (intstatus & CANFD_INTSTS_RB_AVAILABLE) {
        bflb_canfd_int_clear(canfd, CANFD_INTCLR_RB_AVAILABLE);
        bflb_canfd_read_rx_buffer(canfd, &rx_head, rx_buffer);
        print_rx_message(&rx_head);
    }
}

int main(void)
{
    board_init();
    canfd = bflb_device_get_by_name(BFLB_NAME_CAN1);
    printf("canfd_%d = 0x%08lX\r\n", canfd->idx, canfd);
    board_canfd_gpio_init(canfd->idx);

    bflb_canfd_init(canfd, &config);
    bflb_canfd_int_enable(canfd, CANFD_INTEN_RB_AVAILABLE);
    bflb_irq_attach(canfd->irq_num, canfd_isr, NULL);
    bflb_irq_enable(canfd->irq_num);

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
