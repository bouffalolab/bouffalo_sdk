**bsp_common/image_sensor/bsp_image_sensor.c** 中**FORMAT_SEL** 选择 **UYVY**
本实验，通过摄像头采集环境图像，使用 tensorflow-lite 实现了一个可以检测环境中是否有人的 demo。
实验需要在 BL706 AVB 板上连接 ili9341 屏幕；用于显示图像，和检测结果

编译命令：
```bash

$ make APP=tflite_vww BOARD=bl706_avb SUPPORT_FLOAT=y

```
NMSIS加速库:
conv, depthwise_conv

实验现象:
在 log 串口中可以看到 person score 的参数
person score < 0, No Person
person score > 0, Person

同时在 ili9341 屏幕上可以显示图像和检测结果

