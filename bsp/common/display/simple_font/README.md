# Simple ASCII memory renderer

Synchronously renders fixed-size 1bpp ASCII glyphs into caller-owned RGB memory. No initialization, allocation, OS,
DMA, display submission or cache operation. A `static const` configuration can be reused with different buffers.
Supported destinations are RGB565, RGB888 and XRGB8888, not ARGB8888 or YUV.

## Configuration

Enable `CONFIG_DISPLAY_SIMPLE_FONT=y` under `BSP_DISPLAY` (disabled by default). Select fonts independently:

| Kconfig | Font pointer macro | Width × height | Default |
|---|---|---|---|
| `DISPLAY_SIMPLE_FONT_ASCII_16X8` | `DISPLAY_SIMPLE_FONT_ASCII_16X8` | 8×16 | On when the renderer is enabled |
| `DISPLAY_SIMPLE_FONT_ASCII_32X16` | `DISPLAY_SIMPLE_FONT_ASCII_32X16` | 16×32 | Off |
| `DISPLAY_SIMPLE_FONT_ASCII_64X32` | `DISPLAY_SIMPLE_FONT_ASCII_64X32` | 32×64, digits 0..9 only | Off |

Names use height × width; the table gives actual width × height.
The 8×16 and 16×32 fonts each cover 95 printable ASCII characters, `0x20..0x7e`. The 32×64 font contains only
`0x30..0x39`, with no letters, punctuation or `?`; other characters render as blank cells.
Macros expand to addresses of read-only font objects.
Referencing a font requires compiling its corresponding source through Kconfig.

## Built-in fonts and provenance

All three **SDK Code Bitmap** fonts are derived from [Monaspace Neon Regular v1.400](https://github.com/githubnext/monaspace/tree/v1.400),
using the same monospaced outlines, shared ASCII baseline and scaling rules. Each size is rasterized separately,
not enlarged from the smaller bitmap. Generation uses 4x supersampling, Lanczos downsampling and a 96/255 threshold.
Firmware stores only constant 1bpp data and needs no font engine or Python.

| Source suffix | Two-dimensional array | Bitmap data size |
|---|---|---|
| `ascii_16x8.c` | `bitmap[95][16]` | 1520 bytes |
| `ascii_32x16.c` | `bitmap[95][64]` | 6080 bytes |
| `ascii_64x32.c` | `bitmap[10][256]` | 2560 bytes |

Each entry is one character containing explicit hexadecimal bytes, row-major and MSB-left, without bitmap expansion
macros. Sizes exclude the font descriptor. Font data is licensed under [SIL OFL 1.1](tools/LICENSE.OFL); retain its copyright
and license when distributing it. The renderer retains its existing license. Monaspace is mentioned only as source attribution.

`tools/generate_fonts.py` pins the source revision and SHA256. It supports `--font` for a local official OTF,
`--check` to compare generated files, and `--preview` for a PNG sheet. Host generation requires Pillow;
the checked-in data uses Pillow 10.2.0 / FreeType 2.13.2. Use the same versions for byte-for-byte reproduction.
The script is not part of SDK builds, and builds do not download fonts.

## Usage

```c
#include "simple_font/display_simple_font.h"

static const display_simple_font_cfg_t text_cfg = {
    .format = DISPLAY_FMT_RGB565,
    .width = 320,
    .height = 240,
    .stride = 0,
    .font = DISPLAY_SIMPLE_FONT_ASCII_16X8,
    .fg_color = 0xffffffff, /* Opaque white */
    .bg_color = 0x80000000, /* Half-transparent black; 0x00000000 leaves background unchanged */
};

/* buff points to the entire image, not an address adjusted for the text position. */
int draw_status(void *buff)
{
    return display_simple_font_draw(&text_cfg, buff, 10, 20, "FPS: 30\nReady");
}
```

## Memory and drawing rules

- Configured dimensions describe the entire image. Call arguments `(x,y)` give the text's top-left pixel.
- `stride` is in bytes: 0 means `width * BPP`; otherwise it must be at least that value. Provide at least
  `(height - 1) * effective_stride + width * BPP` writable bytes. There is no capacity argument; the caller guarantees allocation size.
- RGB565 is little-endian `RRRRRGGGGGGBBBBB`. RGB888 bytes are B,G,R; XRGB8888 bytes are B,G,R,X, with X preserved.
  The renderer imposes no buffer/stride alignment; subsequent display submission must meet backend constraints.
- Colors are numeric `0xAARRGGBB`, independent of destination format. A=0 leaves the pixel unread and unchanged;
  A=255 overwrites RGB. Otherwise each component uses `(src * A + dst * (255 - A) + 127) / 255`.
  RGB565 channels are bit-replicated to 8 bits before blending and quantized afterwards.
- Glyph bit 1 selects foreground, bit 0 background: exactly one blend per pixel. Background covers only visited
  character cells, including spaces, not the rest of a line or image.
- `\n` advances one character height and returns to the original x; `\r` and other ASCII controls are ignored.
  A character that does not fit at the right edge wraps first. If even the space from the original x is too narrow,
  the character is clipped there rather than repeatedly wrapped. Bottom pixels are clipped. Empty text and outside origins write nothing.
- Missing glyphs use `?`, or a blank cell if a custom font has no `?`. No UTF-8 decoding: each non-ASCII byte is one missing glyph.
- Returns 0 on completion, `-EINVAL` for invalid arguments/stride/font, `-ENOTSUP` for unsupported formats,
  or `-EOVERFLOW` for address/size overflow.
- Config, font, bitmap and text must not overlap writable destination pixels. The caller handles synchronization
  and buffer ownership; never modify a frame being read by hardware. Submit GRAM draws or video flips separately after rendering.

## Custom fonts

Provide a `display_simple_font_t` with character width/height, `first_char`, `char_count` and `bitmap`; assign its
address to `cfg.font`. The character range must be a contiguous printable ASCII subset. Glyphs are consecutive,
each containing height rows of `ceil(width / 8)` bytes, MSB leftmost, byte-padded at row ends. Supply at least
`char_count * height * ceil(width / 8)` bitmap bytes. Font and bitmap must stay readable during drawing.
No registration or callbacks are needed.
