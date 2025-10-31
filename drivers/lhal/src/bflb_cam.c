#include "bflb_cam.h"
#include "hardware/cam_reg.h"
#if !defined(BL702)
#include "hardware/cam_front_reg.h"
#include "bflb_clock.h"
#endif

#if defined(BL616) || defined(BL616L)
#define CAM_FRONT_BASE 0x20050000
#endif
#if defined(BL616D)
#define CAM_FRONT_BASE 0x20040000
#endif
#if defined(BL808)
#define CAM_FRONT_BASE 0x30010000
#endif

void bflb_cam_init(struct bflb_device_s *dev, const struct bflb_cam_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_CAM_INPUT_FORMAT(config->input_format));
    LHAL_PARAM_ASSERT(IS_CAM_INPUT_SOURCE(config->input_source));
    LHAL_PARAM_ASSERT(IS_CAM_OUTPUT_FORMAT(config->output_format));
    LHAL_PARAM_ASSERT(IS_CAM_ADDR(config->output_bufaddr));

#ifdef romapi_bflb_cam_init
    romapi_bflb_cam_init(dev, config);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint8_t data_mode = 0;
    uint16_t resolution_x, resolution_y;
    uint32_t frame_size;
#if !defined(BL702)
    uint32_t threshold;
#endif
#if defined(BL808) || defined(BL616D)
    uint32_t tmpval;
#endif

#if defined(BL808)
    if (config->input_source) {
        tmpval = 0x15;
        regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_PIX_DATA_CTRL_OFFSET);
        regval |= CAM_FRONT_REG_ISP_DTSRC_SRC;
        putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_PIX_DATA_CTRL_OFFSET);

        regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_ADJA_CTRL_2_OFFSET);
        regval |= 1;
        putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_ADJA_CTRL_2_OFFSET);
    } else {
        tmpval = 0x24;
        regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_PIX_DATA_CTRL_OFFSET);
        regval &= ~CAM_FRONT_REG_ISP_DTSRC_SRC;
        putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_PIX_DATA_CTRL_OFFSET);
    }
#endif

    reg_base = dev->reg_base;
    putreg32(config->output_bufaddr, reg_base + CAM_DVP2AXI_ADDR_START_OFFSET);
    putreg32(config->resolution_y << 16 | config->resolution_x, reg_base + CAM_DVP2AXI_FRAM_EXM_OFFSET);
#if !defined(BL616D)
    putreg32(data_mode, reg_base + CAM_DVP_DEBUG_OFFSET);
#endif

    regval = getreg32(reg_base + CAM_DVP2AXI_HSYNC_CROP_OFFSET);
#if defined(BL702)
    if ((regval & 0xffff) > 2 * config->resolution_x) {
        resolution_x = config->resolution_x;
    } else {
        resolution_x = ((regval & 0xffff) - (regval >> 16 & 0xffff)) / 2;
    }
#else
    if ((regval & 0xffff) > config->resolution_x) {
        resolution_x = config->resolution_x;
    } else {
        resolution_x = (regval & 0xffff) - (regval >> 16 & 0xffff);
    }
#endif
    regval = getreg32(reg_base + CAM_DVP2AXI_VSYNC_CROP_OFFSET);
    if ((regval & 0xffff) > config->resolution_y) {
        resolution_y = config->resolution_y;
    } else {
        resolution_y = (regval & 0xffff) - (regval >> 16 & 0xffff);
    }

#if defined(BL616) || defined(BL616L)
    putreg32(0, CAM_FRONT_BASE + CAM_FRONT_DVP2BUS_SRC_SEL_1_OFFSET);
#elif defined(BL616D)
    regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_DVP_MUX_SEL_REG_OFFSET);
    regval &= ~(dev->idx == 0 ? CAM_FRONT_REG_D2XA_IN_SEL_MASK : CAM_FRONT_REG_D2XB_IN_SEL_MASK);
    putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_DVP_MUX_SEL_REG_OFFSET);
#endif

