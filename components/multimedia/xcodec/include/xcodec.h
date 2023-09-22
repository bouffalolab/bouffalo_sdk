/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef _XCODEC_H_
#define _XCODEC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#include <xutils/mringbuffer.h>

typedef enum {
    XCODEC_OK          =  0,
    XCODEC_ERROR       = -1,
    XCODEC_BUSY        = -2,
    XCODEC_TIMEOUT     = -3,
    XCODEC_UNSUPPORTED = -4
} xcodec_error_t;

typedef struct {
   uint8_t          readable;
   uint8_t          writeable;
   uint8_t          error;
} xcodec_state_t;

typedef struct xcodec_dev xcodec_dev_t;

struct xcodec_dev {
    unsigned long   reg_base;
    uint8_t         irq_num;
    uint8_t         idx;
    uint16_t        dev_tag;
    void            (*irq_handler)(void *);
#ifdef CONFIG_PM
    //xcodec_pm_dev_t pm_dev;
#endif
};

typedef enum {
    XCODEC_EVENT_PERIOD_READ_COMPLETE        = 0U,  ///< A peroid data read completed
    XCODEC_EVENT_PERIOD_WRITE_COMPLETE       = 1U,  ///< A peroid data write completed
    XCODEC_EVENT_WRITE_BUFFER_EMPTY          = 2U,  ///< Fifo is empty
    XCODEC_EVENT_READ_BUFFER_FULL            = 3U,  ///< Fifo is full
    XCODEC_EVENT_ERROR_OVERFLOW              = 4U,  ///< Fifo overflow error
    XCODEC_EVENT_ERROR_UNDERFLOW             = 5U,  ///< Fifo underflow error
    XCODEC_EVENT_ERROR                       = 6U,  ///< The device has a hardware error
} xcodec_event_t;

struct xcodec;
typedef struct xcodec xcodec_t;
typedef struct xcodec_output xcodec_output_t;

struct xcodec_dma_ch {
    // void                *parent;
    int8_t              ctrl_id;
    int8_t              ch_id;
    uint8_t             dma_tx_dstPeriph;//Destination peripheral select
    uint8_t             dma_rx_srcPeriph;//Srcous peripheral select
    // msp_slist_t             next;
};

typedef struct xcodec_dma_ch xcodec_dma_ch_t;

struct xcodec_output {
    xcodec_t              *codec;
    uint32_t              ch_idx;            ///< Codec output channel idx
    void (*callback)(xcodec_output_t *output, xcodec_event_t event, void *arg);
    void                  *arg;
    m_ringbuf_t           *ring_buf;         ///< The xcodec_ringbuf used to save audio data
    uint32_t              period;            ///< When the period data is sent, the callback function will be called
    uint32_t              sound_channel_num; ///< Number of sound channel
    xcodec_dma_ch_t       *dma;              ///< Dma channel handle
    xcodec_state_t        state;             ///< Codec output channel current state
    void                  *priv;
    struct xcodec_output  *next;
    uint32_t              pa_pin;
};

typedef struct xcodec_input xcodec_input_t;
struct xcodec_input {
    xcodec_t              *codec;
    uint32_t              ch_idx;             ///< Codec input channel idx
    void (*callback)(xcodec_input_t *input, xcodec_event_t event, void *arg);
    void                  *arg;
    m_ringbuf_t           *ring_buf;         ///< The xcodec_ringbuf used to save audio data
    uint32_t              period;             ///< When the period data is received, the callback function will be called
    uint32_t              sound_channel_num;  ///< Number of sound channel
    xcodec_dma_ch_t       *dma;               ///< Codec input channel current state
    xcodec_state_t        state;              ///< Dma channel handle
    void                  *priv;
    struct xcodec_input   *next;
};

struct xcodec {
    xcodec_dev_t          dev;         ///< Codec hw-device info
    xcodec_output_t       *output_chs; ///< Codec output channel operate handle
    xcodec_input_t        *input_chs;  ///< Codec input channel operate handle
    void                  *priv;       ///< User private param passed to user callback
};

