/**
 * @file soln_fbq.c
 * @brief Fixed Frame Buffer Queue instances used by the BL616 solution.
 *
 * @details
 * Each enabled instance owns a statically allocated, 32-byte-aligned payload
 * pool and an FBQ controller. Descriptor storage and FreeRTOS queues are
 * created by fbq_init(). Initialization failures are rolled back in reverse
 * order so that no partially initialized solution instance remains active.
 */

#include <stdbool.h>
#include <string.h>

#include <compiler/compiler_gcc.h>
#include <compiler/compiler_ld.h>

#include "soln_fbq.h"

#ifdef CONFIG_SOLN_FBQ_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_FBQ_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_FBQ"
#include "log.h"

#if IS_ENABLED(CONFIG_PSRAM)
#define FBQ_BUFFER_ATTR ATTR_NOINIT_PSRAM_SECTION __ALIGNED(32)
#else
#define FBQ_BUFFER_ATTR __ALIGNED(32)
#endif

#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN) || \
    IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN)
/**
 * @brief Reset RAW image metadata before a pooled element is recycled.
 * @param[in,out] elem RAW image element whose final reference was released.
 * @param[in] user_data Unused cleanup context.
 * @note This callback may execute in interrupt context and does not block.
 */
static void solution_fbq_vid_raw_cleanup(fbq_elem_t *elem, void *user_data)
{
    soln_fbq_img_raw_ext_t *ext = soln_fbq_img_raw_ext(elem);

    (void)user_data;
    memset(ext, 0, sizeof(*ext));
    ext->format = IMG_RAW_FRAME_FORMAT_INVALID;
}
#endif

#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN)
static fbq_ctrl_t g_vid_raw_local_fbq;
static bool g_vid_raw_local_initialized;
static FBQ_BUFFER_ATTR uint8_t g_vid_raw_local_buffer[CONFIG_SOLN_FBQ_VID_RAW_LOCAL_NUM][CONFIG_SOLN_FBQ_VID_RAW_LOCAL_SIZE];

fbq_ctrl_t *soln_fbq_vid_raw_local(void)
{
    return &g_vid_raw_local_fbq;
}

/**
 * @brief Initialize the local RAW video fixed pool.
 * @return FBQ_OK on success; a negative FBQ error otherwise.
 */
static int soln_fbq_vid_raw_local_init(void)
{
    const fbq_config_t config = {
        .name = "vid_raw_local",
        .default_type_mask = 0U,
        .ext_type = SOLUTION_FBQ_EXT_IMG_RAW,
        .elem_count = CONFIG_SOLN_FBQ_VID_RAW_LOCAL_NUM,
        .ext_size = sizeof(soln_fbq_img_raw_ext_t),
        .elem_stride = 0U,
        .data_storage = g_vid_raw_local_buffer,
        .data_storage_size = sizeof(g_vid_raw_local_buffer),
        .data_capacity = CONFIG_SOLN_FBQ_VID_RAW_LOCAL_SIZE,
        .data_stride = sizeof(g_vid_raw_local_buffer[0]),
        .cleanup = solution_fbq_vid_raw_cleanup,
        .user_data = NULL,
    };
    int ret = fbq_init(&g_vid_raw_local_fbq, &config);

    if (ret == FBQ_OK) {
        g_vid_raw_local_initialized = true;
    }
    return ret;
}
#endif

#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN)
static fbq_ctrl_t g_vid_raw_remote_fbq;
static bool g_vid_raw_remote_initialized;
static FBQ_BUFFER_ATTR uint8_t g_vid_raw_remote_buffer[CONFIG_SOLN_FBQ_VID_RAW_REMOTE_NUM][CONFIG_SOLN_FBQ_VID_RAW_REMOTE_SIZE];

fbq_ctrl_t *soln_fbq_vid_raw_remote(void)
{
    return &g_vid_raw_remote_fbq;
}

/**
 * @brief Initialize the remote RAW video fixed pool.
 * @return FBQ_OK on success; a negative FBQ error otherwise.
 */
