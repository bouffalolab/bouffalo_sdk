Menuconfig 配置系统使用指南
=============================

配置优先级（从高到低）
--------------------
1. `.config` (用户通过 menuconfig 修改后的配置)
2. `boards/defconfig` (板级默认配置)
3. `proj.conf` (项目基础配置)

快速开始
--------
无需预先运行 menuconfig，直接make即可：

.. code-block:: bash

    make

自定义配置
----------

.. code-block:: bash

    make menuconfig    # 命令行配置界面
    make guiconfig     # 图形化配置界面
    make savedefconfig # 保存当前配置为 boards/defconfig

配置流程
--------
首次构建时：
1. 检查 `build/generated/bouffalolab/.config`
2. 不存在则从 `boards/defconfig` 复制
3. 不存在 defconfig 则创建空配置
4. 自动生成 `autoconf.h` 和 `config.cmake`

注意事项
--------
- `.config` 文件格式为 Kconfig，不要手动编辑