typedef enum {
    XCODEC_OUTPUT_SINGLE_ENDED = 0,        ///< Single-ended output
    XCODEC_OUTPUT_DIFFERENCE   = 1,        ///< Differential output
} xcodec_output_mode_t;

typedef enum {
    XCODEC_INPUT_SINGLE_ENDED  = 0,        ///< Single-ended input
    XCODEC_INPUT_DIFFERENCE    = 1,        ///< Differential input
} xcodec_input_mode_t;

typedef struct {
    uint32_t              sample_rate;                ///< Input data sample rate
    uint32_t              bit_width;                  ///< Input data sample width
    xcodec_input_mode_t   mode;                       ///< Input work mode
    uint8_t               *buffer;                    ///< The buffer used to save audio data
    uint32_t              buffer_size;                ///< Input buffer size
    uint32_t              period;                     ///< When a peroid data is reached,the callback function is called
    uint32_t              sound_channel_num;          ///< Number of soundtrack per channel
    uint32_t              mic_type;                   ///< MIC type: 1 is digital, 0 is analog
    uint32_t              pdm_channel;                ///< used for digital,0 is pdm0, 1 is pdm1
    uint32_t              pdm_in_pin;                 ///< used for digital
    uint32_t              pdm_clk_pin;                ///< used for digital
    uint32_t              negative_pin;               ///< used for negative_pin
    uint32_t              positive_pin;               ///< used for positive_pin
} xcodec_input_config_t;

typedef struct {
    uint32_t              sample_rate;                ///< Output data sample rate
    uint32_t              bit_width;                  ///< Onput data sample width
    xcodec_output_mode_t  mode;                       ///< Onput work mode
    uint8_t               *buffer;                    ///< The buffer used to save audio data
    uint32_t              buffer_size;                ///< Output buffer size
    uint32_t              period;                     ///< When a peroid data is reached,the callback function is called
    uint32_t              sound_channel_num;          ///< Number of soundchannel per channel
    uint32_t              pa_pin;
    uint32_t              negative_pin;               ///< used for negative_pin
    uint32_t              positive_pin;               ///< used for positive_pin
} xcodec_output_config_t;

/**
  \brief       Init the codec according to the specified
  \param[in]   codec  Codec handle to operate
  \param[in]   idx    Codec interface idx
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_init(xcodec_t *codec, uint32_t idx);

/**
  \brief       Codec uninit
  \param[in]   codec    Codec handle to operate
  \return      None
*/
void xcodec_uninit(xcodec_t *codec);

/**
  \brief       Open a codec output channel
  \param[in]   codec     Codec handle to operate
  \param[in]   ch        Codec output channel handle
  \param[in]   ch_idx    Codec output channel idx
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_open(xcodec_t *codec, xcodec_output_t *ch, uint32_t ch_idx);

/**
  \brief       Config codec output channel
  \param[in]   ch        Codec output channel handle
  \param[in]   config    Codec channel param. \ref xcodec_output_config_t
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_config(xcodec_output_t *ch, xcodec_output_config_t *config);

/**
  \brief       Attach the callback handler to codec output
  \param[in]   ch     Codec output channel handle
  \param[in]   cb     Callback function
  \param[in]   arg    User private param
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_attach_callback(xcodec_output_t *ch, void *callback, void *arg);

/**
  \brief       Detach the callback handler
  \param[in]   ch    Codec output channel handle
  \return      None
*/
void xcodec_output_detach_callback(xcodec_output_t *ch);

/**
  \brief       Close a codec output channel
  \param[in]   ch    Codec output channel handle
  \return      error code \ref xcodec_error_t
*/
void xcodec_output_close(xcodec_output_t *ch);

