Flash tool 差异
=======================

当前，我们提供了以下 flash tool：

- Bouffalo Lab DevCube （GUI + CMD(bflb_mcu_tool + bflb_iot_tool)）
- bouffalo_flash_cube (GUI + CMD)

Bouffalo Lab DevCube 会将用户的运行固件（不包含bootheader），根据界面上的 clock 和 flash 配置生成 bootheader 并与固件打包成 whole img bin,
iot 和 mcu 主要区别是烧录的文件种类不一样， iot 在烧录时除了运行固件还有 boot2/dts/mfg/media 等等。

而 bouffalo_flash_cube 则是直接烧录 bin 文件，如果是烧录运行固件，则是烧录 whole img bin，whole img bin 不再由工具生成，而是代码自动在固件前面增加 bootheader，后续修改非常方便。
如果想要增加 bin 文件烧录，比如 boot2/dts/mfg/media ，则直接在 **Flash prog cfg.ini** 文件添加即可，到此 bouffalo_flash_cube 就不再有其他功能了。

你可能会遇到以下情况：

- iot/mcu sdk 的单核固件（无bootheader），请用 Bouffalo Lab DevCube
- iot/mcu sdk 的单核固件（带bootheader），两者都可以
- iot/mcu sdk 多核固件，请用 Bouffalo Lab DevCube
- BouffaloSDK 的单核固件（带bootheader），两者都可以
- BouffaloSDK 多核固件，请用 bouffalo_flash_cube，不准使用 Bouffalo Lab DevCube