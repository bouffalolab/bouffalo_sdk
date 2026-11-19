#include "bflb_cam.h"
#include "bflb_dma2d.h"
#include "bflb_i2c.h"
#include "bflb_irq.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bl618dg_clock.h"
#include "board.h"
#include "default/display_default.h"
#include "hardware/cam_reg.h"
#include "image_sensor.h"
#include <stdbool.h>
#include <string.h>

#define CAMERA_FRAME_MAX_WIDTH  1280U
#define CAMERA_FRAME_MAX_HEIGHT 720U
#define CAMERA_FRAME_MAX_PIXELS (CAMERA_FRAME_MAX_WIDTH * CAMERA_FRAME_MAX_HEIGHT)
#define CAMERA_FRAME_COUNT      2
#define DISPLAY_FRAME_COUNT     3
#define RGB565_BYTES_PER_PIXEL  2
#define FPS_REPORT_INTERVAL_US  1000000ULL
#define FRAME_TIMEOUT_US        2000000ULL
#define FRAME_ALIGNMENT         64U
#define ALIGN_FRAME_BYTES(n)    (((n) + FRAME_ALIGNMENT - 1U) & ~(FRAME_ALIGNMENT - 1U))
#define DISPLAY_WIDTH           DISPLAY_DEFAULT_VIDEO_WIDTH
#define DISPLAY_HEIGHT          DISPLAY_DEFAULT_VIDEO_HEIGHT

#if !defined(BL618DG)
#error "cam_lcd_mipi requires BL618DG"
#endif

#if !defined(CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO) || !defined(CONFIG_DISPLAY_DEFAULT_DSI)
#error "cam_lcd_mipi requires a MIPI DSI panel"
#endif

#if !defined(CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_RGB565)
#error "cam_lcd_mipi requires an RGB565 panel"
#endif

static struct bflb_device_s *i2c;
static struct bflb_device_s *cam0;
static struct bflb_device_s *dma2d_ch0;
static display_t *screen;

struct frame_geometry_s {
    uint16_t sensor_width;
    uint16_t sensor_height;
    uint16_t crop_x;
    uint16_t crop_y;
    uint16_t crop_width;
    uint16_t crop_height;
    uint16_t display_x;
    uint16_t display_y;
    uint8_t transform;
    bool direct;
};

static struct frame_geometry_s frame_geometry;

enum display_slot_state {
    DISPLAY_FREE, DISPLAY_WRITING, DISPLAY_READY, DISPLAY_PENDING, DISPLAY_CURRENT
};

enum camera_slot_state {
    CAMERA_FREE, CAMERA_CAPTURING, CAMERA_READY, CAMERA_READING
};

static volatile enum display_slot_state display_state[DISPLAY_FRAME_COUNT];
static volatile enum camera_slot_state camera_state[CAMERA_FRAME_COUNT];
static volatile int camera_write_index = -1;
static volatile bool camera_paused = true;
/* While drain is pending, camera_write_index still owns WRITING/CAPTURING. */
static volatile bool camera_drain_pending;
static volatile bool camera_frame_complete;
static volatile uint64_t camera_stopped_us;
static bool camera_initialized;
static volatile bool production_frozen;
static volatile bool pipeline_fault;
static volatile uint8_t dma2d_done;
static volatile uint8_t display_framebuffer_queue[DISPLAY_FRAME_COUNT];
static volatile uint8_t display_queue_head;
static volatile uint8_t display_queue_tail;
static volatile uint8_t display_queue_count;
static volatile uint8_t camera_queue[CAMERA_FRAME_COUNT];
static volatile uint8_t camera_queue_head;
static volatile uint8_t camera_queue_tail;
static volatile uint8_t camera_queue_count;
static int current_display_index = -1;
static uint32_t displayed_frame_count;

/* One main-loop owner. Arm BEFORE flip; an IRQ may complete before flip returns.
 * The callback never mutates slot ownership, queues, or the current index. */