/**
  \brief       Link DMA channel to codec output channel
  \param[in]   ch     Codec output channel handle
  \param[in]   dma    The codec output DMA channel handle, when it is NULL means to unlink the channel
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_link_dma(xcodec_output_t *ch, xcodec_dma_ch_t *dma);//void

/**
  \brief       Send an amount of data to buffer in blocking mode
  \param[in]   ch      The codec output channel
  \param[in]   data    Pointer to send data buffer
  \param[in]   size    Send data size
  \return      the num of data witch is send successful
*/
uint32_t xcodec_output_write(xcodec_output_t *ch, const void *data, uint32_t size);

/**
  \brief       Send data to the buffer with asynchronous sending
               The data is first written to the buffer and then output through the codec interface
               This function does not block, and the return value is the number
               Of data that was successfully written to the buffer
  \param[in]   ch      The codec output channel
  \param[in]   data    Pointer to send data buffer
  \param[in]   size    Send data size
  \return      The data size that send to buffer
*/
uint32_t xcodec_output_write_async(xcodec_output_t *ch, const void *data, uint32_t size);

/**
  \brief       Start sending data from the buffer
  \param[in]   ch    Codec output channel handle
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_start(xcodec_output_t *ch);

/**
  \brief       Stop sending data from the buffer
  \param[in]   ch    Codec output channel handle
  \return      None
*/
void xcodec_output_stop(xcodec_output_t *ch);

/**
  \brief       Pause sending data from the buffer
  \param[in]   ch    Codec output channel handle
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_pause(xcodec_output_t *ch);

/**
  \brief       Resume sending data from the buffer
  \param[in]   ch    Codec output channel handle
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_resume(xcodec_output_t *ch);

/**
  \brief       Get output-buffer free space
  \param[in]   ch    Codec output channel handle
  \return      Buffer free space (bytes)
*/
uint32_t xcodec_output_buffer_avail(xcodec_output_t *ch);

/**
  \brief       Get used space of output-buffer
  \param[in]   ch    Codec output channel handle
  \return      Buffer free space (bytes)
*/
uint32_t xcodec_output_buffer_remain(xcodec_output_t *ch);

/**
  \brief       Reset the buf, discard all data in the buffer
  \param[in]   ch    Codec output channel handle
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_buffer_reset(xcodec_output_t *ch);

/**
  \brief       Mute codec ouput channel
  \param[in]   ch    Codec output channel handle
  \param[in]   en    True enable codec mute. false disable codec mute
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_mute(xcodec_output_t *ch, bool enable);

/**
  \brief       Set codec ouput channel digital gain
  \param[in]   ch     Codec output channel handle
  \param[in]   val    Gain val
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_digital_gain(xcodec_output_t *ch, float val);

/**
  \brief       Set codec ouput channel analog gain
  \param[in]   ch     Codec output channel handle
  \param[in]   val    Gain val
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_analog_gain(xcodec_output_t *ch, float val);

/**
  \brief       Set codec ouput channel mix gain
  \param[in]   ch     Codec output channel handle
  \param[in]   val    Gain val
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_output_mix_gain(xcodec_output_t *ch, float val);

/**
  \brief       Get codec output channel state
  \param[in]   ch       Codec output channel handle
  \param[out]  state    Channel state. \ref xcodec_state_t
  \return      channel state
*/
xcodec_error_t xcodec_output_get_state(xcodec_output_t *ch, xcodec_state_t *state);

/**
  \brief       Open a codec input channel
  \param[in]   codec     Codec handle to operate
  \param[in]   ch        Codec input channel handle
  \param[in]   ch_idx    Codec input channel idx
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_input_open(xcodec_t *codec, xcodec_input_t *ch, uint32_t ch_idx);

/**
  \brief       Config codec input channel
  \param[in]   ch        Codec input channel handle
  \param[in]   config    Codec channel prarm. \ref xcodec_input_config_t
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_input_config(xcodec_input_t *ch, xcodec_input_config_t *config);

/**
  \brief       Attach the callback handler to codec output
  \param[in]   ch     Codec input channel handle
  \param[in]   cb     Callback function
  \param[in]   arg    User private param for event callback
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_input_attach_callback(xcodec_input_t *ch, void *callback, void *arg);

/**
  \brief       Detach the callback handler
  \param[in]   ch    Codec input channel handle
  \return      None
*/
void xcodec_input_detach_callback(xcodec_input_t *ch);

