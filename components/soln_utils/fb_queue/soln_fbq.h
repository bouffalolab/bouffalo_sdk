/**
 * @file soln_fbq.h
 * @brief Fixed Frame Buffer Queue instances used by the BL616 solution.
 *
 * @details
 * This file defines solution-wide producer type masks, RAW image extension
 * metadata, fixed-pool defaults, stream identifiers, and accessors for every
 * FBQ instance enabled by Kconfig. Producers assign one exact type mask before
 * publishing; consumers may combine masks to subscribe to multiple sources.
 */

#ifndef SOLUTION_FBQ_H
#define SOLUTION_FBQ_H

#include <stdint.h>

#include "fbq_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Solution-wide data types identified by format and direct producer.
 *
 * @note These IDs are independent of FBQ instances. Producers assign exactly
 *       one corresponding type mask before publishing; consumers may combine
 *       multiple masks when opening an output subscription.
 */
typedef enum {
    SOLUTION_FBQ_TYPE_IMG_RAW_CAM = 0,
    SOLUTION_FBQ_TYPE_IMG_RAW_UVC,
    SOLUTION_FBQ_TYPE_IMG_RAW_JPEG_DEC,

    SOLUTION_FBQ_TYPE_IMG_JPEG_UVC,
    SOLUTION_FBQ_TYPE_IMG_JPEG_JPEG_ENC,
    SOLUTION_FBQ_TYPE_IMG_JPEG_HB_REC,

    SOLUTION_FBQ_TYPE_AUDIO_PCM_AUADC,
    SOLUTION_FBQ_TYPE_AUDIO_PCM_I2S_IN,
    SOLUTION_FBQ_TYPE_AUDIO_PCM_UAC_IN,
    SOLUTION_FBQ_TYPE_AUDIO_PCM_LOOPBACK,

    SOLUTION_FBQ_TYPE_COUNT,

    SOLUTION_FBQ_TYPE_CUSTOM_N = 24,
    SOLUTION_FBQ_TYPE_CUSTOM_MAX = 31,
} solution_fbq_type_id_t;

/** @name Exact producer type masks
 *  Assign one of these masks to fbq_elem_t::type_mask before publishing.
 *  @{
 */
/** RAW image produced by a local camera interface such as DVP or CSI. */
#define SOLUTION_FBQ_TYPE_MASK_IMG_RAW_CAM        FBQ_TYPE_BIT(SOLUTION_FBQ_TYPE_IMG_RAW_CAM)
/** RAW image received from USB UVC. */
#define SOLUTION_FBQ_TYPE_MASK_IMG_RAW_UVC        FBQ_TYPE_BIT(SOLUTION_FBQ_TYPE_IMG_RAW_UVC)
/** RAW image produced by the JPEG decoder. */
#define SOLUTION_FBQ_TYPE_MASK_IMG_RAW_JPEG_DEC   FBQ_TYPE_BIT(SOLUTION_FBQ_TYPE_IMG_RAW_JPEG_DEC)
/** JPEG image received from USB UVC. */
#define SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_UVC       FBQ_TYPE_BIT(SOLUTION_FBQ_TYPE_IMG_JPEG_UVC)
/** JPEG image produced by the JPEG encoder. */
#define SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_JPEG_ENC  FBQ_TYPE_BIT(SOLUTION_FBQ_TYPE_IMG_JPEG_JPEG_ENC)
/** JPEG image received by HiBooster. */
#define SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_HB_REC    FBQ_TYPE_BIT(SOLUTION_FBQ_TYPE_IMG_JPEG_HB_REC)
/** PCM audio captured by the internal AUADC. */
#define SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_AUADC    FBQ_TYPE_BIT(SOLUTION_FBQ_TYPE_AUDIO_PCM_AUADC)
/** PCM audio captured from an I2S input. */
#define SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_I2S_IN   FBQ_TYPE_BIT(SOLUTION_FBQ_TYPE_AUDIO_PCM_I2S_IN)
/** PCM audio received from USB UAC. */
#define SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_UAC_IN   FBQ_TYPE_BIT(SOLUTION_FBQ_TYPE_AUDIO_PCM_UAC_IN)
/** PCM audio produced by the loopback task. */
#define SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_LOOPBACK FBQ_TYPE_BIT(SOLUTION_FBQ_TYPE_AUDIO_PCM_LOOPBACK)
/** @} */

