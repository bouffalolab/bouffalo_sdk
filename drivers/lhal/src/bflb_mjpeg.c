#include "bflb_mjpeg.h"
#include "hardware/mjpeg_reg.h"

static const uint16_t q_table_50_y[64] = {
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99
};

static const uint16_t q_table_50_uv[64] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};

static void bflb_mjpeg_set_yuv422_interleave_order(struct bflb_device_s *dev, uint8_t y0, uint8_t u0, uint8_t y1, uint8_t v0)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_HEADER_BYTE_OFFSET);
    regval &= ~MJPEG_REG_Y0_ORDER_MASK;
    regval &= ~MJPEG_REG_U0_ORDER_MASK;
    regval &= ~MJPEG_REG_Y1_ORDER_MASK;
    regval &= ~MJPEG_REG_V0_ORDER_MASK;

    regval |= (y0 << MJPEG_REG_Y0_ORDER_SHIFT);
    regval |= (u0 << MJPEG_REG_U0_ORDER_SHIFT);
    regval |= (y1 << MJPEG_REG_Y1_ORDER_SHIFT);
    regval |= (v0 << MJPEG_REG_V0_ORDER_SHIFT);

    putreg32(regval, reg_base + MJPEG_HEADER_BYTE_OFFSET);
}

static void bflb_mjpeg_set_framesize(struct bflb_device_s *dev, uint16_t x, uint16_t y)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = 0;
    regval |= (x << MJPEG_REG_FRAME_WBLK_SHIFT);
    regval |= (y << MJPEG_REG_FRAME_HBLK_SHIFT);
    putreg32(regval, reg_base + MJPEG_FRAME_SIZE_OFFSET);
}

void bflb_mjpeg_init(struct bflb_device_s *dev, const struct bflb_mjpeg_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_MJPEG_FORMAT(config->format));
    LHAL_PARAM_ASSERT(IS_MJPEG_QUALITY(config->quality));
    LHAL_PARAM_ASSERT(IS_MJPEG_RESOLUTION(config->resolution_x));
    LHAL_PARAM_ASSERT(IS_MJPEG_RESOLUTION(config->resolution_y));
    LHAL_PARAM_ASSERT(IS_MJPEG_ADDR(config->input_bufaddr0));
    LHAL_PARAM_ASSERT(IS_MJPEG_ADDR(config->input_bufaddr1));
    LHAL_PARAM_ASSERT(IS_MJPEG_ADDR(config->output_bufaddr));

#ifdef romapi_bflb_mjpeg_init
    romapi_bflb_mjpeg_init(dev, config);
