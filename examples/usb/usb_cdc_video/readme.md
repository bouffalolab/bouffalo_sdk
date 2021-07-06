## 修改程序
- **board/bl706_avb/pinmux_config.h** 中 **PINMUX_SELECT** 选择 **PINMUX_UVC**

- **bsp/bsp_common/image_sensor/bsp_image_sensor.c** 中 **FORMAT_SEL** 选择 **UYVY**

- **bsp/bsp_common/image_sensor/bsp_image_sensor.c** 中 **CAM_CODE** 选择 **CAM_USING_POLL_MODE**

- **example/usb/usb_cdc_video** 中 **mjpeg_cfg.quality** 的值越大，图片质量越高

## 编译命令
```bash
$ make APP=usb_cdc_video BOARD=bl706_avb
```

## 硬件环境
- **BL706_AVB** 开发板上的 **FUN1** 跳线帽摘掉
- **BL706_AVB** 开发板上的 **J5** 接上摄像头

## 期望结果
1. 烧入代码后，按下 **BL706_AVB** 开发板上的 **RST** 按键，**USB** 串行设备 **COM** 口将更新，接着打开 **BLDVP.exe** 工具；

2. 取消 **BLDVP.exe** 工具中 **HASH CHECK OUT** 框的勾，然后 **listen COM** 框选择刚刚更新的 **COM** 口；

3. 按下 **BLDVP.exe** 工具中 **Start** 按键，再按下 **BL706_AVB** 开发板的 **RST** 按键；

4.  **BLDVP.exe** 工具将会显示摄像头拍摄的画面，如果没有，就重复第 **2** 、**3** 步。


注：**USB** 串行设备发送的数据虽然可以保存为 **bin** 文件，但是直接打开 **bin** 文件是无法显示画面的，必须使用 **BLDVP.exe** 工具解压缩 **bin** 文件和解码，才能显示拍摄的画面，**BLDVP.exe** 工具地址 **Share(\\10.28.10.192)\tool\bouffalo_dvp\v1.1.1**。