static volatile struct {
    bool armed;
    bool done;
    display_event_type_t expected;
    display_event_type_t completed;
    int result;
    int previous;
    int pending;
} flip_completion;
static uint64_t flip_started_us;

/* Set from a debugger to stop capture and retire the final scanned frame. */
volatile bool cam_lcd_mipi_stop_requested;

ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(FRAME_ALIGNMENT)))
static uint16_t camera_framebuffer[CAMERA_FRAME_COUNT]
                                  [ALIGN_FRAME_BYTES(CAMERA_FRAME_MAX_PIXELS * RGB565_BYTES_PER_PIXEL) / 2];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(FRAME_ALIGNMENT)))
static uint16_t display_framebuffer[DISPLAY_FRAME_COUNT]
                                   [ALIGN_FRAME_BYTES(DISPLAY_WIDTH * DISPLAY_HEIGHT * RGB565_BYTES_PER_PIXEL) / 2];

_Static_assert(sizeof(camera_framebuffer[0]) % FRAME_ALIGNMENT == 0, "camera slot alignment");
_Static_assert(sizeof(display_framebuffer[0]) % FRAME_ALIGNMENT == 0, "display slot alignment");

/* Dedicated DSI pads need no GPIO mux. An engineering board can override this
 * hook in its overlay for additional DSI power/control pins. */
void __attribute__((weak)) cam_lcd_mipi_dsi_gpio_init(void)
{
}

/* Local MMIO for ROM and non-ROM builds alike: interrupt APIs need not expose
 * BUS/AXI idle. A NORMAL interrupt alone does not retire outstanding writes. */
static uint32_t camera_status(void)
{
    return getreg32(cam0->reg_base + CAM_DVP_STATUS_AND_ERROR_OFFSET);
}

static bool camera_status_error(uint32_t status)
{
    return (status & (CAM_INTSTS_FIFO_OVERWRITE | CAM_INTSTS_HSYNC_MISMATCH | CAM_INTSTS_VSYNC_MISMATCH)) != 0;
}

/* Caller holds a short IRQ guard (or is the CAM ISR). Never wait here.
 * Repeated NORMAL IRQs while paused must not reset the drain deadline. */
static void camera_stop_locked(void)
{
    if (!camera_paused) {
        bflb_cam_stop(cam0);
        camera_paused = true;
        camera_drain_pending = true;
        camera_stopped_us = bflb_mtimer_get_time_us();
    }
}

static void __attribute__((noreturn)) fail_stop(const char *message)
{
    uintptr_t flags = bflb_irq_save();

    production_frozen = true;
    if (camera_initialized) {
        camera_stop_locked();
        bflb_cam_int_mask(cam0, CAM_INTMASK_NORMAL, true);
        bflb_irq_disable(cam0->irq_num);
    }
    if (dma2d_ch0 != NULL) {
        bflb_dma2d_channel_stop(dma2d_ch0);
    }
    bflb_irq_restore(flags);
    /* Do not release any slot: stop/timeout/error is NOT a retirement event. */
    printf("%s\r\n", message);
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}

static void camera_devices_open(const char *i2c_name)
{
    i2c = bflb_device_get_by_name(i2c_name);
    cam0 = bflb_device_get_by_name(BFLB_NAME_CAM0);
    if (i2c == NULL || cam0 == NULL) {
        fail_stop("Error: get DVP device failed");
    }
}

static struct image_sensor_config_s *camera_sensor_detect(void)
{
    struct image_sensor_config_s *sensor_config;

    if (!image_sensor_scan(i2c, &sensor_config)) {
        fail_stop("Error: can't identify DVP image sensor");
    }

    printf("Sensor: %s, %ux%u, format %u\r\n", sensor_config->name, sensor_config->resolution_x,
           sensor_config->resolution_y, sensor_config->output_format);
    return sensor_config;
}