#else
    uint32_t regval;
    uint32_t reg_base;
    uint16_t blocks;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_CONTROL_1_OFFSET);
    regval &= ~MJPEG_REG_MJPEG_ENABLE;
    putreg32(regval, reg_base + MJPEG_CONTROL_1_OFFSET);

    regval = 0;
    regval |= (3 << MJPEG_REG_W_XLEN_SHIFT); /* burst count 16 increment */
    regval |= MJPEG_REG_READ_FWRAP;
    regval |= MJPEG_REG_MJPEG_BIT_ORDER;

    switch (config->format) {
        case MJPEG_FORMAT_YUV422_YUYV:
            regval |= (3 << MJPEG_REG_YUV_MODE_SHIFT);
            bflb_mjpeg_set_yuv422_interleave_order(dev, 0, 1, 2, 3);
            bflb_mjpeg_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV422_YVYU:
            regval |= (3 << MJPEG_REG_YUV_MODE_SHIFT);
            bflb_mjpeg_set_yuv422_interleave_order(dev, 0, 3, 2, 1);
            bflb_mjpeg_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV422_UYVY:
            regval |= (3 << MJPEG_REG_YUV_MODE_SHIFT);
            bflb_mjpeg_set_yuv422_interleave_order(dev, 1, 0, 3, 2);
            bflb_mjpeg_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV422_VYUY:
            regval |= (3 << MJPEG_REG_YUV_MODE_SHIFT);
            bflb_mjpeg_set_yuv422_interleave_order(dev, 1, 2, 3, 0);
            bflb_mjpeg_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV422SP_NV16:
            regval |= (2 << MJPEG_REG_YUV_MODE_SHIFT);
            regval |= MJPEG_REG_ORDER_U_EVEN;
            bflb_mjpeg_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV422SP_NV61:
            regval |= (2 << MJPEG_REG_YUV_MODE_SHIFT);
            bflb_mjpeg_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV420SP_NV12:
            regval |= (0 << MJPEG_REG_YUV_MODE_SHIFT);
            regval |= MJPEG_REG_ORDER_U_EVEN;

            if (config->resolution_x % 16) {
                regval |= MJPEG_REG_LAST_HF_WBLK_DMY;
            }

            bflb_mjpeg_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 15) >> 4);
            break;
        case MJPEG_FORMAT_YUV420SP_NV21:
            regval |= (0 << MJPEG_REG_YUV_MODE_SHIFT);

            if (config->resolution_x % 16) {
                regval |= MJPEG_REG_LAST_HF_WBLK_DMY;
            }

            bflb_mjpeg_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 15) >> 3);
            break;
        case MJPEG_FORMAT_GRAY:
            regval |= (1 << MJPEG_REG_YUV_MODE_SHIFT);

            if (config->resolution_x % 16) {
                regval |= MJPEG_REG_LAST_HF_WBLK_DMY;
            }

            if (config->resolution_y % 16) {
                regval |= MJPEG_REG_LAST_HF_HBLK_DMY;
            }

            bflb_mjpeg_set_framesize(dev, (config->resolution_x + 7) >> 3, (config->resolution_y + 7) >> 3);
            break;

        default:
            break;
    }

    putreg32(regval, reg_base + MJPEG_CONTROL_1_OFFSET);

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval &= ~MJPEG_REG_MJPEG_WAIT_CYCLE_MASK;
    regval |= (0x100 << MJPEG_REG_MJPEG_WAIT_CYCLE_SHIFT);
    regval &= ~MJPEG_REG_MJPEG_SW_MODE;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);

    regval = getreg32(reg_base + MJPEG_SWAP_MODE_OFFSET);
    regval &= ~MJPEG_REG_W_SWAP_MODE;
    putreg32(regval, reg_base + MJPEG_SWAP_MODE_OFFSET);

    putreg32(config->input_bufaddr0, reg_base + MJPEG_YY_FRAME_ADDR_OFFSET);
    putreg32(config->input_bufaddr1, reg_base + MJPEG_UV_FRAME_ADDR_OFFSET);

    blocks = config->rows / 8;

    switch (config->format) {
        case MJPEG_FORMAT_YUV422_YUYV:
        case MJPEG_FORMAT_YUV422_YVYU:
        case MJPEG_FORMAT_YUV422_UYVY:
        case MJPEG_FORMAT_YUV422_VYUY:
            putreg32((0 << 16) + blocks, reg_base + MJPEG_YUV_MEM_OFFSET); /* uv << 16 + yy */
            break;
        case MJPEG_FORMAT_YUV422SP_NV16:
        case MJPEG_FORMAT_YUV422SP_NV61:
            putreg32((blocks << 16) + blocks, reg_base + MJPEG_YUV_MEM_OFFSET);
            break;
        case MJPEG_FORMAT_YUV420SP_NV12:
        case MJPEG_FORMAT_YUV420SP_NV21:
            putreg32((blocks << 16) + blocks, reg_base + MJPEG_YUV_MEM_OFFSET);
            break;
        case MJPEG_FORMAT_GRAY:
            putreg32((0 << 16) + blocks, reg_base + MJPEG_YUV_MEM_OFFSET);
            break;

        default:
            break;
    }

    putreg32(config->output_bufaddr, reg_base + MJPEG_JPEG_FRAME_ADDR_OFFSET);
    putreg32(config->output_bufsize / 128, reg_base + MJPEG_JPEG_STORE_MEMORY_OFFSET);

    regval = getreg32(reg_base + MJPEG_CONTROL_3_OFFSET);
    regval &= ~MJPEG_REG_INT_NORMAL_EN;
    regval &= ~MJPEG_REG_INT_CAM_EN;
    regval &= ~MJPEG_REG_INT_MEM_EN;
    regval &= ~MJPEG_REG_INT_FRAME_EN;
    regval &= ~MJPEG_REG_INT_IDLE_EN;
    regval &= ~MJPEG_REG_INT_SWAP_EN;
    regval &= ~MJPEG_REG_FRAME_CNT_TRGR_INT_MASK;
    regval |= (1 << MJPEG_REG_FRAME_CNT_TRGR_INT_SHIFT); /* trigger int with one frame */
    putreg32(regval, reg_base + MJPEG_CONTROL_3_OFFSET);

    regval = getreg32(reg_base + MJPEG_HEADER_BYTE_OFFSET);
    regval &= ~MJPEG_REG_HEAD_BYTE_MASK;
    regval &= ~MJPEG_REG_TAIL_EXP;
    putreg32(regval, reg_base + MJPEG_HEADER_BYTE_OFFSET);

    /* Clear interrupt */
    putreg32(0x3F00, reg_base + MJPEG_FRAME_FIFO_POP_OFFSET);

    uint16_t tmp_table_y[64] = { 0 };
    uint16_t tmp_table_uv[64] = { 0 };

    if (config->input_yy_table) {
        bflb_mjpeg_calculate_quantize_table(config->quality, config->input_yy_table, tmp_table_y);
    } else {
        bflb_mjpeg_calculate_quantize_table(config->quality, (uint16_t *)q_table_50_y, tmp_table_y);
    }
    if (config->input_uv_table) {
        bflb_mjpeg_calculate_quantize_table(config->quality, config->input_uv_table, tmp_table_uv);
    } else {
        bflb_mjpeg_calculate_quantize_table(config->quality, (uint16_t *)q_table_50_uv, tmp_table_uv);
    }

    bflb_mjpeg_fill_quantize_table(dev, tmp_table_y, tmp_table_uv);
