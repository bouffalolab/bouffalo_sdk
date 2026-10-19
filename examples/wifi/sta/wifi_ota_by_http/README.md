# wifi6

## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL616/BL618 |        |
| BL618DG     |        |
| BL602       |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

The OTA header uses `CONFIG_PROJECT_SDK_VERSION` because `CONFIG_OTA_HEADER_USE_SDK_VERSION=y`. The default version is `1.0.0`; override `PROJECT_SDK_VERSION` when building to package another version. The image version prefix defaults to `EVENT_V`. This example does not enable `CONFIG_OTA_VERSION_CHECK`, so the device does not reject images based on version ordering.

```bash
make CHIP=bl616 BOARD=bl616dk PROJECT_SDK_VERSION=2.1.5
```

- BL618DG (AP core)

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

- BL602

```
make CHIP=bl602 BOARD=bl602dk
```

## Program Flash

```
make flash COMX=xxx ## xxx is your com name
```

## How use wifi OTA test


```
              ))                                 ____________
    \|/      )))         \|/  \|/               |            |
     |______           ___|____|___             |  server pc |
    |       |         |            |            |____________|
    | BL616 |         |   Router   |#<-------->#/            /#
    |_______|         |____________|           /____________/#
   192.168.1.3         192.168.1.1              192.168.1.2
                      SSID: BL_TEST
                    Password:12345678
```

### Start HTTP OTA server

Run an http server using the python command in the directory where the OTA bin file is located.

```bash
$ python3 https.py
```

### Start HTTPS OTA server with mutual TLS

Run an https server with server authentication and required client certificates.

```bash
$ python3 https.py --https --cert ./ssl/server_1.crt --key ./ssl/server_1.key --ca ./ssl/ca_1.crt --require-client-cert
```

The sample certificate set in `ssl/` contains a CA, server certificate/key, and client certificate/key. The firmware embeds `ca_1.crt`, `client_1.crt`, and `client_1.key` from `https_ota_tls_material.h` and injects them into the active `https_fota` shell path automatically. If you replace the files in `ssl/`, regenerate the header first and then rebuild the firmware.

```bash
python3 gen_tls_material.py --input-dir ./ssl --output ./https_ota_tls_material.h
```

### Start TCP OTA server

Run a simple TCP server in the directory where the OTA file is located.

```bash
cat validation_artifacts/2.1.5/wifi_ota_bl616.bin.ota | nc -N -nvl 3365
```

### Connect wifi

Use CLI commands on the BL616 serial port to connect Wi-Fi and trigger OTA.

The HTTP/HTTPS and TCP shell commands accept a `reboot` option:

```text
https_ota_start <url> [reboot]
wifi_ota_test <url> [reboot]
tcp_ota_start <server_ip> [port] [reboot]
```

`wifi_ota_test` is a backward-compatible alias of `https_ota_start`. For all three commands, `reboot` defaults to `1`. Set it to `0` to remain on the currently running firmware after a successful update. Every command invocation starts an OTA regardless of this option; `reboot` only controls the reset after success and is not saved. To specify `reboot` for TCP OTA, the port argument must also be supplied.

```bash
bouffalolab />wifi_sta_connect BL_TEST 12345678
bouffalolab />tcp_ota_start 192.168.18.125 3365
bouffalolab />wifi_ota_test http://192.168.18.125:5000/build/build_out/wifi_ota_bl616.bin.ota
bouffalolab />wifi_ota_test https://192.168.18.125:5000/build/build_out/wifi_ota_bl616.bin.ota
bouffalolab />tcp_ota_start 192.168.18.125 3365 0
bouffalolab />https_ota_start http://192.168.18.125:5000/build/build_out/wifi_ota_bl616.bin.ota 0
bouffalolab />https_ota_start https://192.168.18.125:5000/build/build_out/wifi_ota_bl616.bin.ota 0

```

if connect success, Linux server pc have send data to BL616.

### OTA version and integrity checks

1. The OTA header software version is generated from `PROJECT_SDK_VERSION` and prefixed with `EVENT_V`.
2. The OTA header SHA256 is verified by the common OTA core for TCP, HTTP, and HTTPS paths. A corrupted `.ota` file is aborted and does not switch the active partition.
3. HTTPS with `--require-client-cert` verifies the device against `client_1.crt` signed by `ca_1.crt`, and the device also verifies the server certificate chain with the embedded CA.

## HTTP(S) Server OTA (BL616/BL618/BL618DG)

Start a SoftAP, then set the firmware URL with the shell command:

```text
wifi_ap_start -s SoftAP-OTA -k 12345678 -I 192.168.169.1 -S 2 -L 4
https_server_ota_start <http-or-https-url> [reboot: 0|1]
```

Connect a phone or computer to the AP and open `http://192.168.169.1/` or
`https://192.168.169.1/`. The browser downloads the `.bin.ota` file from the
specified URL and uploads it to the device. The browser needs Internet access
and the firmware host must allow CORS. For the HTTPS page, use an HTTPS firmware
URL and accept the demo certificate warning. `reboot` defaults to `1`; run the
command again and refresh the page to use a new firmware URL.

`CONFIG_FOTA_HTTPD_SERVER=y` enables the OTA POST handler in the FOTA component.
The example registers its HTTPS certificate and supplies the web page.


## OTA use partition

This OTA case use default partition `bsp/board/bl616dk/config/partition_cfg_4M.toml`;
The addresses in the partition table are divided into Address0(A zone) and Address1(B zone), and when the firmware is OTA, it is also the address that the upgraded firmware should be written from the partition table, and the upgrade of the new firmware Boot2 is also based on the active partition in the partition table to get the latest application firmware to start running.