static void camera_config_from_sensor(struct bflb_cam_config_s *cam_config,
                                      const struct image_sensor_config_s *sensor_config, uint8_t output_format,
                                      uint32_t output_bufaddr, uint32_t output_bufsize)
{
    memcpy(cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config->with_mjpeg = false;
    cam_config->input_source = CAM_INPUT_SOURCE_DVP;
    cam_config->output_format = output_format;
    cam_config->output_bufaddr = output_bufaddr;
    cam_config->output_bufsize = output_bufsize;
}

static uint32_t frame_overlap_area(uint16_t width, uint16_t height)
{
    uint16_t display_width = width < DISPLAY_WIDTH ? width : DISPLAY_WIDTH;
    uint16_t display_height = height < DISPLAY_HEIGHT ? height : DISPLAY_HEIGHT;

    return display_width * display_height;
}

static void frame_geometry_init(uint16_t sensor_width, uint16_t sensor_height)
{
    uint32_t unrotated_area = frame_overlap_area(sensor_width, sensor_height);
    uint32_t rotated_area = frame_overlap_area(sensor_height, sensor_width);
    bool rotate = rotated_area == unrotated_area
                      ? (sensor_width > sensor_height) != (DISPLAY_WIDTH > DISPLAY_HEIGHT)
                      : rotated_area > unrotated_area;
    uint16_t crop_width = sensor_width < (rotate ? DISPLAY_HEIGHT : DISPLAY_WIDTH)
                              ? sensor_width : (rotate ? DISPLAY_HEIGHT : DISPLAY_WIDTH);
    uint16_t crop_height = sensor_height < (rotate ? DISPLAY_WIDTH : DISPLAY_HEIGHT)
                               ? sensor_height : (rotate ? DISPLAY_WIDTH : DISPLAY_HEIGHT);
    uint16_t output_width = rotate ? crop_height : crop_width;
    uint16_t output_height = rotate ? crop_width : crop_height;

    frame_geometry = (struct frame_geometry_s){
        .sensor_width = sensor_width,
        .sensor_height = sensor_height,
        .crop_x = (sensor_width - crop_width) / 2,
        .crop_y = (sensor_height - crop_height) / 2,
        .crop_width = crop_width,
        .crop_height = crop_height,
        .display_x = (DISPLAY_WIDTH - output_width) / 2,
        .display_y = (DISPLAY_HEIGHT - output_height) / 2,
        .transform = rotate ? DMA2D_IMAGE_ROTATE_DEGREE_90 : DMA2D_IMAGE_TRANSLATE,
        .direct = !rotate && sensor_width == DISPLAY_WIDTH && sensor_height == DISPLAY_HEIGHT,
    };

    printf("Frame geometry: crop %ux%u at (%u,%u), %s at (%u,%u)\r\n", frame_geometry.crop_width,
           frame_geometry.crop_height, frame_geometry.crop_x, frame_geometry.crop_y,
           frame_geometry.direct ? "direct" : (rotate ? "rotate 90" : "copy"), frame_geometry.display_x,
           frame_geometry.display_y);
}

/* All *_locked helpers require a short local IRQ guard (or CAM ISR context). */
static int display_framebuffer_acquire_locked(void)
{
    for (int i = 0; i < DISPLAY_FRAME_COUNT; i++) {
        if (display_state[i] == DISPLAY_FREE) {
            display_state[i] = DISPLAY_WRITING;
            return i;
        }
    }
    return -1;
}

static bool display_framebuffer_enqueue_locked(int index)
{
    if (index < 0 || index >= DISPLAY_FRAME_COUNT || display_state[index] != DISPLAY_WRITING ||
        display_queue_count == DISPLAY_FRAME_COUNT) {
        pipeline_fault = true;
        return false;
    }
    display_state[index] = DISPLAY_READY;
    display_framebuffer_queue[display_queue_tail] = index;
    display_queue_tail = (display_queue_tail + 1) % DISPLAY_FRAME_COUNT;
    display_queue_count++;
    return true;
}

static int camera_acquire_locked(void)
{
    if (frame_geometry.direct) {
        return display_framebuffer_acquire_locked();
    }
    for (int i = 0; i < CAMERA_FRAME_COUNT; i++) {
        if (camera_state[i] == CAMERA_FREE) {
            camera_state[i] = CAMERA_CAPTURING;
            return i;
        }
    }
    return -1;
}

static void camera_resume_locked(void)
{
    if (!camera_initialized || !camera_paused || camera_drain_pending || camera_write_index != -1 ||
        production_frozen || pipeline_fault) {
        return;
    }
    int index = camera_acquire_locked();
    if (index < 0) {
        return;
    }
    camera_write_index = index;
    if (bflb_cam_feature_control(cam0, CAM_CMD_SET_OUTPUT_ADDR,
                                 (size_t)(uintptr_t)(frame_geometry.direct ? display_framebuffer[index]
                                                                         : camera_framebuffer[index])) < 0) {
        pipeline_fault = true;
        return;
    }
    bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);
    camera_paused = false;
    bflb_cam_start(cam0);
}