#endif
}

void bflb_mjpeg_start(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjpeg_start
    romapi_bflb_mjpeg_start(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_CONTROL_1_OFFSET);
    regval |= MJPEG_REG_MJPEG_ENABLE;
    putreg32(regval, reg_base + MJPEG_CONTROL_1_OFFSET);
#endif
}

void bflb_mjpeg_stop(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjpeg_stop
    romapi_bflb_mjpeg_stop(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_CONTROL_1_OFFSET);
    regval &= ~MJPEG_REG_MJPEG_ENABLE;
    putreg32(regval, reg_base + MJPEG_CONTROL_1_OFFSET);
#endif
}

void bflb_mjpeg_sw_run(struct bflb_device_s *dev, uint8_t frame_count)
{
#ifdef romapi_bflb_mjpeg_sw_run
    romapi_bflb_mjpeg_sw_run(dev, frame_count);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval &= ~MJPEG_REG_SW_KICK_MODE;
    regval &= ~MJPEG_REG_SW_FRAME_MASK;
    regval |= (frame_count << MJPEG_REG_SW_FRAME_SHIFT);
    regval |= MJPEG_REG_MJPEG_SW_MODE;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval &= ~MJPEG_REG_MJPEG_SW_MODE;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval |= MJPEG_REG_MJPEG_SW_RUN;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval &= ~MJPEG_REG_MJPEG_SW_RUN;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);
#endif
}

void bflb_mjpeg_kick_run(struct bflb_device_s *dev, uint16_t kick_count)
{
#ifdef romapi_bflb_mjpeg_kick_run
    romapi_bflb_mjpeg_kick_run(dev, kick_count);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval |= MJPEG_REG_SW_KICK_MODE;
    regval &= ~MJPEG_REG_SW_FRAME_MASK;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval |= MJPEG_REG_MJPEG_SW_MODE;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval &= ~MJPEG_REG_MJPEG_SW_MODE;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);

    regval = getreg32(reg_base + MJPEG_YUV_MEM_SW_OFFSET);
    regval &= ~MJPEG_REG_SW_KICK_HBLK_MASK;
    regval |= (kick_count << MJPEG_REG_SW_KICK_HBLK_SHIFT);
    putreg32(regval, reg_base + MJPEG_YUV_MEM_SW_OFFSET);

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval |= MJPEG_REG_MJPEG_SW_RUN;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval &= ~MJPEG_REG_MJPEG_SW_RUN;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);
#endif
}

void bflb_mjpeg_kick_stop(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjpeg_kick_stop
    romapi_bflb_mjpeg_kick_stop(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval &= ~MJPEG_REG_SW_KICK_MODE;
    regval &= ~MJPEG_REG_SW_FRAME_MASK;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval |= MJPEG_REG_MJPEG_SW_MODE;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval &= ~MJPEG_REG_MJPEG_SW_MODE;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);
#endif
}

void bflb_mjpeg_kick(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjpeg_kick
    romapi_bflb_mjpeg_kick(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval |= MJPEG_REG_SW_KICK;
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);
#endif
}

void bflb_mjpeg_tcint_mask(struct bflb_device_s *dev, bool mask)
{
#ifdef romapi_bflb_mjpeg_tcint_mask
    romapi_bflb_mjpeg_tcint_mask(dev, mask);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_CONTROL_3_OFFSET);

    if (mask) {
        regval &= ~MJPEG_REG_INT_NORMAL_EN;
    } else {
        regval |= MJPEG_REG_INT_NORMAL_EN;
    }

    putreg32(regval, reg_base + MJPEG_CONTROL_3_OFFSET);