## partition Flash layout

In general IOT application scenarios, the Flash layout is as follows:

```c
/*
************************
 ____________________________   0x0
|      BOOT2 bootheader      |
|----------------------------|
|          Padding           |
|----------------------------|  0x2000
|           BOOT2            |
|----------------------------|
|          Padding           |
|----------------------------|  0xE000
|         Partition0         |
|----------------------------|  0xF000
|         Partition1         |
|----------------------------|  0x10000
|       FW A bootheader      |
|----------------------------|  0x11000
|         FW A zone          |
|----------------------------|
|          Padding           |
|----------------------------|  0x210000
|       FW B bootheader      |
|----------------------------|  0x211000
|         FW B zone          |
|----------------------------|
|          Padding           |
|----------------------------|  0x378000
|         Media zone         |
|----------------------------|
|          Padding           |
|----------------------------|
|           .....            |
|----------------------------|
|          others            |
|____________________________|

*/
```

1. Flash's starting 4K position is Bootheader of Boot2, which is used to analyze and run Boot2 for the first level Bootrom of the chip.
2. Flash 0x2000-0xDFFF is Boot2, and the main functions of Boot2 are as follows:

    2.1 According to the partition table, start the latest firmware.

    2.2 The latest firmware is abnormal, roll back to the old firmware to run.

    2.3 Support compressing firmware, you can decompress the compressed mirror and write it to Flash, and then start running.

    2.4 Support IAP function, you can upgrade firmware through UART/USB.

3. 0xE000-0xEFFF of Flash is partition table 0, 0xF000-0xFFFF of Flash is partition table 1.
4. 0x10000-0x10FFF of Flash is the BootHeader of the application firmware A, which is used to analyze and run the application firmware for the second-level Boot:Boot2 of the chip.
5. 0x11000-0x20FFFF of Flash is the entity of area A of the application firmware, which is the content of the Bin file output after compiling the SDK.
6. 0x210000-0x210FFF of Flash is the BootHeader of the application firmware B, which is used to analyze and run the application firmware for the second-level Boot:Boot2 of the chip.
7. 0x211000-0x377FFF of Flash is the entity of area B of the application firmware, which is the content of the Bin file output after compiling the SDK.
8. Flash's 0x378000-0x3E8FFF is a Media partition, which stores files used by the application.

## This case OTA flow

1. Get the partition table，check partition;
2. Erase the currently inactive FW slot selected from the partition table.
3. Connect OTA server.
4. Check the OTA Header sent by the server; if check is successful, continue to get the OTA data.
5. Calculates whether the hash value of the OTA data is correct.
6. If the hash value is calculated incorrectly, the OTA process is exited and the partition table and currently active partition flags are not updated. If hash is correct, then partition table and current active partition flags are updated, and the `age++` indicates the number of OTA updates.
7. If `reboot` is `1` (the default), reboot the system. If it is `0`, continue running the current firmware; the updated partition is used on a later reboot.
8. If the OTA case run crashes, recover it according to your system policy. This example no longer injects an extra watchdog reset path for OTA automation.

## Dump OTA log

The changes in the partition table before and after OTA are shown below:

OTA before:

```bash
Active PT:0,Age 0
======= PtTable_Config @0x62ff7a68=======
magicCode 0x54504642; version 0x0000; entryCnt 8; age 0; crc32 0x39B2C2F4
 idx  type device active_index    name    address[0]   address[1]   length[0]    length[1]   age
[00]   16     0        0          Boot2   0x00000000   0x00000000   0x0000e000   0x00000000   0
[01]   00     0        0             FW   0x00010000   0x00210000   0x00200000   0x00168000   0
[02]   10     0        0            mfg   0x00210000   0x00000000   0x00168000   0x00000000   0
[03]   02     0        0          media   0x00378000   0x00000000   0x00071000   0x00000000   0
[04]   03     0        0            PSM   0x003e9000   0x00000000   0x00008000   0x00000000   0
[05]   04     0        0            KEY   0x003f1000   0x00000000   0x00002000   0x00000000   0
[06]   05     0        0           DATA   0x003f3000   0x00000000   0x00005000   0x00000000   0
[07]   06     0        0        factory   0x003f8000   0x00000000   0x00008000   0x00000000   0
```

OTA after:

```bash
Active PT:1,Age 1
======= PtTable_Config @0x62ff7cbc=======
magicCode 0x54504642; version 0x0000; entryCnt 8; age 1; crc32 0x810EA591
 idx  type device active_index    name    address[0]   address[1]   length[0]    length[1]   age
[00]   16     0        0          Boot2   0x00000000   0x00000000   0x0000e000   0x00000000   0
[01]   00     0        1             FW   0x00010000   0x00210000   0x00200000   0x00168000   1
[02]   10     0        0            mfg   0x00210000   0x00000000   0x00168000   0x00000000   0
[03]   02     0        0          media   0x00378000   0x00000000   0x00071000   0x00000000   0
[04]   03     0        0            PSM   0x003e9000   0x00000000   0x00008000   0x00000000   0
[05]   04     0        0            KEY   0x003f1000   0x00000000   0x00002000   0x00000000   0
[06]   05     0        0           DATA   0x003f3000   0x00000000   0x00005000   0x00000000   0
[07]   06     0        0        factory   0x003f8000   0x00000000   0x00008000   0x00000000   0
```