static void camera_int_cb(int irq, void *arg)
{
    (void)irq;
    (void)arg;
    uint32_t status = camera_status();

    if (camera_status_error(status)) {
        pipeline_fault = true;
    }
    if (!(status & CAM_INTSTS_NORMAL) && !pipeline_fault) {
        return;
    }
    if (!camera_paused) {
        camera_stop_locked();
        camera_frame_complete = (status & CAM_INTSTS_NORMAL) && !production_frozen && !pipeline_fault;
    }
    /* Keep the stopped slot and address untouched until main confirms drain.
     * A stale NORMAL while paused only gets acknowledged, never restarts CAM. */
    bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);
}

/* Main-loop only, with IRQs enabled. No start/address change is allowed while
 * pending, so idle cannot belong to a newer capture. Timeout retains all slots. */
static void camera_wait_idle(void)
{
    const uint32_t idle_mask = CAM_ST_BUS_IDLE | CAM_AXI_IDLE;

    while (1) {
        uint32_t status = camera_status();

        if (pipeline_fault || camera_status_error(status)) {
            fail_stop("Error: camera drain fault (frames retained)");
        }
        if ((status & idle_mask) == idle_mask) {
            return;
        }
        if (bflb_mtimer_get_time_us() - camera_stopped_us > FRAME_TIMEOUT_US) {
            fail_stop("Error: camera drain timeout (frames retained)");
        }
        bflb_mtimer_delay_us(1);
    }
}

static void camera_completion_service(void)
{
    uintptr_t flags = bflb_irq_save();
    bool pending = camera_drain_pending;

    bflb_irq_restore(flags);
    if (!pending) {
        return;
    }

    camera_wait_idle();
    flags = bflb_irq_save();
    int index = camera_write_index;

    if (pipeline_fault || camera_status_error(camera_status()) || production_frozen || !camera_paused ||
        !camera_drain_pending || !camera_frame_complete) {
        pipeline_fault = true;
    } else if (frame_geometry.direct) {
        display_framebuffer_enqueue_locked(index);
    } else if (index < 0 || index >= CAMERA_FRAME_COUNT || camera_state[index] != CAMERA_CAPTURING ||
               camera_queue_count == CAMERA_FRAME_COUNT) {
        pipeline_fault = true;
    } else {
        camera_state[index] = CAMERA_READY;
        camera_queue[camera_queue_tail] = index;
        camera_queue_tail = (camera_queue_tail + 1) % CAMERA_FRAME_COUNT;
        camera_queue_count++;
    }
    if (!pipeline_fault) {
        camera_write_index = -1;
        camera_frame_complete = false;
        camera_drain_pending = false;
        camera_resume_locked();
    }
    bool fault = pipeline_fault;

    bflb_irq_restore(flags);
    if (fault) {
        fail_stop("Error: camera completion ownership mismatch (frames retained)");
    }
}

static void dma2d_int_cb(void *arg)
{
    (void)arg;
    dma2d_done = 1;
}