#endif
}

void bflb_mjpeg_errint_mask(struct bflb_device_s *dev, bool mask)
{
#ifdef romapi_bflb_mjpeg_errint_mask
    romapi_bflb_mjpeg_errint_mask(dev, mask);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_CONTROL_3_OFFSET);

    if (mask) {
        regval &= ~MJPEG_REG_INT_CAM_EN;
        regval &= ~MJPEG_REG_INT_MEM_EN;
        regval &= ~MJPEG_REG_INT_FRAME_EN;
        regval &= ~MJPEG_REG_INT_IDLE_EN;
        regval &= ~MJPEG_REG_INT_SWAP_EN;
    } else {
        regval |= MJPEG_REG_INT_CAM_EN;
        regval |= MJPEG_REG_INT_MEM_EN;
        regval |= MJPEG_REG_INT_FRAME_EN;
        regval |= MJPEG_REG_INT_IDLE_EN;
        regval |= MJPEG_REG_INT_SWAP_EN;
    }

    putreg32(regval, reg_base + MJPEG_CONTROL_3_OFFSET);
#endif
}

uint32_t bflb_mjpeg_get_intstatus(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjpeg_get_intstatus
    return romapi_bflb_mjpeg_get_intstatus(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_CONTROL_3_OFFSET);
    regval &= 0xf0;

    return regval;
#endif
}

void bflb_mjpeg_int_clear(struct bflb_device_s *dev, uint32_t int_clear)
{
#ifdef romapi_bflb_mjpeg_int_clear
    romapi_bflb_mjpeg_int_clear(dev, int_clear);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(int_clear, reg_base + MJPEG_FRAME_FIFO_POP_OFFSET);
#endif
}

uint8_t bflb_mjpeg_get_frame_count(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjpeg_get_frame_count
    return romapi_bflb_mjpeg_get_frame_count(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    return ((getreg32(reg_base + MJPEG_CONTROL_3_OFFSET) & MJPEG_FRAME_VALID_CNT_MASK) >> MJPEG_FRAME_VALID_CNT_SHIFT);
#endif
}

void bflb_mjpeg_pop_one_frame(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjpeg_pop_one_frame
    romapi_bflb_mjpeg_pop_one_frame(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(1, reg_base + MJPEG_FRAME_FIFO_POP_OFFSET);
#endif
}

uint32_t bflb_mjpeg_get_frame_info(struct bflb_device_s *dev, uint8_t **pic)
{
#ifdef romapi_bflb_mjpeg_get_frame_info
    return romapi_bflb_mjpeg_get_frame_info(dev, pic);
#else
    uint32_t bytes;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    *pic = (uint8_t *)(uintptr_t)getreg32(reg_base + MJPEG_START_ADDR0_OFFSET);
    bytes = (getreg32(reg_base + MJPEG_BIT_CNT0_OFFSET) + 7) >> 3;

    return bytes;
#endif
}

void bflb_mjpeg_calculate_quantize_table(uint8_t quality, uint16_t *input_table, uint16_t *output_table)
{
#ifdef romapi_bflb_mjpeg_calculate_quantize_table
    romapi_bflb_mjpeg_calculate_quantize_table(dev, input_table, output_table);
#else
    uint32_t scale_factor, i;

    if (quality == 0) {
        quality = 1;
    } else if (quality > 100) {
        quality = 100;
    }

    if (quality < 50) {
        scale_factor = 5000000 / quality;
    } else {
        scale_factor = 200000 - quality * 2000;
    }

    for (i = 0; i < 64; i++) {
        output_table[i] = (input_table[i] * scale_factor + 50000) / 100000;

        if (output_table[i] == 0) {
            output_table[i] = 1;
        } else if (output_table[i] > 0xff) {
            output_table[i] = 0xff;
        }
    }
#endif
}

void bflb_mjpeg_fill_quantize_table(struct bflb_device_s *dev, uint16_t *input_yy, uint16_t *input_uv)
{
#ifdef romapi_bflb_mjpeg_fill_quantize_table
    romapi_bflb_mjpeg_fill_quantize_table(dev, input_yy, input_uv);
#else
#define MJPEG_Q_PARAM_00_OFFSET (0x400)
#define MJPEG_Q_PARAM_40_OFFSET (0x480)

    uint8_t i, j;
    uint16_t tmp1;
    uint16_t tmp2;
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            tmp1 = 2048 / input_yy[16 * j + i];
            tmp2 = 2048 / input_yy[16 * j + i + 8];

            if (20480 / input_yy[16 * j + i] % 10 > 4) {
                tmp1++;
            }

            if (20480 / input_yy[16 * j + i + 8] % 10 > 4) {
                tmp2++;
            }

            putreg32(tmp1 | tmp2 << 16, reg_base + MJPEG_Q_PARAM_00_OFFSET + (i * 4 + j) * 4);
        }
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            tmp1 = 2048 / input_uv[16 * j + i];
            tmp2 = 2048 / input_uv[16 * j + i + 8];

            if (20480 / input_uv[16 * j + i] % 10 > 4) {
                tmp1++;
            }

            if (20480 / input_uv[16 * j + i + 8] % 10 > 4) {
                tmp2++;
            }

            putreg32(tmp1 | tmp2 << 16, reg_base + MJPEG_Q_PARAM_40_OFFSET + (i * 4 + j) * 4);
        }
    }

    regval = getreg32(reg_base + MJPEG_Q_ENC_OFFSET);
    regval |= MJPEG_REG_Q_SRAM_SW;
    putreg32(regval, reg_base + MJPEG_Q_ENC_OFFSET);