static int soln_fbq_vid_raw_remote_init(void)
{
    const fbq_config_t config = {
        .name = "vid_raw_remote",
        .default_type_mask = 0U,
        .ext_type = SOLUTION_FBQ_EXT_IMG_RAW,
        .elem_count = CONFIG_SOLN_FBQ_VID_RAW_REMOTE_NUM,
        .ext_size = sizeof(soln_fbq_img_raw_ext_t),
        .elem_stride = 0U,
        .data_storage = g_vid_raw_remote_buffer,
        .data_storage_size = sizeof(g_vid_raw_remote_buffer),
        .data_capacity = CONFIG_SOLN_FBQ_VID_RAW_REMOTE_SIZE,
        .data_stride = sizeof(g_vid_raw_remote_buffer[0]),
        .cleanup = solution_fbq_vid_raw_cleanup,
        .user_data = NULL,
    };
    int ret = fbq_init(&g_vid_raw_remote_fbq, &config);

    if (ret == FBQ_OK) {
        g_vid_raw_remote_initialized = true;
    }
    return ret;
}
#endif

#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN)
static fbq_ctrl_t g_vid_jpeg_local_fbq;
static bool g_vid_jpeg_local_initialized;
static FBQ_BUFFER_ATTR uint8_t g_vid_jpeg_local_buffer[CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NUM][CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_SIZE];

fbq_ctrl_t *soln_fbq_vid_jpeg_local(void)
{
    return &g_vid_jpeg_local_fbq;
}

/**
 * @brief Initialize the local JPEG video fixed pool.
 * @return FBQ_OK on success; a negative FBQ error otherwise.
 */
static int soln_fbq_vid_jpeg_local_init(void)
{
    const fbq_config_t config = {
        .name = "vid_jpeg_local",
        .default_type_mask = 0U,
        .ext_type = SOLUTION_FBQ_EXT_NONE,
        .elem_count = CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NUM,
        .ext_size = 0U,
        .elem_stride = 0U,
        .data_storage = g_vid_jpeg_local_buffer,
        .data_storage_size = sizeof(g_vid_jpeg_local_buffer),
        .data_capacity = CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_SIZE,
        .data_stride = sizeof(g_vid_jpeg_local_buffer[0]),
        .cleanup = NULL,
        .user_data = NULL,
    };
    int ret = fbq_init(&g_vid_jpeg_local_fbq, &config);

    if (ret == FBQ_OK) {
        g_vid_jpeg_local_initialized = true;
    }
    return ret;
}
#endif

#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_EN)
static fbq_ctrl_t g_vid_jpeg_remote_fbq;
static bool g_vid_jpeg_remote_initialized;
static FBQ_BUFFER_ATTR uint8_t g_vid_jpeg_remote_buffer[CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_NUM][CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_SIZE];

fbq_ctrl_t *soln_fbq_vid_jpeg_remote(void)
{
    return &g_vid_jpeg_remote_fbq;
}

/**
 * @brief Initialize the remote JPEG video fixed pool.
 * @return FBQ_OK on success; a negative FBQ error otherwise.
 */
static int soln_fbq_vid_jpeg_remote_init(void)
{
    const fbq_config_t config = {
        .name = "vid_jpeg_remote",
        .default_type_mask = 0U,
        .ext_type = SOLUTION_FBQ_EXT_NONE,
        .elem_count = CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_NUM,
        .ext_size = 0U,
        .elem_stride = 0U,
        .data_storage = g_vid_jpeg_remote_buffer,
        .data_storage_size = sizeof(g_vid_jpeg_remote_buffer),
        .data_capacity = CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_SIZE,
        .data_stride = sizeof(g_vid_jpeg_remote_buffer[0]),
        .cleanup = NULL,
        .user_data = NULL,
    };
    int ret = fbq_init(&g_vid_jpeg_remote_fbq, &config);

    if (ret == FBQ_OK) {
        g_vid_jpeg_remote_initialized = true;
    }
    return ret;
}
#endif

#if IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_EN)
static fbq_ctrl_t g_aud_pcm_local_fbq;
static bool g_aud_pcm_local_initialized;
static FBQ_BUFFER_ATTR uint8_t g_aud_pcm_local_buffer[CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM][CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_SIZE];

fbq_ctrl_t *soln_fbq_aud_pcm_local(void)
{
    return &g_aud_pcm_local_fbq;
}

/**
 * @brief Initialize the local PCM audio fixed pool.
 * @return FBQ_OK on success; a negative FBQ error otherwise.
 */
static int soln_fbq_aud_pcm_local_init(void)
{
    const fbq_config_t config = {
        .name = "aud_pcm_local",
        .default_type_mask = 0U,
        .ext_type = SOLUTION_FBQ_EXT_NONE,
        .elem_count = CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM,
        .ext_size = 0U,
        .elem_stride = 0U,
        .data_storage = g_aud_pcm_local_buffer,
        .data_storage_size = sizeof(g_aud_pcm_local_buffer),
        .data_capacity = CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_SIZE,
        .data_stride = sizeof(g_aud_pcm_local_buffer[0]),
        .cleanup = NULL,
        .user_data = NULL,
    };
    int ret = fbq_init(&g_aud_pcm_local_fbq, &config);

    if (ret == FBQ_OK) {
        g_aud_pcm_local_initialized = true;
    }
    return ret;
}
#endif