static void display_event_cb(display_t *display, const display_event_t *event, void *ctx)
{
    (void)display;
    (void)ctx;
    if (event->type == DISPLAY_EVT_VSYNC) {
        return; /* A boundary without completion never releases a frame. */
    }
    if (!flip_completion.armed || flip_completion.done) {
        pipeline_fault = true;
        return;
    }
    flip_completion.completed = event->type;
    flip_completion.result = event->result;
    flip_completion.done = true;
}

/* Called only outside display_flip(), including when its callback ran inline. */
static void display_completion_consume(void)
{
    uintptr_t flags = bflb_irq_save();
    bool fault = pipeline_fault;
    bool waiting = flip_completion.armed;

    if (waiting && flip_completion.done && !fault) {
        int old = flip_completion.previous;
        int next = flip_completion.pending;
        display_event_type_t type = flip_completion.completed;

        fault = flip_completion.result != 0 || type != flip_completion.expected || old != current_display_index;
        if (type == DISPLAY_EVT_FB_FIRST) {
            fault |= old != -1;
        } else if (type == DISPLAY_EVT_FB_RETIRED || type == DISPLAY_EVT_FB_LAST) {
            fault |= old < 0 || old >= DISPLAY_FRAME_COUNT;
            if (!fault) {
                fault = display_state[old] != DISPLAY_CURRENT;
            }
        } else {
            fault = true;
        }
        if (type == DISPLAY_EVT_FB_LAST) {
            fault |= next != -1;
        } else {
            fault |= next < 0 || next >= DISPLAY_FRAME_COUNT;
            if (!fault) {
                fault = display_state[next] != DISPLAY_PENDING;
            }
        }
        if (!fault) {
            if (old >= 0) {
                display_state[old] = DISPLAY_FREE; /* RETIRED or LAST only */
            }
            current_display_index = next;
            if (next >= 0) {
                display_state[next] = DISPLAY_CURRENT;
                displayed_frame_count++;
            }
            flip_completion.armed = false;
            flip_completion.done = false;
            waiting = false;
        }
    }
    bflb_irq_restore(flags);
    if (fault) {
        fail_stop("Error: framebuffer ownership/completion mismatch");
    }
    if (waiting && bflb_mtimer_get_time_us() - flip_started_us > FRAME_TIMEOUT_US) {
        fail_stop("Error: display completion timeout (frames retained)");
    }
}

/* Only the main loop submits. No IRQ guard spans display_flip(). */
static void display_frame_commit(bool last)
{
    uintptr_t flags = bflb_irq_save();
    int index = -1;

    if (flip_completion.armed || pipeline_fault ||
        (last ? current_display_index < 0 : display_queue_count == 0)) {
        bflb_irq_restore(flags);
        return;
    }
    if (!last) {
        index = display_framebuffer_queue[display_queue_head];
        if (index < 0 || index >= DISPLAY_FRAME_COUNT || display_state[index] != DISPLAY_READY) {
            bflb_irq_restore(flags);
            fail_stop("Error: invalid display READY queue");
        }
        display_queue_head = (display_queue_head + 1) % DISPLAY_FRAME_COUNT;
        display_queue_count--;
        display_state[index] = DISPLAY_PENDING;
    }
    flip_completion.previous = current_display_index;
    flip_completion.pending = index;
    flip_completion.expected = last ? DISPLAY_EVT_FB_LAST
                                   : (current_display_index < 0 ? DISPLAY_EVT_FB_FIRST : DISPLAY_EVT_FB_RETIRED);
    flip_completion.done = false;
    flip_completion.armed = true;
    bflb_irq_restore(flags);
    flip_started_us = bflb_mtimer_get_time_us();

    display_fb_t fb = { .data = index < 0 ? NULL : display_framebuffer[index],
                        .stride = DISPLAY_WIDTH * RGB565_BYTES_PER_PIXEL, .format = DISPLAY_FMT_RGB565 };
    if (display_flip(screen, last ? NULL : &fb) != 0) {
        /* Even a rejected submission is quarantined; never guess ownership. */
        fail_stop("Error: display flip failed (frames retained)");
    }
    display_completion_consume();
}

