# 简单 ASCII 内存绘字

同步把等宽 1bpp ASCII 字模渲染到调用者的 RGB 缓冲。无初始化、堆分配、OS、DMA、屏幕提交或 cache 操作。
配置可声明为 `static const`，重复用于不同缓冲。支持 RGB565、RGB888、XRGB8888，不支持 ARGB8888 或 YUV 目标。

## 配置

在 `BSP_DISPLAY` 下启用 `CONFIG_DISPLAY_SIMPLE_FONT=y`（默认关闭）。内置字体独立选择：

| Kconfig | 字体指针宏 | 宽×高 | 默认 |
|---|---|---|---|
| `DISPLAY_SIMPLE_FONT_ASCII_16X8` | `DISPLAY_SIMPLE_FONT_ASCII_16X8` | 8×16 | 启用功能后打开 |
| `DISPLAY_SIMPLE_FONT_ASCII_32X16` | `DISPLAY_SIMPLE_FONT_ASCII_32X16` | 16×32 | 关闭 |
| `DISPLAY_SIMPLE_FONT_ASCII_64X32` | `DISPLAY_SIMPLE_FONT_ASCII_64X32` | 32×64，仅数字 0～9 | 关闭 |

名称按高×宽排列，表中列出实际宽×高。8×16 和 16×32 字体覆盖 `0x20..0x7e`，各 95 个可打印 ASCII 字符；32×64 字体只含 `0x30..0x39`，
不含字母、标点或 `?`，其余字符按空白单元处理。宏展开为只读字体对象的地址，未编译的字体不能引用。

## 内置字库与来源

三套 **SDK Code Bitmap** 字库均由 [Monaspace Neon Regular v1.400](https://github.com/githubnext/monaspace/tree/v1.400)
生成，采用同样的等宽轮廓、ASCII 基线和缩放规则，保持统一风格。每种尺寸单独栅格化，不是放大较小点阵。
生成时使用 4 倍超采样、Lanczos 缩小及 96/255 阈值，最终固件只存 1bpp 常量，不需要字体引擎或 Python。

| 源文件后缀 | 二维数组 | 字模数据大小 |
|---|---|---|
| `ascii_16x8.c` | `bitmap[95][16]` | 1520 字节 |
| `ascii_32x16.c` | `bitmap[95][64]` | 6080 字节 |
| `ascii_64x32.c` | `bitmap[10][256]` | 2560 字节 |

每个数组元素对应一个字符，内部是直接十六进制字节，逐行存放、MSB 在左，无点阵展开宏。
上述大小不包括字体描述对象。字库数据遵循 [SIL OFL 1.1](tools/LICENSE.OFL)，分发时须保留版权和许可；
渲染器继续使用原有许可。Monaspace 名称仅用于原字体来源归属。

`tools/generate_fonts.py` 保存固定源版本和 SHA256，支持 `--font` 使用本地官方 OTF、`--check` 比较产物，
以及 `--preview` 输出 PNG。主机生成依赖 Pillow；当前产物使用 Pillow 10.2.0 / FreeType 2.13.2，
使用相同版本可复现相同字节。脚本不会参与 SDK 构建，构建也不会联网下载字体。

## 使用

```c
#include "simple_font/display_simple_font.h"

static const display_simple_font_cfg_t text_cfg = {
    .format = DISPLAY_FMT_RGB565,
    .width = 320,
    .height = 240,
    .stride = 0,
    .font = DISPLAY_SIMPLE_FONT_ASCII_16X8,
    .fg_color = 0xffffffff, /* 不透明白字 */
    .bg_color = 0x80000000, /* 半透明黑底；0x00000000 为透明底 */
};

/* buff 指向完整图像起点，不需要按文字坐标偏移地址。 */
int draw_status(void *buff)
{
    return display_simple_font_draw(&text_cfg, buff, 10, 20, "FPS: 30\nReady");
}
```

## 内存与绘制规则

- 配置的宽高描述整个目标图像；调用参数 `(x,y)` 是字符串左上角，单位为像素。
- `stride` 单位为字节，0 表示 `width * BPP`，非零必须大于等于该值。最少提供
  `(height - 1) * 实际stride + width * BPP` 字节的可写存储；接口不接收容量，实际大小由调用者保证。
- RGB565 是小端 `RRRRRGGGGGGBBBBB`，RGB888 的内存字节顺序是 B、G、R；XRGB8888 为 B、G、R、X，绘制保持 X 原值。
  内存绘字本身不要求地址或 stride 对齐，但后续提交显示必须满足对应后端约束。
- 颜色参数都是数值 `0xAARRGGBB`，与目标格式无关。A=0 不读写原像素，A=255 覆盖 RGB；其他值按
  `(src * A + dst * (255 - A) + 127) / 255` 混合。RGB565 先位扩展到 8-bit 分量，混合后再量化。
- 字模的 1 位使用前景，0 位使用背景，每像素只混合一次。背景只覆盖实际经过的字符单元（包括空格），不清整行或整图。
- `\n` 换行并回到本次起始 x，`\r` 和其他 ASCII 控制字符忽略。右侧放不下完整字符时先换行；
  如果从起始 x 到右边界仍放不下一个字符，则裁剪该字符，不重复换行。底部裁剪，空字符串和图外起点不写入。
- 缺字用 `?`；自定义字体没有 `?` 时绘制空白字符单元。不解码 UTF-8，每个非 ASCII 字节按一次缺字处理。
- 返回 0 表示完成；参数/stride/字体无效返回 `-EINVAL`，不支持格式返回 `-ENOTSUP`，地址或跨度溢出返回 `-EOVERFLOW`。
- 配置、字模、字符串不能与目标可写像素重叠。调用者负责并发和缓冲所有权，禁止修改正在被硬件读取的帧。
  渲染返回后，应用按需自行提交 GRAM 绘制或视频翻帧。

## 自定义字体

填写 `display_simple_font_t` 的字符宽高、`first_char`、`char_count`、`bitmap`，然后将其地址赋给 `cfg.font`。
字符范围必须是可打印 ASCII 的连续子集。位图依次存放各字符，每个字符逐行存储，每行 `ceil(width / 8)` 字节，
最高位对应最左侧像素，行尾不足字节部分补齐。字模总长度至少为 `char_count * height * ceil(width / 8)` 字节。
字体及字模必须在调用期间可读，不需要注册或回调。
