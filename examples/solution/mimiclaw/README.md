# mimiclaw (BouffaloSDK Solution Skeleton)

This solution is the initial migration skeleton for running upstream `mimiclaw` on BouffaloSDK targets.

Current status:
- websocket transport: Bouffalo `xwebsocket` adapter
- kv storage: EasyFlash adapter
- `tool_get_time`: NTP (UDP/123) sync + on-chip RTC update (UTC format output)
- `tool_get_time`: external RTC fallback/writeback via DS3231 (I2C0)
- `tool_files`: dual backend
  - `/spiffs/*` -> LittleFS (`media` -> `DATA` -> `PSM` fallback)
  - `/sd/*` -> FatFS on SD card
- web search: Bocha AI Search API over HTTPS by default, Brave kept as compatibility provider (`tool_web_search`)
- cron scheduler: upstream `cron_service` + `tool_cron_*`
- Telegram bot channel:
  - long polling `getUpdates` via HTTPS
  - inbound Telegram text -> message bus -> local agent/LLM
  - outbound bus response -> `sendMessage`
  - bot token/update offset persisted in EasyFlash KV
- direct LLM path (minimal):
  - inbound WS `{"type":"message","content":"..."}` -> message bus
  - local `agent_loop` + `llm_proxy` (HTTPS to Anthropic/OpenAI)
  - outbound bus -> WS `{"type":"response","chat_id":"...","content":"..."}`
- upstream source built directly:
  - `upstream/main/bus/message_bus.c`
  - `upstream/main/cron/cron_service.c`
  - `upstream/main/tools/tool_cron.c`
  - `upstream/main/tools/tool_registry.c`
- audio/ota integration: pending
- Telegram polling auto-starts after network is ready
- websocket backend auto-connect is disabled by default

## Directory Layout

- `upstream/`: pristine upstream mirror (managed via `git subtree`)
- `port/`: SDK/platform adaptation layer
- `app/`: integration glue and runtime control
- `tools/sync_upstream.sh`: upstream sync helper

## Why this layout

- Keep upstream source unchanged for low-conflict updates.
- Put all SoC/SDK adaptations in `port/`.
- Update upstream fast and safely by pulling only `upstream/`.

## Build

```bash
make
```

## Flash

```bash
make flash CHIP=<chip> COMX=<serial_port>
```

## Runtime

1. Connect Wi-Fi:

```text
wifi_sta_connect <ssid> <psk>
```

2. Configure backend endpoint:

```text
mimiclaw_cfg <ws|wss> <host> <port> [path]
mimiclaw_cfg_show
```

3. Start / stop:

```text
mimiclaw_start
mimiclaw_stop
```

Note:
- `mimiclaw_start` now starts the local agent path normally, and Telegram polling auto-starts when Wi-Fi is ready.
- WebSocket backend control channel is kept available in code, but auto-connect is disabled by default.

4. Check registered tools / run one tool:

```text
mimiclaw_tools
mimiclaw_set_search_provider <bocha|brave>
mimiclaw_set_search_key <search_api_key>
mimiclaw_set_api_key <llm_api_key>
mimiclaw_set_provider <anthropic|openai|deepseek|minimax>
mimiclaw_set_model <model_id>
mimiclaw_set_tg_token <telegram_bot_token>
mimiclaw_tool_exec get_current_time {}
mimiclaw_tool_exec get_current_time {"sync":true}
mimiclaw_tool_exec write_file {"path":"/spiffs/test.txt","content":"hello"}
mimiclaw_tool_exec read_file {"path":"/spiffs/test.txt"}
mimiclaw_tool_exec write_file {"path":"/sd/test.txt","content":"hello sd"}
mimiclaw_tool_exec list_dir {"prefix":"/sd"}
mimiclaw_tool_exec web_search {"query":"Bouffalo BL616 news"}
mimiclaw_tool_exec cron_add {"name":"hello","schedule_type":"every","interval_s":300,"message":"say hello"}
mimiclaw_tool_exec cron_list {}
mimiclaw_tool_exec play_audio {"path":"/sd/test.wav"}
mimiclaw_tool_exec play_audio {"path":"/sd/test.mp3"}
mimiclaw_tool_exec audio_status {}
mimiclaw_tool_exec stop_audio {}
mimiclaw_audio_play /sd/test.wav
mimiclaw_audio_play /sd/test.mp3
mimiclaw_audio_status
mimiclaw_audio_stop
mimiclaw_ws_send hello_from_device
```

Note:
- `web_search` defaults to provider `bocha`; switch with `mimiclaw_set_search_provider <bocha|brave>`.
- `web_search` requires the active provider API key (set `MIMI_SECRET_SEARCH_KEY` or call `mimiclaw_set_search_key`).
- LLM call requires API key (set `MIMI_SECRET_API_KEY` or call `mimiclaw_set_api_key`).
- Telegram bot requires token (set `MIMI_SECRET_TG_TOKEN` or call `mimiclaw_set_tg_token`).
- For DeepSeek, set `mimiclaw_set_provider deepseek`, then set model like `deepseek-chat` or `deepseek-reasoner`.
- For MiniMax, set `mimiclaw_set_provider minimax`, then set model like `MiniMax-M2.5` (OpenAI-compatible endpoint: `https://api.minimaxi.com/v1/chat/completions`).
- backend config (`scheme/host/port/path`) is persisted in EasyFlash KV.
- Time output is currently UTC text.
- `get_current_time` default behavior is RTC read; pass `{"sync":true}` to force NTP sync.
- External RTC path defaults to DS3231 over I2C0.
- Audio playback supports `/sd/*.wav` and `/sd/*.mp3` (requires audio codec hardware on board).

## Mock WS Backend (No Self-Hosted Backend Needed)

Use the local mock backend to run end-to-end WS interaction before you migrate `agent_loop + llm_proxy`.

1. Start backend on your PC:

```bash
cd examples/solution/mimiclaw
python3 -m pip install websockets
python3 tools/mock_ws_backend.py --host 0.0.0.0 --port 8080
```

2. On board CLI, connect to the PC backend (`<pc_lan_ip>`):

```text
mimiclaw_cfg ws <pc_lan_ip> 8080 /
mimiclaw_cfg_show
```

3. In backend terminal, call device tools directly:

```text
list
tool all get_current_time {"sync":true}
tool all list_dir {"prefix":"/spiffs"}
msg all hello mimiclaw
```

Device will:
- execute tool requests and send back `tool_result` JSON.
- process `msg` text with local LLM and send back `response` JSON.

## Upstream Sync

```bash
./tools/sync_upstream.sh
# or
./tools/sync_upstream.sh <tag_or_branch>
```

Notes:
- `auto` mode is default.
- clean worktree: tries `git subtree` sync.
- dirty worktree: falls back to snapshot sync.
