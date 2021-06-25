**board/bl706_avb/pinmux_config.h** 中 **PINMUX_SELECT** 选择 **PINMUX_CAMERA_LCD**
**bsp_common/image_sensor/bsp_image_sensor.c** 中**FORMAT_SEL** 选择 **RGB565**

```bash

$ make APP=camera_interleave_psram BOARD=bl706_avb

```