/**
 * @brief Build an application-defined producer type mask.
 * @param[in] n Custom type offset in the range 0 through 7.
 * @return A single type bit in the application-reserved range [24, 31].
 */
#define SOLUTION_FBQ_TYPE_MASK_CUSTOM_N(n)        FBQ_TYPE_BIT(SOLUTION_FBQ_TYPE_CUSTOM_N + (n))

/** @name Aggregate consumer type masks
 *  Use these masks as fbq_output_open() accept masks when all listed producers
 *  are acceptable to a consumer.
 *  @{
 */
/** All direct RAW capture producers. */
#define SOLUTION_FBQ_TYPE_MASK_IMG_RAW_CAPTURE_ALL \
    (SOLUTION_FBQ_TYPE_MASK_IMG_RAW_CAM | SOLUTION_FBQ_TYPE_MASK_IMG_RAW_UVC)
/** All built-in RAW image producers. */
#define SOLUTION_FBQ_TYPE_MASK_IMG_RAW_ALL \
    (SOLUTION_FBQ_TYPE_MASK_IMG_RAW_CAPTURE_ALL | SOLUTION_FBQ_TYPE_MASK_IMG_RAW_JPEG_DEC)
/** All JPEG producers publishing to the local JPEG pool. */
#define SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_LOCAL_ALL \
    (SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_JPEG_ENC | SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_UVC)
/** All built-in JPEG image producers. */
#define SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_ALL \
    (SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_LOCAL_ALL | SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_HB_REC)
/** All built-in PCM capture producers. */
#define SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_CAPTURE_ALL                                    \
    (SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_AUADC | SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_I2S_IN | \
     SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_UAC_IN)
/** All built-in PCM audio producers. */
#define SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_ALL \
    (SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_CAPTURE_ALL | SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_LOOPBACK)
/** @} */

/** @brief Extension metadata type identifiers used by solution FBQ elements. */
typedef enum {
    SOLUTION_FBQ_EXT_NONE = 0, /**< Element has no solution-specific extension. */
    SOLUTION_FBQ_EXT_IMG_RAW,  /**< Extension is soln_fbq_img_raw_ext_t. */
} solution_fbq_ext_type_t;

/** @brief Pixel formats represented by RAW image FBQ elements. */
typedef enum {
    IMG_RAW_FRAME_FORMAT_INVALID = 0, /**< Invalid or uninitialized format. */
    IMG_RAW_FRAME_FORMAT_RGB565,      /**< Packed 16-bit RGB565 pixels. */
    IMG_RAW_FRAME_FORMAT_RGB888,      /**< Packed 24-bit RGB888 pixels. */
    IMG_RAW_FRAME_FORMAT_NRGB8888,    /**< Packed 32-bit NRGB8888 pixels. */
    IMG_RAW_FRAME_FORMAT_YUYV,        /**< Packed YUYV 4:2:2 pixels. */
    IMG_RAW_FRAME_FORMAT_YUV,         /**< Generic YUV data. */
} img_raw_frame_format_t;

/**
 * @brief Type-specific metadata appended to a RAW image FBQ descriptor.
 *
 * @details Coordinates describe an inclusive image region. The payload size
 * remains in fbq_elem_t::size and is not duplicated in this extension.
 */
typedef struct {
    uint16_t x_start;              /**< Inclusive horizontal start coordinate. */
    uint16_t y_start;              /**< Inclusive vertical start coordinate. */
    uint16_t x_end;                /**< Inclusive horizontal end coordinate. */
    uint16_t y_end;                /**< Inclusive vertical end coordinate. */
    img_raw_frame_format_t format; /**< Pixel format of the RAW payload. */
} soln_fbq_img_raw_ext_t;

