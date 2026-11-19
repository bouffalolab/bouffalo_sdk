/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
/**
 * @file xcodec_dev.h
 * @brief Audio device lifecycle, PCM input/output and converter controls.
 *
 * Create the data and optional Codec interfaces before creating a device. The
 * device borrows these interfaces; delete the device before releasing them.
 * A data interface may serve separate input and output devices, with one owner
 * per direction. A Codec interface must belong to only one device.
 *
 * Calls on the same handle must be serialized by the caller. Stop all ongoing
 * reads and writes before closing or deleting the device. There is no internal
 * device lock. Serialize open/close calls across devices sharing a data interface.
 *
 * Unless stated otherwise, an int return value is XCODEC_DEV_OK (0) on success
 * or a negative XCODEC_DEV_* / backend error on failure. PCM lengths are bytes,
 * not samples; read/write never return a successful byte count.
 */
#ifndef XCODEC_DEV_H
#define XCODEC_DEV_H

#include "xcodec_if.h"
#include "xcodec_data_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Channel defaults read by board initialization. Serialize writes with init;
 *  set fields before init, or deinit/reinit to apply changes to new interfaces. */
extern xcodec_channel_cfg_t xcodec_channel_default_config;

/** Opaque device handle returned by xcodec_dev_new(). */
typedef void *xcodec_dev_handle_t;

/** Interfaces and direction used to construct a device. */
typedef struct {
    xcodec_dev_type_t dev_type;          /**< IN, OUT or IN_OUT; NONE is invalid. */
    const xcodec_if_t *codec_if;         /**< Borrowed, initialized Codec interface; may be NULL. */
    const xcodec_data_if_t *data_if;     /**< Borrowed, initialized data interface; required. */
} xcodec_dev_cfg_t;

/**
 * @brief Allocate a device without opening its audio paths.
 * @param[in] cfg Device direction and interfaces. The configuration is copied,
 *                so the structure itself need not outlive this call.
 * @return New handle, or NULL for invalid configuration or allocation failure.
 * @note Does not initialize hardware, create interfaces or take their ownership.
 *       The default close policy disables both the Codec and data paths.
 */
xcodec_dev_handle_t xcodec_dev_new(xcodec_dev_cfg_t *cfg);

/**
 * @brief Get the device API implementation version.
 * @return Static, null-terminated version string; do not modify or free it.
 */
const char *xcodec_dev_get_version(void);

/**
 * @brief Configure the PCM format and enable the device's audio paths.
 * @param[in] codec Device handle.
 * @param[in] fs Requested format; supported rates, widths, slots and channel
 *               masks depend on the data and Codec interfaces. The I2S backend
 *               uses its optional board clock callback before configuring
 *               the running rate; no resampling is performed. Both
 *               directions sharing I2S must use the same physical sample rate.
 * @return 0 on success, XCODEC_DEV_INVALID_ARG for invalid format/handle,
 *         XCODEC_DEV_NOT_SUPPORT for missing interface capabilities,
 *         XCODEC_DEV_WRONG_STATE for pending teardown, or a backend error.
 * @note An already open device returns success for a valid format argument
 *       without comparing or applying that format. Close before changing it.
 *       Successfully set volume, global gain and mute values are reapplied.
 *       Shared I2S starts all configured DMA directions on the first open;
 *       unopened output sends silence and unopened input is discarded. No
 *       application silence writes or initial read are required for startup.
 *       Format errors do not roll back earlier format changes. Enable/restore
 *       errors attempt cleanup and return the original error; if cleanup fails,
 *       call close and check its result before retrying or deleting the device.
 */
int xcodec_dev_open(xcodec_dev_handle_t codec, xcodec_dev_sample_info_t *fs);

/**
 * @brief Read PCM from an open input device.
 * @param[in] codec Device with IN capability, opened successfully.
 * @param[out] data Writable buffer of at least len bytes.
 * @param[in] len Requested byte count, greater than zero.
 * @return 0 only when the entire buffer has been filled; negative error otherwise.
 * @note May block until data arrives or the backend timeout expires. An error
 *       may follow a partial read; the API does not report the partial length.
 *       Platform backends retain remaining PCM frame bytes for the next read;
 *       neither PCM frame nor DMA block alignment is required for len.
 */
int xcodec_dev_read(xcodec_dev_handle_t codec, void *data, int len);