static void framebuffers_init(void)
{
    /* The CPU touches pixels only here, before CAM/DMA2D/scanout ownership.
     * Invalidate as well as clean: no stale dirty line may overwrite a DMA result. */
    memset(camera_framebuffer, 0, sizeof(camera_framebuffer));
    bflb_l1c_dcache_clean_invalidate_range(camera_framebuffer, sizeof(camera_framebuffer));
    memset(display_framebuffer, 0, sizeof(display_framebuffer));
    bflb_l1c_dcache_clean_invalidate_range(display_framebuffer, sizeof(display_framebuffer));
}

static uint8_t dma2d_burst_size(uint16_t width)
{
    uint32_t row_bytes = width * RGB565_BYTES_PER_PIXEL;

    if (row_bytes % 16 == 0) {
        return DMA2D_BURST_INCR16;
    }
    if (row_bytes % 4 == 0) {
        return DMA2D_BURST_INCR4;
    }
    return DMA2D_BURST_INCR1;
}

static void dma2d_blit_to_display(const uint16_t *src, uint16_t *dst)
{
    uint8_t burst = dma2d_burst_size(frame_geometry.crop_width);
    struct bflb_dma2d_channel_config_s dma2d_config = {
        .src_addr = 0,
        .dst_addr = 0,
        .next_lli_addr = 0,
        .control = {
            .bits = {
                .transfer_size = 0,
                .src_burst = burst,
                .dst_burst = burst,
                .src_data_width = DMA2D_DATA_WIDTH_16BIT,
                .dst_data_width = DMA2D_DATA_WIDTH_16BIT,
                .src_incr = 0,
                .dst_incr = 0,
                .int_enable = 1,
            },
        },
    };
    struct bflb_dma2d_image_s image_config = {
        .transfer_data_width = DMA2D_DATA_WIDTH_16BIT,
        .pixel_data_width = RGB565_BYTES_PER_PIXEL,
        .src_image_addr = (uint32_t)(uintptr_t)src,
        .src_image_width = frame_geometry.sensor_width,
        .src_x_start = frame_geometry.crop_x,
        .src_y_start = frame_geometry.crop_y,
        .src_x_end = frame_geometry.crop_x + frame_geometry.crop_width,
        .src_y_end = frame_geometry.crop_y + frame_geometry.crop_height,
        .dst_image_addr = (uint32_t)(uintptr_t)dst,
        .dst_image_width = DISPLAY_WIDTH,
        .dst_x_start = frame_geometry.display_x,
        .dst_y_start = frame_geometry.display_y,
    };

    dma2d_done = 0;

    bflb_dma2d_image_geometric_transfor_calculate(dma2d_ch0, &dma2d_config, &image_config,
                                                  frame_geometry.transform);
    bflb_dma2d_channel_init(dma2d_ch0, &dma2d_config);
    bflb_dma2d_channel_start(dma2d_ch0);

    uint64_t started_us = bflb_mtimer_get_time_us();
    while (!dma2d_done) {
        if (pipeline_fault || bflb_mtimer_get_time_us() - started_us > FRAME_TIMEOUT_US) {
            fail_stop("Error: DMA2D timeout/camera fault (frames retained)");
        }
        bflb_mtimer_delay_us(1);
    }

    bflb_dma2d_channel_stop(dma2d_ch0);
}

