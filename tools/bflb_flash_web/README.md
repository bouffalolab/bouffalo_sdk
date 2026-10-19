# bflb_flash_web

Pure static Web Serial flasher for Bouffalo chips. It mirrors the target
configuration and UART protocol used by `tools/bflb_flash` and supports
`BL602`, `BL616`, and `BL616CL`.

Serve this directory from localhost and open it in desktop Chrome or Edge:

```sh
python3 -m http.server 8000 --directory tools/bflb_flash_web
```

The page writes the selected `whole.bin` at flash address `0x00000000`, performs
an XIP SHA-256 verification, and can reset the board after a successful write.
The BL602 helper loader and flash parameter files are served from the
corresponding `targets/bl602/` directory. Select a chip explicitly or let the
BootROM identify it automatically.