#if !defined(BL702)
#if defined(BL808)
    if (config->input_source != CAM_INPUT_SOURCE_CSI) {
#endif

        regval = bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_CAMERA, 0) / 1000000;

        if (regval == 0) {
            regval = 80;
        }
        threshold = config->resolution_x - (config->resolution_x * (config->pixel_clock / 1000000) / regval) / 2 + 10;

        if (threshold > (config->resolution_x - 1)) {
            threshold = config->resolution_x - 1;
        }

        if (threshold < 2) {
            threshold = 2;
        } else if (threshold > 1024) {
            threshold = 1024;
        }

        regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
        regval &= ~CAM_FRONT_RG_DVPAS_FIFO_TH_MASK;
        regval |= threshold << CAM_FRONT_RG_DVPAS_FIFO_TH_SHIFT;
        putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
#if defined(BL808)
    }
#endif
#endif

    /* Set output format */
    frame_size = resolution_x * resolution_y * 2;
    regval = getreg32(reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
    if (config->with_mjpeg) {
#if defined(BL616D)
        regval |= CAM_REG_WRAP_MODE;
#else
        regval &= ~CAM_REG_SW_MODE;
#endif
    } else {
#if defined(BL616D)
        regval &= ~CAM_REG_WRAP_MODE;
#else
        regval |= CAM_REG_SW_MODE;
#endif
    }
#if defined(BL702)
    regval |= CAM_REG_INTERLV_MODE;
    regval &= ~(CAM_REG_DROP_EN | CAM_REG_DROP_EVEN | CAM_REG_SUBSAMPLE_EN | CAM_REG_SUBSAMPLE_EVEN);
#else
    regval &= ~(CAM_REG_DROP_EN | CAM_REG_DROP_EVEN | CAM_REG_DVP_DATA_MODE_MASK | CAM_REG_DVP_DATA_BSEL | CAM_REG_V_SUBSAMPLE_EN | CAM_REG_V_SUBSAMPLE_POL);
#endif
    switch (config->input_format) {
        case CAM_INPUT_FORMAT_YUV422_YUYV:
#if defined(BL808)
            if (config->output_format >= CAM_OUTPUT_FORMAT_RGB888_OR_BGR888 && config->output_format <= CAM_OUTPUT_FORMAT_RGB888_TO_RGBA8888) {
                tmpval = 0x23;
                if (config->input_source) {
                    putreg32(0x18000000, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_0_OFFSET);
                } else {
                    putreg32(0xa8000000, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_0_OFFSET);
                }
                putreg32(0xff80, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_1_OFFSET);
                putreg32(0xff80, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_2_OFFSET);
                putreg32(0x200, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_3_OFFSET);
                putreg32(0x20002ce, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_4_OFFSET);
                putreg32(0xfe92ff50, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_5_OFFSET);
                putreg32(0x38b0200, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_6_OFFSET);
                putreg32(0, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_7_OFFSET);

                if (config->output_format == CAM_OUTPUT_FORMAT_RGB888_OR_BGR888) {
                    data_mode = 1;
                    frame_size = resolution_x * resolution_y * 3;
                    break;
                } else if (config->output_format == CAM_OUTPUT_FORMAT_RGB888_TO_RGB565) {
                    data_mode = 2;
                    frame_size = resolution_x * resolution_y * 2;
                    putreg32(5 << CAM_REG_FORMAT_565_SHIFT, reg_base + CAM_DVP2AXI_MISC_OFFSET);
                    break;
                } else if (config->output_format == CAM_OUTPUT_FORMAT_RGB888_TO_BGR565) {
                    data_mode = 2;
                    frame_size = resolution_x * resolution_y * 2;
                    putreg32(0, reg_base + CAM_DVP2AXI_MISC_OFFSET);
                    break;
                } else if (config->output_format == CAM_OUTPUT_FORMAT_RGB888_TO_RGBA8888) {
                    data_mode = 3;
                    frame_size = resolution_x * resolution_y * 4;
                    break;
                }
            }
#endif
        case CAM_INPUT_FORMAT_YUV422_YVYU:
            if (config->output_format == CAM_OUTPUT_FORMAT_AUTO || config->output_format == CAM_OUTPUT_FORMAT_YUV422) {
                data_mode = 0;
                frame_size = resolution_x * resolution_y * 2;
            } else if (config->output_format == CAM_OUTPUT_FORMAT_GRAY) {
#if defined(BL702)
                regval |= CAM_REG_DROP_EN;
#endif
                data_mode = 4;
                frame_size = resolution_x * resolution_y;
            } else if (config->output_format == CAM_OUTPUT_FORMAT_YUV422_UV) {
#if defined(BL702)
                regval |= CAM_REG_DROP_EN | CAM_REG_DROP_EVEN;
#else
                regval |= CAM_REG_DVP_DATA_BSEL;
#endif
                data_mode = 4;
                frame_size = resolution_x * resolution_y;
            } else if (config->output_format == CAM_OUTPUT_FORMAT_YUV420_UV) {
#if defined(BL702)
                regval |= CAM_REG_DROP_EN | CAM_REG_DROP_EVEN | CAM_REG_SUBSAMPLE_EN;
#else
                regval |= CAM_REG_DVP_DATA_BSEL | CAM_REG_V_SUBSAMPLE_EN | CAM_REG_V_SUBSAMPLE_POL;
#endif
                data_mode = 4;
                frame_size = resolution_x * resolution_y / 2;
#if defined(BL616) || defined(BL616L)
                putreg32(1, CAM_FRONT_BASE + CAM_FRONT_DVP2BUS_SRC_SEL_1_OFFSET);
#elif defined(BL616D)
                tmpval = getreg32(CAM_FRONT_BASE + CAM_FRONT_DVP_MUX_SEL_REG_OFFSET);
                tmpval &= ~(3 << (dev->idx == 0 ? CAM_FRONT_REG_D2XA_IN_SEL_SHIFT : CAM_FRONT_REG_D2XB_IN_SEL_SHIFT));
                putreg32(tmpval, CAM_FRONT_BASE + CAM_FRONT_DVP_MUX_SEL_REG_OFFSET);
#endif
            }
            break;

        case CAM_INPUT_FORMAT_YUV422_UYVY:
#if defined(BL808)
            if (config->output_format >= CAM_OUTPUT_FORMAT_RGB888_OR_BGR888 && config->output_format <= CAM_OUTPUT_FORMAT_RGB888_TO_RGBA8888) {
                bflb_cam_feature_control(dev, CAM_CMD_INVERSE_YUYV2UYVY, true);
                tmpval = 0x23;
                if (config->input_source) {
                    putreg32(0x18000000, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_0_OFFSET);
                } else {
                    putreg32(0xa8000000, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_0_OFFSET);
                }
                putreg32(0xff80, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_1_OFFSET);
                putreg32(0xff80, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_2_OFFSET);
                putreg32(0x200, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_3_OFFSET);
                putreg32(0x20002ce, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_4_OFFSET);
                putreg32(0xfe92ff50, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_5_OFFSET);
                putreg32(0x38b0200, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_6_OFFSET);
                putreg32(0, CAM_FRONT_BASE + CAM_FRONT_Y2RA_CONFIG_7_OFFSET);

                if (config->output_format == CAM_OUTPUT_FORMAT_RGB888_OR_BGR888) {
                    data_mode = 1;
                    frame_size = resolution_x * resolution_y * 3;
                    break;
                } else if (config->output_format == CAM_OUTPUT_FORMAT_RGB888_TO_RGB565) {
                    data_mode = 2;
                    frame_size = resolution_x * resolution_y * 2;
                    putreg32(5 << CAM_REG_FORMAT_565_SHIFT, reg_base + CAM_DVP2AXI_MISC_OFFSET);
                    break;
                } else if (config->output_format == CAM_OUTPUT_FORMAT_RGB888_TO_BGR565) {
                    data_mode = 2;
                    frame_size = resolution_x * resolution_y * 2;
                    putreg32(0, reg_base + CAM_DVP2AXI_MISC_OFFSET);
                    break;
                } else if (config->output_format == CAM_OUTPUT_FORMAT_RGB888_TO_RGBA8888) {
                    data_mode = 3;
                    frame_size = resolution_x * resolution_y * 4;
                    break;
                }
            }
#endif
        case CAM_INPUT_FORMAT_YUV422_VYUY:
            if (config->output_format == CAM_OUTPUT_FORMAT_AUTO || config->output_format == CAM_OUTPUT_FORMAT_YUV422) {
                data_mode = 0;
                frame_size = resolution_x * resolution_y * 2;
            } else if (config->output_format == CAM_OUTPUT_FORMAT_GRAY) {
#if defined(BL702)
                regval |= CAM_REG_DROP_EN | CAM_REG_DROP_EVEN;
#else
                regval |= CAM_REG_DVP_DATA_BSEL;
#endif
                data_mode = 4;
                frame_size = resolution_x * resolution_y;
            } else if (config->output_format == CAM_OUTPUT_FORMAT_YUV422_UV) {
#if defined(BL702)
                regval |= CAM_REG_DROP_EN;
#endif
                data_mode = 4;
                frame_size = resolution_x * resolution_y;
            } else if (config->output_format == CAM_OUTPUT_FORMAT_YUV420_UV) {
#if defined(BL702)
                regval |= CAM_REG_DROP_EN | CAM_REG_DROP_EVEN | CAM_REG_SUBSAMPLE_EN | CAM_REG_SUBSAMPLE_EVEN;
#else
                regval |= CAM_REG_V_SUBSAMPLE_EN;
#endif
                data_mode = 4;
                frame_size = resolution_x * resolution_y / 2;
#if defined(BL616) || defined(BL616L)
                putreg32(1, CAM_FRONT_BASE + CAM_FRONT_DVP2BUS_SRC_SEL_1_OFFSET);
#elif defined(BL616D)
                tmpval = getreg32(CAM_FRONT_BASE + CAM_FRONT_DVP_MUX_SEL_REG_OFFSET);
                tmpval &= ~(3 << (dev->idx == 0 ? CAM_FRONT_REG_D2XA_IN_SEL_SHIFT : CAM_FRONT_REG_D2XB_IN_SEL_SHIFT));
                putreg32(tmpval, CAM_FRONT_BASE + CAM_FRONT_DVP_MUX_SEL_REG_OFFSET);
#endif
            }
            break;

        case CAM_INPUT_FORMAT_GRAY:
            if (config->output_format == CAM_OUTPUT_FORMAT_AUTO || config->output_format == CAM_OUTPUT_FORMAT_GRAY) {
                data_mode = 0;
                frame_size = resolution_x * resolution_y;
            }
            break;

        case CAM_INPUT_FORMAT_RGB565:
            /* Same as CAM_INPUT_FORMAT_BGR565 */
        case CAM_INPUT_FORMAT_BGR565:
            if (config->output_format == CAM_OUTPUT_FORMAT_AUTO || config->output_format == CAM_OUTPUT_FORMAT_RGB565_OR_BGR565) {
                data_mode = 0;
                frame_size = resolution_x * resolution_y * 2;
            }
            break;

        case CAM_INPUT_FORMAT_RGB888:
            /* Same as CAM_INPUT_FORMAT_BGR888 */
        case CAM_INPUT_FORMAT_BGR888:
            if (config->output_format == CAM_OUTPUT_FORMAT_AUTO || config->output_format == CAM_OUTPUT_FORMAT_RGB888_OR_BGR888) {
                data_mode = 1;
                frame_size = resolution_x * resolution_y * 3;
#if !defined(BL702)
            } else if (config->output_format == CAM_OUTPUT_FORMAT_RGB888_TO_RGB565) {
                data_mode = 2;
                frame_size = resolution_x * resolution_y * 2;
                putreg32(5 << CAM_REG_FORMAT_565_SHIFT, reg_base + CAM_DVP2AXI_MISC_OFFSET);
            } else if (config->output_format == CAM_OUTPUT_FORMAT_RGB888_TO_BGR565) {
                data_mode = 2;
                frame_size = resolution_x * resolution_y * 2;
                putreg32(0, reg_base + CAM_DVP2AXI_MISC_OFFSET);
            } else if (config->output_format == CAM_OUTPUT_FORMAT_RGB888_TO_RGBA8888) {
                /* Default A = 0 */
                data_mode = 3;
                frame_size = resolution_x * resolution_y * 4;
#endif
            }
            break;

        default:
            break;
    }
#if !defined(BL702)
#if defined(BL616D)
    if (config->output_format != CAM_OUTPUT_FORMAT_YUV420_UV) {
        putreg32(resolution_y << CAM_REG_FRAME_HEIGHT_SHIFT | (frame_size / resolution_y / 8), reg_base + CAM_DVP2AXI_FRAME_BCNT_OFFSET);
    } else {
        putreg32((resolution_y / 2) << CAM_REG_FRAME_HEIGHT_SHIFT | (frame_size / resolution_y / 4), reg_base + CAM_DVP2AXI_FRAME_BCNT_OFFSET);
    }
#else
    putreg32(frame_size, reg_base + CAM_DVP2AXI_FRAME_BCNT_OFFSET);
#endif
    regval |= data_mode << CAM_REG_DVP_DATA_MODE_SHIFT;
#endif
    putreg32(regval, reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);

    /* Set output buffer burst count */
    regval = getreg32(reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
    regval = (regval & CAM_REG_XLEN_MASK) >> CAM_REG_XLEN_SHIFT;
    switch (regval) {
        case CAM_BURST_INCR1:
#if defined(BL702)
            regval = config->output_bufsize >> 2;
            frame_size = frame_size >> 2;
#else
            regval = config->output_bufsize >> 3;
#endif
            break;

        case CAM_BURST_INCR4:
#if defined(BL702)
            regval = config->output_bufsize >> 4;
            frame_size = frame_size >> 4;
#else
            regval = config->output_bufsize >> 5;
#endif
            break;

        case CAM_BURST_INCR8:
#if defined(BL702)
            regval = config->output_bufsize >> 5;
            frame_size = frame_size >> 5;
#else
            regval = config->output_bufsize >> 6;
#endif
            break;

        case CAM_BURST_INCR16:
#if defined(BL702)
            regval = config->output_bufsize >> 6;
            frame_size = frame_size >> 6;
#else
            regval = config->output_bufsize >> 7;
#endif
            break;

#if !defined(BL702)
        case CAM_BURST_INCR32:
            regval = config->output_bufsize >> 8;
            break;

        case CAM_BURST_INCR64:
            regval = config->output_bufsize >> 9;
            break;
#endif

        default:
            regval = config->output_bufsize >> 7;
            frame_size = frame_size >> 6;
            break;
    }
#if defined(BL616D)
    tmpval = getreg32(reg_base + CAM_DVP2AXI_FRAME_BCNT_OFFSET);
    tmpval &= CAM_REG_FRAME_WIDTH_X8_MASK;
    tmpval = config->output_bufsize / tmpval / 8;
    putreg32(tmpval << CAM_REG_WRAP_LCNT_SHIFT, reg_base + CAM_DVP2AXI_MEM_BCNT_OFFSET);
#else
    putreg32(regval, reg_base + CAM_DVP2AXI_MEM_BCNT_OFFSET);
#endif

#if defined(BL702)
    putreg32(frame_size, reg_base + CAM_DVP2AXI_FRAME_BCNT_0_OFFSET);
#endif

#if defined(BL808)
    if (dev->idx < 4) {
        regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_DVP2BUS_SRC_SEL_1_OFFSET);
        regval &= ~(0x3f << (dev->idx * 8));
        regval |= tmpval << (dev->idx * 8);
        putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_DVP2BUS_SRC_SEL_1_OFFSET);
    } else {
        regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_DVP2BUS_SRC_SEL_2_OFFSET);
        regval &= ~(0x3f << ((dev->idx - 4) * 8));
        regval |= tmpval << ((dev->idx - 4) * 8);
        putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_DVP2BUS_SRC_SEL_2_OFFSET);
    }