static void camera_frame_process(void)
{
    uintptr_t flags = bflb_irq_save();
    int display_index = -1;
    int camera_index = -1;

    /* READY dequeue and output reservation must be one atomic operation. */
    if (!production_frozen && !pipeline_fault && !frame_geometry.direct && camera_queue_count != 0) {
        camera_index = camera_queue[camera_queue_head];
        if (camera_index < 0 || camera_index >= CAMERA_FRAME_COUNT || camera_state[camera_index] != CAMERA_READY) {
            pipeline_fault = true;
        } else {
            display_index = display_framebuffer_acquire_locked();
            if (display_index >= 0) {
                camera_state[camera_index] = CAMERA_READING;
                camera_queue_head = (camera_queue_head + 1) % CAMERA_FRAME_COUNT;
                camera_queue_count--;
            }
        }
    }
    camera_resume_locked();
    bflb_irq_restore(flags);

    if (display_index < 0) {
        return;
    }
    /* IRQs stay enabled. CAM can fill only FREE slots, never this READING slot.
     * Geometry is fixed, so the initialized black border needs no CPU clearing. */
    dma2d_blit_to_display(camera_framebuffer[camera_index], display_framebuffer[display_index]);
    flags = bflb_irq_save();
    if (pipeline_fault || camera_state[camera_index] != CAMERA_READING ||
        !display_framebuffer_enqueue_locked(display_index)) {
        bflb_irq_restore(flags);
        fail_stop("Error: DMA2D frame ownership mismatch");
    }
    camera_state[camera_index] = CAMERA_FREE;
    camera_resume_locked();
    bflb_irq_restore(flags);
}

static void dvp_camera_init(void)
{
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s *sensor_config;

    board_dvp_gpio_init();
    GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_BCLK, 3);

    /* board_dvp_gpio_init() configures the selected board DVP bus on I2C0. */
    camera_devices_open(BFLB_NAME_I2C0);
    sensor_config = camera_sensor_detect();

    if (sensor_config->resolution_x == 0 || sensor_config->resolution_y == 0 ||
        (uint32_t)sensor_config->resolution_x * sensor_config->resolution_y > CAMERA_FRAME_MAX_PIXELS) {
        fail_stop("Error: camera frame exceeds 1280x720 RGB565 buffer capacity");
    }
    if (sensor_config->output_format != IMAGE_SENSOR_FORMAT_YUV422_YUYV) {
        fail_stop("Error: this case requires YUYV camera output");
    }

    frame_geometry_init(sensor_config->resolution_x, sensor_config->resolution_y);
    uintptr_t flags = bflb_irq_save();
    camera_write_index = camera_acquire_locked();
    bflb_irq_restore(flags);
    if (camera_write_index < 0) {
        fail_stop("Error: no initial camera framebuffer available");
    }

    camera_config_from_sensor(&cam_config, sensor_config, CAM_OUTPUT_FORMAT_RGB888_TO_RGB565,
                              (uint32_t)(uintptr_t)(frame_geometry.direct ? display_framebuffer[camera_write_index]
                                                                           : camera_framebuffer[camera_write_index]),
                              sensor_config->resolution_x * sensor_config->resolution_y * RGB565_BYTES_PER_PIXEL);

    bflb_cam_init(cam0, &cam_config);
    camera_initialized = true;
    bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);
    bflb_cam_int_mask(cam0, CAM_INTMASK_NORMAL, false);
    bflb_irq_attach(cam0->irq_num, camera_int_cb, NULL);
    bflb_irq_enable(cam0->irq_num);
}

static void pipeline_stop(void)
{
    uintptr_t flags = bflb_irq_save();

    /* Quarantine an interrupted capture: stopping CAM does not publish READY. */
    production_frozen = true;
    camera_stop_locked();
    bflb_cam_int_mask(cam0, CAM_INTMASK_NORMAL, true);
    bflb_irq_disable(cam0->irq_num);
    bflb_irq_restore(flags);

    /* Also handle a NORMAL IRQ that stopped CAM just before this guard.
     * Completed or partial, the capture stays quarantined and is not enqueued. */
    if (camera_drain_pending) {
        camera_wait_idle();
    }
    while (flip_completion.armed) {
        display_completion_consume();
    }
    /* Drop no READY entries: drain the display FIFO, then request LAST. */
    while (display_queue_count != 0) {
        display_completion_consume();
        display_frame_commit(false);
        while (flip_completion.armed) {
            display_completion_consume();
        }
    }
    if (display_set_backlight(screen, 0) != 0) {
        fail_stop("Error: backlight off failed");
    }
    display_frame_commit(true);
    while (flip_completion.armed) {
        display_completion_consume();
    }
    if (display_deinit(screen) != 0) {
        fail_stop("Error: display deinit failed (frames retained)");
    }
    printf("Camera stopped; final display frame retired\r\n");
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}

