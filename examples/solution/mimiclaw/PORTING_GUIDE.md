# Mimiclaw Porting Guide

## Objective

Keep `mimiclaw` upstream code easy to sync while isolating BouffaloSDK/platform differences.

## Layering

- `upstream/`
  - upstream mirror only, no local changes
- `port/`
  - OS timing/sleep
  - websocket transport
  - KV storage
  - future: audio, OTA, network wrappers
- `app/`
  - integration entry
  - runtime lifecycle and CLI glue

## Suggested Upstream Mapping

When upstream code is imported, map dependencies like this:

- upstream `port.*` -> `port/include/mimiclaw_port.h` + `port/src/*.c`
- upstream websocket service -> `port/src/mimiclaw_transport_ws.c`
- upstream NVS/config store -> `port/src/mimiclaw_storage.c`
- upstream app entry/state machine -> `app/src/mimiclaw_app.c`

## Current Integration Matrix

- direct upstream build:
  - `upstream/main/bus/message_bus.c`
  - `upstream/main/cron/cron_service.c`
  - `upstream/main/tools/tool_cron.c`
  - `upstream/main/tools/tool_registry.c`
- ported adapter:
  - `port/src/mimiclaw_transport_ws.c`
  - `port/src/mimiclaw_storage.c`
  - `port/src/mimiclaw_tool_get_time.c`
  - `port/src/mimiclaw_tool_files.c`
  - `port/src/mimiclaw_tool_web_search.c`

Notes on file/time adapters:
- `tool_files` now supports dual backends:
  - `/spiffs/*` via native LittleFS APIs (`lfs_*`)
  - `/sd/*` via FatFS (`ff.h`) for SD card
- LittleFS partition auto-probe order: `media` -> `DATA` -> `PSM`.
- `tool_get_time` uses UDP NTP and writes UTC to RTC via `bflb_rtc_set_utc_time`.
- `tool_get_time` reads time from `bflb_rtc_get_utc_time` by default and only syncs NTP when RTC is invalid or `{"sync":true}` is requested.
- external RTC default implementation in `port/src/mimiclaw_ext_rtc.c`:
  - DS3231 over I2C0 (`0x68`, 24h mode)
  - used as fallback source when NTP is unavailable
  - updated by `tool_get_time` after successful NTP sync
- `tool_web_search` uses SDK `https_client_request` to access Brave Search API.
- `tool_cron_*` is active with upstream `cron_service` (persistent file: `/spiffs/cron.json`).

## Sync Workflow

1. Pull upstream into mirror area:

```bash
./tools/sync_upstream.sh
```

2. Compare interfaces between new upstream and local adapter:

- check `port.*` APIs
- update only `port/` and `app/` where needed

3. Rebuild and validate runtime scenarios:

- Wi-Fi reconnect
- websocket reconnect
- audio path (later phase)
- OTA rollback (later phase)

Suggested incremental replacement order for stubs:
1. add proxy support for `tool_web_search` (currently direct HTTPS path)
2. integrate cron execution path with full agent loop (currently scheduler + bus message injection only)

## Multi-chip Strategy

Keep names generic and introduce chip-specific files only when required:

- common first: `port/src/audio_hal.c`
- per chip optional split later: `audio_hal_chip_a.c`, `audio_hal_chip_b.c`, etc.

This keeps default code shared and avoids hardcoding one chip in directory names.
