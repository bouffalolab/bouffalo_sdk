# change log on Matter MFD module

## 2026/6/29
- Version: 1.6.4
- Changes
  - Refined `mfd_copyDataItem` return values: -2 for element not found (mValuePtr is NULL), 0 for zero-length data, -1 for buffer too small or NULL pBuf argument.
  - Added `static` qualifier to `g_mfd_var` global variable.
  - Added NULL pointer validation for `pBuf` parameter in `mfd_copyDataItem`.

## 2026/6/8
- Version: 1.6.3
- Changes
  - Support to get efuse aes iv using the mfd_getElementById() API.

## 2026/5/8
- Version: 1.6.2
- Changes
  - Refactored to support all Bouffalo chip series (BL616/BL616CL/BL602/BL702/BL702L)
  - Renamed all source files and APIs from `bl_mfd` to `bflb_mfd` prefix
  - Removed build-system-level platform configuration, using compile-time chip selection instead
  - Added BL616CL platform support
  - Added `bflb_mfd_encrypt` public API for all platforms
  - Added product appearance attributes (`mfd_getProductFinish`, `mfd_getProductPrimaryColor`)
  - Merged encrypt/decrypt implementations to reduce code size
  - Replaced hardcoded AES device name with `BFLB_NAME_SEC_AES` constant
  - Fixed bouffalo.mk source file name mismatch

## 2024/7/5
- Version: 1.6.1
- Changes
  - Support zero cipher text data for development/test
  - Support cached memory for BL616 platform.