/**
 * @brief Get writable RAW image extension metadata.
 * @param[in,out] elem Element with type SOLUTION_FBQ_EXT_IMG_RAW.
 * @return Writable RAW image extension pointer.
 * @note An FBQ assertion is raised when the element or extension is invalid.
 */
static inline soln_fbq_img_raw_ext_t *soln_fbq_img_raw_ext(fbq_elem_t *elem)
{
    FBQ_ASSERT(elem != NULL);
    FBQ_ASSERT(elem->ext_type == SOLUTION_FBQ_EXT_IMG_RAW);
    FBQ_ASSERT(elem->ext_size >= sizeof(soln_fbq_img_raw_ext_t));
    return (soln_fbq_img_raw_ext_t *)fbq_elem_extension(elem);
}

/**
 * @brief Get read-only RAW image extension metadata.
 * @param[in] elem Element with type SOLUTION_FBQ_EXT_IMG_RAW.
 * @return Read-only RAW image extension pointer.
 * @note An FBQ assertion is raised when the element or extension is invalid.
 */
static inline const soln_fbq_img_raw_ext_t *soln_fbq_img_raw_ext_const(const fbq_elem_t *elem)
{
    FBQ_ASSERT(elem != NULL);
    FBQ_ASSERT(elem->ext_type == SOLUTION_FBQ_EXT_IMG_RAW);
    FBQ_ASSERT(elem->ext_size >= sizeof(soln_fbq_img_raw_ext_t));
    return (const soln_fbq_img_raw_ext_t *)elem->extension;
}

/** @name Local RAW video FBQ
 *  Configuration and streams for locally captured RAW images.
 *  @{
 */
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN)
/** Payload capacity in bytes; automatic mode uses width * height * 2. */
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_LOCAL_SIZE_AUTO) || !defined(CONFIG_SOLN_FBQ_VID_RAW_LOCAL_SIZE)
#undef CONFIG_SOLN_FBQ_VID_RAW_LOCAL_SIZE
#define CONFIG_SOLN_FBQ_VID_RAW_LOCAL_SIZE (CONFIG_SOLN_VID_DEFAULT_WIDTH * CONFIG_SOLN_VID_DEFAULT_HEIGHT * 2)
#endif
/** Default number of elements in the local RAW video fixed pool. */
#ifndef CONFIG_SOLN_FBQ_VID_RAW_LOCAL_NUM
#define CONFIG_SOLN_FBQ_VID_RAW_LOCAL_NUM (2)
#endif
/** Output ID reserved for the JPEG encoder consumer; defaults to 0. */
#ifndef CONFIG_SOLN_FBQ_VID_RAW_LOCAL_ENC_ID
#define CONFIG_SOLN_FBQ_VID_RAW_LOCAL_ENC_ID (0U)
#endif
/** Queue depth reserved for the JPEG encoder consumer; defaults to 1. */
#ifndef CONFIG_SOLN_FBQ_VID_RAW_LOCAL_ENC_DEPTH
#define CONFIG_SOLN_FBQ_VID_RAW_LOCAL_ENC_DEPTH (1U)
#endif
/** Output ID reserved for the display consumer; defaults to 1. */
#ifndef CONFIG_SOLN_FBQ_VID_RAW_LOCAL_DISP_ID
#define CONFIG_SOLN_FBQ_VID_RAW_LOCAL_DISP_ID (1U)
#endif
/** Queue depth reserved for the display consumer; defaults to 1. */
#ifndef CONFIG_SOLN_FBQ_VID_RAW_LOCAL_DISP_DEPTH
#define CONFIG_SOLN_FBQ_VID_RAW_LOCAL_DISP_DEPTH (1U)
#endif