#endif

    bflb_cam_int_mask(dev, CAM_INTMASK_FIFO_OVERWRITE, true);

#if !defined(BL702)
#if defined(BL616D)
    regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_MM_MISC_CTRL_OFFSET);
    regval |= CAM_FRONT_CR_DVP_S2P_EN;
    putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_MM_MISC_CTRL_OFFSET);
#endif

#if defined(BL808)
    if (config->input_source == 0) {
        regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
        regval |= CAM_FRONT_RG_DVPAS_ENABLE;
        putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
    }
#else
    regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
    regval |= CAM_FRONT_RG_DVPAS_ENABLE;
    putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
#endif
#endif
#endif
}

void bflb_cam_start(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_cam_start
    romapi_bflb_cam_start(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
    regval |= CAM_REG_DVP_ENABLE;
    putreg32(regval, reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
#endif
}

void bflb_cam_stop(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_cam_stop
    romapi_bflb_cam_stop(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
    regval &= ~CAM_REG_DVP_ENABLE;
    putreg32(regval, reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
#endif
}

void bflb_cam_int_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask)
{
#ifdef romapi_bflb_cam_int_mask
    romapi_bflb_cam_int_mask(dev, int_type, mask);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
#if defined(BL702)
    regval = getreg32(reg_base + CAM_INT_CONTROL_OFFSET);
#else
    regval = getreg32(reg_base + CAM_DVP_STATUS_AND_ERROR_OFFSET);
#endif
    if (mask) {
        regval &= ~int_type;
    } else {
        regval |= int_type;
    }
#if defined(BL702)
    putreg32(regval, reg_base + CAM_INT_CONTROL_OFFSET);
#else
    putreg32(regval, reg_base + CAM_DVP_STATUS_AND_ERROR_OFFSET);
#endif
#endif
}

void bflb_cam_int_clear(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_cam_int_clear
    romapi_bflb_cam_int_clear(dev, int_type);
#else
    putreg32(int_type, dev->reg_base + CAM_DVP_FRAME_FIFO_POP_OFFSET);
#endif
}

void bflb_cam_crop_vsync(struct bflb_device_s *dev, uint16_t start_line, uint16_t end_line)
{
#ifdef romapi_bflb_cam_crop_vsync
    romapi_bflb_cam_crop_vsync(dev, start_line, end_line);
#else
    /* Get start_line ~ (end_line - 1), not include end_line */
    putreg32(start_line << 16 | end_line, dev->reg_base + CAM_DVP2AXI_VSYNC_CROP_OFFSET);
#endif
}

void bflb_cam_crop_hsync(struct bflb_device_s *dev, uint16_t start_pixel, uint16_t end_pixel)
{
#ifdef romapi_bflb_cam_crop_hsync
    romapi_bflb_cam_crop_hsync(dev, start_pixel, end_pixel);
#else
#if defined(BL702)
    start_pixel = start_pixel * 2;
    end_pixel = end_pixel * 2;
#endif
    /* Get start_pixel ~ (end_pixel - 1), not include end_pixel */
    putreg32(start_pixel << 16 | end_pixel, dev->reg_base + CAM_DVP2AXI_HSYNC_CROP_OFFSET);
#endif
}

void bflb_cam_pop_one_frame(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_cam_pop_one_frame
    romapi_bflb_cam_pop_one_frame(dev);
#else
#if defined(BL702)
    putreg32(3, dev->reg_base + CAM_DVP_FRAME_FIFO_POP_OFFSET);
#else
#if !defined(BL616D)
    putreg32(1, dev->reg_base + CAM_DVP_FRAME_FIFO_POP_OFFSET);
#endif
#endif
#endif
}

uint8_t bflb_cam_get_frame_count(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_cam_get_frame_count
    return romapi_bflb_cam_get_frame_count(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + CAM_DVP_STATUS_AND_ERROR_OFFSET);
#if defined(BL616D)
    return ((regval >> 12) & 1);
#else
    regval &= CAM_FRAME_VALID_CNT_MASK;
    return (regval >> CAM_FRAME_VALID_CNT_SHIFT);
#endif
#endif
}

uint32_t bflb_cam_get_frame_info(struct bflb_device_s *dev, uint8_t **pic)
{
#ifdef romapi_bflb_cam_get_frame_info
    return romapi_bflb_cam_get_frame_info(dev, pic);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;
    *pic = (uint8_t *)(uintptr_t)getreg32(reg_base + CAM_FRAME_START_ADDR0_OFFSET);
#if defined(BL702)
    return (getreg32(reg_base + CAM_FRAME_BYTE_CNT0_0_OFFSET));
#else
#if defined(BL616D)
    *pic = (uint8_t *)(uintptr_t)getreg32(reg_base + CAM_DVP2AXI_ADDR_START_OFFSET);
    reg_base = getreg32(reg_base + CAM_DVP2AXI_FRAME_BCNT_OFFSET);
    return ((reg_base & CAM_REG_FRAME_WIDTH_X8_MASK) * 8 * (reg_base >> CAM_REG_FRAME_HEIGHT_SHIFT));
#else
    return (getreg32(reg_base + CAM_DVP2AXI_FRAME_BCNT_OFFSET));
#endif
#endif
#endif
}

uint32_t bflb_cam_get_intstatus(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_cam_get_intstatus
    return romapi_bflb_cam_get_intstatus(dev);
#else
    return (getreg32(dev->reg_base + CAM_DVP_STATUS_AND_ERROR_OFFSET));
#endif
}

int bflb_cam_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_cam_feature_control
    return romapi_bflb_cam_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    switch (cmd) {
        case CAM_CMD_SET_VSYNC_POLARITY:
            /* Set vsync polarity, arg use @ref CAM_POLARITY */
            regval = getreg32(reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
            if (arg) {
                regval |= CAM_REG_FRAM_VLD_POL;
            } else {
                regval &= ~CAM_REG_FRAM_VLD_POL;
            }
            putreg32(regval, reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
            break;

        case CAM_CMD_SET_HSYNC_POLARITY:
            /* Set hsync polarity, arg use @ref CAM_POLARITY */
            regval = getreg32(reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
            if (arg) {
                regval |= CAM_REG_LINE_VLD_POL;
            } else {
                regval &= ~CAM_REG_LINE_VLD_POL;
            }
            putreg32(regval, reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
            break;

        case CAM_CMD_SET_BURST:
            /* Set burst length, arg use @ref CAM_BURST */
            regval = getreg32(reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
            regval &= ~CAM_REG_XLEN_MASK;
            regval |= (arg << CAM_REG_XLEN_SHIFT) & CAM_REG_XLEN_MASK;
            putreg32(regval, reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
            break;

#if !defined(BL702)
        case CAM_CMD_SET_RGBA8888_ALPHA:
            /* Set alpha value of RGBA8888 output, arg is alpha */
            regval = getreg32(reg_base + CAM_DVP2AXI_MISC_OFFSET);
            regval &= ~CAM_REG_ALPHA_MASK;
            regval |= arg & CAM_REG_ALPHA_MASK;
            putreg32(regval, reg_base + CAM_DVP2AXI_MISC_OFFSET);
            break;

#if !defined(BL616D)
        case CAM_CMD_GET_FRAME_ID:
            /* Get frame id */
            *(uint16_t *)arg = getreg32(reg_base + CAM_FRAME_ID_STS01_OFFSET) & 0xffff;
            break;
#endif
#endif

        case CAM_CMD_WRAP_MODE:
            /* Wrap to output buffer start address, only effective in mjpeg mode, arg use ENABLE or DISABLE */
            regval = getreg32(reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
            if (arg) {
#if defined(BL616D)
                regval |= CAM_REG_WRAP_MODE;
#else
                regval |= CAM_REG_HW_MODE_FWRAP;
#endif
            } else {
#if defined(BL616D)
                regval &= ~CAM_REG_WRAP_MODE;
#else
                regval &= ~CAM_REG_HW_MODE_FWRAP;
#endif
            }
            putreg32(regval, reg_base + CAM_DVP2AXI_CONFIGUE_OFFSET);
            break;

        case CAM_CMD_COUNT_TRIGGER_NORMAL_INT:
            /* Set frame count to trigger normal interrupt, arg is frame count */
#if defined(BL702)
            regval = getreg32(reg_base + CAM_INT_CONTROL_OFFSET);
            regval &= ~CAM_REG_FRAME_CNT_TRGR_INT_MASK;
            regval |= arg << CAM_REG_FRAME_CNT_TRGR_INT_SHIFT;
            putreg32(regval, reg_base + CAM_INT_CONTROL_OFFSET);
#else
            regval = getreg32(reg_base + CAM_DVP_STATUS_AND_ERROR_OFFSET);
            regval &= ~CAM_REG_FRAME_CNT_TRGR_INT_MASK;
            regval |= arg & CAM_REG_FRAME_CNT_TRGR_INT_MASK;
            putreg32(regval, reg_base + CAM_DVP_STATUS_AND_ERROR_OFFSET);
#endif
            break;

#if !defined(BL702)
#if !defined(BL616D)
        case CAM_CMD_FRAME_ID_RESET:
            /* Reset frame id */
            regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_ISP_ID_YUV_OFFSET);
            regval |= CAM_FRONT_REG_YUV_IDGEN_RST;
            putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_ISP_ID_YUV_OFFSET);
            break;
#endif

        case CAM_CMD_INVERSE_VSYNC_POLARITY:
            /* Inverse vsync polarity */
            regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
            if (arg) {
                regval |= CAM_FRONT_RG_DVPAS_VS_INV;
            } else {
                regval &= ~CAM_FRONT_RG_DVPAS_VS_INV;
            }
            putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
            break;

        case CAM_CMD_INVERSE_HSYNC_POLARITY:
            /* Inverse hsync polarity */
            regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
            if (arg) {
                regval |= CAM_FRONT_RG_DVPAS_HS_INV;
            } else {
                regval &= ~CAM_FRONT_RG_DVPAS_HS_INV;
            }
            putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
            break;

        case CAM_CMD_INVERSE_YUYV2UYVY:
            /* If image sensor output format is YUYV, it will be changed to UYVY */
#if defined(BL616D)
            regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_MM_MISC_CTRL_OFFSET);
            if (arg) {
                regval |= CAM_FRONT_CR_DVP_S2P_DA_ORDER;
            } else {
                regval &= ~CAM_FRONT_CR_DVP_S2P_DA_ORDER;
            }
            putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_MM_MISC_CTRL_OFFSET);
#else
            regval = getreg32(CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
            if (arg) {
                regval |= CAM_FRONT_RG_DVPAS_DA_ORDER;
            } else {
                regval &= ~CAM_FRONT_RG_DVPAS_DA_ORDER;
            }
            putreg32(regval, CAM_FRONT_BASE + CAM_FRONT_CONFIG_OFFSET);
#endif
            break;

        case CAM_CMD_FRAME_FILTER: {
            /* For example: frame_count is 4, frame_valid is 0x14 (10100b). 
            Third/fifth frame will be retained,First/second/fourth frame will be dropped in every (4 + 1) frames */
            struct bflb_cam_frame_filter_config_s *config = (struct bflb_cam_frame_filter_config_s *)arg;

            putreg32(config->frame_count, reg_base + CAM_DVP2AXI_FRAME_PERIOD_OFFSET);
            putreg32(config->frame_valid, reg_base + CAM_DVP2AXI_FRAME_VLD_OFFSET);
        } break;
#endif

#if defined(BL616D)
        case CAM_CMD_SET_OUTPUT_ADDR:
            putreg32(arg, reg_base + CAM_DVP2AXI_ADDR_START_OFFSET);
            break;
#endif

        default:
            ret = -EPERM;
            break;
    }
    return ret;
#endif
}