/**
 * @brief Write PCM to an open output device.
 * @param[in] codec Device with OUT capability, opened successfully.
 * @param[in] data PCM buffer containing at least len bytes.
 * @param[in] len Byte count, greater than zero.
 * @return 0 only when the backend has accepted the entire buffer; negative error
 *         otherwise. Success does not mean playback has finished.
 * @note May block for backend buffer space or timeout. An error may follow a
 *       partial write; retrying the whole buffer can duplicate audio. Backends
 *       retain incomplete PCM frames until subsequent writes complete them;
 *       closing discards any remaining fragment. Platform backends require no
 *       PCM frame or DMA block alignment for len.
 */
int xcodec_dev_write(xcodec_dev_handle_t codec, void *data, int len);

/**
 * @brief Set playback volume on an open output device.
 * @param[in] codec Open device with OUT capability.
 * @param[in] volume Volume in the inclusive range 0..100. Zero maps to -96 dB;
 *                   1..100 maps to -49.5..0 dB in 0.5 dB steps at the Codec
 *                   interface. Actual range/quantization depends on the driver.
 * @return 0 on success, negative error otherwise; XCODEC_DEV_NOT_SUPPORT if the
 *         Codec has no volume control.
 * @note Caches the requested value only after a successful hardware operation.
 */
int xcodec_dev_set_out_vol(xcodec_dev_handle_t codec, int volume);

/**
 * @brief Get the cached playback volume, without reading hardware.
 * @param[in] codec Open device with OUT capability and Codec volume control.
 * @param[out] volume Last successfully requested volume, or 0 before the first
 *                    successful set. This initial value may differ from hardware.
 * @return 0 on success, negative error otherwise.
 */
int xcodec_dev_get_out_vol(xcodec_dev_handle_t codec, int *volume);

/**
 * @brief Set playback mute on an open output device.
 * @param[in] codec Open device with OUT capability.
 * @param[in] mute true to mute, false to unmute.
 * @return 0 on success, negative error otherwise; XCODEC_DEV_NOT_SUPPORT if the
 *         Codec has no output mute control.
 * @note Updates the cached state only on success; does not stop data transfer.
 */
int xcodec_dev_set_out_mute(xcodec_dev_handle_t codec, bool mute);

/**
 * @brief Get the cached playback mute state, without reading hardware.
 * @param[in] codec Open device with OUT capability and Codec output mute control.
 * @param[out] muted Last successfully requested state, or false before the first
 *                   successful set. This initial value may differ from hardware.
 * @return 0 on success, negative error otherwise.
 */
int xcodec_dev_get_out_mute(xcodec_dev_handle_t codec, bool *muted);

/**
 * @brief Set microphone gain on an open input device.
 * @param[in] codec Open device with IN capability.
 * @param[in] db_value Finite gain in dB; range and quantization are driver-specific.
 * @return 0 on success, negative error otherwise; XCODEC_DEV_NOT_SUPPORT if the
 *         Codec has no microphone gain control.
 * @note Caches the requested global gain only after a successful operation.
 */
int xcodec_dev_set_in_gain(xcodec_dev_handle_t codec, float db_value);

/**
 * @brief Set gain for selected microphone channels on an open input device.
 * @param[in] codec Open device with IN capability.
 * @param[in] channel_mask Nonzero mask of channels; bit n selects channel n.
 *                         Supported channel mapping is driver-specific.
 * @param[in] db_value Finite gain in dB; range and quantization are driver-specific.
 * @return 0 on success, negative error otherwise; XCODEC_DEV_NOT_SUPPORT if
 *         per-channel gain is unavailable. Does not fall back to global gain.
 * @note Does not update the global gain cache and is not replayed on reopen.
 */
int xcodec_dev_set_in_channel_gain(xcodec_dev_handle_t codec, uint16_t channel_mask, float db_value);

/**
 * @brief Get the cached global microphone gain, without reading hardware.
 * @param[in] codec Open device with IN capability and Codec global gain control.
 * @param[out] db_value Last successfully requested global gain in dB, or 0 before
 *                      the first successful set; not a hardware measurement.
 * @return 0 on success, negative error otherwise.
 * @note Does not reflect gain set through xcodec_dev_set_in_channel_gain().
 */
int xcodec_dev_get_in_gain(xcodec_dev_handle_t codec, float *db_value);