/**
 * @brief Get the local RAW video FBQ controller.
 * @return Pointer to the persistent local RAW video controller.
 * @note Call soln_fbq_init_all() before using the returned controller.
 */
fbq_ctrl_t *soln_fbq_vid_raw_local(void);
#endif
/** @} */

/** @name Remote RAW video FBQ
 *  Configuration and streams for decoded RAW images.
 *  @{
 */
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN)
/** Payload capacity in bytes; automatic mode uses width * height * 2. */
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_REMOTE_SIZE_AUTO) || !defined(CONFIG_SOLN_FBQ_VID_RAW_REMOTE_SIZE)
#undef CONFIG_SOLN_FBQ_VID_RAW_REMOTE_SIZE
#define CONFIG_SOLN_FBQ_VID_RAW_REMOTE_SIZE (CONFIG_SOLN_VID_DEFAULT_WIDTH * CONFIG_SOLN_VID_DEFAULT_HEIGHT * 2)
#endif
/** Default number of elements in the remote RAW video fixed pool. */
#ifndef CONFIG_SOLN_FBQ_VID_RAW_REMOTE_NUM
#define CONFIG_SOLN_FBQ_VID_RAW_REMOTE_NUM (2)
#endif
/** Output ID reserved for the display consumer; defaults to 1. */
#ifndef CONFIG_SOLN_FBQ_VID_RAW_REMOTE_DISP_ID
#define CONFIG_SOLN_FBQ_VID_RAW_REMOTE_DISP_ID (1U)
#endif
/** Queue depth reserved for the display consumer; defaults to 1. */
#ifndef CONFIG_SOLN_FBQ_VID_RAW_REMOTE_DISP_DEPTH
#define CONFIG_SOLN_FBQ_VID_RAW_REMOTE_DISP_DEPTH (1U)
#endif

/**
 * @brief Get the remote RAW video FBQ controller.
 * @return Pointer to the persistent remote RAW video controller.
 * @note Call soln_fbq_init_all() before using the returned controller.
 */
fbq_ctrl_t *soln_fbq_vid_raw_remote(void);
#endif
/** @} */

/** @name Local JPEG video FBQ
 *  Configuration and streams for locally encoded or captured JPEG images.
 *  @{
 */
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN)
/** Default payload capacity of one local JPEG video element, in bytes. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_SIZE
#define CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_SIZE (128U * 1024U)
#endif
/** Default number of elements in the local JPEG video fixed pool. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NUM
#define CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NUM (6)
#endif
/** Output ID reserved for the SD-card recorder consumer; defaults to 0. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_SD_ID
#define CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_SD_ID (0U)
#endif
/** Queue depth reserved for the SD-card recorder consumer; defaults to 2. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_SD_DEPTH
#define CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_SD_DEPTH (2U)
#endif
/** Output ID reserved for the Wi-Fi/RTSP/HiBooster consumer; defaults to 2. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NET_ID
#define CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NET_ID (2U)
#endif
/** Queue depth reserved for the Wi-Fi/RTSP/HiBooster consumer; defaults to 2. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NET_DEPTH
#define CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NET_DEPTH (2U)
#endif

/**
 * @brief Get the local JPEG video FBQ controller.
 * @return Pointer to the persistent local JPEG video controller.
 * @note Call soln_fbq_init_all() before using the returned controller.
 */
fbq_ctrl_t *soln_fbq_vid_jpeg_local(void);
#endif
/** @} */

/** @name Remote JPEG video FBQ
 *  Configuration and streams for remotely received JPEG images.
 *  @{
 */
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_EN)
/** Default payload capacity of one remote JPEG video element, in bytes. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_SIZE
#define CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_SIZE (128U * 1024U)
#endif
/** Default number of elements in the remote JPEG video fixed pool. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_NUM
#define CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_NUM (6)
#endif
/** Output ID reserved for the SD-card recorder consumer; defaults to 0. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_SD_ID
#define CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_SD_ID (0U)
#endif
/** Queue depth reserved for the SD-card recorder consumer; defaults to 3. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_SD_DEPTH
#define CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_SD_DEPTH (3U)
#endif
/** Output ID reserved for the JPEG decoder consumer; defaults to 2. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_DEC_ID
#define CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_DEC_ID (2U)
#endif
/** Queue depth reserved for the JPEG decoder consumer; defaults to 3. */
#ifndef CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_DEC_DEPTH
#define CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_DEC_DEPTH (3U)
#endif

