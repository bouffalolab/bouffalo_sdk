#ifndef __EX_CODEC_I2S_H__
#define __EX_CODEC_I2S_H__

#include <stdint.h>

/***************** i2s cfg *****************/

#define I2S_CH_NUM_CFG              (2) /* i2s channel num cfg, 1 or 2 */

/***************** i2s input cfg: *****************/

#define I2S_INPUT_DMA_NUME          "dma0_ch0"
#define I2S_INPUT_DMA_AUTO_DELE_EN  (1)

#define I2S_INPUT_TASK_PRIORITY     (24)

#define I2S_INPUT_FRAME_SIZE        (640)

/***************** i2s output cfg: *****************/

#define I2S_OUTPUT_DMA_NUME         "dma0_ch1"
#define I2S_OUTPUT_DMA_AUTO_DELE_EN (1)

#define I2S_OUTPUT_TASK_PRIORITY    (25)

/********** i2s input and output task init ********/
int external_codec_i2s_task_init(void);

#endif