/**
  \brief       Close a codec input channel
  \param[in]   ch    Codec input channel handle
  \return      None
*/
void xcodec_input_close(xcodec_input_t *ch);

/**
  \brief       Link DMA channel to codec input channel
  \param[in]   ch     Codec input channel handle
  \param[in]   dma    The codec input DMA channel handle, when it is NULL means to unlink the channel
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_input_link_dma(xcodec_input_t *ch, xcodec_dma_ch_t *dma);

/**
  \brief       Read an amount of data in blocking mode
  \param[in]   ch      Codec input channel handle
  \param[in]   data    Pointer to receive data buffer
  \param[in]   size    Receive data size
  \return      The size of data read successfully
*/
uint32_t xcodec_input_read(xcodec_input_t *ch, void *data, uint32_t size);

/**
  \brief       Read data from the buffer
               using asynchronous receive
               this function read data from the buffer, returns the number of successful receive
               and returns 0 if there is no data in the buffer
  \param[in]   ch      Codec input channel handle
  \param[in]   data    Pointer to receive data buffer
  \param[in]   size    Receive data size
  \return      The size of data read successfully
*/
uint32_t xcodec_input_read_async(xcodec_input_t *ch, void *data, uint32_t size);

/**
  \brief       Start receive data to the buffer
  \param[in]   ch    Codec input channel handle
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_input_start(xcodec_input_t *ch);

/**
  \brief       Stop receive data
  \param[in]   ch    Codec input channel handle
  \return      None
*/
void xcodec_input_stop(xcodec_input_t *ch);

/**
  \brief       Reset the buf, discard all data in the buffer
  \param[in]   ch    Codec input channel handle
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_input_buffer_reset(xcodec_input_t *ch);

/**
  \brief       Get input-buffer free space
  \param[in]   ch    Codec input channel handle
  \return      Buffer free space (bytes)
*/
uint32_t xcodec_input_buffer_avail(xcodec_input_t *ch);

/**
  \brief       Get used space of input-buffer
  \param[in]   ch    Codec input channel handle
  \return      Buffer free space (bytes)
*/
uint32_t xcodec_input_buffer_remain(xcodec_input_t *ch);

/**
  \brief       Mute codec input channel
  \param[in]   ch    Codec input channel handle
  \param[in]   en    True enable codec mute. false disable codec mute
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_input_mute(xcodec_input_t *ch, bool en);

/**
  \brief       Set codec input channel digital gain
  \param[in]   ch     Codec input channel handle
  \param[in]   val    Gain val
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_input_digital_gain(xcodec_input_t *ch, float val);

/**
  \brief       Set codec input channel analog gain
  \param[in]   ch     Codec input channel handle
  \param[in]   val    Gain val
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_input_analog_gain(xcodec_input_t *ch, float val);

/**
  \brief       Set codec input channel mix gain
  \param[in]   ch     Codec input channel handle
  \param[in]   val    Gain val
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_input_mix_gain(xcodec_input_t *ch, float val);

/**
  \brief       Get codec input channel state
  \param[in]   ch       Codec input channel handle
  \param[out]  state    Channel state
  \return      Channel state
*/
xcodec_error_t xcodec_input_get_state(xcodec_input_t *ch, xcodec_state_t *state);

/**
  \brief       Enable codec power manage
  \param[in]   codec    Codec handle to operate
  \return      error code \ref xcodec_error_t
*/
xcodec_error_t xcodec_enable_pm(xcodec_t *codec);

/**
  \brief       Disable codec power manage
  \param[in]   codec    Codec handle to operate
  \return      None
*/
void xcodec_disable_pm(xcodec_t *codec);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_CODEC_H_  */
