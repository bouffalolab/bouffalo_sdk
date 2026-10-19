MQuickJS Component
==================

This directory now keeps only the SDK component runtime for `mquickjs`.

## Scope

- Core engine/runtime sources live here.
- Demo entry, flash config, media packing, and JS test assets live in
  `examples/mquickjs`.

## Component Sources

- `mquickjs.c`: JavaScript engine core
- `cutils.c`, `dtoa.c`, `libm.c`: runtime support code
- `bouffalo_mquickjs.c`: Bouffalo SDK binding layer and shell commands
- `mqjs_stdlib.h`: embedded pre-generated JS stdlib blob used by the runtime

## Build

This directory is built as an SDK component through:

- [CMakeLists.txt](/home/pfchen/workspace/tmp/bouffalo_sdk/components/utils/mquickjs/CMakeLists.txt)
- [Kconfig](/home/pfchen/workspace/tmp/bouffalo_sdk/components/utils/mquickjs/Kconfig)

The normal integration and validation flow is under:

- [examples/mquickjs](/home/pfchen/workspace/tmp/bouffalo_sdk/examples/mquickjs)

## Runtime Commands

After the example is flashed and booted:

- `js_run <file>`
- `js_eval "<expr>"`
- `js_status`
- `js_info`

For LittleFS test content and media generation, use the example directory rather
than this component directory.