/**
 * @brief Get the remote JPEG video FBQ controller.
 * @return Pointer to the persistent remote JPEG video controller.
 * @note Call soln_fbq_init_all() before using the returned controller.
 */
fbq_ctrl_t *soln_fbq_vid_jpeg_remote(void);
#endif
/** @} */

/** @name Local PCM audio FBQ
 *  Configuration and streams for captured PCM audio.
 *  @{
 */
#if IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_EN)
/** Default payload capacity of one local PCM audio element, in bytes. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_SIZE
#define CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_SIZE (2048U)
#endif
/** Default number of elements in the local PCM audio fixed pool. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM
#define CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM (20)
#endif
/** Output ID reserved for the audio loopback consumer; defaults to 0. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_LOOPBACK_ID
#define CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_LOOPBACK_ID (0U)
#endif
/** Queue depth reserved for the audio loopback consumer; defaults to 20. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_LOOPBACK_DEPTH
#define CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_LOOPBACK_DEPTH (20U)
#endif
/**
 * @brief Get the local PCM audio FBQ controller.
 * @return Pointer to the persistent local PCM audio controller.
 * @note Call soln_fbq_init_all() before using the returned controller.
 */
fbq_ctrl_t *soln_fbq_aud_pcm_local(void);
#endif
/** @} */

/** @name Remote PCM audio FBQ
 *  Configuration and streams for PCM audio ready for playback or storage.
 *  @{
 */
#if IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_EN)
/** Default payload capacity of one remote PCM audio element, in bytes. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_SIZE
#define CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_SIZE (2048U)
#endif
/** Default number of elements in the remote PCM audio fixed pool. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM
#define CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM (20)
#endif
/** Output ID reserved for the internal AUDAC consumer; defaults to 0. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_AUDAC_ID
#define CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_AUDAC_ID (0U)
#endif
/** Queue depth reserved for the internal AUDAC consumer; defaults to 20. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_AUDAC_DEPTH
#define CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_AUDAC_DEPTH (20U)
#endif
/** Output ID reserved for the external I2S consumer; defaults to 1. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_I2S_ID
#define CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_I2S_ID (1U)
#endif
/** Queue depth reserved for the external I2S consumer; defaults to 20. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_I2S_DEPTH
#define CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_I2S_DEPTH (20U)
#endif
/** Output ID reserved for the SD-card recorder consumer; defaults to 2. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_SD_ID
#define CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_SD_ID (2U)
#endif
/** Queue depth reserved for the SD-card recorder consumer; defaults to 10. */
#ifndef CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_SD_DEPTH
#define CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_SD_DEPTH (10U)
#endif

/**
 * @brief Get the remote PCM audio FBQ controller.
 * @return Pointer to the persistent remote PCM audio controller.
 * @note Call soln_fbq_init_all() before using the returned controller.
 */
fbq_ctrl_t *soln_fbq_aud_pcm_remote(void);
#endif
/** @} */

/**
 * @brief Initialize every fixed FBQ instance enabled by Kconfig.
 * @retval FBQ_OK All enabled instances were initialized successfully.
 * @return A negative FBQ error when an instance cannot be initialized.
 * @note On failure, all instances initialized by this call are deinitialized
 *       in reverse order before the error is returned.
 * @note This function must be called from task context before any accessor's
 *       controller is used.
 */
int soln_fbq_init_all(void);

#ifdef __cplusplus
}
#endif

#endif /* SOLUTION_FBQ_H */