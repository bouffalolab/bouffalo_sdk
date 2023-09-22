# Audio Related Demo Description[中文](README_zh.md)

**Special Note**: As of the latest update to this documentation, the coexistence of WiFi and Bluetooth on the BL616 has not been ported yet. Therefore, playing Bluetooth music while playing WiFi music may lead to unknown anomalies, and vice versa. Please stay tuned for upcoming updates.

## Audio-Related Demo Planning

| Name         |    Use Case    | Planned Features   | Completion |
|:------------|:-------------|:------------|:---------|
| codec       | Requires only basic audio driver capabilities | Basic audio-level Demo, planning to support recording, playback, and loopback | Currently supports recording and playback, loopback support to be added later     |
| player      | Extensive audio playback capabilities | Player-related Demo, planning to support audio streaming from memory, flash, SD card, WiFi, and BT | Currently supports memory and flash playback, also supports WiFi and BT, but not WiFi-BT coexistence (to be supported in a future system update)    |
| player_bt   | Requires only BT playback capabilities | Player-related Demo, planning to support only BT audio streaming | Completed     |
| player_wifi | Requires only WiFi playback capabilities | Player-related Demo, planning to support only WiFi audio streaming | Completed                       |
| player_mp3  | Requires only alert capabilities and minimal resource usage | Simplified version, supports mp3 format playback only | Completed                    |

PS: The system supports the "record" command, which allows real-time recording to a PC (Linux or Windows).

## Comprehensive Audio Decoders

|  Format      |  BL606P/BL808  | BL616/BL618 |
|:------------:|:--------------:|:-----------:|
|  pcm         |      √         |      √      |
|  wav         |      √         |      √      |
|  mp3         |      √         |      √      |
|  mp4/m4a     |      √         |      √      |
|  aac         |      √         |      √      |
|  amr         |      √         |      √      |
|  flac        |      √         |      √      |
|  HLS         |      √         |      √      |
|  m3u8        |      √         |      √      |
|  MPEG-Dash   |      √         |      ○      |
|  ogg         |      √         |      √      |
|  opus        |      √         |      √      |
|  vorbis      |      √         |      √      |
|  speex       |      √         |      √      |
|  sbc         |      √         |      √      |