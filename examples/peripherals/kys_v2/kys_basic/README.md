# BL618DG KYS v2

This example configures the board-defined key matrix and supports two build-time modes:

- Default interrupt mode: `make`
- Query mode: `make CONFIG_EXAMPLE_KYS_V2_POLL_MODE=y`

Both modes use `struct bflb_kys_v2_result_s` and the driver's common result
capture path. Query mode runs one blocking scan through
`bflb_kys_v2_trigger_poll()`.

Matrix rows use `KEY_SCAN_DRIVE`, and matrix columns use `KEY_SCAN_IN`. The GPIO
mapping and matrix size come from `board_kys_v2_gpio_config_get()` in the board
GPIO configuration.