#endif
}

void bflb_mjpeg_fill_jpeg_header_tail(struct bflb_device_s *dev, uint8_t *header, uint32_t header_len)
{
#ifdef romapi_bflb_mjpeg_fill_jpeg_header_tail
    romapi_bflb_mjpeg_fill_jpeg_header_tail(dev, header, header_len);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    arch_memcpy_fast((void *)(uintptr_t)(reg_base + 0x800), header, header_len);

    regval = getreg32(reg_base + MJPEG_HEADER_BYTE_OFFSET);
    regval &= ~MJPEG_REG_HEAD_BYTE_MASK;
    regval |= (header_len << MJPEG_REG_HEAD_BYTE_SHIFT);
    regval |= MJPEG_REG_TAIL_EXP;
    putreg32(regval, reg_base + MJPEG_HEADER_BYTE_OFFSET);
#endif
}

void bflb_mjpeg_set_yuv420sp_cam_input(struct bflb_device_s *dev, uint8_t yy, uint8_t uv)
{
#ifdef romapi_bflb_mjpeg_set_yuv420sp_cam_input
    romapi_bflb_mjpeg_set_yuv420sp_cam_input(dev, yy, uv);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJPEG_CONTROL_2_OFFSET);
    regval &= ~MJPEG_REG_YY_DVP2AXI_SEL_MASK;
    regval &= ~MJPEG_REG_UV_DVP2AXI_SEL_MASK;
    regval |= (yy << MJPEG_REG_YY_DVP2AXI_SEL_SHIFT);
    regval |= (uv << MJPEG_REG_UV_DVP2AXI_SEL_SHIFT);
    putreg32(regval, reg_base + MJPEG_CONTROL_2_OFFSET);
#endif
}

void bflb_mjpeg_update_input_output_buff(struct bflb_device_s *dev, void *input_buf0, void *input_buf1, void *output_buff, size_t output_buff_size)
{
#ifdef romapi_bflb_mjpeg_update_input_output_buff
    romapi_bflb_mjpeg_update_input_output_buff(dev, input_buf0, input_buf1, output_buff, output_buff_size);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    if (input_buf0 != NULL) {
        putreg32((uint32_t)(uintptr_t)input_buf0, reg_base + MJPEG_YY_FRAME_ADDR_OFFSET);
    }

    if (input_buf1 != NULL) {
        putreg32((uint32_t)(uintptr_t)input_buf1, reg_base + MJPEG_UV_FRAME_ADDR_OFFSET);
    }

    if (output_buff != NULL) {
        putreg32((uint32_t)(uintptr_t)output_buff, reg_base + MJPEG_JPEG_FRAME_ADDR_OFFSET);
        putreg32((uint32_t)output_buff_size / 128, reg_base + MJPEG_JPEG_STORE_MEMORY_OFFSET);
    }
#endif
}

int bflb_mjpeg_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_mjpeg_feature_control
    return romapi_bflb_mjpeg_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    switch (cmd) {
        case MJPEG_CMD_SET_INPUTADDR0:
            putreg32(arg, reg_base + MJPEG_YY_FRAME_ADDR_OFFSET);
            break;
        case MJPEG_CMD_SET_INPUTADDR1:
            putreg32(arg, reg_base + MJPEG_UV_FRAME_ADDR_OFFSET);
            break;

        default:
            ret = -EPERM;
            break;
    }

    return ret;
#endif
}