#if IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_EN)
static fbq_ctrl_t g_aud_pcm_remote_fbq;
static bool g_aud_pcm_remote_initialized;
static FBQ_BUFFER_ATTR uint8_t g_aud_pcm_remote_buffer[CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM][CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_SIZE];

fbq_ctrl_t *soln_fbq_aud_pcm_remote(void)
{
    return &g_aud_pcm_remote_fbq;
}

/**
 * @brief Initialize the remote PCM audio fixed pool.
 * @return FBQ_OK on success; a negative FBQ error otherwise.
 */
static int soln_fbq_aud_pcm_remote_init(void)
{
    const fbq_config_t config = {
        .name = "aud_pcm_remote",
        .default_type_mask = 0U,
        .ext_type = SOLUTION_FBQ_EXT_NONE,
        .elem_count = CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM,
        .ext_size = 0U,
        .elem_stride = 0U,
        .data_storage = g_aud_pcm_remote_buffer,
        .data_storage_size = sizeof(g_aud_pcm_remote_buffer),
        .data_capacity = CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_SIZE,
        .data_stride = sizeof(g_aud_pcm_remote_buffer[0]),
        .cleanup = NULL,
        .user_data = NULL,
    };
    int ret = fbq_init(&g_aud_pcm_remote_fbq, &config);

    if (ret == FBQ_OK) {
        g_aud_pcm_remote_initialized = true;
    }
    return ret;
}
#endif

/**
 * @brief Deinitialize every solution FBQ instance initialized so far.
 * @note Instances are released in reverse initialization order.
 */
static void solution_fbq_deinit_initialized(void)
{
#if IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_EN)
    if (g_aud_pcm_remote_initialized) {
        (void)fbq_deinit(&g_aud_pcm_remote_fbq);
        g_aud_pcm_remote_initialized = false;
    }
#endif
#if IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_EN)
    if (g_aud_pcm_local_initialized) {
        (void)fbq_deinit(&g_aud_pcm_local_fbq);
        g_aud_pcm_local_initialized = false;
    }
#endif
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_EN)
    if (g_vid_jpeg_remote_initialized) {
        (void)fbq_deinit(&g_vid_jpeg_remote_fbq);
        g_vid_jpeg_remote_initialized = false;
    }
#endif
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN)
    if (g_vid_jpeg_local_initialized) {
        (void)fbq_deinit(&g_vid_jpeg_local_fbq);
        g_vid_jpeg_local_initialized = false;
    }
#endif
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN)
    if (g_vid_raw_remote_initialized) {
        (void)fbq_deinit(&g_vid_raw_remote_fbq);
        g_vid_raw_remote_initialized = false;
    }
#endif
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN)
    if (g_vid_raw_local_initialized) {
        (void)fbq_deinit(&g_vid_raw_local_fbq);
        g_vid_raw_local_initialized = false;
    }
#endif
}

/** @copydoc soln_fbq_init_all() */
int soln_fbq_init_all(void)
{
    int ret = FBQ_OK;

    LOG_I("soln_fbq_init_all\r\n");

#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN)
    ret = soln_fbq_vid_raw_local_init();
    if (ret != FBQ_OK) {
        goto failed;
    }
#endif
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN)
    ret = soln_fbq_vid_raw_remote_init();
    if (ret != FBQ_OK) {
        goto failed;
    }
#endif
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN)
    ret = soln_fbq_vid_jpeg_local_init();
    if (ret != FBQ_OK) {
        goto failed;
    }
#endif
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_EN)
    ret = soln_fbq_vid_jpeg_remote_init();
    if (ret != FBQ_OK) {
        goto failed;
    }
#endif
#if IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_EN)
    ret = soln_fbq_aud_pcm_local_init();
    if (ret != FBQ_OK) {
        goto failed;
    }
#endif
#if IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_EN)
    ret = soln_fbq_aud_pcm_remote_init();
    if (ret != FBQ_OK) {
        goto failed;
    }
#endif

    return FBQ_OK;

failed:
    solution_fbq_deinit_initialized();
    return ret;
}