/**
 * @brief Set microphone mute on an open input device.
 * @param[in] codec Open device with IN capability.
 * @param[in] mute true to mute, false to unmute.
 * @return 0 on success, negative error otherwise; XCODEC_DEV_NOT_SUPPORT if the
 *         Codec has no input mute control.
 * @note Updates the cached state only on success; does not stop data capture.
 */
int xcodec_dev_set_in_mute(xcodec_dev_handle_t codec, bool mute);

/**
 * @brief Get the cached microphone mute state, without reading hardware.
 * @param[in] codec Open device with IN capability and Codec input mute control.
 * @param[out] muted Last successfully requested state, or false before the first
 *                   successful set. This initial value may differ from hardware.
 * @return 0 on success, negative error otherwise.
 */
int xcodec_dev_get_in_mute(xcodec_dev_handle_t codec, bool *muted);

/**
 * @brief Read a Codec register through the control interface.
 * @param[in] codec Device handle; the device need not be open.
 * @param[in] reg Register address in the driver's native encoding.
 * @param[out] val Register value; use it only on success.
 * @return 0 on success, negative error otherwise; XCODEC_DEV_NOT_SUPPORT if
 *         register reads are unavailable. The driver checks hardware readiness.
 */
int xcodec_dev_read_reg(xcodec_dev_handle_t codec, int reg, int *val);

/**
 * @brief Write a Codec register through the control interface.
 * @param[in] codec Device handle; the device need not be open.
 * @param[in] reg Register address in the driver's native encoding.
 * @param[in] val Register value in the driver's native encoding.
 * @return 0 on success, negative error otherwise; XCODEC_DEV_NOT_SUPPORT if
 *         register writes are unavailable. The driver checks hardware readiness.
 * @note Direct register writes do not update device volume/gain/mute caches.
 */
int xcodec_dev_write_reg(xcodec_dev_handle_t codec, int reg, int val);

/**
 * @brief Ask the Codec driver to dump its registers using its diagnostic output.
 * @param[in] codec Device handle; the device need not be open.
 * @return 0 if the dump callback was invoked, XCODEC_DEV_INVALID_ARG for a NULL
 *         handle, or XCODEC_DEV_NOT_SUPPORT if the callback is absent.
 * @note The callback returns void, so errors during the dump cannot be returned.
 */
int xcodec_dev_dump_reg(xcodec_dev_handle_t codec);

/**
 * @brief Select whether close disables the Codec path in addition to data.
 * @param[in] codec Device handle; the device need not be open.
 * @param[in] disable true (default) to disable the Codec path on close;
 *                    false to leave it enabled. Application IO stops on close;
 *                    shared I2S DMA stops when the last direction closes.
 * @return 0 on success, XCODEC_DEV_INVALID_ARG for a NULL handle.
 * @note Changes only the policy; does not operate hardware immediately. With
 *       false, even deleting the device may leave its Codec path enabled.
 */
int xcodec_dev_set_disable_when_closed(xcodec_dev_handle_t codec, bool disable);

/**
 * @brief Stop the device's audio paths without freeing the handle or interfaces.
 * @param[in] codec Device handle.
 * @return 0 on success, XCODEC_DEV_INVALID_ARG for a NULL handle, or the first
 *         backend error while disabling Codec/data paths.
 * @note Stop ongoing I/O before calling. Marks the device closed immediately;
 *       data shutdown is attempted even if Codec shutdown fails. Failed path
 *       state is retained for another close attempt. Closing an already closed
 *       device is harmless. Cached controls remain available for the next open.
 *       Codec shutdown follows xcodec_dev_set_disable_when_closed().
 *       A shared I2S interface keeps DMA and clocks running for the other open
 *       direction, sending silence or discarding capture for the closed path.
 */
int xcodec_dev_close(xcodec_dev_handle_t codec);

/**
 * @brief Close and free a device, leaving its borrowed interfaces untouched.
 * @param[in] codec Device handle, or NULL for no action. Invalid after successful
 *                  deletion; clear the caller's copy of the handle afterwards.
 * @note Stop ongoing I/O before calling. Frees the object only if close succeeds;
 *       on close failure the handle remains valid for retry. Call close and
 *       check its result first when teardown errors must be observed, since
 *       delete has no return value. Release interfaces only after device deletion.
 */
void xcodec_dev_delete(xcodec_dev_handle_t codec);

#ifdef __cplusplus
}
#endif
#endif
