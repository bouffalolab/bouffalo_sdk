
#include "image_proc.h"

#define ai_input_w       50
#define ai_input_h       50
#define ai_input_c       1
#define PSRAM_START_ADDR (0x26000000)

static float get_pixel(int8_t *data, int x, int y, int c, int hin, int win)
{
    return data[c * hin * win + y * win + x];
}

static void set_pixel(int8_t *data, int x, int y, int c, float val, int cin, int hin, int win)
{
    if (x < 0 || y < 0 || c < 0 || x >= win || y >= hin || c >= cin)
        return;
    data[c * hin * win + y * win + x] = val;
}

static void add_pixel(int8_t *data, int x, int y, int c, float val, int hin, int win)
{
    data[c * hin * win + y * win + x] += val;
}

void resize_image(uint8_t *src_8, int8_t *BL_resized, int win, int hin, int cin, int size_o, int8_t *buffer)
{
    //int8_t src[win*hin*cin];
    //int8_t BL_part[size_o*hin*cin];
    int8_t *src = buffer;
    int8_t *BL_part = buffer + win * hin * cin;

    int r, c, k, wout, hout;
    wout = size_o;
    hout = size_o;
    float w_scale = (float)(win - 1) / (wout - 1);
    float h_scale = (float)(hin - 1) / (hout - 1);

    // normalize image to zero
    for (int i = 0; i < win * hin * cin; i++) {
        src[i] = (int8_t)(src_8[i] - 128);
    }

    for (k = 0; k < cin; ++k) {
        for (r = 0; r < hin; ++r) {
            for (c = 0; c < wout; ++c) {
                float val = 0;
                if (c == wout - 1 || win == 1) {
                    val = get_pixel(src, win - 1, r, k, hin, win);
                } else {
                    float sx = c * w_scale;
                    int ix = (int)sx;
                    float dx = sx - ix;
                    val = (1 - dx) * get_pixel(src, ix, r, k, hin, win) + dx * get_pixel(src, ix + 1, r, k, hin, win);
                }
                set_pixel(BL_part, c, r, k, val, cin, hin, wout);
            }
        }
    }
    for (k = 0; k < cin; ++k) {
        for (r = 0; r < hout; ++r) {
            float sy = r * h_scale;
            int iy = (int)sy;
            float dy = sy - iy;
            for (c = 0; c < wout; ++c) {
                float val = (1 - dy) * get_pixel(BL_part, c, iy, k, hin, wout);
                set_pixel(BL_resized, c, r, k, val, cin, hout, wout);
            }
            if (r == hout - 1 || hin == 1)
                continue;
            for (c = 0; c < wout; ++c) {
                float val = dy * get_pixel(BL_part, c, iy + 1, k, hin, wout);
                add_pixel(BL_resized, c, r, k, val, hout, wout);
            }
        }
    }
}

void resize_image_i8(int8_t *src_8, int8_t *BL_resized, int win, int hin, int cin, int size_o, int8_t *buffer)
{
    //int8_t src[win*hin*cin];
    //int8_t BL_part[size_o*hin*cin];
    int8_t *src = buffer;
    int8_t *BL_part = buffer + win * hin * cin;

    int r, c, k, wout, hout;
    wout = size_o;
    hout = size_o;
    float w_scale = (float)(win - 1) / (wout - 1);
    float h_scale = (float)(hin - 1) / (hout - 1);

    // normalize image to zero
    for (int i = 0; i < win * hin * cin; i++) {
        src[i] = (int8_t)(src_8[i]);
    }

    for (k = 0; k < cin; ++k) {
        for (r = 0; r < hin; ++r) {
            for (c = 0; c < wout; ++c) {
                float val = 0;
                if (c == wout - 1 || win == 1) {
                    val = get_pixel(src, win - 1, r, k, hin, win);
                } else {
                    float sx = c * w_scale;
                    int ix = (int)sx;
                    float dx = sx - ix;
                    val = (1 - dx) * get_pixel(src, ix, r, k, hin, win) + dx * get_pixel(src, ix + 1, r, k, hin, win);
                }
                set_pixel(BL_part, c, r, k, val, cin, hin, wout);
            }
        }
    }
    for (k = 0; k < cin; ++k) {
        for (r = 0; r < hout; ++r) {
            float sy = r * h_scale;
            int iy = (int)sy;
            float dy = sy - iy;
            for (c = 0; c < wout; ++c) {
                float val = (1 - dy) * get_pixel(BL_part, c, iy, k, hin, wout);
                set_pixel(BL_resized, c, r, k, val, cin, hout, wout);
            }
            if (r == hout - 1 || hin == 1)
                continue;
            for (c = 0; c < wout; ++c) {
                float val = dy * get_pixel(BL_part, c, iy + 1, k, hin, wout);
                add_pixel(BL_resized, c, r, k, val, hout, wout);
            }
        }
    }
}

int8_t *image_proc(uint8_t *picture, int frame_size, int CAMERA_RESOLUTION_X, int CAMERA_RESOLUTION_Y, int *piture_lcd)
{
    int psram_ptr = CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y;
    int8_t *resized = (int8_t *)(BL702_PSRAM_XIP_BASE + psram_ptr);
    int resized_size = (int)ai_input_w * (int)ai_input_h * (int)ai_input_c;
    psram_ptr += resized_size;

    int mid_size = 160;
    int8_t *m_resized = (int8_t *)(BL702_PSRAM_XIP_BASE + psram_ptr);
    *piture_lcd = psram_ptr;
    int m_resized_size = (int)mid_size * (int)mid_size * (int)ai_input_c;
    psram_ptr += m_resized_size;

    resize_image(picture, m_resized, CAMERA_RESOLUTION_X, CAMERA_RESOLUTION_Y, ai_input_c, mid_size, (int8_t *)(BL702_PSRAM_XIP_BASE + psram_ptr));
    resize_image_i8(m_resized, resized, mid_size, mid_size, ai_input_c, ai_input_w, (int8_t *)(BL702_PSRAM_XIP_BASE + psram_ptr));

    for (int i = 0; i < m_resized_size; i++) {
        m_resized[i] = (uint8_t)(((int16_t)m_resized[i]) + 128);
    }
    return resized;
}