int main(void)
{
    uint32_t last_fps_frame_count = 0;
    uint64_t last_fps_time_us;

    board_init();
    framebuffers_init();
    cam_lcd_mipi_dsi_gpio_init();
    screen = display_default_get();
    /* Hardware-only pixels after initialization: disable automatic CPU clean. */
    screen->scanout->cfg.dcache_dis = true;
    if (display_init(screen) != 0) {
        fail_stop("Error: display init failed");
    }
    display_info_t info;
    if (display_get_info(screen, &info) != 0 || info.kind != DISPLAY_KIND_FB ||
        info.width != DISPLAY_WIDTH || info.height != DISPLAY_HEIGHT || info.format != DISPLAY_FMT_RGB565) {
        fail_stop("Error: incompatible display geometry/format");
    }
    if (display_set_callback(screen, display_event_cb, NULL) != 0) {
        fail_stop("Error: display callback registration failed");
    }
    /* Explicit black slot 0 FIRST. It remains CURRENT until a real RETIRED. */
    uintptr_t flags = bflb_irq_save();
    int black_index = display_framebuffer_acquire_locked();
    bool black_ready = black_index == 0 && display_framebuffer_enqueue_locked(black_index);
    bflb_irq_restore(flags);
    if (!black_ready) {
        fail_stop("Error: initial black slot unavailable");
    }
    display_frame_commit(false);
    while (flip_completion.armed) {
        display_completion_consume();
    }
    if (display_set_backlight(screen, DISPLAY_BACKLIGHT_MAX) != 0) {
        fail_stop("Error: backlight on failed");
    }

    dma2d_ch0 = bflb_device_get_by_name(BFLB_NAME_DMA2D_CH0);
    if (dma2d_ch0 == NULL) {
        fail_stop("Error: get DMA2D channel failed");
    }
    bflb_dma2d_channel_stop(dma2d_ch0);
    bflb_dma2d_channel_tcint_mask(dma2d_ch0, true);
    bflb_dma2d_channel_tcint_clear(dma2d_ch0);
    bflb_dma2d_channel_irq_attach(dma2d_ch0, dma2d_int_cb, NULL);
    bflb_dma2d_channel_tcint_mask(dma2d_ch0, false);

    dvp_camera_init();
    flags = bflb_irq_save();
    if (pipeline_fault) {
        bflb_irq_restore(flags);
        fail_stop("Error: camera fault before initial start");
    }
    bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);
    camera_paused = false;
    bflb_cam_start(cam0);
    bflb_irq_restore(flags);
    last_fps_frame_count = displayed_frame_count; /* exclude startup black */
    last_fps_time_us = bflb_mtimer_get_time_us();

    while (1) {
        uint64_t now_us;

        display_completion_consume();
        if (cam_lcd_mipi_stop_requested) {
            pipeline_stop();
        }
        camera_completion_service();
        display_frame_commit(false);
        camera_frame_process();
        display_completion_consume();
        display_frame_commit(false);

        now_us = bflb_mtimer_get_time_us();
        if ((now_us - last_fps_time_us) >= FPS_REPORT_INTERVAL_US) {
            uint64_t elapsed_us = now_us - last_fps_time_us;
            uint32_t frame_count = displayed_frame_count;
            uint32_t frame_delta = frame_count - last_fps_frame_count;
            uint32_t fps_x1000 = (uint32_t)((frame_delta * 1000000000ULL + elapsed_us / 2) / elapsed_us);

            printf("camera display fps: %lu.%03lu\r\n",
                   (unsigned long)(fps_x1000 / 1000), (unsigned long)(fps_x1000 % 1000));
            last_fps_frame_count = frame_count;
            last_fps_time_us = now_us;
        }
    }
}
